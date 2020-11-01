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

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("react-obs", "en-US")

#define DEBUG_LAYOUT false
#define DEBUG_UPDATE false

static pthread_mutex_t render_mutex = PTHREAD_MUTEX_INITIALIZER;

#include "api-server.hpp"
#include "yoga-logger.hpp"
#include "shadow-sources.hpp"
#include "props.hpp"
#include "layout.hpp"
#include "obs-adapter.hpp"
#include "experiments.hpp"

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

//
// Server API
//

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
// Plugin API
//

void initialize() {
    blog(LOG_INFO, "[react-obs] Initializing react-obs");

    YGConfigSetLogger(yoga_config, yoga_logger);

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
