#include "server.h"

void Server::connect() {
    std::cout << "Waiting for client...";
    acceptor_.accept(socket_);
    std::cout << "connected!\n";
}

void Server::send(const std::string& message) {
    if (!socket_.is_open()) {
        connect();
    }
    const std::string msg = message + "\n";
    boost::asio::write(socket_, boost::asio::buffer(msg));
}

Command Server::getCommand() {
    if (!socket_.is_open()) {
        connect();
    }
    std::string command = boost::asio::buffer_cast<const char*>(buf.data());;
    try {
        command = read();
        command.erase(command.size() - 1);
        std::cout << command << std::endl;
    }
    catch (...) {}

    auto parsedCommand = Parser::parse(command);
    return parsedCommand;
}

std::string Server::read() {
    boost::asio::streambuf buf;
    boost::asio::read_until(socket_, buf, "\n");
    std::string data = boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}
