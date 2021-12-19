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

using Vec3D = common::Vec3<int>;

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
	int scannerId = 0;
	for (std::string line; reader.nextLine(line);) {
		ScannerData scannerData;
		scannerData.scannerId = scannerId++;
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
			c.push_back(Connection{i, j, data[i].distance(data[j])});
		}
	}
	return c;
}

std::vector<RelativeConnection> createRelativeConnections(const std::vector<Vec3D>& data) {
	std::vector<RelativeConnection> c;
	for (int i = 0; i < data.size(); ++i) {
		for (int j = i + 1; j < data.size(); ++j) {
			c.push_back(RelativeConnection{ i, j, data[i].relativeVec(data[j]) });
			c.push_back(RelativeConnection{ j, i, data[j].relativeVec(data[i]) });
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
		variants.push_back(std::move(data));
	}
	return variants;
}

std::pair<std::set<int>, std::set<int>> countMatchingPoints(const std::vector<Connection>& connections1, const std::vector<Connection>& connections2) {
	static auto EPSILON = std::numeric_limits<double>::epsilon();
	std::set<int> idx1;
	std::set<int> idx2;
	for (const auto& c1 : connections1) {
		auto found = std::find_if(connections2.begin(), connections2.end(), [&c1](const Connection& conn) {
			return abs(conn.distance - c1.distance) < EPSILON;
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
		const DataType& scanners,
		const ScannerData& baseScanner,
		std::map<int, ScannerData>& scannersWithoutPair,
		const std::map<int, std::vector<Connection>>& connectionsMap) {

	std::vector<ScannerRelation> localPairs;
	const auto& baseConnections = connectionsMap.find(baseScanner.scannerId)->second;
	for (const auto& [scannerId, _] : scannersWithoutPair) {
		const auto& otherConnections = connectionsMap.find(scannerId)->second;
		auto [points1, points2] = countMatchingPoints(baseConnections, otherConnections);
		if (std::min(points1.size(), points2.size()) >= 12) {
			localPairs.push_back(ScannerRelation{ baseScanner.scannerId, scannerId, points1, points2 });
		}
	}

	std::vector<int> scannerIdsToCheck;
	for (const auto& matchResult : localPairs) {
		int scannerId = matchResult.secondScannerId;
		scannerIdsToCheck.push_back(scannerId);
		scannersWithoutPair.erase(scannerId);
	}
	if (scannersWithoutPair.empty()) {
		return localPairs;
	}

	auto allPairs = localPairs;
	for (const auto& scannerId : scannerIdsToCheck) {
		const auto& scanner = scanners[scannerId];
		auto matches = findMatchingScanners(scanners, scanner, scannersWithoutPair, connectionsMap);
		std::move(matches.begin(), matches.end(), std::back_inserter(allPairs));
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

	constexpr int MIN_INT = std::numeric_limits<int>::min();
	int maxXBase = MIN_INT;
	int maxXSecond = MIN_INT;
	int maxYBase = MIN_INT;
	int maxYSecond = MIN_INT;
	int maxZBase = MIN_INT;
	int maxZSecond = MIN_INT;

	for (const auto [x, y, z] : baseScannerCommonPoints) {
		maxXBase = std::max(maxXBase, x);
		maxYBase = std::max(maxYBase, y);
		maxZBase = std::max(maxZBase, z);
	}
	for (const auto [x, y, z] : secondScannerCommonPoints) {
		maxXSecond = std::max(maxXSecond, x);
		maxYSecond = std::max(maxYSecond, y);
		maxZSecond = std::max(maxZSecond, z);
	}

	return {maxXBase - maxXSecond, maxYBase - maxYSecond, maxZBase - maxZSecond};
}

static std::vector<Vec3D> scannerPositions{ Vec3D{0, 0, 0} };

void translateScanner(ScannerData& scannerToTranslate, const ScannerRelation& relation, const ScannerData& baseScanner) {
	Vec3D translationVector = getTranslationVector(scannerToTranslate, relation, baseScanner);
	scannerPositions.push_back(Vec3D{ -translationVector.x, -translationVector.y, -translationVector.z});
	scannerToTranslate.translate(translationVector);
}

int countMatchingConnections(const std::vector<RelativeConnection>& left, const std::vector<RelativeConnection>& right) {
	int noOfMatchingConnections = 0;
	for (const auto& firstConnection : left) {
		for (const auto& secondConnection : right) {
			if (firstConnection.relative == secondConnection.relative) {
				++noOfMatchingConnections;
			}
		}
	}
	return noOfMatchingConnections;
}

bool areScannersMatching(const ScannerRelation& relation, const std::vector<RelativeConnection>& firstScannerConnections, const ScannerData& secondScanner) {
	const auto secondScannerCommonPoints = filterPoints(secondScanner.beacons, relation.secondScannerPoints);
	const auto secondScannerConnections = createRelativeConnections(secondScannerCommonPoints);
	int noOfMatchingConnections = countMatchingConnections(firstScannerConnections, secondScannerConnections);
	constexpr int doubleConnectionsOfTwelvePoints = 132;
	return noOfMatchingConnections == doubleConnectionsOfTwelvePoints;
}

std::map<int, ScannerData> rotateScanners(const DataType& scanners, const std::vector<ScannerRelation>& relations) {
	std::map<int, ScannerData> rotatedScanners;
	const auto& firstScannerInTheList = scanners[0];
	rotatedScanners.emplace(firstScannerInTheList.scannerId, firstScannerInTheList);
	for (const auto& relation : relations) {
		const auto& firstScanner = rotatedScanners.find(relation.firstScannerId)->second;
		const auto& secondScanner = scanners[relation.secondScannerId];
		const auto firstScannerCommonPoints = filterPoints(firstScanner.beacons, relation.firstScannerPoints);
		const auto firstScannerConnections = createRelativeConnections(firstScannerCommonPoints);

		const auto secondScannerVariants = getAllVariants(secondScanner);
		for (const auto& secondScannerVariant : secondScannerVariants) {
			if (areScannersMatching(relation, firstScannerConnections, secondScannerVariant)) {
				rotatedScanners.emplace(secondScannerVariant.scannerId, secondScannerVariant);
			}
		}
	}
	return rotatedScanners;
}

int partOne(const DataType& scanners) {
	std::map<int, ScannerData> scannersWithoutPair;
	std::transform(scanners.begin(), scanners.end(), std::inserter(scannersWithoutPair, scannersWithoutPair.end()),
		[](auto& scanner) { return std::make_pair(scanner.scannerId, scanner); });

	std::map<int, std::vector<Connection>> connectionsMap;
	std::transform(scanners.begin(), scanners.end(), std::inserter(connectionsMap, connectionsMap.end()),
		[](auto& scanner) { return std::make_pair(scanner.scannerId, createConnections(scanner.beacons)); });

	const auto& baseScanner = scanners[0];
	scannersWithoutPair.erase(0);
	const auto relations = findMatchingScanners(scanners, baseScanner, scannersWithoutPair, connectionsMap);

	auto rotatedScanners = rotateScanners(scanners, relations);
	for (const auto& relation : relations) {
		const auto& baseScanner = rotatedScanners.find(relation.firstScannerId)->second;
		auto& secondScanner = rotatedScanners.find(relation.secondScannerId)->second;
		translateScanner(secondScanner, relation, baseScanner);
	}

	std::set<Vec3D> allBeacons;
	for (const auto& [_, scanner] : rotatedScanners) {
		allBeacons.insert(scanner.beacons.begin(), scanner.beacons.end());
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