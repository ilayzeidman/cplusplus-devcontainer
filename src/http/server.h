#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "../gstreamer/gst_pipeline.hpp"
using tcp = boost::asio::ip::tcp;

class HttpServer {
public:
    HttpServer(boost::asio::io_context& io_context, boost::asio::ip::tcp::endpoint endpoint, const char* pipeline_description);

    void start();
    void stop();
    void do_accept();

private:    
    tcp::acceptor acceptor_;
    std::unique_ptr<GstPipelineWrapper> gst_pipeline_;

    void handleRequest(
        boost::beast::http::request<boost::beast::http::string_body>& req,
        boost::beast::http::response<boost::beast::http::string_body>& res);
};