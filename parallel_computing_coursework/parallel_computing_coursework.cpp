#include <iomanip>
#include "inverted_index.h"
//#include "server.h"
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;

inline constexpr auto path = "datasets";

std::string read_(tcp::socket& socket) {
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n");
    std::string data = boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}
void send_(tcp::socket& socket, const std::string& message) {
    const std::string msg = message + "\n";
    boost::asio::write(socket, boost::asio::buffer(message));
}

void testCase(int numThreads) {
    std::cout << "THREADS: " << std::setw(3) << numThreads << " ";// << endl << endl;
    auto start = std::chrono::high_resolution_clock::now();
    indexing::InvertedIndex index;
    index.index(path, numThreads);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    for (int j = 0; j < duration.count() / 200; j++) std::cout << "#";
    std::cout << " " << duration.count() / 1000 << "." << duration.count() % 1000 << " seconds\n";
}

void test() {
    for (int i = 1; i <= 16; i++)
        testCase(i);
    test();
    indexing::InvertedIndex index;
    index.index(path, 4);
    std::string word;
    std::cout << "Enter word: ";
    std::cin >> word;
    auto result = index.find(word);
    if (result.empty()) {
        std::cout << "No such entry!\n";
        return;
    }
    auto last = std::unique(result.begin(), result.end());
    std::size_t uniqueCount = std::distance(result.begin(), last);
    std::cout << "Word: " << word << std::endl;
    std::cout << "Found " << result.size() << " entries in " << uniqueCount << " file" << (uniqueCount > 1 ? "s" : "") << ".\n";
}

int main()
{
    srand(time(NULL));

    boost::asio::io_service io_service;
    //listen for new connection
    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 1234));
    //socket creation 
    tcp::socket socket_(io_service);
    //waiting for connection
    acceptor_.accept(socket_);

    while (1) {
        //read operation
        std::string message = read_(socket_);
        cout << message << endl;
        //write operation
        //send_(socket_, "Hello From Server!");
        //cout << "Servent sent Hello message to Client!" << endl;
    }


    //try {
    //    boost::asio::io_service io_service;

    //    // Start server on port 12345
    //    TCPServer server(io_service, 12345);

    //    // Run the IO service
    //    io_service.run();
    //}
    //catch (std::exception& e) {
    //    std::cerr << "Exception: " << e.what() << std::endl;
    //}
}