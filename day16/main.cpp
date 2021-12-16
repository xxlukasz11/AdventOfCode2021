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

struct Packet {
	int version = 0;
	int typeId = 0;

	int64_t value = 0;

	int lengthType = 0;
	int lengthBits = 0;
	int lengthPackets = 0;
	std::vector<Packet> subPackets;

	bool isLiteral() const {
		return typeId == 4;
	}
};

using DataType = std::vector<char>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	std::string line = reader.nextLine();
	std::istringstream ss(line);
	for (char c; ss >> c;) {
		data.push_back(c);
	}
	return data;
}

int64_t parseNumber(const std::vector<bool>& data, const int pos, const int length) {
	int64_t number = 0;
	int bitPos = pos;
	for (int i = length - 1; i >= 0; --i) {
		number |= (static_cast<int64_t>(data[bitPos++]) << i);
	}
	if (number < 0) {
		throw "So bad";
	}
	return number;
}

static int sumV = 0;

std::pair<int, Packet> parseLiteralPacket(const std::vector<bool>& data, const int pos, int version, int typeId, bool isStadalonePacket) {
	sumV += version;
	Packet packet;
	packet.version = version;
	packet.typeId = typeId;

	int noOfChunks = 0;
	int tmpPos = pos;
	while (true) {
		++noOfChunks;
		bool isLast = !(data[tmpPos]);
		if (isLast) {
			break;
		}
		tmpPos += 5;
	}

	int currentPos = pos;
	for (int chunkIdx = noOfChunks - 1; chunkIdx >= 0; --chunkIdx) {
		currentPos++;
		int64_t chunkValue = parseNumber(data, currentPos, 4);
		packet.value |= (chunkValue << (chunkIdx * 4));
		currentPos += 4;
	}
	if (isStadalonePacket) {
		int packetLength = currentPos - (pos - 6);
		int extraBits = packetLength % 4;
		if (extraBits != 0) {
			currentPos += (4 - extraBits);
		}
	}
	return { currentPos,  packet };
}

std::pair<int, Packet> parseOperatorPacket(const std::vector<bool>& data, const int pos, int version, int typeId) {
	sumV += version;
	Packet packet;
	packet.version = version;
	packet.typeId = typeId;

	int currentPos = pos;
	packet.lengthType = parseNumber(data, currentPos++, 1);
	if (packet.lengthType == 0) {
		packet.lengthBits = parseNumber(data, currentPos, 15);
		currentPos += 15;

		int expectedEnd = currentPos + packet.lengthBits;
		while (currentPos < expectedEnd) {
			int localVersion = parseNumber(data, currentPos, 3);
			currentPos += 3;
			int localTypeId = parseNumber(data, currentPos, 3);
			currentPos += 3;

			if (localTypeId == 4) {
				auto [nextBit, literal] = parseLiteralPacket(data, currentPos, localVersion, localTypeId, false);
				packet.subPackets.push_back(literal);
				currentPos = nextBit;
			} else {
				auto [nextBit, oper] = parseOperatorPacket(data, currentPos, localVersion, localTypeId);
				packet.subPackets.push_back(oper);
				currentPos = nextBit;
			}
		}

	} else {
		packet.lengthPackets = parseNumber(data, currentPos, 11);
		currentPos += 11;

		for (int i = 0; i < packet.lengthPackets; ++i) {
			int localVersion = parseNumber(data, currentPos, 3);
			currentPos += 3;
			int localTypeId = parseNumber(data, currentPos, 3);
			currentPos += 3;

			if (localTypeId == 4) {
				auto [nextBit, literal] = parseLiteralPacket(data, currentPos, localVersion, localTypeId, false);
				packet.subPackets.push_back(literal);
				currentPos = nextBit;
			} else {
				auto [nextBit, oper] = parseOperatorPacket(data, currentPos, localVersion, localTypeId);
				packet.subPackets.push_back(oper);
				currentPos = nextBit;
			}
		}
	}
	return {currentPos, packet };
}

std::string hexToBin(char c) {
	switch (c) {
		case '0': return "0000";
		case '1': return "0001";
		case '2': return "0010";
		case '3': return "0011";
		case '4': return "0100";
		case '5': return "0101";
		case '6': return "0110";
		case '7': return "0111";
		case '8': return "1000";
		case '9': return "1001";
		case 'A': return "1010";
		case 'B': return "1011";
		case 'C': return "1100";
		case 'D': return "1101";
		case 'E': return "1110";
		case 'F': return "1111";
		default: throw "So bad";
	}
}

std::vector<bool> toBinary(const DataType& data) {
	std::vector<bool> bits;
	for (char c : data) {
		std::string bin = hexToBin(c);
		for (char xx : bin) {
			bits.push_back(xx == '1');
		}
	}
	return bits;
}

Packet decodeOutermostPacket(const std::vector<bool>& bits) {
	int currentPos = 0;
	int version = parseNumber(bits, currentPos, 3);
	currentPos += 3;
	int typeId = parseNumber(bits, currentPos, 3);
	currentPos += 3;
	return parseOperatorPacket(bits, currentPos, version, typeId).second;
}

int partOne(const DataType& data) {
	std::vector<bool> bits = toBinary(data);
	decodeOutermostPacket(bits);
	return sumV;
}

int64_t mul(const std::vector<int64_t>& values) {
	int64_t result = 1;
	for (auto val : values) {
		result *= val;
	}
	return result;
}

int64_t calculateValue(const Packet& packet) {
	if (packet.isLiteral()) {
		return packet.value;
	}

	std::vector<int64_t> values;
	const auto& subPackets = packet.subPackets;
	for (const auto& subPacket : subPackets) {
		int64_t value = calculateValue(subPacket);
		values.push_back(value);
	}

	int64_t result;
	switch (packet.typeId) {
		case 0: result = std::accumulate(values.begin(), values.end(), 0LL); break;
		case 1: result = mul(values); break;
		case 2: result = *std::min_element(values.begin(), values.end()); break;
		case 3: result = *std::max_element(values.begin(), values.end()); break;
		case 5: result = values[0] > values[1]; break;
		case 6: result = values[0] < values[1]; break;
		case 7: result = values[0] == values[1]; break;
		default: throw "So bad";
	}

	return result;
}

int64_t partTwo(const DataType& data) {
	std::vector<bool> bits = toBinary(data);
	const auto outermostPacket = decodeOutermostPacket(bits);

	int64_t result = calculateValue(outermostPacket);
	return result;
}

int main(int argc, char** argv) {
	const auto data = read();
	common::measureAndPrint("Part 1", partOne, data);
	common::measureAndPrint("Part 2", partTwo, data);
	return 0;
}