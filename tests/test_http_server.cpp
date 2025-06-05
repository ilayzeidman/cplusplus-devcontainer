#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "../src/http/server.h"
#include "utils/pipeline_descriptions.hpp"


namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;

TEST(HttpServerTest, BasicGet) {
    // Lancer le serveur dans un thread séparé
    std::thread server_thread([]{
        boost::asio::io_context ioc;
        tcp::endpoint endpoint{tcp::v4(), 8081};
        HttpServer server(ioc, endpoint, LIVE_WINDOW_PIPELINE_DESC);
        std::cout << "[Test] Server started on port 8081" << std::endl;
        ioc.run();
    });

    // Attendre que le serveur démarre
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << "[Test] Server should be ready, proceeding with client request" << std::endl;

    // Effectuer une requête HTTP GET
    boost::asio::io_context ioc;
    tcp::resolver resolver(ioc);
    auto const results = resolver.resolve("127.0.0.1", "8081");
    std::cout << "[Test] Resolved server address" << std::endl;

    beast::tcp_stream stream(ioc);
    stream.connect(results);
    std::cout << "[Test] Connected to server" << std::endl;

    http::request<http::string_body> req{http::verb::get, "/", 11};
    req.set(http::field::host, "127.0.0.1");
    req.set(http::field::user_agent, "GTest");

    http::write(stream, req);
    std::cout << "[Test] Sent HTTP GET request" << std::endl;

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);
    std::cout << "[Test] Received HTTP response" << std::endl;

        // Vérifie la réponse
        EXPECT_EQ(res.result(), http::status::ok);
        EXPECT_NE(res.body().find("Hello"), std::string::npos);
        std::cout << "[Test] Response checks passed" << std::endl;

    // Fermer la connexion
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);
    std::cout << "[Test] Connection closed" << std::endl;

    // Arrêter le serveur (dans un vrai projet, prévoir un mécanisme d'arrêt propre)
    server_thread.detach(); // ou std::terminate() si besoin
    std::cout << "[Test] Server thread detached" << std::endl;
}