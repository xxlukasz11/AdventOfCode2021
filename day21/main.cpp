#include "../common/pch.h"
#include <iostream>
#include <cstdint>
#include <string>
#include <map>

struct Data {
	int player1Pos;
	int player2Pos;
};

Data read() {
	common::FileReader reader("input.txt");
	Data data;
	std::string line1 = reader.nextLine();
	auto colonPos1 = line1.find(':');
	data.player1Pos = std::stoi(line1.substr(colonPos1 + 2));
	std::string line2 = reader.nextLine();
	auto colonPos2 = line2.find(':');
	data.player2Pos = std::stoi(line2.substr(colonPos2 + 2));
	return data;
}

int rollDie(int& die) {
	if (die == 100) {
		die = 1;
		return 100;
	}
	return die++;
}

int partOne(const Data& data) {
	auto [plOnePos, plTwoPos] = data;
	int plOneScore = 0;
	int plTwoScore = 0;
	int plOneTurns = 0;
	int plTwoTurns = 0;

	int die = 1;
	bool player1ToMove = true;
	while (plOneScore < 1000 && plTwoScore < 1000) {
		if (player1ToMove) {
			plOneTurns += 3;
			player1ToMove = false;
			int moves = rollDie(die) + rollDie(die) + rollDie(die);
			moves %= 10;
			plOnePos += moves == 0 ? 10 : moves;
			plOnePos %= 10;
			if (plOnePos == 0) {
				plOnePos = 10;
			}
			plOneScore += plOnePos;
		}
		else {
			plTwoTurns += 3;
			player1ToMove = true;
			int moves = rollDie(die) + rollDie(die) + rollDie(die);
			moves %= 10;
			plTwoPos += moves == 0 ? 10 : moves;
			plTwoPos %= 10;
			if (plTwoPos == 0) {
				plTwoPos = 10;
			}
			plTwoScore += plTwoPos;
		}
	}
	if (plOneScore > plTwoScore) {
		return plTwoScore * (plOneTurns + plTwoTurns);
	}
	return plOneScore * (plOneTurns + plTwoTurns);
}

struct GameData {
	int plOnePos{};
	int plTwoPos{};
	int plOneScore{};
	int plTwoScore{};
	bool player1ToMove{ true };
};

void playTurn(
		GameData gameData,
		uint64_t& plOneWins,
		uint64_t& plTwoWins,
		int moves,
		uint64_t currentUniverses,
		const std::map<int, int>& rollSumToUniverses) {

	auto& [plOnePos, plTwoPos, plOneScore, plTwoScore, player1ToMove] = gameData;

	if (player1ToMove) {
		player1ToMove = false;
		plOnePos += moves;
		plOnePos %= 10;
		if (plOnePos == 0) {
			plOnePos = 10;
		}
		plOneScore += plOnePos;
	}
	else {
		player1ToMove = true;
		plTwoPos += moves;
		plTwoPos %= 10;
		if (plTwoPos == 0) {
			plTwoPos = 10;
		}
		plTwoScore += plTwoPos;
	}

	if (plOneScore >= 21) {
		plOneWins += currentUniverses;
	}
	else if (plTwoScore >= 21) {
		plTwoWins += currentUniverses;
	}
	else {
		for (const auto& [sum, universes] : rollSumToUniverses) {
			playTurn(gameData, plOneWins, plTwoWins, sum, currentUniverses*universes, rollSumToUniverses);
		}
	}
}

uint64_t partTwo(const Data& data) {
	std::map<int, int> rollSumToUniverses;
	for (int i = 1; i <= 3; ++i) {
		for (int j = 1; j <= 3; ++j) {
			for (int k = 1; k <= 3; ++k) {
				rollSumToUniverses[i + j + k] += 1;
			}
		}
	}

	GameData gameData;
	gameData.plOnePos = data.player1Pos;
	gameData.plTwoPos = data.player2Pos;

	uint64_t plOneWins = 0;
	uint64_t plTwoWins = 0;
	for (const auto& [sum, universes] : rollSumToUniverses) {
		playTurn(gameData, plOneWins, plTwoWins, sum, universes, rollSumToUniverses);
	}

	if (plOneWins > plTwoWins) {
		return plOneWins;
	}
	return plTwoWins;
}

int main(int argc, char** argv) {
	const auto data = read();
	common::measureAndPrint("Part 1", partOne, data);
	common::measureAndPrint("Part 2", partTwo, data);
	return 0;
}