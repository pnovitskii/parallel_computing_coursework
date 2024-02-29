#pragma once
#include <string>
#include <vector>
#include <sstream>

enum class CommandType {
	IDLE,
	TEST,
	INDEXING,
	FIND,
};

using CommandArguments = std::vector<std::string>;

class Command {
public:
	template <typename... Args>
	Command(CommandType type, Args &&...args) : type(type), params{ std::forward<Args>(args)... } {}
	std::pair<CommandType, CommandArguments> get();

private:
	CommandType type;
	CommandArguments params;
};

class Parser {
public:
	static Command parse(const std::string& message);
};
