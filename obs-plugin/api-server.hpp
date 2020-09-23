#include <cstdint>
#include <thread>
#include <functional>

#include <sockpp/tcp_acceptor.h>
#include "generated/protocol.pb.h"

enum class ReadPacketResult {
    Success,
    Error,
    Disconnected
};

class ClientConnection;
class ApiServer;

typedef std::function<void(ClientConnection&, const protocol::ClientMessage&)> MessageReceivedHandler;

class ClientConnection {
    sockpp::tcp_socket socket;
    MessageReceivedHandler message_handler;
    bool should_keep_running = true;

public:
    ClientConnection(
        sockpp::tcp_socket _socket,
        const MessageReceivedHandler& _message_handler
    )
        : socket(std::move(_socket)), message_handler(_message_handler) {}

    void send(const protocol::ServerMessage &message) {
        auto size = message.ByteSizeLong();
        std::vector<unsigned char> buffer(size);
        buffer.reserve(size);

        message.SerializeToArray(&buffer[0], size);

        uint32_t packet_size = htonl(size);
        socket.write_n(&packet_size, sizeof(uint32_t));

        auto written_size = socket.write_n(&buffer[0], size);

        if (written_size < size) {
            blog(LOG_ERROR, "[react-obs] Written size of message to socket is less than message size");
            return;
        }
    }

    // TODO: Disconnect this whenever the server stops
    void run() {
        std::vector<unsigned char> buffer;
        protocol::ClientMessage message;

        while (should_keep_running) {
            auto result = read_packet(message, socket, buffer);

            if (result != ReadPacketResult::Success) {
                break;
            }

            message_handler(*this, message);
        }

        blog(LOG_INFO, "[react-obs] Stopped client handler");
    }

    void disconnect() {
        should_keep_running = false;
    }

private:
    ReadPacketResult read_packet(protocol::ClientMessage &message, sockpp::tcp_socket &socket, std::vector<unsigned char> &buffer) {
        uint32_t packet_size;
        size_t read_bytes = socket.read_n(&packet_size, sizeof(uint32_t));

        packet_size = ntohl(packet_size);

        if (read_bytes <= 0) {
            return ReadPacketResult::Disconnected;
        }

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
};

class ApiServer {
    int16_t port;
    std::thread server_thread;
    sockpp::tcp_acceptor server_acceptor;

    MessageReceivedHandler message_handler;

public:
    ApiServer(int16_t _port, const MessageReceivedHandler& _message_handler)
        : port(_port), message_handler(_message_handler) {}

    void start() {
        std::thread thread(&ApiServer::run, this);

        server_thread.swap(thread);
    }

    void stop() {
        server_acceptor.close();
        server_thread.join();
    }

private:
    void run() {
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
            std::thread client_thread(&ApiServer::client_thread_runner, this, std::move(socket));
            client_thread.detach();
        }

        blog(LOG_INFO, "[react-obs] Stopped listening for connections");
    }

    void client_thread_runner(sockpp::tcp_socket socket) {
        ClientConnection client(std::move(socket), message_handler);

        client.run();
    }
};
