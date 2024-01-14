#include "inverted_index.h"

using namespace indexing;

[[nodiscard]] bool checkDirectory(const std::filesystem::path& path) {
	return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

[[nodiscard]] std::vector<std::filesystem::directory_entry> getEntries(const std::filesystem::path& path) {
	std::vector<std::filesystem::directory_entry> entries;
	for (auto entry : std::filesystem::directory_iterator(path)) {
		entries.push_back(entry);
	}
	return entries;
}

InvertedIndex::InvertedIndex(const std::string& folderPath, int numThreads) : path(folderPath), numThreads(numThreads) {
	if (!checkDirectory(path)) {
		std::cout << "Error: directory does not exist.\n";
		return;
	}
	getEntries(path);
	auto entries = getEntries(path);
	
	std::cout << "Files: " << entries.size() << std::endl;

	//ProgressBar progressBar(entries.size(), 50);

	std::sort(std::execution::par, entries.begin(), entries.end(), [](const auto& a, const auto& b) {
		return a.path().filename().string() < b.path().filename().string();
		});
	
	size_t chunkSize = entries.size() / numThreads;
	std::vector<std::vector<std::filesystem::directory_entry>> chunks;
	auto start = entries.begin();
	while (start != entries.end()) {
		auto end = std::next(start, std::min(chunkSize, static_cast<size_t>(std::distance(start, entries.end()))));
		chunks.emplace_back(start, end);
		start = end;
	}
	std::vector<std::jthread> threads;
	for (int i = 0; i < numThreads; i++) {
		threads.emplace_back([this, &chunks, i]() {
			for (const auto& entry : chunks[i]) {
				processEntry(entry);
				//progressBar.update();
			}
			std::cout << i << " is done!\n"; 
		});
			
	}
	/*for (auto x : threads) {
		x.join();
	}*/
}

void InvertedIndex::processEntry(const std::filesystem::directory_entry& entry) {
	
	if (!std::filesystem::is_regular_file(entry)) {
		std::cout << "Error!\n";
		return;
	}
	std::filesystem::path filePath = entry.path();
	std::fstream file(filePath.string());
	if (!file.is_open()) {
		std::cout << "Error: Can't open file.\n";
		return;
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
			std::lock_guard<std::mutex> lock(mutex);
			auto it = hashMap.find(token);
			
			hashMap[token].push_back(entry.path().filename().string());
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

std::vector<std::string> InvertedIndex::find(const std::string& mWord) {
	auto result = hashMap.find(mWord);
	if (result == hashMap.end()) {
		return {};
	}
	auto [word, files] = *result;
	std::sort(std::execution::par, files.begin(), files.end());
	return files;
}
