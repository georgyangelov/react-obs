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
