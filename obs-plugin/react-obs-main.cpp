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
#include <sstream>

#include "api-server.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("react-obs", "en-US")

#define DEBUG_LAYOUT false
#define DEBUG_UPDATE false

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
    UNUSED_PARAMETER(config);
    UNUSED_PARAMETER(node);

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
    UNUSED_PARAMETER(widthMode);
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

std::optional<float> as_float(const protocol::Prop &prop) {
    if (prop.value_case() == protocol::Prop::ValueCase::kIntValue) {
        return (float)prop.int_value();
    }

    if (prop.value_case() == protocol::Prop::ValueCase::kFloatValue) {
        return prop.float_value();
    }

    return {};
}

struct Size {
    bool is_percentage;
    float value;
};

std::optional<Size> as_size(const protocol::Prop &prop) {
    switch (prop.value_case()) {
        case protocol::Prop::ValueCase::kIntValue:
            return Size {
                .is_percentage = false,
                .value = (float)prop.int_value()
            };

        case protocol::Prop::ValueCase::kFloatValue:
            return Size {
                .is_percentage = false,
                .value = (float)prop.float_value()
            };

        case protocol::Prop::ValueCase::kStringValue: {
            float value;
            std::string unit;

            std::istringstream parser(prop.string_value());

            parser >> value >> unit;

            if (parser.fail() || parser.bad()) {
                return {};
            }

            if (unit == "px") {
                return Size {
                    .is_percentage = false,
                    .value = value
                };
            } else if (unit == "%") {
                return Size {
                    .is_percentage = true,
                    .value = value
                };
            } else {
                return {};
            }
        }

        default: return {};
    }
}

std::optional<std::unordered_map<std::string, protocol::Prop>> as_object(const protocol::Prop &prop) {
    if (prop.value_case() != protocol::Prop::ValueCase::kObjectValue) {
        return {};
    }

    return as_prop_map(prop.object_value().props());
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

template <class T>
void assign_prop_default(
    const PropMap &props,
    const char* name,
    std::function<std::optional<T>(const protocol::Prop &prop)> converter,
    T default_value,
    std::function<void(const T&)> apply
) {
    auto propFind = props.find(name);
    if (propFind == props.end()) {
        apply(default_value);
        return;
    }

    auto prop = propFind->second;
    auto value = converter(prop);

    if (value.has_value()) {
        apply(*value);
    } else {
        blog(LOG_ERROR, "[react-obs] Property %s is of the wrong type, was %i", name, prop.value_case());
    }
}

//
// Update functions
//

//void update_intrinsic_dimensions(ShadowSource* shadow) {
//    YGNodeStyleSetWidth(shadow->yoga_node, obs_source_get_width(shadow->source));
//    YGNodeStyleSetHeight(shadow->yoga_node, obs_source_get_height(shadow->source));
//}

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

    pthread_mutex_lock(&render_mutex);
    pthread_mutex_lock(&shadow_sources_mutex);

    for (auto shadow : shadow_sources) {
        if (shadow->measured_externally) {
            check_source_dimensions(shadow);
        }
    }

    for (auto container : containers) {
        perform_layout(container);
    }

    pthread_mutex_unlock(&shadow_sources_mutex);
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

            if (DEBUG_UPDATE) {
                blog(LOG_DEBUG, "[react-obs] Received update request: %s", command.DebugString().c_str());
            }

            pthread_mutex_lock(&render_mutex);
            apply_updates(command);
            pthread_mutex_unlock(&render_mutex);

            break;
        }

        case protocol::ClientMessage::MessageCase::kFindSource: {
            auto command = message.find_source();

            if (DEBUG_UPDATE) {
                blog(LOG_DEBUG, "[react-obs] Received find source request for name %s", command.name().c_str());
            }

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
