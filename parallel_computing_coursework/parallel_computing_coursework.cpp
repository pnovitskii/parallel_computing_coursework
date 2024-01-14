//#include <iomanip>
//#include "inverted_index.h"
//
//inline constexpr auto path = "neg";
//
//void testCase(int numThreads) {
//    std::cout << "THREADS: " << std::setw(3) << numThreads << " ";// << endl << endl;
//    auto start = std::chrono::high_resolution_clock::now();
//    indexing::InvertedIndex index(path, numThreads);
//    auto end = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//    for (int j = 0; j < duration.count() / 200; j++) std::cout << "#";
//    std::cout << "Time taken by code: " << duration.count() / 1000 << "." << duration.count() % 1000 << " seconds\n";
//}
//
//void test() {
//    for (int i = 1; i <= 16; i *= 2)
//        testCase(i);
//}
//
//int main()
//{
//    test();
//    indexing::InvertedIndex index(path, 4);
//    std::string word;
//    std::cout << "Enter word: ";
//    std::cin >> word;
//    auto result = index.find(word);
//    if (result.empty()) {
//        std::cout << "No such entry!\n";
//        return 0;
//    }
//    auto last = std::unique(result.begin(), result.end());
//    std::size_t uniqueCount = std::distance(result.begin(), last);
//    std::cout << "Word: " << word << std::endl;
//    std::cout << "Found " << result.size() << " entries in " << uniqueCount << " file" << (uniqueCount > 1 ? "s" : "") << ".\n";
//}
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <list>
using namespace std;
namespace fs = std::filesystem;

#include <memory>
#include <functional>
#include <mutex>
#include <thread>
#include <queue>

template <typename Key, typename Value>
class HashTable {
private:
    struct Node
    {
        Key key;
        Value value;
        Node(Key k, Value v)
            : key(k), value(v) {}
    };

    size_t m_size;
    vector<list<Node>> m_table;

    size_t HashFunctional(const Key& key) const
    {
        return hash<Key>()(key) % m_size;
    }

public:
    HashTable(size_t size = 5)
        : m_size(size)
    {
        m_table.resize(size);
    }

    void Insert(const Key& key, const Value& value) {
        size_t index = HashFunctional(key);
        m_table[index].push_back(Node(key, value));
    }

    Value& Search(const Key& key) {
        size_t index = HashFunctional(key);

        for (auto& x : m_table[index])
        {
            if (x.key == key)
            {
                return x.value;
            }
        }
        m_table[index].emplace_back(key, Value{});
        return m_table[index].back().value;
    }

    void PrintAll() const {
        for (auto x : m_table)
        {
            for (auto y : x)
            {
                cout << "Key: " << y.key << " Value: ";
                for (auto& file : y.value)
                {
                    cout << file << " ";
                }
                cout << endl;
            }
        }
    }

    void ShowValue(const Key& key) const {
        size_t index = HashFunctional(key);

        for (const auto& node : m_table[index])
        {
            if (node.key == key)
            {
                cout << "Key: " << key << " Value: ";
                for (const auto& file : node.value)
                {
                    cout << file << " ";
                }
                cout << endl;
                return;
            }
        }
        cout << "Key not found: " << key << endl;
    }
};

struct FileContent {
    string filename;
    vector<string> content;
};

class SimpleThreadPool {
public:
    explicit SimpleThreadPool(size_t threadCount) {
        for (size_t i = 0; i < threadCount; ++i) {
            threads.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });

                        if (this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
                });
        }
    }

    ~SimpleThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : threads)
            worker.join();
    }

    void Enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");
            tasks.push(std::move(task));
        }
        condition.notify_one();
    }

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop = false;
};

class InvertedIndex {
public:

    void IndexDirectory(const string& directoryPath, size_t numofstreams)
    {
        SimpleThreadPool pool(numofstreams);  // Створення пулу потоків з 4 потоками

        for (const auto& entry : fs::recursive_directory_iterator(directoryPath))
        {
            if (entry.is_regular_file())
            {
                // Додаємо завдання в пул потоків для обробки кожного файлу
                pool.Enqueue([this, entry] {
                    FileContent fileContent = { entry.path().filename().string(), ReadFileContent(entry.path()) };
                    AddFile(fileContent);
                    });
            }
        }
    }

    void ShowIndex() const {
        m_hashTable.PrintAll();
    }

    void ShowValue(const string& key) const {
        m_hashTable.ShowValue(key);
    }

private:
    std::mutex mutex;
    HashTable<string, vector<string>> m_hashTable;

    vector<string> ReadFileContent(const fs::path& filePath) {
        ifstream fileStream(filePath);
        vector<string> words;

        if (fileStream.is_open())
        {
            string word, processedWord;
            while (fileStream >> word)
            {
                processedWord.clear();
                for (char ch : word)
                {
                    unsigned char uch = static_cast<unsigned char>(ch);
                    if (isalpha(uch) || isdigit(uch) || ch == '\'')
                    {
                        processedWord.push_back(tolower(uch));
                    }
                    else if (!processedWord.empty())
                    {
                        words.push_back(processedWord);
                        processedWord.clear();
                    }
                }
                if (!processedWord.empty())
                {
                    words.push_back(processedWord);
                }
            }
            fileStream.close();
        }
        else
        {
            cout << "Unable to open file: " << filePath << endl;
        }

        return words;
    }

    void AddFile(const FileContent& fileContent) {
        for (const auto& word : fileContent.content)
        {
            string processedWord = ProcessContent(word);
            if (!processedWord.empty())
            {
                std::lock_guard<std::mutex> lock(this->mutex);  // Захист доступу до спільного ресурсу
                auto& fileList = m_hashTable.Search(processedWord);
                if (std::find(fileList.begin(), fileList.end(), fileContent.filename) == fileList.end())
                {
                    fileList.push_back(fileContent.filename);
                }
            }
        }
    }

    string ProcessContent(const string& content) const {
        string processed;
        processed.reserve(content.length()); // Reserve space for efficiency

        for (char ch : content)
        {
            unsigned char uch = static_cast<unsigned char>(ch);
            if (std::isalpha(uch) || std::isdigit(uch) || ch == '\'')
            {
                processed.push_back(std::tolower(uch)); // Convert to lowercase
            }
        }
        return processed;
    }
};

#include <chrono>

int main() {
    system("chcp 65001");

    //InvertedIndex index;
    //index.IndexDirectory("datasets", 10);
    //index.ShowIndex();
    //index.ShowValue("for");

    for (int threadCount = 1; threadCount <= 20; ++threadCount) { // Example: testing from 1 to 20 threads
        auto start = std::chrono::high_resolution_clock::now(); // Start timing

        InvertedIndex index;
        index.IndexDirectory("neg", threadCount);

        auto end = std::chrono::high_resolution_clock::now(); // End timing
        std::chrono::duration<double> diff = end - start; // Calculate duration

        std::cout << "Time taken with " << threadCount << " threads: "
            << diff.count() << " seconds" << std::endl;
    }
    return 0;
}
