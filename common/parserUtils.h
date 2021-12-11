#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace common {

/*
Parses space-delimited string
*/
template<typename T>
std::vector<T> parseArray(const std::string& str) {
	std::istringstream stream(str);
	std::vector<T> data;
	for (T value; stream >> value;) {
		data.push_back(value);
	}
	return data;
}

/*
Parses space-delimited string and puts T into Wrapper class
*/
template<typename T, typename Wrapper>
std::vector<Wrapper> parseArray(const std::string& str) {
	std::istringstream stream(str);
	std::vector<Wrapper> data;
	for (T value; stream >> value;) {
		data.push_back(Wrapper{value});
	}
	return data;
}

/*
Parses string ignoring specified delimiter
*/
template<typename T>
std::vector<T> parseArray(const std::string& str, char delimiter) {
	std::istringstream stream(str);
	std::vector<T> data;
	for (T value; stream >> value;) {
		data.push_back(value);
		if (stream.peek() == delimiter) {
			stream.ignore();
		}
	}
	return data;
}

/*
Parses string ignoring specified delimiter and puts T into Wrapper class
*/
template<typename T, typename Wrapper>
std::vector<Wrapper> parseArray(const std::string& str, char delimiter) {
	std::istringstream stream(str);
	std::vector<Wrapper> data;
	for (T value; stream >> value;) {
		data.push_back(Wrapper{ value });
		if (stream.peek() == delimiter) {
			stream.ignore();
		}
	}
	return data;
}

}