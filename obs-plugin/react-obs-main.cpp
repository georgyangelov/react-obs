#include <obs-module.h>
#include <obs-frontend-api.h>
#include "react-obs-main.h"
#include <sockpp/tcp_acceptor.h>

#include <thread>
#include <vector>
#include <optional>
#include "generated/protocol.pb.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("react-obs", "en-US")

std::thread server_thread;
sockpp::tcp_acceptor server_acceptor;

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
