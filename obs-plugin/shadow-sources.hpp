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

static pthread_mutex_t shadow_sources_mutex = PTHREAD_MUTEX_INITIALIZER;

auto yoga_config = YGConfigNew();

// TODO: Cleanup when reconnections happen
std::vector<ShadowSource*> shadow_sources;
std::set<ShadowSource*> containers;
std::unordered_map<std::string, ShadowSource*> shadow_sources_by_uid;

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
    refresh_containers();
    pthread_mutex_unlock(&shadow_sources_mutex);

    if (measured_externally) {
        if (DEBUG_LAYOUT) {
            blog(LOG_DEBUG, "[react-obs] [layout] Adding externally measured node %s", uid.c_str());
        }

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

    shadow_sources.erase(std::remove(shadow_sources.begin(), shadow_sources.end(), shadow), shadow_sources.end());

    obs_source_release(shadow->source);
    delete shadow;

    refresh_containers();
    pthread_mutex_unlock(&shadow_sources_mutex);
}
