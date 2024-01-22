#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include "parser.h"

class Server {
public:
    using tcp = boost::asio::ip::tcp;

    void connect();
    void send(const std::string& message);
    Command getCommand();

private:
    boost::asio::io_context io_context;
    tcp::acceptor acceptor_ = tcp::acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
    tcp::socket socket_ = tcp::socket(io_context);

    std::string read();
};