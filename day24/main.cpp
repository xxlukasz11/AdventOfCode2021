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

struct Section {
	int index;
	std::vector<Instruction> instructions;
};

using DataType = std::vector<Section>;

class Decoder {
private:
	const DataType& sections;
	std::istringstream input;
	std::map<char, int64_t> registry;
	const int steps;

public:
	Decoder(const DataType& sections, const std::string& input) :
		sections(sections), input(input), steps(input.size()) {}

	bool run() {
		for (int digitIdx = 0; digitIdx < steps; ++digitIdx) {
			const auto& section = sections[digitIdx];
			for (const auto& [keyword, variable, valOrVar] : section.instructions) {
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
				
			}

			bool isPositive = positiveTurns[section.index];
			if (!isPositive && registry['x'] != 0) {
				return false;
			}
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
	std::vector<Instruction> instructions;
	int sectionIndex = 0;
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

		if (instruction.keyword != INP) {
			std::string last;
			ss >> last;
			if (last.size() == 1 && (last[0] < '0' || last[0] > '9')) {
				instruction.valOrVar = ValOrVar{ 0, last[0] };
			}
			else {
				instruction.valOrVar = ValOrVar{ std::stoi(last) };
			}
		}
		if (instruction.keyword == INP && !instructions.empty()) {
			data.push_back(Section{ sectionIndex++, instructions });
			instructions.clear();
		}
		instructions.push_back(instruction);
	}
	data.push_back(Section{ sectionIndex++, instructions });
	return data;
}

bool checkNumber(const DataType& data, const std::string& str) {
	Decoder decoder(data, str);
	bool valid = decoder.run();
	if (str.size() == data.size()) {
		if (valid && decoder.getVariable('z') == 0) {
			std::cout << str << std::endl;
			return true;
		}
	}
	else {
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
	Decoder decoder(data, str);
	bool valid = decoder.run();
	if (str.size() == data.size()) {
		if (valid && decoder.getVariable('z') == 0) {
			std::cout << str << std::endl;
			return true;
		}
	}
	else {
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