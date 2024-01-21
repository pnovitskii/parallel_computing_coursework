#pragma once
#include <iostream>
#include "inverted_index.h"

void testCase(int numThreads, const std::string& path) {
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