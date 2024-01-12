#include "inverted_index.h"

inline constexpr auto path = "neg";
inline constexpr auto word = "he";

int main()
{
    std::cout << std::filesystem::current_path().string() << std::endl;
    indexing::InvertedIndex index(path);
    //index.show();
    auto result = index.find(word);
    auto last = std::unique(result.begin(), result.end());
    std::size_t uniqueCount = std::distance(result.begin(), last);
    std::cout << "Word: " << word << std::endl;
    std::cout << "Found " << result.size() << " entries in " << uniqueCount << " file" << (uniqueCount > 1 ? "s" : "") << ".\n";
}
