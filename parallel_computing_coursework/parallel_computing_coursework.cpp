#include <iostream>
#include "inverted_index.h"
#include "test.h"
#include "server.h"

inline constexpr auto path = "datasets";



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
            int numThreads = 2;
            std::cout << "THREADS: " << std::setw(3) << numThreads << " ";// << endl << endl;
            indexing::InvertedIndex index;
            measure_time(&indexing::InvertedIndex::index, &index, path, numThreads);
            continue;
        }
        std::cout << "Unknown command. Size: " << message.size() << std::endl;
        //write operation
        //send_(socket_, "Hello From Server!");
        //cout << "Servent sent Hello message to Client!" << endl;
    }
}