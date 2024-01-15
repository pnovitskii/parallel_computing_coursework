#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <cctype>
#include <execution>
#include <thread>
#include <mutex>
#include <algorithm>
#include <chrono>

#include "progress_bar.h"
#include "thread_pool.h"

#define THREAD_POOL

namespace indexing {

class InvertedIndex {
public:
	InvertedIndex(const std::string& folderPath, int numThreads = 1);
	void show();
	[[nodiscard]] std::vector<std::string> find(const std::string& mWord);

private:
	std::unordered_map<std::string, std::vector<std::string>> hashMap;
	std::filesystem::path path;
	std::mutex mutex;
	int numThreads;

#ifdef THREAD_POOL
	ThreadPool threadPool;
#endif

	void processEntry(const std::filesystem::directory_entry& entry);
};

} // namespace

