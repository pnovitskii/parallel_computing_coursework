#include <iostream>
#include <sstream>
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
        auto [commandType, params] = server.getCommand().get();
        
        //std::cout << message << std::endl;
        if (commandType == CommandType::TEST) {
            int numThreads = 2;
            
            std::ostringstream ss;
            for (int i = 1; i <= 8; i *= 2) {
                ss << "THREADS: " << std::setw(3) << i << " ";// << endl << endl;
                indexing::InvertedIndex index;
                ss << measure_time(&indexing::InvertedIndex::index, &index, path, i).str();
            }
            server.send(ss.str());
            continue;
        }
        //std::cout << "Unknown command: " << message << std::endl;
    }
}