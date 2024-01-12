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
#include "progress_bar.h"

class InvertedIndex {
public:
	InvertedIndex(std::string folderPath);
	void show();
	[[nodiscard]] std::vector<std::string> find(const std::string& mWord);
	void processEntry(const std::filesystem::directory_entry& entry);

private:
	std::unordered_map<std::string, std::vector<std::string>> hashMap;
	std::filesystem::path path;
};
