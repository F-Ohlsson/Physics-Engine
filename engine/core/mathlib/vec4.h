#pragma once
#include<iostream>
#include "glm.hpp"


class vec4 {
public:
	union
	{
		float v[4];
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
	};



	vec4() {
		for (int i = 0; i < 4; i++) {
			this->v[i] = 0;
		}
	}

	vec4(float x, float y, float z, float w) {
		float input[4] = { x,y,z,w };
		insertValues(input);
	}


	vec4(vec4 const& v) {
		float input[4] = { v.x,v.y,v.z,v.w };
		insertValues(input);
	}

	vec4& operator=(vec4 const& rhs) {
		float input[4] = { rhs.x,rhs.y,rhs.z, rhs.w };
		insertValues(input);
		return *this;
	}

	vec4 operator-() {
		return vec4(this->x * -1, this->y * -1, this->z * -1, this->w * -1);
	}

	vec4 operator+(vec4 const& rhs) {
		return vec4(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w);
	}

	vec4& operator+=(vec4 const& rhs) {
		float input[4] = { rhs.x,rhs.y,rhs.z, rhs.w };
		for (int i = 0; i < 4; i++) {
			this->v[i] += input[i];
		}
		return *this;
	}

	vec4 operator-(vec4 const& rhs) {
		return vec4(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z, this->w - rhs.w);
	}

	vec4 operator-=(vec4 const& rhs) {
		float input[4] = { rhs.x,rhs.y,rhs.z, rhs.w };
		for (int i = 0; i < 4; i++) {
			this->v[i] -= input[i];
		}
		return *this;
	}

	vec4 operator *(float const scalar) {
		return vec4(this->x * scalar, this->y * scalar, this->z * scalar, this->w * scalar);
	}

	vec4& operator *=(float const scalar) {
		for (int i = 0; i < 4; i++) {
			this->v[i] *= scalar;
		}
		return *this;
	}

	bool operator==(vec4 const& rhs) {
		bool check = false;
		if (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w)
			check = true;
		return check;
	}

	bool operator!=(vec4 const& rhs) {
		bool check = true;
		if (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w)
			check = false;
		return check;
	}

	float& operator[](unsigned int const i) {
		if ((i < 0) || (i > 3)) {
			std::cout << "Error: Index outside range, returning index 0.\n";
			return this->v[0];
		}
		return this->v[i];
	}


	vec4& insertValues(float const* input) {

		for (int i = 0; i < 4; i++) {
			this->v[i] = input[i];
		}
		return *this;
	}

};


namespace MathlibVec4 {

inline float Dot(vec4 const& a, vec4 const& b) {
	return float(a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
}

inline float Length(vec4 const& v) {
	return float(sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2) + pow(v.w, 2)));
}

inline vec4 Normalize(vec4 const& v) {
	float vectorlen = MathlibVec4::Length(v);
	return vec4(v.x / vectorlen, v.y / vectorlen, v.z / vectorlen, v.w / vectorlen);
}

inline glm::vec4 ToGlmVec4(vec4 vect) {
	glm::vec4 result;
	for (int i = 0; i < 3; i++) {
		result[i] = vect.v[i];
	}
	return result;
	return result;
}

inline vec4 FromGlmVec4(glm::vec4 glmVect) {
	vec4 result;
	result = { glmVect.x,glmVect.y ,glmVect.z ,glmVect.w };

	return result;
}
}