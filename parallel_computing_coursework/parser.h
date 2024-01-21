#pragma once
#include <string>
#include <vector>
#include <sstream>

std::vector<std::string> tokenizeString(const std::string& inputString) {
	std::istringstream stream(inputString);
	std::vector<std::string> tokens{ std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>() };

	return tokens;
}

enum class CommandType {
	INDEX,
};

class Parser {
public:
	static CommandType parse(const std::string message) {
		auto commandTokens = tokenizeString(message);
		switch (commandTokens.size()) {
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		}
	}
	
};