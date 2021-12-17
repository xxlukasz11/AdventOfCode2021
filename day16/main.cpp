#include "../common/pch.h"
#include <iostream>
#include <cstdint>
#include <string>
#include <algorithm>
#include <numeric>
#include <utility>
#include <sstream>
#include <vector>
#include <unordered_map>
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

std::pair<int, Packet> parseLiteralPacketBody(const std::vector<bool>& data, const int initialPos) {
	int noOfChunks = 0;
	int tmpPos = initialPos;
	while (true) {
		++noOfChunks;
		bool isLast = !(data[tmpPos]);
		if (isLast) {
			break;
		}
		tmpPos += 5;
	}

	Packet packet;
	int currentPos = initialPos;
	for (int chunkIdx = noOfChunks - 1; chunkIdx >= 0; --chunkIdx) {
		currentPos++;
		int64_t chunkValue = parseNumber(data, currentPos, 4);
		packet.value |= (chunkValue << (chunkIdx * 4));
		currentPos += 4;
	}
	return { currentPos,  packet };
}

std::pair<int, Packet> parsePacket(const std::vector<bool>& bits, const int initialPos);

std::pair<int, Packet> parseOperatorPacketBody(const std::vector<bool>& data, const int initialPos) {
	Packet packet;
	int currentPos = initialPos;
	packet.lengthType = parseNumber(data, currentPos++, 1);
	if (packet.lengthType == 0) {
		packet.lengthBits = parseNumber(data, currentPos, 15);
		currentPos += 15;

		int expectedEnd = currentPos + packet.lengthBits;
		while (currentPos < expectedEnd) {
			auto [nextBit, subPacket] = parsePacket(data, currentPos);
			packet.subPackets.push_back(subPacket);
			currentPos = nextBit;
		}

	}
	else {
		packet.lengthPackets = parseNumber(data, currentPos, 11);
		currentPos += 11;

		for (int i = 0; i < packet.lengthPackets; ++i) {
			auto [nextBit, subPacket] = parsePacket(data, currentPos);
			packet.subPackets.push_back(subPacket);
			currentPos = nextBit;
		}
	}
	return { currentPos, packet };
}

static int sumV = 0;

std::pair<int, Packet> parsePacket(const std::vector<bool>& bits, const int initialPos) {
	int currentPos = initialPos;
	int version = parseNumber(bits, currentPos, 3);
	sumV += version;
	currentPos += 3;
	int typeId = parseNumber(bits, currentPos, 3);
	currentPos += 3;
	if (typeId == 4) {
		auto [nextBit, packet] = parseLiteralPacketBody(bits, currentPos);
		packet.version = version;
		packet.typeId = typeId;
		return { nextBit, packet };
	}
	else {
		auto [nextBit, packet] = parseOperatorPacketBody(bits, currentPos);
		packet.version = version;
		packet.typeId = typeId;
		return { nextBit, packet };
	}
}

Packet decodeOutermostPacket(const std::vector<bool>& bits) {
	return parsePacket(bits, 0).second;
}

int partOne(const DataType& data) {
	std::vector<bool> bits = common::hexToBinArray(data);
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

int64_t calculatePacketValue(const Packet& packet) {
	if (packet.isLiteral()) {
		return packet.value;
	}

	std::vector<int64_t> values;
	const auto& subPackets = packet.subPackets;
	for (const auto& subPacket : subPackets) {
		int64_t value = calculatePacketValue(subPacket);
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
	std::vector<bool> bits = common::hexToBinArray(data);
	const auto outermostPacket = decodeOutermostPacket(bits);
	int64_t result = calculatePacketValue(outermostPacket);
	return result;
}

int main(int argc, char** argv) {
	const auto data = read();
	common::measureAndPrint("Part 1", partOne, data);
	common::measureAndPrint("Part 2", partTwo, data);
	return 0;
}