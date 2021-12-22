#include "../common/pch.h"
#include <iostream>
#include <cstdint>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include <set>
#include <cmath>

using Point = common::Vec3<int>;

struct Instruction {
	bool state;
	Point min;
	Point max;
};

using DataType = std::vector<Instruction>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		Instruction instruction;
		std::istringstream ss(line);
		std::string state;
		ss >> state;
		if (state == "on") {
			instruction.state = true;
		}
		else {
			instruction.state = false;
		}
		
		ss.ignore(3);
		ss >> instruction.min.x;
		ss.ignore(2);
		ss >> instruction.max.x;

		ss.ignore(3);
		ss >> instruction.min.y;
		ss.ignore(2);
		ss >> instruction.max.y;

		ss.ignore(3);
		ss >> instruction.min.z;
		ss.ignore(2);
		ss >> instruction.max.z;

		data.push_back(instruction);
	}
	return data;
}

void turnOnCubes(std::set<Point>& cubesOn, const Instruction& instruction) {
	for (int x = instruction.min.x; x <= instruction.max.x; ++x) {
		for (int y = instruction.min.y; y <= instruction.max.y; ++y) {
			for (int z = instruction.min.z; z <= instruction.max.z; ++z) {
				cubesOn.insert(Point{ x, y, z });
			}
		}
	}
}

void turnOffCubes(std::set<Point>& cubesOn, const Instruction& instruction) {
	for (int x = instruction.min.x; x <= instruction.max.x; ++x) {
		for (int y = instruction.min.y; y <= instruction.max.y; ++y) {
			for (int z = instruction.min.z; z <= instruction.max.z; ++z) {
				cubesOn.erase(Point{ x, y, z });
			}
		}
	}
}

int partOne(const DataType& data) {
	std::set<Point> cubesOn;
	for (const auto& instruction : data) {
		if (instruction.min.x < -50 ||
			instruction.min.y < -50 ||
			instruction.min.z < -50 ||
			instruction.max.x > 50 ||
			instruction.max.y > 50 ||
			instruction.max.z > 50) {
			continue;
		}

		if (instruction.state == true) {
			turnOnCubes(cubesOn, instruction);
		}
		else {
			turnOffCubes(cubesOn, instruction);
		}
	}
	return cubesOn.size();
}

struct Cuboid {
	Point min;
	Point max;
	uint64_t volume;
};

uint64_t calculateCuboidVolume(const Point& min, const Point& max) {
	uint64_t x = abs(max.x - min.x + 1);
	uint64_t y = abs(max.y - min.y + 1);
	uint64_t z = abs(max.z - min.z + 1);
	return x * y * z;
}

template<typename T>
std::optional<Cuboid> getIntersectionCuboid(const Cuboid& cuboid, const T& instruction) {
	const auto bottomInside = cuboid.max.x >= instruction.min.x &&
		cuboid.max.y >= instruction.min.y &&
		cuboid.max.z >= instruction.min.z;
	const auto topInside = cuboid.min.x <= instruction.max.x &&
		cuboid.min.y <= instruction.max.y &&
		cuboid.min.z <= instruction.max.z;
	
	if (topInside && bottomInside) {
		Point min{ std::max(cuboid.min.x, instruction.min.x), std::max(cuboid.min.y, instruction.min.y), std::max(cuboid.min.z, instruction.min.z) };
		Point max{ std::min(cuboid.max.x, instruction.max.x), std::min(cuboid.max.y, instruction.max.y), std::min(cuboid.max.z, instruction.max.z) };
		uint64_t volume = calculateCuboidVolume(min, max);
		return Cuboid{ min, max, volume };
	}
	return std::nullopt;
}

struct CuboidWithSubtractions {
	Cuboid cuboid;
	std::vector<Cuboid> cuboidsToSubtract;
};

uint64_t calculateVolumeOf(const std::vector<Cuboid>& cuboids) {
	std::vector<CuboidWithSubtractions> cuboidsWithSubtraction;
	for (const auto& cuboid : cuboids) {
		for (auto& cuboidWithSubtraction : cuboidsWithSubtraction) {
			const auto intersectionOpt = getIntersectionCuboid(cuboidWithSubtraction.cuboid, cuboid);
			if (!intersectionOpt.has_value()) {
				continue;
			}
			const auto& intersection = intersectionOpt.value();
			cuboidWithSubtraction.cuboidsToSubtract.push_back(intersection);
		}

		const uint64_t volume = calculateCuboidVolume(cuboid.min, cuboid.max);
		cuboidsWithSubtraction.emplace_back(CuboidWithSubtractions{ cuboid });
	}

	uint64_t totalVolume = 0;
	for (const auto& [cuboid, subtractions] : cuboidsWithSubtraction) {
		totalVolume += cuboid.volume;
		if (!subtractions.empty()) {
			totalVolume -= calculateVolumeOf(subtractions);
		}
	}
	return totalVolume;
}

uint64_t partTwo(const DataType& data) {
	std::vector<CuboidWithSubtractions> cuboids;
	for (const auto& instruction : data) {
		const auto& [state, min, max] = instruction;

		for (auto& cuboidWithSubtraction : cuboids) {
			const auto intersectionOpt = getIntersectionCuboid(cuboidWithSubtraction.cuboid, instruction);
			if (!intersectionOpt.has_value()) {
				continue;
			}
			const auto& intersection = intersectionOpt.value();
			cuboidWithSubtraction.cuboidsToSubtract.push_back(intersection);
		}

		if (state == true) {
			const uint64_t volume = calculateCuboidVolume(min, max);
			cuboids.emplace_back(CuboidWithSubtractions{ Cuboid{ min, max, volume } });
		}
	}

	uint64_t totalVolume = 0;
	for (const auto& [cuboid, subtractions] : cuboids) {
		totalVolume += cuboid.volume;
		if (!subtractions.empty()) {
			totalVolume -= calculateVolumeOf(subtractions);
		}
	}
	return totalVolume;
}

int main(int argc, char** argv) {
	const auto data = read();
	common::measureAndPrint("Part 1", partOne, data);
	common::measureAndPrint("Part 2", partTwo, data);
	return 0;
}