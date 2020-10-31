#include <obs-module.h>
#include <obs-frontend-api.h>
#include "react-obs-main.h"
#include <sockpp/tcp_acceptor.h>
#include <yoga/Yoga.h>

#include <algorithm>
#include <thread>
#include <vector>
#include <optional>
#include <variant>
#include <functional>
#include <unordered_map>

#include "api-server.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("react-obs", "en-US")

#define DEBUG_LAYOUT false

struct ShadowSource {
    std::string uid;
    obs_source_t* source;
    YGNodeRef yoga_node;
    ShadowSource* container;

    obs_sceneitem_t* sceneitem;

    bool managed;
    bool measured_externally;

    uint32_t previous_width;
    uint32_t previous_height;
};

//
// Yoga
//

int yoga_logger(
    YGConfigRef config,
    YGNodeRef node,
    YGLogLevel level,
    const char* format,
    va_list args
) {
    int blog_level = LOG_ERROR;

    switch (level) {
        case YGLogLevelVerbose:
        case YGLogLevelDebug:
            blog_level = LOG_DEBUG;
            break;

        case YGLogLevelInfo:
            blog_level = LOG_INFO;
            break;

        case YGLogLevelWarn:
            blog_level = LOG_WARNING;
            break;

        case YGLogLevelError:
        case YGLogLevelFatal:
            blog_level = LOG_ERROR;
            break;
    }

    auto new_format = std::string("[react-obs] [yoga] ") + format;

    blogva(blog_level, new_format.c_str(), args);

    return 0;
}

//void yoga_remove_child(const YGNodeRef parent, const YGNodeRef childToRemove) {
//    auto child_count = YGNodeGetChildCount(parent);
//
//    for (size_t i = 0; i < child_count; i++) {
//        auto child = YGNodeGetChild(parent, i);
//
//        if (child == childToRemove) {
//            YGNodeRemoveChild(<#YGNodeRef node#>, <#YGNodeRef child#>)
//        }
//    }
//}

//
// State
//

// void message_received_from_client(const ClientConnection&, const protocol::ClientMessage&);

static pthread_mutex_t shadow_sources_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t render_mutex = PTHREAD_MUTEX_INITIALIZER;

// TODO: Cleanup when reconnections happen
std::vector<ShadowSource*> shadow_sources;
std::set<ShadowSource*> containers;
std::unordered_map<std::string, ShadowSource*> shadow_sources_by_uid;
std::unordered_map<const obs_source_t*, ShadowSource*> shadow_sources_by_source;

auto yoga_config = YGConfigNew();

//
// Utility functions
//

void log_scene_names() {
    auto scene_names = obs_frontend_get_scene_names();

    for (size_t i = 0; scene_names[i]; i++) {
        auto scene_name = scene_names[i];
        auto scene = obs_get_source_by_name(scene_name);

        blog(LOG_INFO, "[react-obs] Found scene %s", scene_name);

        obs_source_release(scene);
    }

    bfree(scene_names);
}

//
// Elements API
//
obs_scene_t* get_scene() {
    return obs_scene_from_source(obs_frontend_get_current_scene());
}

//
// Managed sources
//

ShadowSource* get_shadow_source(const std::string &uid) {
    pthread_mutex_lock(&shadow_sources_mutex);
    auto shadow = shadow_sources_by_uid.find(uid);

    if (shadow == shadow_sources_by_uid.end()) {
        pthread_mutex_unlock(&shadow_sources_mutex);
        return nullptr;
    }

    pthread_mutex_unlock(&shadow_sources_mutex);
    return shadow->second;
}

ShadowSource* get_shadow_source_by_source(const obs_source_t* source) {
    pthread_mutex_lock(&shadow_sources_mutex);
    auto shadow = shadow_sources_by_source.find(source);

    if (shadow == shadow_sources_by_source.end()) {
        pthread_mutex_unlock(&shadow_sources_mutex);
        return nullptr;
    }

    pthread_mutex_unlock(&shadow_sources_mutex);
    return shadow->second;
}

void refresh_containers() {
    containers.clear();

    for (size_t i = 0; i < shadow_sources.size(); i++) {
        auto container = shadow_sources[i]->container;

        if (container) {
            containers.insert(container);
        }
    }
}

YGSize yoga_measure_function(
    YGNodeRef node,
    float width,
    YGMeasureMode widthMode,
    float height,
    YGMeasureMode heightMode
);

ShadowSource* add_shadow_source(const std::string &uid, obs_source_t* source, bool managed, bool measured_externally, ShadowSource* container) {
    auto yoga_node = YGNodeNewWithConfig(yoga_config);
    auto shadow = new ShadowSource {
        .uid = uid,
        .source = source,
        .yoga_node = yoga_node,
        .container = container,
        .managed = managed,
        .measured_externally = measured_externally,
        .sceneitem = nullptr,

        .previous_width = 0,
        .previous_height = 0
    };

    YGNodeSetContext(yoga_node, shadow);

    pthread_mutex_lock(&shadow_sources_mutex);
    shadow_sources.push_back(shadow);
    shadow_sources_by_uid[uid] = shadow;
    shadow_sources_by_source[shadow->source] = shadow;
    refresh_containers();
    pthread_mutex_unlock(&shadow_sources_mutex);

    if (measured_externally) {
        blog(LOG_DEBUG, "Adding externally measured node %s", uid.c_str());

        YGNodeSetMeasureFunc(yoga_node, yoga_measure_function);
    }

    return shadow;
}

void remove_shadow_source(const std::string &uid) {
    auto shadow = get_shadow_source(uid);

    if (!shadow) {
        return;
    }

    pthread_mutex_lock(&shadow_sources_mutex);
    YGNodeSetContext(shadow->yoga_node, nullptr);

    shadow_sources_by_uid.erase(uid);
    shadow_sources_by_source.erase(shadow->source);

    shadow_sources.erase(std::remove(shadow_sources.begin(), shadow_sources.end(), shadow), shadow_sources.end());

    obs_source_release(shadow->source);
    delete shadow;

    refresh_containers();
    pthread_mutex_unlock(&shadow_sources_mutex);
}

//
// Layout
//

YGSize yoga_measure_function(
    YGNodeRef node,
    float width,
    YGMeasureMode widthMode,
    float height,
    YGMeasureMode heightMode
) {
    UNUSED_PARAMETER(width);
    UNUSED_PARAMETER(widthMode);
    UNUSED_PARAMETER(height);
    UNUSED_PARAMETER(heightMode);

    auto shadow = (ShadowSource*)YGNodeGetContext(node);

    auto measured_width = (float)obs_source_get_width(shadow->source);
    auto measured_height = (float)obs_source_get_height(shadow->source);

    if (DEBUG_LAYOUT) {
        blog(LOG_DEBUG, "[react-obs] [layout] Measuring %s to %f x %f", shadow->uid.c_str(), measured_width, measured_height);
    }

    return YGSize {
        .width = measured_width,
        .height = measured_height
    };
}

//
// Signal handler - source update tracking
//

//void track_source_updates_signal_handler(void *context, const char *signal_name_str, calldata_t *data) {
//    blog(LOG_DEBUG, "[react-obs] Signal %s", signal_name_str);
//
//    std::string signal_name(signal_name_str);
//
//    if (signal_name == "item_transform") {
//        auto sceneitem = (obs_sceneitem_t *)calldata_ptr(data, "item");
//        auto source = obs_sceneitem_get_source(sceneitem);
//        auto shadow = get_shadow_source_by_source(source);
//
//        if (!shadow) {
//            // Transform was not on an item we managed by us
//            return;
//        }
//
// //        YGNodeMarkDirty(shadow->yoga_node);
//
//        blog(LOG_DEBUG, "[react-obs] Detected transform of sceneitem %s", shadow->uid.c_str());
//    }
//}
//
//void start_tracking_source_updates(ShadowSource* shadow) {
// //    auto signal_handler = obs_source_get_signal_handler(source);
// //
// //    signal_handler_connect_global(signal_handler, track_source_updates_signal_handler, nullptr);
//}
//
//void stop_tracking_source_updates(ShadowSource* source) {
//    auto signal_handler = obs_source_get_signal_handler(source);
//
//    signal_handler_disconnect_global(signal_handler, track_source_updates_signal_handler, nullptr);
//}

void perform_layout(ShadowSource* container) {
    auto yoga_node = container->yoga_node;

    if (!YGNodeIsDirty(yoga_node)) {
        return;
    }

    if (DEBUG_LAYOUT) {
        blog(LOG_DEBUG, "[react-obs] [layout] Performing layout on %s...", container->uid.c_str());
    }

    YGNodeCalculateLayout(yoga_node, YGUndefined, YGUndefined, YGDirectionLTR);
    YGTraversePreOrder(yoga_node, [](YGNodeRef node) {
        if (!YGNodeGetHasNewLayout(node)) {
            return;
        }

        YGNodeSetHasNewLayout(node, false);

        auto shadow = (ShadowSource*)YGNodeGetContext(node);
        auto sceneitem = shadow->sceneitem;

        if (!sceneitem) {
            if (shadow->managed) {
                blog(LOG_ERROR, "[react-obs] Sceneitem is not set, but yoga node is a child");
            }

            // Can't set positioning or sizing for unmanaged nodes
            return;
        }

        vec2 position {
            .x = YGNodeLayoutGetLeft(node),
            .y = YGNodeLayoutGetTop(node)
        };

        vec2 bounds {
            .x = YGNodeLayoutGetWidth(node),
            .y = YGNodeLayoutGetHeight(node)
        };

        if (DEBUG_LAYOUT) {
            blog(LOG_DEBUG, "[react-obs] [layout] Node %s set to %f x %f", shadow->uid.c_str(), bounds.x, bounds.y);
        }

        obs_sceneitem_defer_update_begin(sceneitem);
        obs_sceneitem_set_pos(sceneitem, &position);
        obs_sceneitem_set_bounds(sceneitem, &bounds);
        obs_sceneitem_defer_update_end(sceneitem);
    });

    if (DEBUG_LAYOUT) {
        blog(LOG_DEBUG, "[react-obs] [layout] Layout complete");
    }
}

//
// Props
//

using PropMap = std::unordered_map<std::string, protocol::Prop>;

template <class T>
PropMap as_prop_map(
    const T &props
) {
    std::unordered_map<std::string, protocol::Prop> result;

    for (int i = 0; i < props.size(); i++) {
        auto prop = props[i];

        result[prop.key()] = prop;
    }

    return result;
}

std::optional<std::string> as_string(const protocol::Prop &prop) {
    if (prop.value_case() != protocol::Prop::ValueCase::kStringValue) {
        return {};
    }

    return prop.string_value();
}

std::optional<int64_t> as_int(const protocol::Prop &prop) {
    if (prop.value_case() != protocol::Prop::ValueCase::kIntValue) {
        return {};
    }

    return prop.int_value();
}

template <class T>
void assign_prop(
    const PropMap &props,
    const char* name,
    std::function<std::optional<T>(const protocol::Prop &prop)> converter,
    std::function<void(const T&)> apply,
    std::function<void()> reset
) {
    auto propFind = props.find(name);
    if (propFind == props.end()) {
        reset();
        return;
    }

    auto prop = propFind->second;
    auto value = converter(prop);

    if (value.has_value()) {
        apply(*value);
    } else {
        blog(LOG_ERROR, "[react-obs] Property %s is of the wrong type", name);
    }
}

//
// Update functions
//

//void update_intrinsic_dimensions(ShadowSource* shadow) {
//    YGNodeStyleSetWidth(shadow->yoga_node, obs_source_get_width(shadow->source));
//    YGNodeStyleSetHeight(shadow->yoga_node, obs_source_get_height(shadow->source));
//}

void update_layout_props(const std::string &uid, const protocol::ObjectValue &propsObject) {
    auto props = as_prop_map(propsObject.props());
    auto shadow = get_shadow_source(uid);
    auto yoga_node = shadow->yoga_node;

    // TODO: Set aspect ratio based on some heuristic
//    YGNodeStyleSetWidth(shadow->yoga_node, obs_source_get_width(shadow->source));
//    YGNodeStyleSetHeight(shadow->yoga_node, obs_source_get_height(shadow->source));
//    update_intrinsic_dimensions(shadow);

    pthread_mutex_lock(&render_mutex);

    assign_prop<std::string>(props, "flexDirection", as_string, [yoga_node](auto value) {
        if (value == "row") {
            YGNodeStyleSetFlexDirection(yoga_node, YGFlexDirectionRow);
        } else if (value == "column") {
            YGNodeStyleSetFlexDirection(yoga_node, YGFlexDirectionColumn);
        } else if (value == "row-reverse") {
            YGNodeStyleSetFlexDirection(yoga_node, YGFlexDirectionRowReverse);
        } else if (value == "column-reverse") {
            YGNodeStyleSetFlexDirection(yoga_node, YGFlexDirectionColumnReverse);
        }
    }, [yoga_node]() {
        YGNodeStyleSetFlexDirection(yoga_node, YGFlexDirectionColumn);
    });

    pthread_mutex_unlock(&render_mutex);
}

// TODO: Exclude props which are used elsewhere (e.g. styling, name)
void update_settings(
    obs_data_t* settings,
    const protocol::ObjectValue& object
) {
    auto props = object.props();

    for (int i = 0; i < props.size(); i++) {
        protocol::Prop prop = props[i];
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

//
// Updates
//

void check_source_dimensions(ShadowSource* shadow) {
    auto width = obs_source_get_width(shadow->source);
    auto height = obs_source_get_height(shadow->source);

    if (shadow->previous_width != width || shadow->previous_height != height) {
        shadow->previous_width = width;
        shadow->previous_height = height;

        if (DEBUG_LAYOUT) {
            blog(LOG_DEBUG, "[react-obs] [layout] Node %s changed dimensions to %i x %i", shadow->uid.c_str(), width, height);
        }

        YGNodeMarkDirty(shadow->yoga_node);
    }
}

void on_tick_callback(void* context, float elapsed_seconds) {
    UNUSED_PARAMETER(context);
    UNUSED_PARAMETER(elapsed_seconds);

    pthread_mutex_lock(&shadow_sources_mutex);
    for (auto shadow : shadow_sources) {
        if (shadow->measured_externally) {
            check_source_dimensions(shadow);
        }
    }
    pthread_mutex_unlock(&shadow_sources_mutex);

    pthread_mutex_lock(&render_mutex);
    for (auto container : containers) {
        perform_layout(container);
    }
    pthread_mutex_unlock(&render_mutex);
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

    // TODO: Measure other sources as well, except scene ones
    bool is_measured = create_source.id() == "text_ft2_source";

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

    // TODO: Make non-scene sources measured
    auto shadow = add_shadow_source(uid, source, false, false, nullptr);

    // TODO: This may not be correct. These are the dimensions of the source, not the transformed one...
    //       This will work for top-level scenes, but will not work for other sources.
    // TODO: Need to find a way to get the transformed width/height. Is this even possible?
    auto source_width = obs_source_get_width(source);
    auto source_height = obs_source_get_height(source);

    auto yoga_node = shadow->yoga_node;
    YGNodeStyleSetWidth(yoga_node, source_width);
    YGNodeStyleSetHeight(yoga_node, source_height);

    // TODO: Attach to signal of parent scene and listen for transform changes

    // blog(LOG_DEBUG, "[react-obs] Container dimensions are %dx%d", source_width, source_height);

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
    blog(LOG_DEBUG, "[react-obs] Updating source %s", update.uid().c_str());

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

    // TODO: This is probably not instantaneous. Is there a signal for this?
    // See obs_source_deferred_update.
    // I may need to wait for the next frame and update it then...
    obs_source_update(source, settings);

    // TODO: This doesn't catch updates to width of 3 texts until a next update. Why?
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

//void commit_updates(const protocol::CommitUpdates &commit_updates) {
//    // Nothing to do here, layout is now updated in `tick`
//    auto container_uid = commit_updates.container_uid();
//
//    blog(LOG_DEBUG, "[react-obs] Commiting updates for container %s", container_uid.c_str());
//
//    perform_layout(container_uid);
//}

void apply_updates(protocol::ApplyUpdate &update) {
    switch (update.change_case()) {
        case protocol::ApplyUpdate::ChangeCase::kCreateSource:
            create_source(update.create_source());
            break;

        case protocol::ApplyUpdate::ChangeCase::kUpdateSource:
            update_source(update.update_source());
            break;

        case protocol::ApplyUpdate::ChangeCase::kCreateScene:
            create_scene(update.create_scene());
            break;

        case protocol::ApplyUpdate::ChangeCase::kAppendChild:
            append_child(update.append_child());
            break;

        case protocol::ApplyUpdate::ChangeCase::kRemoveChild:
            remove_child(update.remove_child());
            break;

        case protocol::ApplyUpdate::ChangeCase::kCommitUpdates:
            // Nothing to do here, layout is now performed in the video tick
            // commit_updates(update.commit_updates());
            break;

        case protocol::ApplyUpdate::ChangeCase::CHANGE_NOT_SET:
            blog(LOG_ERROR, "[react-obs] Received update request with no change");
            break;
    }
}

//
// Server API
//

void message_received_from_client(
    ClientConnection& client,
    const protocol::ClientMessage& message
) {
    switch (message.message_case()) {
        case protocol::ClientMessage::MessageCase::kInitRequest: {
            auto init_request = message.init_request();

            blog(LOG_DEBUG, "[react-obs] Received init request from %s", init_request.client_id().c_str());

            protocol::ServerMessage message;
            auto response = message.mutable_response();
            response->set_request_id(init_request.request_id());
            response->set_success(true);

            client.send(message);

            break;
        }

        case protocol::ClientMessage::MessageCase::kApplyUpdate: {
            auto command = message.apply_update();

            blog(LOG_DEBUG, "[react-obs] Received update request: %s", command.DebugString().c_str());
            apply_updates(command);

            break;
        }

        case protocol::ClientMessage::MessageCase::kFindSource: {
            auto command = message.find_source();

            blog(LOG_DEBUG, "[react-obs] Received find source request for name %s", command.name().c_str());
            auto success = register_unmanaged_source(command.uid(), command.name());

            protocol::ServerMessage message;
            auto response = message.mutable_response();
            response->set_request_id(command.request_id());
            response->set_success(success);

            client.send(message);

            break;
        }

        case protocol::ClientMessage::MessageCase::MESSAGE_NOT_SET:
            blog(LOG_ERROR, "[react-obs] Empty message with no cases received");
            client.disconnect();
            break;

        default:
            blog(LOG_ERROR, "[react-obs] Unknown client message received");
            client.disconnect();
            break;
    }
}

ApiServer api_server(6666, message_received_from_client);

//
// Test stuffs
//

void test_yoga() {
    auto root = YGNodeNewWithConfig(yoga_config);
    YGNodeStyleSetWidth(root, 1920);
    YGNodeStyleSetHeight(root, 1080);
    YGNodeStyleSetAlignItems(root, YGAlignCenter);
    YGNodeStyleSetJustifyContent(root, YGJustifyCenter);

    auto child = YGNodeNewWithConfig(yoga_config);
    YGNodeStyleSetWidth(child, 800);
    YGNodeStyleSetHeight(child, 600);
    YGNodeInsertChild(root, child, 0);

    auto child_child = YGNodeNewWithConfig(yoga_config);
    YGNodeStyleSetWidthPercent(child_child, 50);
    YGNodeStyleSetAspectRatio(child_child, (float)16 / 9);
    YGNodeInsertChild(child, child_child, 0);

    YGNodeCalculateLayout(root, 1920, 1080, YGDirectionLTR);

    auto left = YGNodeLayoutGetLeft(child_child);
    auto top = YGNodeLayoutGetTop(child_child);

    auto width = YGNodeLayoutGetWidth(child_child);
    auto height = YGNodeLayoutGetHeight(child_child);

    blog(LOG_INFO, "[react-obs] Calculated child dimensions: x = %f, y = %f, w = %f, h = %f",
         left,
         top,
         width,
         height);

    YGTraversePreOrder(root, [](YGNodeRef node) {
        YGNodeSetHasNewLayout(node, false);
    });

    blog(LOG_DEBUG, "[react-obs] Is dirty: root=%d, child=%d child_child=%d",
         YGNodeIsDirty(root), YGNodeIsDirty(child), YGNodeIsDirty(child_child));
    blog(LOG_DEBUG, "[react-obs] Has new layout: root=%d, child=%d child_child=%d",
         YGNodeGetHasNewLayout(root), YGNodeGetHasNewLayout(child), YGNodeGetHasNewLayout(child_child));

    YGNodeStyleSetWidth(child, 700);

    blog(LOG_DEBUG, "[react-obs] Is dirty: root=%d, child=%d child_child=%d",
         YGNodeIsDirty(root), YGNodeIsDirty(child), YGNodeIsDirty(child_child));
    blog(LOG_DEBUG, "[react-obs] Has new layout: root=%d, child=%d child_child=%d",
         YGNodeGetHasNewLayout(root), YGNodeGetHasNewLayout(child), YGNodeGetHasNewLayout(child_child));

    YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

    blog(LOG_DEBUG, "[react-obs] Is dirty: root=%d, child=%d child_child=%d",
         YGNodeIsDirty(root), YGNodeIsDirty(child), YGNodeIsDirty(child_child));
    blog(LOG_DEBUG, "[react-obs] Has new layout: root=%d, child=%d child_child=%d",
         YGNodeGetHasNewLayout(root), YGNodeGetHasNewLayout(child), YGNodeGetHasNewLayout(child_child));

    auto left2 = YGNodeLayoutGetLeft(child_child);
    auto top2 = YGNodeLayoutGetTop(child_child);

    auto width2 = YGNodeLayoutGetWidth(child_child);
    auto height2 = YGNodeLayoutGetHeight(child_child);

    blog(LOG_INFO, "[react-obs] Calculated child dimensions: x = %f, y = %f, w = %f, h = %f",
         left2,
         top2,
         width2,
         height2);

    YGNodeFreeRecursive(root);
}

//void log_signal_callback(void *context, const char *signal_name_str, calldata_t *data) {
//    if (strcmp(signal_name_str, "item_transform") == 0) {
//        auto sceneitem = (obs_sceneitem_t *)calldata_ptr(data, "item");
//        auto source = obs_sceneitem_get_source(sceneitem);
//        auto shadow = get_shadow_source_by_source(source);
//
//        if (!shadow) {
//            // Transform was not on an item we managed by us
//            return;
//        }
//
// //        YGNodeMarkDirty(shadow->yoga_node);
//
//        blog(LOG_DEBUG, "[react-obs] Detected transform of sceneitem %s", shadow->uid.c_str());
//    }
//}

//void test_events() {
//    auto source = obs_get_source_by_name("react-obs");
//    auto signal_handler = obs_source_get_signal_handler(source);
//
//    signal_handler_connect_global(signal_handler, log_signal_callback, nullptr);
//}

//
// Plugin API
//

void initialize() {
    blog(LOG_INFO, "[react-obs] Initializing react-obs");

    YGConfigSetLogger(yoga_config, yoga_logger);

    // obs_frontend_source_list scenes;
    // obs_frontend_get_scenes(&scenes);
    //
    // for (size_t i = 0; i < scenes.sources.num; i++) {
    //     obs_source_t *source = scenes.sources.array[i];
    //     auto scene_name = source->info.get_name(source->info.type_data);
    //
    //     blog(LOG_INFO, "[react-obs] Found scene %s", scene_name);
    // }
    //
    // obs_frontend_source_list_free(&scenes);

    api_server.start();
    obs_add_tick_callback(on_tick_callback, nullptr);

    // log_scene_names();

    // test_yoga();

    // test_events();
}

void shutdown() {
    api_server.stop();
}

//
// OBS API
//

static void OBSEvent(enum obs_frontend_event event, void *) {
    switch (event) {
        case OBS_FRONTEND_EVENT_FINISHED_LOADING:
            blog(LOG_DEBUG, "[react-obs] Frontend finished loading");
            initialize();
            break;

        case OBS_FRONTEND_EVENT_EXIT:
            blog(LOG_DEBUG, "[react-obs] Frontend shutting down");
            break;

        case OBS_FRONTEND_EVENT_SCENE_LIST_CHANGED:
            blog(LOG_DEBUG, "[react-obs] Scene list changed");
            log_scene_names();
            break;

        default:
            break;
    }
}

bool obs_module_load(void) {
    obs_frontend_add_event_callback(OBSEvent, nullptr);

    return true;
}

void obs_module_unload(void) {
    blog(LOG_DEBUG, "[react-obs] Module shutting down");
    shutdown();
}
