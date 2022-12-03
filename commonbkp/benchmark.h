#pragma once

#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

namespace common {

template<typename ResultT>
void printResultAndTime(const std::string& title, ResultT result, double millis) {
	static const std::string startFill = "***************";
	static const char fill = '*';
	static const int width = 30;

	std::cout  << startFill << std::setw(width) << std::setfill(fill) << std::left << ' ' + title + ' ' << "\n";
	std::cout << "Result = " << result << "\n";
	std::ostringstream timeStr;
	timeStr << ' ' << std::fixed << std::setprecision(3) << millis << "ms ";
	std::cout << startFill << std::setw(width) << std::setfill(fill) << std::left << timeStr.str() << std::endl;
}

template<typename Fun, typename... Args>
void measureAndPrint(const std::string& title, Fun f, Args&&... args) {
	const auto start = std::chrono::high_resolution_clock::now();
	const auto result = f(std::forward<Args>(args)...);
	const auto end = std::chrono::high_resolution_clock::now();
	auto micros = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	double millis = micros / 1000 + (micros % 1000) / 1000.0;
	printResultAndTime(title, result, millis);
}

}