#include "inverted_index.h"

InvertedIndex::InvertedIndex(std::string mpath) : path(mpath) {
	if (!std::filesystem::exists(path) && !std::filesystem::is_directory(path)) {
		std::cout << "Error: directory does not exist.\n";
		return;
	}
	std::vector<std::filesystem::directory_entry> entries;
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		entries.push_back(entry);
	}
	std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
		return a.path().filename().string() < b.path().filename().string();
		});
	for (const auto& entry : entries) {
		if (!std::filesystem::is_regular_file(entry)) {
			std::cout << "Error!\n";
		}
		//std::cout << "File: " << entry.path().filename().string() << std::endl;
		std::filesystem::path filePath = entry.path();
		std::fstream file(filePath.string());
		if (!file.is_open()) {
			std::cout << "Error: Can't open file.\n";
		}
		std::string line;
		while (std::getline(file, line)) {
			// to lower
			std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c) { return std::tolower(c); });
			// delete punctuation
			try {
				// Set the locale to a known, standard locale
				std::locale::global(std::locale("en_US.UTF-8"));

				// Remove punctuation characters
				line.erase(std::remove_if(line.begin(), line.end(), [](char c) {
					// Check if c is a punctuation character
					return std::ispunct(static_cast<unsigned char>(c));
					}), line.end());

				// Print the modified line
				//std::cout << "Modified line: " << line << std::endl;
			}
			catch (const std::exception& e) {
				std::cerr << "Exception caught: " << e.what() << std::endl;
			}
			/*try {
				line.erase(std::remove_if(line.begin(), line.end(), [](char c) { return std::ispunct(c); }), line.end());
			}
			catch (...) {
				std::cout << "Error deleting punctuation!\n";
			}*/
			
			// tokenizing
			std::istringstream stream(line);
			std::vector<std::string> tokens{ std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>() };
			for (const auto& token : tokens) {
				auto it = hashMap.find(token);
				if (it == hashMap.end()) {
					// Ключ не найден, можно вставить новую пару
					hashMap[token].push_back(entry.path().filename().string());
					//std::cout << "Inserted: Key=" << token << ", Value=" << path.string() << std::endl;
				}
				//std::cout << token << std::endl;
				//hashMap[token].push_back(path.string());
			}
		}
	}
	
	
}

void InvertedIndex::show() {
	for (const auto& words : hashMap) {
		auto [key, value] = words;
		std::cout << key << ":\n";
		for (const auto& word : value) {
			std::cout << word << std::endl;
		}
		std::cout << std::endl;
	}
}