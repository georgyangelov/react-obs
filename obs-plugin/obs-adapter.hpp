// TODO: Exclude props which are used elsewhere (e.g. styling, name)
void update_settings(
    obs_data_t* settings,
    const protocol::ObjectValue& object
) {
    auto props = object.props();

    for (int i = 0; i < props.size(); i++) {
        protocol::Prop prop = props[i];

        // TODO: Smarter exclusions
        if (prop.key() == "style") {
            continue;
        }

        const char* key = prop.key().c_str();

        switch (prop.value_case()) {
            case protocol::Prop::ValueCase::kUndefined:
                obs_data_unset_user_value(settings, key);
                break;

            case protocol::Prop::ValueCase::kBoolValue:
                obs_data_set_bool(settings, key, prop.bool_value());
                break;

            case protocol::Prop::ValueCase::kIntValue:
                obs_data_set_int(settings, key, prop.int_value());
                break;

            case protocol::Prop::ValueCase::kFloatValue:
                obs_data_set_double(settings, key, prop.float_value());
                break;

            case protocol::Prop::ValueCase::kStringValue:
                obs_data_set_string(settings, key, prop.string_value().c_str());
                break;

            case protocol::Prop::ValueCase::kObjectValue: {
                auto object = obs_data_get_obj(settings, key);
                if (!object) {
                    object = obs_data_create();
                    obs_data_set_obj(settings, key, object);
                    obs_data_release(object);
                }

                update_settings(object, prop.object_value());
                break;
            }

            default:
                blog(LOG_ERROR, "[react-obs] Unsupported prop type");
        }
    }
}

void create_source(const protocol::CreateSource &create_source) {
    blog(LOG_DEBUG, "[react-obs] Creating source: id=%s, container=%s, name=%s, uid=%s",
         create_source.id().c_str(),
         create_source.container_uid().c_str(),
         create_source.name().c_str(),
         create_source.uid().c_str());

    auto settings = obs_data_create();
    update_settings(settings, create_source.settings());

    // TODO: private or non-private?
    auto source = obs_source_create_private(
        create_source.id().c_str(),
        create_source.name().c_str(),
        settings
    );

    obs_data_release(settings);

    auto uid = create_source.uid();

    // Only scenes should not be measured, and these are created with create_scene
    bool is_measured = true;

    auto container_shadow = get_shadow_source(create_source.container_uid());

    if (!container_shadow) {
        blog(LOG_ERROR, "[react-obs] Tried to create source %s in an unregistered container %s",
             uid.c_str(),
             create_source.container_uid().c_str());

        obs_source_release(source);

        return;
    }

    add_shadow_source(uid, source, true, is_measured, container_shadow);

    update_layout_props(uid, create_source.settings());
}

bool register_unmanaged_source(const std::string &uid, const std::string &name) {
    auto source = obs_get_source_by_name(name.c_str());
    if (!source) {
        blog(LOG_ERROR, "[react-obs] Could not find source with name %s", name.c_str());
        return false;
    }

    auto is_scene = !!obs_scene_from_source(source);
    bool is_measured = !is_scene;

    auto shadow = add_shadow_source(uid, source, false, is_measured, nullptr);

    if (is_scene) {
        auto source_width = obs_source_get_width(source);
        auto source_height = obs_source_get_height(source);

        auto yoga_node = shadow->yoga_node;
        YGNodeStyleSetWidth(yoga_node, source_width);
        YGNodeStyleSetHeight(yoga_node, source_height);
    }

    return true;
}

void append_child(const protocol::AppendChild &append_child) {
    blog(LOG_DEBUG, "[react-obs] Appending %s to %s",
         append_child.child_uid().c_str(),
         append_child.parent_uid().c_str());

    auto parent = get_shadow_source(append_child.parent_uid());
    if (!parent) {
        blog(LOG_ERROR, "[react-obs] Cannot find parent source with uid %s", append_child.parent_uid().c_str());
        return;
    }

    if (parent->sceneitem) {
        blog(LOG_ERROR, "[react-obs] Source already added to a scene");
        return;
    }

    auto scene = obs_scene_from_source(parent->source);
    if (!scene) {
        blog(LOG_ERROR, "[react-obs] Parent source %s is not a scene", append_child.parent_uid().c_str());
        return;
    }

    auto child = get_shadow_source(append_child.child_uid());
    if (!child) {
        blog(LOG_ERROR, "[react-obs] Cannot find child source with uid %s", append_child.child_uid().c_str());
        return;
    }

    auto item = obs_scene_add(scene, child->source);

    obs_sceneitem_addref(item);
    child->sceneitem = item;

    YGNodeInsertChild(
        parent->yoga_node,
        child->yoga_node,
        YGNodeGetChildCount(parent->yoga_node)
    );
}

void update_source(const protocol::UpdateSource &update) {
    if (DEBUG_UPDATE) {
        blog(LOG_DEBUG, "[react-obs] Updating source %s", update.uid().c_str());
    }

    auto shadow_source = get_shadow_source(update.uid());
    if (!shadow_source) {
        blog(LOG_ERROR, "[react-obs] Cannot find source %s", update.uid().c_str());
        return;
    }
    auto source = shadow_source->source;

    auto settings = obs_source_get_settings(source);
    if (!settings) {
        blog(LOG_ERROR, "[react-obs] Source %s does not have settings object, WTF", update.uid().c_str());
        return;
    }

    update_settings(settings, update.changed_props());

    // NOTE: This is deferred - actual updates are performed on the next tick
    obs_source_update(source, settings);
    update_layout_props(shadow_source->uid, update.changed_props());
}

void create_scene(const protocol::CreateScene &create_scene) {
    auto scene = obs_scene_create_private(create_scene.name().c_str());
    auto source = obs_scene_get_source(scene);

    auto uid = create_scene.uid();
    auto container_shadow = get_shadow_source(create_scene.container_uid());

    if (!container_shadow) {
        blog(LOG_ERROR, "[react-obs] Tried to create scene %s in an unregistered container %s",
             uid.c_str(),
             create_scene.container_uid().c_str());

        obs_source_release(source);

        return;
    }

    add_shadow_source(uid, source, true, false, container_shadow);
    update_layout_props(uid, create_scene.props());
}

void remove_child(const protocol::RemoveChild &remove) {
    blog(LOG_DEBUG, "[react-obs] Removing child %s", remove.child_uid().c_str());

    auto parent = get_shadow_source(remove.parent_uid());
    if (!parent) {
        blog(LOG_ERROR, "[react-obs] Cannot find parent source with uid %s", remove.parent_uid().c_str());
        return;
    }

    auto scene = obs_scene_from_source(parent->source);
    if (!scene) {
        blog(LOG_ERROR, "[react-obs] Parent source %s is not a scene", remove.parent_uid().c_str());
        return;
    }

    auto child = get_shadow_source(remove.child_uid());
    if (!child) {
        blog(LOG_ERROR, "[react-obs] Cannot find child source with uid %s", remove.child_uid().c_str());
        return;
    }

    if (!child->sceneitem) {
        blog(LOG_ERROR, "[react-obs] Child does not have a sceneitem set");
        return;
    }

    auto sceneitem_scene = obs_sceneitem_get_scene(child->sceneitem);

    if (sceneitem_scene != scene) {
        blog(LOG_ERROR, "[react-obs] Child's parent and the container are different");
        return;
    }

    obs_sceneitem_remove(child->sceneitem);
    obs_sceneitem_release(child->sceneitem);
    child->sceneitem = nullptr;

    YGNodeRemoveChild(parent->yoga_node, child->yoga_node);

    // TODO: Should we GC the child node? Can it be added again in the future?
    // obs_source_remove(child.source);
    //
    // obs_source_release(child.source);
}
