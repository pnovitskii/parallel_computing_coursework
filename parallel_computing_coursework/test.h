#pragma once
#include <iostream>
#include <type_traits>
#include "inverted_index.h"

template <typename F, typename... Args>
auto measure_time(F func, Args&&... args) -> typename std::enable_if<!std::is_void<typename std::invoke_result<F, Args...>::type>::value, typename std::invoke_result<F, Args...>::type>::type {
    auto start = std::chrono::high_resolution_clock::now();

    auto result = std::invoke(func, std::forward<Args>(args)...);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    return result;
}

template <typename F, typename... Args>
auto measure_time(F func, Args&&... args) -> typename std::enable_if<std::is_void<typename std::invoke_result<F, Args...>::type>::value>::type
{
    auto start = std::chrono::high_resolution_clock::now();

    std::invoke(func, std::forward<Args>(args)...);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    for (int j = 0; j < duration.count() / 200; j++) std::cout << "#";
    std::cout << " " << duration.count() / 1000 << "." << duration.count() % 1000 << " seconds\n";
}

void testCase(const std::string& path, int numThreads) {
    std::cout << "THREADS: " << std::setw(3) << numThreads << " ";// << endl << endl;
    indexing::InvertedIndex index;
    measure_time(&indexing::InvertedIndex::index, &index, path, numThreads);
}

//void test() {
//    for (int i = 1; i <= 16; i++)
//        testCase(i);
//    test();
//    indexing::InvertedIndex index;
//    index.index(path, 4);
//    std::string word;
//    std::cout << "Enter word: ";
//    std::cin >> word;
//    auto result = index.find(word);
//    if (result.empty()) {
//        std::cout << "No such entry!\n";
//        return;
//    }
//    auto last = std::unique(result.begin(), result.end());
//    std::size_t uniqueCount = std::distance(result.begin(), last);
//    std::cout << "Word: " << word << std::endl;
//    std::cout << "Found " << result.size() << " entries in " << uniqueCount << " file" << (uniqueCount > 1 ? "s" : "") << ".\n";
//}