#pragma once
#include <iostream>
#include <type_traits>
#include <sstream>
#include "inverted_index.h"

template <typename F, typename... Args>
auto measure_time(F func, Args&&... args) -> std::stringstream
{
    std::stringstream output;
    auto start = std::chrono::high_resolution_clock::now();

    std::invoke(func, std::forward<Args>(args)...);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    for (int j = 0; j < duration.count() / 200; j++) {
        output << "#";
    }
    output << " " << duration.count() / 1000 << "." << duration.count() % 1000 << " seconds\n";

    return output;
}

//void testCase(const std::string& path, int numThreads) {
//    std::cout << "THREADS: " << std::setw(3) << numThreads << " ";// << endl << endl;
//    indexing::InvertedIndex index;
//    measure_time(&indexing::InvertedIndex::index, &index, path, numThreads);
//}
