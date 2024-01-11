#include "inverted_index.h"

bool checkDirectory(std::filesystem::path path) {
	return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

std::vector<std::filesystem::directory_entry> getEntries(const std::filesystem::path& path) {
	std::vector<std::filesystem::directory_entry> entries;
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		entries.push_back(entry);
	}
	return entries;
}

InvertedIndex::InvertedIndex(std::string folderPath) : path(folderPath) {
	if (!checkDirectory(path)) {
		std::cout << "Error: directory does not exist.\n";
		return;
	}
	//
	auto entries = getEntries(path);
	
	std::cout << "Files: " << entries.size() << std::endl;

	size_t totalFiles = entries.size();
	size_t processedFiles = 0;
	size_t updateInterval = totalFiles / 100;

	std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
		return a.path().filename().string() < b.path().filename().string();
		});
	for (const auto& entry : entries) {
		if (!std::filesystem::is_regular_file(entry)) {
			std::cout << "Error!\n";
		}
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
			}
			catch (const std::exception& e) {
				std::cerr << "Exception caught: " << e.what() << std::endl;
			}

			// tokenizing
			std::istringstream stream(line);
			std::vector<std::string> tokens{ std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>() };

			for (const auto& token : tokens) {
				auto it = hashMap.find(token);
				//if (it == hashMap.end()) {
					// Ключ не найден, можно вставить новую пару
					//hashMap[token].push_back(entry.path().filename().string());
					//std::cout << "Inserted: Key=" << token << ", Value=" << path.string() << std::endl;
				//}
				hashMap[token].push_back(entry.path().filename().string());
				//std::cout << token << std::endl;
				//hashMap[token].push_back(path.string());
			}

			++processedFiles;

			// Update progress every updateInterval files
			if (processedFiles % updateInterval == 0 || processedFiles == totalFiles) {
				float progress = static_cast<float>(processedFiles) / totalFiles * 100.0;
				std::cout << "Progress: " << progress << "% (" << processedFiles << "/" << totalFiles << " files)\n";
			}
		}
		//std::cout << ".";
	}
	//std::cout << "\n";
	
	
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

void InvertedIndex::find(const std::string& mWord) {
	auto result = hashMap.find(mWord);
	if (result == hashMap.end()) {
		std::cout << "No such entry!\n";
		return;
	}
	auto [word, files] = *result;
	std::sort(files.begin(), files.end());
	auto last = std::unique(files.begin(), files.end());
	std::size_t uniqueCount = std::distance(files.begin(), last);
	std::cout << "Word: " << word << std::endl;
	std::cout << "Found " << files.size() << " entries in " << uniqueCount << " file(s)<<.\n";
	
	//for (const auto& file : files) {
		//std::cout << file << std::endl;
	//}
}
