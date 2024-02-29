#include "parser.h"

constexpr std::string_view testCommand = "test";
constexpr std::string_view indexCommand = "index";
constexpr std::string_view findCommand = "find";

std::vector<std::string> tokenizeString(const std::string& inputString) {
	std::istringstream stream(inputString);
	std::vector<std::string> tokens{ std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>() };

	return tokens;
}

Command Parser::parse(const std::string& message) {
	auto commandTokens = tokenizeString(message);
	switch (commandTokens.size()) {
		case 0:
			break;
		case 1:
			if (commandTokens.at(0) == testCommand) {
				return { CommandType::TEST };
			}
			if (commandTokens.at(0) == indexCommand) {
				return { CommandType::INDEXING };
			}
			break;
		case 2:
			if (commandTokens.at(0) == indexCommand) {
				try {
					int number = std::stoi(commandTokens.at(1));
				}
				catch (...) {
					break;
				}
				return { CommandType::INDEXING, commandTokens.at(1) };
			}
			if (commandTokens.at(0) == findCommand) {
				return { CommandType::FIND, commandTokens.at(1) };
			}
			break;
		case 3:
			break;
	}

		return { CommandType::IDLE };
}

std::pair<CommandType, CommandArguments> Command::get() {
	return { type, params };
}
