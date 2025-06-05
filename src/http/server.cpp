#include "server.h"
#include <boost/beast.hpp>
#include <memory>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;   

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket) : socket_(std::move(socket)) {
        std::cout << "[Session] New session started\n";
    }

    void start() {
        std::cout << "[Session] Starting session\n";
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        std::cout << "[Session] Waiting to read request\n";
        http::async_read(socket_, buffer_, request_,
            [this, self](beast::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    std::cout << "[Session] Request received (" << bytes_transferred << " bytes)\n";
                    handleRequest();
                } else {
                    std::cerr << "[Session] Read error: " << ec.message() << "\n";
                }
            });
    }

    void handleRequest() {
        std::cout << "[Session] Handling HTTP request\n";
        // Create a simple "Hello, World!" response
        http::response<http::string_body> res{
            http::status::ok, request_.version()};
        res.set(http::field::server, "Beast");
        res.set(http::field::content_type, "text/plain");
        res.body() = "Hello, World!";
        res.prepare_payload();

        auto self(shared_from_this());
        http::async_write(socket_, res,
            [this, self](beast::error_code ec, std::size_t) {
                if (ec) {
                    std::cerr << "[Session] Write error: " << ec.message() << "\n";
                }
                // Gracefully close the socket after response
                socket_.shutdown(tcp::socket::shutdown_send, ec);
            });
    }

    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> request_;
};


HttpServer::HttpServer(boost::asio::io_context& ioc, tcp::endpoint endpoint, const char* pipeline_description) 
    : acceptor_(ioc, endpoint) {
    std::cout << "[HttpServer] Server created, listening on " << endpoint << "\n";

    std::cout << "[HttpServer] Initializing GStreamer pipeline\n";
    gst_pipeline_ = std::make_unique<GstPipelineWrapper>(pipeline_description);
    std::cout << "[HttpServer] Starting GStreamer pipeline\n";
    gst_pipeline_->start();
    do_accept();
}

void HttpServer::start() {
    std::cout << "[HttpServer] Server started\n";
    // Implémentation à compléter
}

void HttpServer::stop() {
    std::cout << "[HttpServer] Server stopped\n";
    // Implémentation à compléter
}

void HttpServer::handleRequest(
    boost::beast::http::request<boost::beast::http::string_body>& req,
    boost::beast::http::response<boost::beast::http::string_body>& res){
    std::cout << "[HttpServer] Handling request: " << req.method_string() << " " << req.target() << "\n";
    // Implémentation à compléter
}

void HttpServer::do_accept() {
    std::cout << "[HttpServer] Waiting for new connection...\n";
    acceptor_.async_accept(
        [this](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "[HttpServer] Accepted new connection\n";
                std::make_shared<Session>(std::move(socket))->start();
            } else {
                std::cerr << "[HttpServer] Accept error: " << ec.message() << "\n";
            }
            do_accept();
        });
}
