#include "../common/pch.h"
#include <iostream>
#include <cstdint>
#include <string>
#include <algorithm>
#include <numeric>
#include <utility>
#include <sstream>
#include <vector>
#include <map>
#include <set>

enum Keyword {
	INP,
	ADD,
	MUL,
	DIV,
	MOD,
	EQL
};

static const std::vector<bool> positiveTurns{
	true, true, true,
	false, true, false, false,
	true, true ,true,
	false, false, false, false
};

struct ValOrVar {
	int64_t val{ 0 };
	char var{ '\0' };

	bool isVal() const {
		return var == '\0';
	}
};

struct Instruction {
	Keyword keyword;
	char variable;
	ValOrVar valOrVar;
};

using DataType = std::vector<Instruction>;

class Decoder {
private:
	const DataType& instructions;
	std::istringstream input;
	std::map<char, int64_t> registry;
	int maxTurns;

public:
	Decoder(const DataType& instructions, const std::string& input) : instructions(instructions), input(input), maxTurns(input.size()){}

	bool run() {
		int64_t initialZ = -1;
		int turns = -1;
		int instructionIndex = 0;
		for (const auto& [keyword, variable, valOrVar] : instructions) {
			if (keyword == INP) {
				++turns;
				initialZ = registry['z'];
			}
			if (turns >= maxTurns) {
				break;
			}

			auto& left = registry[variable];
			auto right = valOrVar.isVal() ? valOrVar.val : registry[valOrVar.var];

			switch (keyword) {
				case INP: {
					char digit;
					input >> digit;
					left = digit - '0';
				} break;
				case ADD: left += right; break;
				case MUL: left *= right; break;
				case DIV: {
					if (right == 0) {
						return false;
					}
					left /= right;
				} break;
				case MOD: {
					if (left < 0 || right < 0) {
						return false;
					}
					left %= right;
				} break;
				case EQL: left = (left == right); break;
			}

			if (keyword == ADD && variable == 'z' && valOrVar.var == 'y') {
				bool isPositive = positiveTurns[turns];
				if (!isPositive && registry['x'] != 0) {
					return false;
				}
			}
			++instructionIndex;
		}

		return true;
	}

	int64_t getVariable(char var) const {
		return registry.find(var)->second;
	}
};

Keyword decodeKeyword(std::string str) {
	if (str == "inp") {
		return INP;
	}
	if (str == "add") {
		return ADD;
	}
	if (str == "mul") {
		return MUL;
	}
	if (str == "div") {
		return DIV;
	}
	if (str == "mod") {
		return MOD;
	}
	if (str == "eql") {
		return EQL;
	}
	throw "So bad";
}

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		if (line.empty()) {
			continue;
		}

		Instruction instruction;
		std::istringstream ss(line);
		std::string keyword;
		ss >> keyword;
		instruction.keyword = decodeKeyword(keyword);

		char var;
		ss >> var;
		instruction.variable = var;

		if (instruction.keyword == INP) {
			data.push_back(instruction);
			continue;
		}

		std::string last;
		ss >> last;
		if (last.size() == 1 && (last[0] < '0' || last[0] > '9')) {
			instruction.valOrVar = ValOrVar{ 0, last[0] };
		}
		else {
			instruction.valOrVar = ValOrVar{ std::stoi(last) };
		}
		data.push_back(instruction);
	}
	return data;
}

bool checkNumber(const DataType& data, const std::string& str) {
	if (str == "8399799929") {
		int x = 6;
		x += 50;
	}
	if (str.size() == 14) {
		Decoder decoder(data, str);
		bool valid = decoder.run();
		if (valid && decoder.getVariable('z') == 0) {
			std::cout << str << std::endl;
			return true;
		}
	}
	else {
		Decoder decoder(data, str);
		bool valid = decoder.run();
		if (!valid) {
			return false;
		}

		for (char i = '9'; i >= '1'; --i) {
			std::string newStr = str + i;
			bool v = checkNumber(data, newStr);
			if (v) {
				return true;
			}
		}
	}
	return false;
}

bool checkNumberSmall(const DataType& data, const std::string& str) {
	if (str == "8399799929") {
		int x = 6;
		x += 50;
	}
	if (str.size() == 14) {
		Decoder decoder(data, str);
		bool valid = decoder.run();
		if (valid && decoder.getVariable('z') == 0) {
			std::cout << str << std::endl;
			return true;
		}
	}
	else {
		Decoder decoder(data, str);
		bool valid = decoder.run();
		if (!valid) {
			return false;
		}

		for (char i = '1'; i <= '9'; ++i) {
			std::string newStr = str + i;
			bool v = checkNumberSmall(data, newStr);
			if (v) {
				return true;
			}
		}
	}
	return false;
}

int64_t partOne(const DataType& data) {
	checkNumber(data, "");
	return 0;
}

int64_t partTwo(const DataType& data) {
	checkNumberSmall(data, "8");
	return 0;
}

int main(int argc, char** argv) {
	const auto data = read();
	common::measureAndPrint("Part 1", partOne, data);
	common::measureAndPrint("Part 2", partTwo, data);
	return 0;
}