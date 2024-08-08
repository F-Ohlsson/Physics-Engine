#pragma once
#include <iostream>
#include "glm.hpp"

class vec3 {
public:
	union
	{
		float v[3];
		struct
		{
			float x;
			float y;
			float z;
		};
	};



	vec3() {
		for (int i = 0; i < 3; i++) {
			this->v[i] = 0;
		}
	}

	vec3(float x, float y, float z) {
		float input[3] = { x,y,z };
		insertValues(input);
	}


	vec3(vec3 const& v) {
		float input[3] = { v.x,v.y,v.z };
		insertValues(input);
	}

	vec3(glm::vec3 const& v) {
		float input[3] = { v.x,v.y,v.z };
		insertValues(input);
	}
	
	vec3& operator=(vec3 const& rhs) {
		float input[3] = { rhs.x,rhs.y,rhs.z };
		insertValues(input);
		return *this;
	}
	
	vec3 operator-() {
		return vec3(this->x * -1, this->y * -1, this->z * -1);
	}

	vec3 operator+(vec3 const& rhs) {
		return vec3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
	}

	vec3& operator+=(vec3 const& rhs) {
		float input[3] = { rhs.x,rhs.y,rhs.z };
		for (int i = 0; i < 3; i++) {
			this->v[i] += input[i];
		}
		return *this;
	}

	vec3 operator-(vec3 const& rhs) {
		return vec3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
	}

	vec3 operator-=(vec3 const& rhs) {
		float input[3] = { rhs.x,rhs.y,rhs.z };
		for (int i = 0; i < 3; i++) {
			this->v[i] -= input[i];
		}
		return *this;
	}

	vec3 operator *(float const scalar) {
		return vec3(this->x * scalar, this->y * scalar, this->z * scalar);
	}

	vec3& operator *=(float const scalar) {
		for (int i = 0; i < 3; i++) {
			this->v[i] *= scalar;
		}
		return *this;
	}

	bool operator==(vec3 const& rhs) {
		bool check = false;
		if (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z)
			check = true;
		return check;
	}

	bool operator!=(vec3 const& rhs) {
		bool check = true;
		if (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z)
			check = false;
		return check;

	}

	float& operator[](unsigned int const i) {
		if ((i < 0) || (i > 2)) {
			std::cout << "Error: Index outside range, returning index 0.\n";
			return this->v[0];
		}

		return this->v[i];
	}
	

	vec3& insertValues(float const* input) {

		for (int i = 0; i < 3; i++) {
			this->v[i] = input[i];
		}
		return *this;
	}

	glm::vec3 ToGlmVec3() {
		glm::vec3 result;
		for (int i = 0; i < 3; i++) {
			result[i] = this->v[i];
		}
		return result;
	}
};

namespace MathlibVec3 {

inline float Dot(vec3 const& a, vec3 const& b) {
	return float(a.x * b.x + a.y * b.y + a.z * b.z);
}

inline float Length(vec3 const& v) {
	return float(sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2)));
}

inline vec3 Cross(vec3 const& a, vec3 const& b) {
	vec3 temp = vec3();

	temp.x = a.y * b.z - a.z * b.y;
	temp.y = a.z * b.x - a.x * b.z;
	temp.z = a.x * b.y - a.y * b.x;

	return temp;
}

inline vec3 Normalize(vec3 const& v) {
	float vectorlen = MathlibVec3::Length(v);
	return vec3(v.x / vectorlen, v.y / vectorlen, v.z / vectorlen);

}
}