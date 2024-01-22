#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include "parser.h"

class Server {
public:
    using tcp = boost::asio::ip::tcp;

    void connect() {
        std::cout << "Waiting for client...";
        acceptor_.accept(socket_);
        std::cout << "connected!\n";
    }

    void send(const std::string& message) {
        const std::string msg = message + "\n";
        boost::asio::write(socket_, boost::asio::buffer(msg));
    }

    Command getCommand() {
        auto command = read();
        command.erase(command.size() - 1);
        auto parsedCommand = Parser::parse(command);
        return parsedCommand;
    }

private:
    boost::asio::io_context io_context;
    tcp::acceptor acceptor_ = tcp::acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket_ = tcp::socket(io_context);

    std::string read() {
        boost::asio::streambuf buf;
        boost::asio::read_until(socket_, buf, "\n");
        std::string data = boost::asio::buffer_cast<const char*>(buf.data());
        return data;
    }
};