#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <nlohmann/json.hpp>
#include "Base64.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

class FullDuplexServer {
public:
    FullDuplexServer(boost::asio::io_context& io_context, unsigned short port, bool verbose)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), verbose_(verbose) {
        startAccept();
    }

    void log(const std::string& message) {
        if (verbose_) {
            std::cout << "[DEBUG] " << message << std::endl;
        }
    }

private:
    tcp::acceptor acceptor_;
    bool verbose_;
    std::unordered_map<std::string, std::shared_ptr<websocket::stream<tcp::socket>>> clients_;
    std::mutex clients_mutex_;

    void startAccept() {
        auto socket = std::make_shared<tcp::socket>(acceptor_.get_executor());
        acceptor_.async_accept(*socket, [this, socket](boost::system::error_code ec) {
            if (!ec) {
                auto ws = std::make_shared<websocket::stream<tcp::socket>>(std::move(*socket));
                handleConnection(ws);
            }
            startAccept();
        });
    }

    void handleConnection(const std::shared_ptr<websocket::stream<tcp::socket>>& ws) {
        ws->async_accept([this, ws](boost::system::error_code ec) {
            if (!ec) {
                std::string client_id = ws->next_layer().remote_endpoint().address().to_string();
                {
                    std::lock_guard<std::mutex> lock(clients_mutex_);
                    clients_[client_id] = ws;
                }
                log("Connection accepted from: " + client_id);
                startReadLoop(ws, client_id);
            }
        });
    }

    void startReadLoop(const std::shared_ptr<websocket::stream<tcp::socket>>& ws, const std::string& client_id) {
        auto buffer = std::make_shared<beast::flat_buffer>();
        ws->async_read(*buffer, [this, ws, buffer, client_id](boost::system::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                std::string received_message = beast::buffers_to_string(buffer->data());
                log("Message from " + client_id + ": " + received_message);

                // Echo the message back to the client (full-duplex example)
                sendMessage(ws, "Echo: " + received_message);

                // Continue reading
                startReadLoop(ws, client_id);
            } else {
                log("Connection closed: " + client_id);
                std::lock_guard<std::mutex> lock(clients_mutex_);
                clients_.erase(client_id);
            }
        });
    }

    void sendMessage(const std::shared_ptr<websocket::stream<tcp::socket>>& ws, const std::string& message) {
        auto send_buffer = std::make_shared<std::string>(message);
        ws->async_write(boost::asio::buffer(*send_buffer), [this, send_buffer](boost::system::error_code ec, std::size_t) {
            if (ec) {
                log("Error sending message: " + ec.message());
            }
        });
    }
};

int main(int argc, char* argv[]) {
    try {
        bool verbose = false;
        unsigned short port = 1111;

        // Parse command-line arguments
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "--verbose") {
                verbose = true;
            } else if (std::string(argv[i]) == "--port" && i + 1 < argc) {
                port = static_cast<unsigned short>(std::stoi(argv[++i]));
            }
        }

        boost::asio::io_context io_context;
        FullDuplexServer server(io_context, port, verbose);
        std::cout << "Full-duplex server running on port " << port << (verbose ? " [VERBOSE MODE]" : "") << std::endl;
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
