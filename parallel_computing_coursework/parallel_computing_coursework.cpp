#include <iostream>
#include <boost/asio.hpp>
#include "inverted_index.h"
#include "test.h"
//#include "server.h"

using namespace boost::asio;
using ip::tcp;

inline constexpr auto path = "datasets";

class Server {
public:
    void connect() {
        std::cout << "Waiting for client...";
        acceptor_.accept(socket_);
        std::cout << "connected!\n";
    }
    std::string read() {
        boost::asio::streambuf buf;
        boost::asio::read_until(socket_, buf, "\n");
        std::string data = boost::asio::buffer_cast<const char*>(buf.data());
        return data;
    }

    void send_(tcp::socket& socket, const std::string& message) {
        const std::string msg = message + "\n";
        boost::asio::write(socket, boost::asio::buffer(message));
    }
private:
    boost::asio::io_context io_context;
    tcp::acceptor acceptor_ = tcp::acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket_ = tcp::socket(io_context);
    
};

int main()
{
    srand(time(NULL));
    indexing::InvertedIndex index;
    Server server;
    server.connect();
    while (1) {
        //read operation
        std::string message = server.read();
        message.erase(message.size() - 1);
        std::cout << message << std::endl;
        if (message == "test") {
            //index.index(path, 2);
            testCase(2, path);
            continue;
        }
        std::cout << "Unknown command. Size: " << message.size() << std::endl;
        //write operation
        //send_(socket_, "Hello From Server!");
        //cout << "Servent sent Hello message to Client!" << endl;
    }
}