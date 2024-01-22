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
	IDLE,
	TEST,
	INDEXING,
};

using CommandArguments = std::vector<std::string>;

class Command {
public:
	template <typename... Args>
	Command(CommandType type, Args &&...args) : type(type), params{ std::forward<Args>(args)... } {}
	std::pair<CommandType, CommandArguments> get() {
		return { type, params };
	}

private:
	CommandType type;
	CommandArguments params;
};

class Parser {
public:
	static Command parse(const std::string message) {
		auto commandTokens = tokenizeString(message);
		switch (commandTokens.size()) {
		case 0:
			break;
		case 1:
			if (commandTokens.at(0) == "test") {
				return { CommandType::TEST };
			}
			break;
		case 2:
			break;
		case 3:
			break;
		}
		
		return { CommandType::IDLE };
	}
	
};