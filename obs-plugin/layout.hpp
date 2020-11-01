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

    // TODO: Aspect ratio
    // if (widthMode == YGMeasureModeExactly && heightMode == YGMeasureModeExactly) {
//    if (!YGFloatIsUndefined(width) && !YGFloatIsUndefined(height)) {
//        return YGSize {
//            .width = width,
//            .height = height
//        };
//    }

    auto shadow = (ShadowSource*)YGNodeGetContext(node);

    auto measured_width = (float)obs_source_get_width(shadow->source);
    auto measured_height = (float)obs_source_get_height(shadow->source);

    if (DEBUG_LAYOUT) {
        blog(LOG_DEBUG, "[react-obs] [layout] Measuring %s to %f x %f . Params were %f x %f", shadow->uid.c_str(), measured_width, measured_height, width, height);
    }

    return YGSize {
        .width = measured_width,
        .height = measured_height
    };
}

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
        obs_sceneitem_set_bounds_type(sceneitem, OBS_BOUNDS_SCALE_INNER);
        obs_sceneitem_set_bounds(sceneitem, &bounds);
        obs_sceneitem_defer_update_end(sceneitem);
    });

    if (DEBUG_LAYOUT) {
        blog(LOG_DEBUG, "[react-obs] [layout] Layout complete");
    }
}

YGAlign stringToYogaAlign(const std::string &value) {
    if (value == "auto") {
        return YGAlignAuto;
    } else if (value == "center") {
        return YGAlignCenter;
    } else if (value == "stretch") {
        return YGAlignStretch;
    } else if (value == "baseline") {
        return YGAlignBaseline;
    } else if (value == "flex-start") {
        return YGAlignFlexStart;
    } else if (value == "flex-end") {
        return YGAlignFlexEnd;
    } else if (value == "space-around") {
        return YGAlignSpaceAround;
    } else if (value == "space-between") {
        return YGAlignSpaceBetween;
    } else {
        return YGAlignAuto;
    }
}

void update_layout_props(const std::string &uid, const protocol::ObjectValue &propsObject) {
    auto props = as_prop_map(propsObject.props());
    auto shadow = get_shadow_source(uid);
    auto yoga_node = shadow->yoga_node;

    // TODO: Set aspect ratio based on some heuristic?

    auto styleFind = props.find("style");
    if (styleFind == props.end()) {
        return;
    }

    auto styleValue = styleFind->second;
    std::unordered_map<std::string, protocol::Prop> style;

    if (styleValue.undefined()) {
        // Nothing to do, style is empty as it should
    } else {
        auto styleOptional = as_object(styleValue);

        if (!styleOptional.has_value()) {
            blog(LOG_ERROR, "[react-obs] Property styles must be an object, was %i", styleValue.value_case());
            return;
        }

        style = std::move(*styleOptional);
    }

    assign_prop_default<std::string>(style, "flexDirection", as_string, "column", [yoga_node](auto value) {
        if (value == "row") {
            YGNodeStyleSetFlexDirection(yoga_node, YGFlexDirectionRow);
        } else if (value == "column") {
            YGNodeStyleSetFlexDirection(yoga_node, YGFlexDirectionColumn);
        } else if (value == "row-reverse") {
            YGNodeStyleSetFlexDirection(yoga_node, YGFlexDirectionRowReverse);
        } else if (value == "column-reverse") {
            YGNodeStyleSetFlexDirection(yoga_node, YGFlexDirectionColumnReverse);
        }
    });

    assign_prop_default<std::string>(style, "direction", as_string, "inherit", [yoga_node](auto value) {
        if (value == "ltr") {
            YGNodeStyleSetDirection(yoga_node, YGDirectionLTR);
        } else if (value == "rtl") {
            YGNodeStyleSetDirection(yoga_node, YGDirectionRTL);
        } else if (value == "inherit") {
            YGNodeStyleSetDirection(yoga_node, YGDirectionInherit);
        }
    });

    assign_prop_default<std::string>(style, "justifyContent", as_string, "flex-start", [yoga_node](auto value) {
        if (value == "center") {
            YGNodeStyleSetJustifyContent(yoga_node, YGJustifyCenter);
        } else if (value == "flex-start") {
            YGNodeStyleSetJustifyContent(yoga_node, YGJustifyFlexStart);
        } else if (value == "flex-end") {
            YGNodeStyleSetJustifyContent(yoga_node, YGJustifyFlexEnd);
        } else if (value == "space-around") {
            YGNodeStyleSetJustifyContent(yoga_node, YGJustifySpaceAround);
        } else if (value == "space-evenly") {
            YGNodeStyleSetJustifyContent(yoga_node, YGJustifySpaceEvenly);
        } else if (value == "space-between") {
            YGNodeStyleSetJustifyContent(yoga_node, YGJustifySpaceBetween);
        }
    });

    assign_prop_default<std::string>(style, "alignContent", as_string, "flex-start", [yoga_node](auto value) {
        YGNodeStyleSetAlignContent(yoga_node, stringToYogaAlign(value));
    });

    assign_prop_default<std::string>(style, "alignItems", as_string, "stretch", [yoga_node](auto value) {
        YGNodeStyleSetAlignItems(yoga_node, stringToYogaAlign(value));
    });

    assign_prop<std::string>(style, "alignSelf", as_string, [yoga_node](auto value) {
        YGNodeStyleSetAlignSelf(yoga_node, stringToYogaAlign(value));
    }, [yoga_node]() {
        YGNodeStyleSetAlignSelf(yoga_node, YGAlignStretch);
    });

    assign_prop_default<std::string>(style, "position", as_string, "static", [yoga_node](auto value) {
        if (value == "static") {
            YGNodeStyleSetPositionType(yoga_node, YGPositionTypeStatic);
        } else if (value == "relative") {
            YGNodeStyleSetPositionType(yoga_node, YGPositionTypeRelative);
        } else if (value == "absolute") {
            YGNodeStyleSetPositionType(yoga_node, YGPositionTypeAbsolute);
        }
    });

    assign_prop_default<std::string>(style, "flexWrap", as_string, "no-wrap", [yoga_node](auto value) {
        if (value == "no-wrap") {
            YGNodeStyleSetFlexWrap(yoga_node, YGWrapNoWrap);
        } else if (value == "wrap") {
            YGNodeStyleSetFlexWrap(yoga_node, YGWrapWrap);
        } else if (value == "wrap-reverse") {
            YGNodeStyleSetFlexWrap(yoga_node, YGWrapWrapReverse);
        }
    });

    // TODO: This is not supported currently
    assign_prop_default<std::string>(style, "overflow", as_string, "visible", [yoga_node](auto value) {
        if (value == "hidden") {
            YGNodeStyleSetOverflow(yoga_node, YGOverflowHidden);
        } else if (value == "scroll") {
            YGNodeStyleSetOverflow(yoga_node, YGOverflowScroll);
        } else if (value == "visible") {
            YGNodeStyleSetOverflow(yoga_node, YGOverflowVisible);
        }
    });

    // TODO: Setting display to "none" is not supported
    assign_prop_default<std::string>(style, "display", as_string, "flex", [yoga_node](auto value) {
        if (value == "none") {
            YGNodeStyleSetDisplay(yoga_node, YGDisplayNone);
        } else if (value == "flex") {
            YGNodeStyleSetDisplay(yoga_node, YGDisplayFlex);
        }
    });

    assign_prop<float>(style, "flexGrow", as_float, [yoga_node](auto value) {
        YGNodeStyleSetFlexGrow(yoga_node, value);
    }, [yoga_node]() {
        YGNodeStyleSetFlexGrow(yoga_node, YGUndefined);
    });

    assign_prop<float>(style, "flexShrink", as_float, [yoga_node](auto value) {
        YGNodeStyleSetFlexShrink(yoga_node, value);
    }, [yoga_node]() {
        YGNodeStyleSetFlexShrink(yoga_node, YGUndefined);
    });

    assign_prop<Size>(style, "flexBasis", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetFlexBasisPercent(yoga_node, value.value);
        } else {
            YGNodeStyleSetFlexBasis(yoga_node, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetFlexBasisAuto(yoga_node);
    });

    assign_prop<Size>(style, "top", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetPositionPercent(yoga_node, YGEdgeTop, value.value);
        } else {
            YGNodeStyleSetPosition(yoga_node, YGEdgeTop, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetPosition(yoga_node, YGEdgeTop, YGUndefined);
    });

    assign_prop<Size>(style, "left", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetPositionPercent(yoga_node, YGEdgeLeft, value.value);
        } else {
            YGNodeStyleSetPosition(yoga_node, YGEdgeLeft, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetPosition(yoga_node, YGEdgeLeft, YGUndefined);
    });

    assign_prop<Size>(style, "right", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetPositionPercent(yoga_node, YGEdgeRight, value.value);
        } else {
            YGNodeStyleSetPosition(yoga_node, YGEdgeRight, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetPosition(yoga_node, YGEdgeRight, YGUndefined);
    });

    assign_prop<Size>(style, "bottom", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetPositionPercent(yoga_node, YGEdgeBottom, value.value);
        } else {
            YGNodeStyleSetPosition(yoga_node, YGEdgeBottom, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetPosition(yoga_node, YGEdgeBottom, YGUndefined);
    });

    assign_prop<Size>(style, "marginTop", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetMarginPercent(yoga_node, YGEdgeTop, value.value);
        } else {
            YGNodeStyleSetMargin(yoga_node, YGEdgeTop, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetMargin(yoga_node, YGEdgeTop, YGUndefined);
    });

    assign_prop<Size>(style, "marginLeft", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetMarginPercent(yoga_node, YGEdgeLeft, value.value);
        } else {
            YGNodeStyleSetMargin(yoga_node, YGEdgeLeft, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetMargin(yoga_node, YGEdgeLeft, YGUndefined);
    });

    assign_prop<Size>(style, "marginRight", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetMarginPercent(yoga_node, YGEdgeRight, value.value);
        } else {
            YGNodeStyleSetMargin(yoga_node, YGEdgeRight, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetMargin(yoga_node, YGEdgeRight, YGUndefined);
    });

    assign_prop<Size>(style, "marginBottom", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetMarginPercent(yoga_node, YGEdgeBottom, value.value);
        } else {
            YGNodeStyleSetMargin(yoga_node, YGEdgeBottom, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetMargin(yoga_node, YGEdgeBottom, YGUndefined);
    });

//        assign_prop<Size>(style, "margin", as_size, [yoga_node](auto value) {
//            if (value.is_percentage) {
//                YGNodeStyleSetMarginPercent(yoga_node, YGEdgeAll, value.value);
//            } else {
//                YGNodeStyleSetMargin(yoga_node, YGEdgeAll, value.value);
//            }
//        }, [yoga_node]() {
//            YGNodeStyleSetMargin(yoga_node, YGEdgeAll, YGUndefined);
//        });

    assign_prop<Size>(style, "paddingTop", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetPaddingPercent(yoga_node, YGEdgeTop, value.value);
        } else {
            YGNodeStyleSetPadding(yoga_node, YGEdgeTop, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetPadding(yoga_node, YGEdgeTop, YGUndefined);
    });

    assign_prop<Size>(style, "paddingLeft", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetPaddingPercent(yoga_node, YGEdgeLeft, value.value);
        } else {
            YGNodeStyleSetPadding(yoga_node, YGEdgeLeft, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetPadding(yoga_node, YGEdgeLeft, YGUndefined);
    });

    assign_prop<Size>(style, "paddingRight", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetPaddingPercent(yoga_node, YGEdgeRight, value.value);
        } else {
            YGNodeStyleSetPadding(yoga_node, YGEdgeRight, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetPadding(yoga_node, YGEdgeRight, YGUndefined);
    });

    assign_prop<Size>(style, "paddingBottom", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetPaddingPercent(yoga_node, YGEdgeBottom, value.value);
        } else {
            YGNodeStyleSetPadding(yoga_node, YGEdgeBottom, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetPadding(yoga_node, YGEdgeBottom, YGUndefined);
    });

//        assign_prop<Size>(style, "padding", as_size, [yoga_node](auto value) {
//            if (value.is_percentage) {
//                YGNodeStyleSetPaddingPercent(yoga_node, YGEdgeAll, value.value);
//            } else {
//                YGNodeStyleSetMargin(yoga_node, YGEdgeAll, value.value);
//            }
//        }, [yoga_node]() {
//            YGNodeStyleSetPadding(yoga_node, YGEdgeAll, YGUndefined);
//        });

    // TODO: Border is not supported

    assign_prop<float>(style, "aspectRatio", as_float, [yoga_node](auto value) {
        YGNodeStyleSetAspectRatio(yoga_node, value);
    }, [yoga_node]() {
        YGNodeStyleSetAspectRatio(yoga_node, YGUndefined);
    });

    assign_prop<Size>(style, "width", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetWidthPercent(yoga_node, value.value);
        } else {
            YGNodeStyleSetWidth(yoga_node, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetWidthAuto(yoga_node);
    });

    assign_prop<Size>(style, "height", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetHeightPercent(yoga_node, value.value);
        } else {
            YGNodeStyleSetHeight(yoga_node, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetHeightAuto(yoga_node);
    });

    assign_prop<Size>(style, "minWidth", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetMinWidthPercent(yoga_node, value.value);
        } else {
            YGNodeStyleSetMinWidth(yoga_node, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetMinWidth(yoga_node, YGUndefined);
    });

    assign_prop<Size>(style, "maxWidth", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetMaxWidthPercent(yoga_node, value.value);
        } else {
            YGNodeStyleSetMaxWidth(yoga_node, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetMaxWidth(yoga_node, YGUndefined);
    });

    assign_prop<Size>(style, "minHeight", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetMinHeightPercent(yoga_node, value.value);
        } else {
            YGNodeStyleSetMinHeight(yoga_node, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetMinHeight(yoga_node, YGUndefined);
    });

    assign_prop<Size>(style, "maxHeight", as_size, [yoga_node](auto value) {
        if (value.is_percentage) {
            YGNodeStyleSetMaxHeightPercent(yoga_node, value.value);
        } else {
            YGNodeStyleSetMaxHeight(yoga_node, value.value);
        }
    }, [yoga_node]() {
        YGNodeStyleSetMaxHeight(yoga_node, YGUndefined);
    });
}
