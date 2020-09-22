#include <obs-module.h>
#include <obs-frontend-api.h>
#include "react-obs-main.h"
#include <sockpp/tcp_acceptor.h>
#include <yoga/Yoga.h>

#include <thread>
#include <vector>
#include <optional>
#include <variant>
#include "generated/protocol.pb.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("react-obs", "en-US")

std::thread server_thread;
sockpp::tcp_acceptor server_acceptor;

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



class PropError: public std::runtime_error {
public:
    PropError(const std::string& what): std::runtime_error(what) {}
};

std::string as_string(const protocol::Prop prop) {
    if (prop.value_case() != protocol::Prop::ValueCase::kStringValue) {
        throw PropError(std::string("Prop ") + prop.key() + std::string(" must be a string"));
    }

    return prop.string_value();
}

int64_t as_int(const protocol::Prop prop) {
    if (prop.value_case() != protocol::Prop::ValueCase::kIntValue) {
        throw PropError(std::string("Prop ") + prop.key() + std::string(" must be an int"));
    }

    return prop.int_value();
}

//
// Element settings
//

template <class T>
std::unordered_map<std::string, protocol::Prop> as_prop_map(
    const T &props
) {
    std::unordered_map<std::string, protocol::Prop> result;

    for (int i = 0; i < props.size(); i++) {
        auto prop = props[i];

        result[prop.key()] = prop;
    }

    return result;
}

// TODO: Validate settings for particular source types?
void update_settings(
    obs_data_t* settings,
    protocol::ObjectValue object
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

void create_source(const protocol::CreateSource &create_source) {
    blog(LOG_DEBUG, "[react-obs] Creating source %s", create_source.name().c_str());

    auto settings = obs_data_create();
    update_settings(settings, create_source.settings());

    auto source = obs_source_create(create_source.id().c_str(), create_source.name().c_str(), settings, NULL);

    // TODO: Add source to internal map for tracking and GC

    obs_data_release(settings);
}

void append_child(const protocol::AppendChild &append_child) {
    blog(LOG_DEBUG, "[react-obs] Appending %s to %s",
         append_child.child_name().c_str(),
         append_child.parent_name().c_str());

//    auto current_scene = get_scene();
//    if (!current_scene) {
//        blog(LOG_ERROR, "[react-obs] Cannot find current scene");
//        return;
//    }

    auto parent = obs_get_source_by_name(append_child.parent_name().c_str());
    if (!parent) {
        blog(LOG_ERROR, "[react-obs] Cannot find parent source named %s", append_child.parent_name().c_str());
        return;
    }

    auto scene = obs_scene_from_source(parent);
    if (!scene) {
        blog(LOG_ERROR, "[react-obs] Parent source %s is not a scene", append_child.parent_name().c_str());
        return;
    }

    auto child_source = obs_get_source_by_name(append_child.child_name().c_str());
    if (!child_source) {
        blog(LOG_ERROR, "[react-obs] Cannot find child source named %s", append_child.child_name().c_str());
        return;
    }

    auto item = obs_scene_add(scene, child_source);
}

void update_source(const protocol::UpdateSource &update) {
    blog(LOG_DEBUG, "[react-obs] Updating source %s", update.name().c_str());

    auto source = obs_get_source_by_name(update.name().c_str());
    if (!source) {
        blog(LOG_ERROR, "[react-obs] Cannot find source named %s", update.name().c_str());
        return;
    }

    auto settings = obs_source_get_settings(source);
    if (!settings) {
        blog(LOG_ERROR, "[react-obs] Source %s does not have settings object, WTF", update.name().c_str());
        return;
    }

    update_settings(settings, update.changed_props());
    obs_source_update(source, settings);
}

bool enum_scene_item_remove_child(obs_scene_t* scene, obs_sceneitem_t* item, void* child_source_void) {
    auto child_source = (obs_source_t*)child_source_void;

    if (obs_sceneitem_get_source(item) == child_source) {
        obs_sceneitem_remove(item);
        return false;
    }

    return true;
}

void remove_child(const protocol::RemoveChild &remove) {
    blog(LOG_DEBUG, "[react-obs] Removing child %s", remove.child_name().c_str());

    auto parent = obs_get_source_by_name(remove.parent_name().c_str());
    if (!parent) {
        blog(LOG_ERROR, "[react-obs] Cannot find parent source named %s", remove.parent_name().c_str());
        return;
    }

    auto scene = obs_scene_from_source(parent);
    if (!scene) {
        blog(LOG_ERROR, "[react-obs] Parent source %s is not a scene", remove.parent_name().c_str());
        return;
    }

    auto child_source = obs_get_source_by_name(remove.child_name().c_str());
    if (!child_source) {
        blog(LOG_ERROR, "[react-obs] Cannot find child source named %s", remove.child_name().c_str());
        return;
    }

    obs_scene_enum_items(scene, &enum_scene_item_remove_child, (void*)child_source);

    // TODO: This is not exactly correct - this source may be a child of multiple scenes.
    obs_source_remove(child_source);

    obs_source_release(child_source);
}

void apply_updates(protocol::ApplyUpdate &update) {
    try {
        switch (update.change_case()) {
            case protocol::ApplyUpdate::ChangeCase::kCreateSource:
                create_source(update.create_source());
                break;

            case protocol::ApplyUpdate::ChangeCase::kUpdateSource:
                update_source(update.update_source());
                break;

            case protocol::ApplyUpdate::ChangeCase::kAppendChild:
                append_child(update.append_child());
                break;

            case protocol::ApplyUpdate::ChangeCase::kRemoveChild:
                remove_child(update.remove_child());
                break;

            case protocol::ApplyUpdate::ChangeCase::CHANGE_NOT_SET:
                blog(LOG_ERROR, "[react-obs] Received update request with no change");
                break;
        }
    } catch (PropError error) {
        blog(LOG_ERROR, "[react-obs] Prop error: %s", error.what());
    }
}

//
// Server API
//

enum class ReadPacketResult {
    Success,
    Error,
    Disconnected
};

ReadPacketResult read_packet(protocol::ClientMessage &message, sockpp::tcp_socket &socket, std::vector<unsigned char> &buffer) {
    uint32_t packet_size;
    size_t read_bytes = socket.read_n(&packet_size, sizeof(uint32_t));

    if (read_bytes <= 0) {
        return ReadPacketResult::Disconnected;
    }

    // TODO: Do we need to clear? Will this resize?
    // buffer.clear();
    buffer.reserve(packet_size);

    size_t read_size = 0;

    while (read_size < packet_size) {
        size_t read_now_size = socket.read_n(&buffer[0] + read_size, packet_size - read_size);
        read_size += read_now_size;

        if (read_now_size <= 0) {
            blog(LOG_DEBUG, "[react-obs] Client disconnected mid-packet");
            return ReadPacketResult::Disconnected;
        }
    }

    if (!message.ParseFromArray(&buffer[0], packet_size)) {
        blog(LOG_ERROR, "[react-obs] Could not parse protobuf message");
        return ReadPacketResult::Error;
    }

    return ReadPacketResult::Success;
}

// TODO: Disconnect this whenever the server stops
void client_thread_runner(sockpp::tcp_socket socket) {
    std::vector<unsigned char> buffer;
    protocol::ClientMessage message;
    bool keep_running = true;

    while (keep_running) {
        auto result = read_packet(message, socket, buffer);

        if (result != ReadPacketResult::Success) {
            break;
        }

        switch (message.message_case()) {
            case protocol::ClientMessage::MessageCase::kInitRequest: {
                auto init_request = message.init_request();

                blog(LOG_DEBUG, "[react-obs] Received init request from %s", init_request.client_id().c_str());
                break;
            }

            case protocol::ClientMessage::MessageCase::kApplyUpdate: {
                auto command = message.apply_update();

                blog(LOG_DEBUG, "[react-obs] Received update request: %s", command.DebugString().c_str());
                apply_updates(command);

                break;
            }

            case protocol::ClientMessage::MessageCase::MESSAGE_NOT_SET:
                blog(LOG_ERROR, "[react-obs] Empty message with no cases received");
                keep_running = false;
                break;

            default:
                blog(LOG_ERROR, "[react-obs] Unknown client message received");
                keep_running = false;
                break;
        }
    }

    blog(LOG_INFO, "[react-obs] Stopped client handler");
}

void start_server() {
    int16_t port = 6666;
    server_acceptor = sockpp::tcp_acceptor(port);

    if (!server_acceptor) {
        blog(LOG_ERROR, "[react-obs] Cannot create TCP acceptor");
        return;
    }

    blog(LOG_INFO, "[react-obs] Created TCP acceptor, listening on %i", port);

    while (true) {
        sockpp::inet_address peer;

        // Accept a new client connection
        sockpp::tcp_socket socket = server_acceptor.accept(&peer);

        if (!socket) {
            if (!server_acceptor.is_open()) {
                // Server stopped, probably due to `server_acceptor.close()`
                break;
            }

            blog(
                LOG_DEBUG,
                "[react-obs] Error accepting incoming connection: %s\n",
                server_acceptor.last_error_str().c_str()
            );
            continue;
        }

        blog(
            LOG_DEBUG,
            "[react-obs] Received a connection request from %s\n",
            peer.to_string().c_str()
        );

        // Create a thread and transfer the new stream to it.
        std::thread client_thread(client_thread_runner, std::move(socket));
        client_thread.detach();
    }

    blog(LOG_INFO, "[react-obs] Stopped listening for connections");
}

void initialize_server() {
    std::thread thread(start_server);

    server_thread.swap(thread);
}

void stop_server() {
    server_acceptor.close();
    server_thread.join();
}

//
// Test stuffs
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

void test_yoga() {
    auto config = YGConfigNew();
    YGConfigSetLogger(config, yoga_logger);

    auto root = YGNodeNew();
    YGNodeStyleSetWidth(root, 1920);
    YGNodeStyleSetHeight(root, 1080);
    YGNodeStyleSetAlignItems(root, YGAlignCenter);
    YGNodeStyleSetJustifyContent(root, YGJustifyCenter);

    auto child = YGNodeNew();
    YGNodeStyleSetWidth(child, 800);
    YGNodeStyleSetHeight(child, 600);

    YGNodeInsertChild(root, child, 0);

    YGNodeCalculateLayout(root, 1920, 1080, YGDirectionLTR);

    auto left = YGNodeLayoutGetLeft(child);
    auto top = YGNodeLayoutGetTop(child);

    auto width = YGNodeLayoutGetWidth(child);
    auto height = YGNodeLayoutGetHeight(child);

    blog(LOG_INFO, "[react-obs] Calculated child dimenstions: x = %f, y = %f, w = %f, h = %f",
         left,
         top,
         width,
         height);

    YGNodeFreeRecursive(root);
}

//
// Plugin API
//

void initialize() {
    blog(LOG_INFO, "[react-obs] Initializing react-obs");

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

    initialize_server();

    log_scene_names();

    test_yoga();
}

void shutdown() {
    stop_server();
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
