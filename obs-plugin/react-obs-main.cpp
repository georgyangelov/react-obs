#include <obs-module.h>
#include <obs-frontend-api.h>
#include "react-obs-main.h"
#include <sockpp/tcp_acceptor.h>

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

//std::string string_prop(
//    const google::protobuf::Map<std::string, protocol::PropValue> props,
//    const std::string &name
//) {
//    std::string result;
//
//    if (!props.contains(name)) {
//        throw PropError(std::string("Props do not contain ") + name);
//    }
//
//    auto prop = props.at(name);
//
//    if (prop.value_case() != protocol::PropValue::ValueCase::kStringValue) {
//        throw PropError(std::string("Prop ") + name + std::string(" must be a string"));
//    }
//
//    return prop.string_value();
//}

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
    
    for (size_t i = 0; i < props.size(); i++) {
        auto prop = props[i];
        
        result[prop.key()] = prop;
    }
    
    return result;
}

template <class T>
void update_settings(
    obs_data_t* settings,
    protocol::ElementType elementType,
    T propVector
) {
    auto props = as_prop_map(propVector);
    
    switch (elementType) {
        case protocol::ElementType::TEXT: {
            auto children = props["children"];
            if (!children.undefined()) {
                auto text = as_string(children);
                obs_data_set_string(settings, "text", text.c_str());
            } else {
                obs_data_set_string(settings, "text", "");
            }
            
            auto font_data = obs_data_get_obj(settings, "font");
            if (!font_data) {
                font_data = obs_data_create();
                obs_data_set_obj(settings, "font", font_data);
            }
            
            obs_data_set_string(font_data, "face", "Arial");
            obs_data_set_string(font_data, "style", "Normal");
            
            auto font_size = props["fontSize"];
            if (!font_size.undefined()) {
                auto size = as_int(font_size);
                obs_data_set_int(font_data, "size", size);
            } else {
                obs_data_unset_user_value(font_data, "size");
            }
            
            obs_data_set_int(font_data, "flags", 0);
            
            break;
        }
        
        default:
            blog(LOG_ERROR, "[react-obs] Unsupported element type in update_settings");
    }
}

//
// Updates
//

void create_element(const protocol::CreateElement &element) {
    blog(LOG_DEBUG, "[react-obs] Creating element %s", element.name().c_str());
    
    if (element.type() == protocol::ElementType::TEXT) {
        auto props = element.props();
        
        auto settings = obs_data_create();
        update_settings(settings, element.type(), props);
        
        auto text_source = obs_source_create("text_ft2_source", element.name().c_str(), settings, NULL);
        
        // TODO: Should I release this?
        // obs_sceneitem_release(item);
        
        // obs_source_release(text_source);
        
        // TODO: Should I be calling obs_data_release on this?
        // obs_data_release(settings);
        
        return;
    }
    
    blog(LOG_ERROR, "[react-obs] Unsupported element type");
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

void update_element(const protocol::UpdateElement &update) {
    blog(LOG_DEBUG, "[react-obs] Updating element %s", update.name().c_str());
    
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
    
    update_settings(settings, update.type(), update.changed_props());
    obs_source_update(source, settings);
}

void apply_updates(protocol::ApplyUpdate &update) {
    try {
        switch (update.change_case()) {
            case protocol::ApplyUpdate::ChangeCase::kCreateElement:
                create_element(update.create_element());
                break;
            
            case protocol::ApplyUpdate::ChangeCase::kAppendChild:
                append_child(update.append_child());
                break;
                
            case protocol::ApplyUpdate::ChangeCase::kUpdateElement:
                update_element(update.update_element());
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
