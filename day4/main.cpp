#include "../common/fileReader.h"
#include "../common/utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

struct Field {
	int val;
	bool set = false;
};

struct Board {
	int id;
	std::vector<std::vector<Field>> array;
};

struct InputData {
	std::vector<int> numbers;
	std::vector<Board> boards;
};

using DataType = InputData;

DataType read() {
	common::FileReader reader("input.txt");
	DataType data;

	std::string numbers = reader.nextLine();
	data.numbers = common::parseArray<int>(numbers, ',');

	int boardId = 1;
	for (std::string line; reader.nextLine(line);) {
		if (line.empty()) {
			data.boards.push_back(Board{});
			data.boards.back().id = boardId++;
			continue;
		}

		std::vector<Field> row = common::parseArray<int, Field>(line);
		data.boards.back().array.push_back(row);
	}
	return data;
}

void markFields(Board& board, int value) {
	for (auto& row : board.array) {
		for (auto& [num, set] : row) {
			if (num == value) {
				set = true;
			}
		}
	}
}

bool isWinning(const Board& board) {
	// check rows
	for (const auto& row : board.array) {
		int setCount = 0;
		for (const auto& [_, set] : row) {
			if (set) {
				++setCount;
			}
		}
		if (setCount == row.size()) {
			return true;
		}
	}

	// check columns
	int rows = board.array.size();
	int cols = board.array[0].size();
	for (int col = 0; col < cols; ++col) {
		int setCount = 0;
		for (const auto& row : board.array) {
			if (row[col].set) {
				++setCount;
			}
		}
		if (setCount == cols) {
			return true;
		}
	}

	return false;
}

std::pair<Board, int> getWinningBoard(const DataType& data) {
	auto [numbers, boards] = data;
	for (int currentNumber : numbers) {
		
		for (Board& board : boards) {
			markFields(board, currentNumber);
			if (isWinning(board)) {
				return {board, currentNumber};
			}
		}

	}
	throw "So bad";
}

std::pair<Board, int> getLosingBoard(const DataType& data) {
	auto [numbers, boards] = data;
	Board* losingBoard = nullptr;
	for (int currentNumber : numbers) {

		int winningBoardCount = 0;
		for (Board& board : boards) {
			markFields(board, currentNumber);
			if (isWinning(board)) {
				++winningBoardCount;
			}
		}

		int notWinningBoardCount = boards.size() - winningBoardCount;
		if (notWinningBoardCount == 0) {
			if (!losingBoard) {
				throw "So bad";
			}
			return { *losingBoard, currentNumber };
		}
		else if (notWinningBoardCount == 1) {
			for (Board& board : boards) {
				if (!isWinning(board)) {
					losingBoard = &board;
				}
			}
		}
	}

	throw "So bad";
}

int sumUnsetFields(const Board& board) {
	int sum = 0;
	for (const auto& row : board.array) {
		for (auto [val, set] : row) {
			if (!set) {
				sum += val;
			}
		}
	}
	return sum;
}

void partOne(const DataType& data) {
	auto [winningBoard, lastNumber] = getWinningBoard(data);
	int sum = sumUnsetFields(winningBoard);
	std::cout << "Part one: " << sum*lastNumber << std::endl;
}

void partTwo(const DataType& data) {
	auto [losingBoard, lastNumber] = getLosingBoard(data);
	int sum = sumUnsetFields(losingBoard);
	std::cout << "Part two: " << sum * lastNumber << std::endl;
}

int main() {
	const auto data = read();
	partOne(data);
	partTwo(data);
	return 0;
}