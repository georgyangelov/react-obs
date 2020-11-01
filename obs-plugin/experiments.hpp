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
