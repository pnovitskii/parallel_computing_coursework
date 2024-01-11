#include "inverted_index.h"

inline constexpr auto path = "neg";

int main()
{
    std::cout << std::filesystem::current_path().string() << std::endl;
    InvertedIndex index(path);
    index.show();
}

