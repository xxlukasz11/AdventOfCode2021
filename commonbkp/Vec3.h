#pragma once

#include <string>
#include <sstream>
#include <ostream>
#include <cmath>

namespace common {

template<typename ValueT>
struct Vec3;

template<typename ValueT>
std::ostream& operator<<(std::ostream& stream, const Vec3<ValueT>& vec) {
	stream << '(' << vec.x << ", " << vec.y << ", " << vec.z << ')';
	return stream;
}

template<typename ValueT>
struct Vec3 {
	ValueT x{ 0 };
	ValueT y{ 0 };
	ValueT z{ 0 };

	std::string toString() const {
		std::ostringstream ss;
		ss << *this;
		return ss.str();
	}

	Vec3& operator=(const Vec3& vec) {
		x = vec.x;
		y = vec.y;
		z = vec.z;
		return *this;
	}

	bool operator<(const Vec3& vec) const {
		if (x != vec.x) {
			return x < vec.x;
		}
		if (y != vec.y) {
			return y < vec.y;
		}
		return z < vec.z;
	}

	bool operator>(const Vec3& vec) const {
		if (x != vec.x) {
			return x > vec.x;
		}
		if (y != vec.y) {
			return y > vec.y;
		}
		return z > vec.z;
	}

	bool operator==(const Vec3& vec) const {
		return x == vec.x && y == vec.y && z == vec.z;
	}

	Vec3 operator-(const Vec3& vec) const {
		return { x - vec.x, y - vec.y, z - vec.z };
	}

	Vec3 operator+(const Vec3& vec) const {
		return { x + vec.x, y + vec.y, z + vec.z };
	}

	template<typename T>
	Vec3 operator*(const T value) const {
		return { x * value, y * value, z * value };
	}

	Vec3& operator+=(const Vec3& vec) {
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}

	Vec3& operator-=(const Vec3& vec) {
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	}

	template<typename T>
	Vec3& operator*=(const T value) {
		x *= value;
		y *= value;
		z *= value;
		return *this;
	}

	double distance(const Vec3& vec) const {
		return sqrt(pow(x - vec.x, 2) + pow(y - vec.y, 2) + pow(z - vec.z, 2));
	}

	Vec3 relativeVec(const Vec3& vec) const {
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

}