#pragma once
#include <iostream>
#include <boost/asio.hpp>

class Server {
public:
    using tcp = boost::asio::ip::tcp;

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

    void send_(boost::asio::ip::tcp::socket& socket, const std::string& message) {
        const std::string msg = message + "\n";
        boost::asio::write(socket, boost::asio::buffer(message));
    }
private:
    boost::asio::io_context io_context;
    tcp::acceptor acceptor_ = tcp::acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket_ = tcp::socket(io_context);

};