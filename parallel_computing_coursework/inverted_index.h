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

#include "progress_bar.h"

namespace indexing {

class InvertedIndex {
public:
	InvertedIndex(const std::string& folderPath);
	void show();
	[[nodiscard]] std::vector<std::string> find(const std::string& mWord);

private:
	std::unordered_map<std::string, std::vector<std::string>> hashMap;
	std::filesystem::path path;
	std::mutex mutex;

	void processEntry(const std::filesystem::directory_entry& entry);
};

} // namespace

