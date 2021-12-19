#include "../common/pch.h"
#include <iostream>
#include <cstdint>
#include <string>
#include <algorithm>
#include <utility>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <array>

struct Vec3D {
	int x{ 0 };
	int y{ 0 };
	int z{ 0 };

	std::string toString() const {
		std::ostringstream ss;
		ss << x << "," << y << "," << z;
		return ss.str();
	}

	bool operator<(const Vec3D& vec) const {
		if (x != vec.x) {
			return x < vec.x;
		}
		if (y != vec.y) {
			return y < vec.y;
		}
		return z < vec.z;
	}

	bool operator==(const Vec3D& vec) const {
		return x == vec.x && y == vec.y && z == vec.z;
	}

	Vec3D operator-(const Vec3D& vec) const {
		return { x - vec.x, y - vec.y, z - vec.z };
	}

	Vec3D& operator+=(const Vec3D& vec) {
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}

	double dist(const Vec3D& vec) const {
		return sqrt(1.0*pow(x - vec.x, 2) + pow(y - vec.y, 2) + pow(z - vec.z, 2));
	}

	Vec3D relative(const Vec3D& vec) const {
		return { x - vec.x, y - vec.y, z - vec.z };
	}

	void rotateRightXY() {
		int tmp = x;
		x = y;
		y = -tmp;
	}

	void rotateLeftXY() {
		int tmp = x;
		x = -y;
		y = tmp;
	}

	void rotateRightXZ() {
		int tmp = x;
		x = z;
		z = -tmp;
	}

	void rotateLeftXZ() {
		int tmp = x;
		x = -z;
		z = tmp;
	}

	void rotateRightYZ() {
		int tmp = y;
		y = z;
		z = -tmp;
	}

	void rotateLeftYZ() {
		int tmp = y;
		y = -z;
		z = tmp;
	}

	void rotateXY(int turns) {
		if (turns < 0) {
			int count = -turns / 4 + 1;
			turns += 4 * count;
		}
		turns %= 4;
		
		switch (turns) {
			case 0: break;
			case 1: rotateRightXY(); break;
			case 2: y = -y; x = -x; break;
			case 3: rotateLeftXY(); break;
		}
	}

	void rotateXZ(int turns) {
		if (turns < 0) {
			int count = -turns / 4 + 1;
			turns += 4 * count;
		}
		turns %= 4;

		switch (turns) {
			case 0: break;
			case 1: rotateRightXZ(); break;
			case 2: z = -z; x = -x; break;
			case 3: rotateLeftXZ(); break;
		}
	}

	void rotateYZ(int turns) {
		if (turns < 0) {
			int count = -turns / 4 + 1;
			turns += 4 * count;
		}
		turns %= 4;

		switch (turns) {
			case 0: break;
			case 1: rotateRightYZ(); break;
			case 2: z = -z; y = -y; break;
			case 3: rotateLeftYZ(); break;
		}
	}
};

struct ScannerData {
	int scannerId;
	std::vector<Vec3D> beacons;

	void rotateXY(int turns) {
		for (auto& beacon : beacons) {
			beacon.rotateXY(turns);
		}
	}

	void rotateXZ(int turns) {
		for (auto& beacon : beacons) {
			beacon.rotateXZ(turns);
		}
	}

	void rotateYZ(int turns) {
		for (auto& beacon : beacons) {
			beacon.rotateYZ(turns);
		}
	}

	void translate(Vec3D vec) {
		for (auto& beacon : beacons) {
			beacon += vec;
		}
	}
};

struct Connection {
	int firstId;
	int secondId;
	double distance;
};

struct RelativeConnection {
	int firstId;
	int secondId;
	Vec3D relative;
};

struct ScannerRelation {
	int firstScannerId;
	int secondScannerId;
	std::set<int> firstScannerPoints;
	std::set<int> secondScannerPoints;
};

using DataType = std::vector<ScannerData>;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;
	for (std::string line; reader.nextLine(line);) {
		ScannerData scannerData;
		auto rCharacter = line.find('r');
		std::istringstream ss(line.substr(rCharacter + 1));
		ss >> scannerData.scannerId;
		while (reader.nextLine(line) && !line.empty()) {
			auto coords = common::parseArray<int>(line, ',');
			Vec3D vec = { coords[0], coords[1], coords[2] };
			scannerData.beacons.push_back(vec);
		}
		data.push_back(scannerData);
	}
	return data;
}

std::vector<Connection> createConnections(const std::vector<Vec3D>& data) {
	std::vector<Connection> c;
	for (int i = 0; i < data.size(); ++i) {
		for (int j = i + 1; j < data.size(); ++j) {
			c.push_back(Connection{i, j, data[i].dist(data[j])});
		}
	}
	return c;
}

std::vector<RelativeConnection> createRelativeConnections(const std::vector<Vec3D>& data) {
	std::vector<RelativeConnection> c;
	for (int i = 0; i < data.size(); ++i) {
		for (int j = i + 1; j < data.size(); ++j) {
			c.push_back(RelativeConnection{ i, j, data[i].relative(data[j]) });
			c.push_back(RelativeConnection{ j, i, data[j].relative(data[i]) });
		}
	}
	return c;
}

std::vector<ScannerData> getAllVariants(const ScannerData& scannerData) {
	std::vector<ScannerData> variants;
	variants.reserve(24);

	for (int xyTurns = 0; xyTurns < 4; ++xyTurns) {
		for (int yzTurns = 0; yzTurns < 4; ++yzTurns) {
			ScannerData data = scannerData;
			data.rotateXY(xyTurns);
			data.rotateYZ(yzTurns);
			variants.push_back(data);
		}
		ScannerData data = scannerData;
		data.rotateXY(xyTurns);
		data.rotateXZ(1);
		variants.push_back(data);
		data.rotateXZ(2);
		variants.push_back(data);
	}
	return variants;
}

std::pair<std::set<int>, std::set<int>> countMatchingPoints(const std::vector<Connection>& connections1, const std::vector<Connection>& connections2) {
	std::set<int> idx1;
	std::set<int> idx2;
	for (const auto& c1 : connections1) {
		auto found = std::find_if(connections2.begin(), connections2.end(), [&c1](const Connection& conn) {
			return abs(conn.distance - c1.distance) < 0.000000000001;
		});
		if (found != connections2.end()) {
			idx1.insert(c1.firstId);
			idx1.insert(c1.secondId);
			idx2.insert(found->firstId);
			idx2.insert(found->secondId);
		}
	}

	return { idx1, idx2 };
}

std::vector<ScannerRelation> findMatchingScanners(
		const ScannerData& baseScanner,
		std::map<int, ScannerData>& scannersWithoutPair,
		const std::map<int, std::vector<Connection>>& connectionsMap) {

	std::vector<ScannerRelation> localPairs;
	const auto& baseConnections = connectionsMap.find(baseScanner.scannerId)->second;
	for (const auto [scannerId, _] : scannersWithoutPair) {
		const auto& otherConnections = connectionsMap.find(scannerId)->second;
		auto [points1, points2] = countMatchingPoints(baseConnections, otherConnections);
		if (std::min(points1.size(), points2.size()) >= 12) {
			localPairs.push_back(ScannerRelation{ baseScanner.scannerId, scannerId, points1, points2 });
		}
	}

	std::vector<ScannerData> scannersToCheck;
	for (const auto& matchResult : localPairs) {
		int id = matchResult.secondScannerId;
		scannersToCheck.push_back(scannersWithoutPair[id]);
		scannersWithoutPair.erase(id);
	}
	if (scannersWithoutPair.empty()) {
		return localPairs;
	}

	auto allPairs = localPairs;
	for (auto scanner : scannersToCheck) {
		auto matches = findMatchingScanners(scanner, scannersWithoutPair, connectionsMap);
		std::copy(matches.begin(), matches.end(), std::back_inserter(allPairs));
	}
	return allPairs;
}

std::vector<Vec3D> filterPoints(const std::vector<Vec3D>& data, const std::set<int>& indexes) {
	std::vector<Vec3D> filtered;
	for (int idx : indexes) {
		filtered.push_back(data[idx]);
	}
	return filtered;
}

Vec3D getTranslationVector(const ScannerData& scannerToTranslate, const ScannerRelation& relation, const ScannerData& baseScanner) {
	const auto baseScannerCommonPoints = filterPoints(baseScanner.beacons, relation.firstScannerPoints);
	const auto secondScannerCommonPoints = filterPoints(scannerToTranslate.beacons, relation.secondScannerPoints);

	int maxXBase = std::numeric_limits<int>::min();
	int maxXSecond = std::numeric_limits<int>::min();
	int maxYBase = std::numeric_limits<int>::min();
	int maxYSecond = std::numeric_limits<int>::min();
	int maxZBase = std::numeric_limits<int>::min();
	int maxZSecond = std::numeric_limits<int>::min();

	for (const auto [x, y, z] : baseScannerCommonPoints) {
		if (x > maxXBase) {
			maxXBase = x;
		}

		if (y > maxYBase) {
			maxYBase = y;
		}

		if (z > maxZBase) {
			maxZBase = z;
		}
	}
	for (const auto [x, y, z] : secondScannerCommonPoints) {
		if (x > maxXSecond) {
			maxXSecond = x;
		}

		if (y > maxYSecond) {
			maxYSecond = y;
		}

		if (z > maxZSecond) {
			maxZSecond = z;
		}
	}

	return {maxXBase - maxXSecond, maxYBase - maxYSecond, maxZBase - maxZSecond};
}

static std::vector<Vec3D> scannerPositions{ Vec3D{0, 0, 0} };

void translateScanner(ScannerData& scannerToTranslate, const ScannerRelation& relation, const ScannerData& baseScanner) {
	Vec3D translationVector = getTranslationVector(scannerToTranslate, relation, baseScanner);
	scannerPositions.push_back(Vec3D{ -translationVector.x, -translationVector.y, -translationVector.z});
	scannerToTranslate.translate(translationVector);
}

int partOne(const DataType& scanners) {
	std::map<int, ScannerData> scannersWithoutPair;
	for (const auto& scanner : scanners) {
		scannersWithoutPair.emplace(scanner.scannerId, scanner);
	}

	std::map<int, std::vector<Connection>> connectionsMap;
	for (const auto& scanner : scanners) {
		connectionsMap.emplace(scanner.scannerId, createConnections(scanner.beacons));
	}

	ScannerData baseScanner = scanners[0];
	scannersWithoutPair.erase(0);
	auto relations = findMatchingScanners(baseScanner, scannersWithoutPair, connectionsMap);


	std::map<int, ScannerData> rotatedScanners;
	const auto& firstScannerInTheList = scanners[0];
	rotatedScanners.emplace(firstScannerInTheList.scannerId, firstScannerInTheList);
	for (const auto& relation : relations) {
		const auto& firstScanner = rotatedScanners.find(relation.firstScannerId)->second;
		const auto& secondScanner = scanners[relation.secondScannerId];
		const auto firstScannerCommonPoints = filterPoints(firstScanner.beacons, relation.firstScannerPoints);
		auto fistScannerConnections = createRelativeConnections(firstScannerCommonPoints);

		const auto secondScannerVariants = getAllVariants(secondScanner);
		for (const auto& secondScannerVariant : secondScannerVariants) {
			
			const auto secondScannerCommonPoints = filterPoints(secondScannerVariant.beacons, relation.secondScannerPoints);
			auto secondScannerConnections = createRelativeConnections(secondScannerCommonPoints);

			int noOfMatches = 0;
			for (auto firstConnection : fistScannerConnections) {
				for (auto secondConnection : secondScannerConnections) {
					if (firstConnection.relative == secondConnection.relative) {
						++noOfMatches;
					}
				}
			}
			if (noOfMatches == 132) {
				rotatedScanners.emplace(secondScannerVariant.scannerId, secondScannerVariant);
			}
		}
	}

	for (const auto& relation : relations) {
		const auto& baseScanner = rotatedScanners.find(relation.firstScannerId)->second;
		auto& secondScanner = rotatedScanners[relation.secondScannerId];
		translateScanner(secondScanner, relation, baseScanner);
	}

	std::set<Vec3D> allBeacons;
	for (const auto& [_, scanner] : rotatedScanners) {
		for (const auto& beacon : scanner.beacons) {
			allBeacons.insert(beacon);
		}
	}
	return allBeacons.size();
}

int partTwo(const DataType& data) {
	int maxDist = 0;
	Vec3D left;
	Vec3D right;
	for (const auto& vec1 : scannerPositions) {
		for (const auto& vec2 : scannerPositions) {
			const auto diff = vec1 - vec2;
			const int dist = abs(diff.x) + abs(diff.y) + abs(diff.z);
			if (dist > maxDist) {
				maxDist = dist;
			}
		}
	}
	return maxDist;
}

int main(int argc, char** argv) {
	const auto data = read();
	common::measureAndPrint("Part 1", partOne, data);
	common::measureAndPrint("Part 2", partTwo, data);
	return 0;
}