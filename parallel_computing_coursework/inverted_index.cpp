#include "inverted_index.h"

using namespace indexing;

[[nodiscard]] bool checkDirectory(const std::filesystem::path& path) {
	return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

[[nodiscard]] std::vector<std::filesystem::directory_entry> getEntries(const std::filesystem::path& path) {
	std::vector<std::filesystem::directory_entry> entries;
	for (auto entry : std::filesystem::directory_iterator(path)) {
		if (!entry.is_directory()) {
			entries.push_back(entry);
			continue;
		}
		auto subEntries = getEntries(entry);
		entries.insert(entries.end(), subEntries.begin(), subEntries.end());
	}
	return entries;
}

void InvertedIndex::index(const std::string& folderPath, int numThreads) {
	if (!checkDirectory(folderPath)) {
		std::cout << "Error: directory does not exist.\n";
		return;
	}
	
	auto entries = getEntries(folderPath);
	
	//std::cout << "\nFiles: " << entries.size() << std::endl;

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

	

#ifdef THREAD_POOL

	ThreadPool threadPool(numThreads);

	for (auto& chunk : chunks) {
		for (auto& entry : chunk) {
			threadPool.addTask(Task([this, &entry]() { processEntry(entry); }));
		}
	}
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		threadPool.lock();
		if (threadPool.empty()) {
			break;
		}
	}

#else
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
#endif
}

void InvertedIndex::processEntry(const std::filesystem::directory_entry& entry) {
	
	if (!std::filesystem::is_regular_file(entry)) {
		std::cout << "Error!(1)\n";
		return;
		//throw std::exception();
	}
	
	std::filesystem::path filePath;
	try {
		filePath = entry.path();
		// ������� ���-�� � filePath
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	//std::filesystem::path filePath = entry.path();
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
			//auto it = hashMap.find(token);
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
