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

//inline constexpr std::string_view path = "";

class InvertedIndex {
public:
	InvertedIndex(std::string mpath) : path(mpath) {
		if (!std::filesystem::exists(path)) {
			std::cout << "Error: file does not exist.\n";
			return;
		}
		std::fstream file(path);
		if (!file.is_open()) {
			std::cout << "Error: Can't open file.\n";
		}
		std::string line;
		while (std::getline(file, line)) {
			std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c) { return std::tolower(c); });
			line.erase(std::remove_if(line.begin(), line.end(), [](char c) { return std::ispunct(c); }), line.end());
			std::istringstream stream(line);
			std::vector<std::string> tokens{ std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>() };
			for (const auto& token : tokens) {
				hashMap[token].push_back(path.string());
			}
		}

	}
	void show() {
		for (const auto& words : hashMap) {
			auto [key, value] = words;
			std::cout << key << ":\n";
			for (const auto& word : value) {
				std::cout << word << std::endl;
			}
			std::cout << std::endl;
		}
	}
	std::unordered_map<std::string, std::vector<std::string>> hashMap;
	std::filesystem::path path;
};
