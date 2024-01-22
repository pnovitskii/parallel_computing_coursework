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

    while (1) {
        auto [commandType, params] = server.getCommand().get();
        
        switch (commandType) {
            case CommandType::IDLE: 
                break;
            case CommandType::TEST: {
                std::ostringstream ss;
                for (int i = 1; i <= 8; i *= 2) {
                    ss << "THREADS: " << std::setw(3) << i << " ";// << endl << endl;
                    indexing::InvertedIndex index;
                    ss << measure_time(&indexing::InvertedIndex::index, &index, path, i).str();
                }
                server.send(ss.str());
                break;
            }
            case CommandType::INDEXING: {
                int numThreads = 1; 
                if (!params.empty()) {
                    int threads = std::stoi(params.at(0));
                    numThreads = threads > 0 ? threads : 1;
                }
                std::cout << "Starting indexing! Threads: " << numThreads << std::endl;
                index.index(path, numThreads);
                
                server.send("Done!");
                std::cout << "Done!\n";
                break;
            }
            case CommandType::FIND: {
                std::ostringstream ss;
                auto result = index.find(params[0]);
                if (result.empty()) {
                    std::cout << "No such entry!\n";
                    break;
                }
                auto last = std::unique(result.begin(), result.end());
                std::size_t uniqueCount = std::distance(result.begin(), last);
                ss << "Word: " << params[0] << std::endl;
                ss << "Found " << result.size() << " entries in " << uniqueCount << " file" << (uniqueCount > 1 ? "s" : "") << ".\n";
                server.send(ss.str());
                break;
            }

        }
        
        //std::cout << "Unknown command: " << message << std::endl;
    }
}