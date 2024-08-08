#pragma once
#include<iostream>
#include"vec4.h"
#include"vec3.h"
class mat4;
namespace MathlibMat4 {
mat4 Transpose(mat4 const& m);
}

//4x4 matrix
class mat4 {
public:
	vec4 m[4];

	//Creates 4x4 identity matrix
	mat4() {
		for (int i = 0; i < 4; i++) {
			m[i] = vec4();
			for (int j = 0; j < 4; j++) {
				if (i == j)
					m[i][j] = 1;
			}
		}
		
	}

	mat4(vec4 const& r0, vec4 const& r1, vec4 const& r2, vec4 const& r3) {
		vec4 input[4] = { r0,r1,r2,r3 };
		for (int i = 0; i < 4; i++) {
			m[i] = input[i];
		}
	}

	mat4(mat4 const& im) {
		for (int i = 0; i < 4; i++) {
			m[i] = im.m[i];
		}
	}

	mat4(glm::mat4 const& im) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				m[i][j] = im[i][j];
			}
		}
	}

	mat4& operator=(mat4 const& rhs) {
		for (int i = 0; i < 4; i++) {
			m[i] = rhs.m[i];
		}

		return *this;
	}


	const mat4 operator*(mat4 const& rhs) const {
		mat4 result = mat4();
		mat4 colMat;
		mat4 lhsTemp = *this;
		mat4 rhsTemp = rhs;

		//Convert row major to column major

		colMat = MathlibMat4::Transpose(lhsTemp);


		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				result[i][j] = 0;
				result[i][j] = MathlibVec4::Dot(rhsTemp[i], colMat[j]);
			}
		}

 		return result;
	}

	const vec4 operator*(vec4 const& rhs) const {
		vec4 result = vec4();
		vec4 rhsTemp = rhs;

		for (int j = 0; j < 4; j++) {
			
			//column
			for (int k = 0; k < 4; k++) {
				vec4 lhsTemp = this->m[k];
				result[j] += (lhsTemp[j] * rhsTemp[k]);
			}
			 
		}
		int i = 0;
		return result;
	}

	bool operator==(mat4 const& rhs) {
		bool result = true;
		for (int i = 0; i < 4; i++) {
			vec4 temp = rhs.m[i];
			for (int j = 0; j < 4; j++) {
				if (this->m[i][j] != temp[j])
					result = false;
			}
		}

		return result;
	}

	bool operator!=(mat4 const& rhs) {
		bool result = false;
		for (int i = 0; i < 4; i++) {
			vec4 temp = rhs.m[i];
			for (int j = 0; j < 4; j++) {
				if (this->m[i][j] != temp[j])
					result = true;
			}
		}

		return result;
	}

	vec4& operator[](unsigned int const i) {
		if ((i < 0) || (i > 3)) {
			std::cout << "Error: Index outside range, returning index 0.\n";
			return this->m[0];
		}
			
		return this->m[i];
	}
};

namespace MathlibMat4 {

	inline glm::mat4 ToGlmMat4(mat4 inMatrix) {
		glm::mat4 outMatrix;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				outMatrix[i][j] = inMatrix[i][j];
			}
		}

		//for (int i = 0; i < 4; i++) {
		//	if (outMatrix[i][3] == 0)
		//		outMatrix[i][3] = 1;
		//}

		return outMatrix;
	}


	inline float Determinant(mat4 const& m) {
		mat4 tempMatrix = m;
		//prepare zeroes except for center line
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == j) {
					continue;
				}
				else {
					vec4 rowi = tempMatrix.m[i];
					vec4 rowj = tempMatrix.m[j];
					float multiplyingFactor = 0;;
					if (rowi[i] != 0) //prevents division by zero
						multiplyingFactor = (rowj[i] / rowi[i]);
					for (int k = 0; k < 4; k++) {
						rowj[k] -= (multiplyingFactor * rowi[k]);
					}
					tempMatrix.m[j] = rowj;
				}

			}
		}
		//multiply center line
		float det = 1;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == j) {
					vec4 rowi = tempMatrix.m[i];
					det *= rowi[i];
					break;
				}
			}
		}

		return det;
	}

	inline mat4 Inverse(mat4 const& m) {
		mat4 inverse = mat4();
		if (MathlibMat4::Determinant(m) == 0) {
			return inverse;
		}

		mat4 tempMatrix = m;
		//prepare zeroes besides center line
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == j) {
					//Normalize row
					vec4 rowi = tempMatrix.m[i];
					vec4 inverseRowi = inverse.m[i];

					float multiplyingFactor = 0;
					if (rowi[i] != 0)
						multiplyingFactor = 1 / rowi[i];

					for (int k = 0; k < 4; k++) {
						rowi[k] *= multiplyingFactor;
						inverseRowi[k] *= multiplyingFactor;

					}
					tempMatrix.m[i] = rowi;
					inverse.m[i] = inverseRowi;
					continue;
				}
				else {
					vec4 rowi = tempMatrix.m[i];
					vec4 rowj = tempMatrix.m[j];
					vec4 inverseRowi = inverse.m[i];
					vec4 inverseRowj = inverse.m[j];

					float multiplyingFactor = 0;
					if (rowi[i] != 0) //prevents division by zero
						multiplyingFactor = (rowj[i] / rowi[i]);

					for (int k = 0; k < 4; k++) {
						rowj[k] -= (multiplyingFactor * rowi[k]);
						inverseRowj[k] -= (multiplyingFactor * inverseRowi[k]);
					}
					tempMatrix.m[j] = rowj;
					inverse.m[j] = inverseRowj;

				}

			}
		}

		return inverse;
	}

	inline mat4 Transpose(mat4 const& m) {
		mat4 transpose = mat4();
		mat4 tempMat = m;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				transpose[j][i] = tempMat[i][j];
			}
		}
		return transpose;
	}

	inline mat4 RotationX(float const rad) {
		vec4 row1 = vec4({ 1,0,0,0 });
		vec4 row2 = vec4({ 0,cos(rad),-sin(rad),0 });
		vec4 row3 = vec4({ 0,sin(rad),cos(rad),0 });
		vec4 row4 = vec4({ 0,0,0,1 });

		mat4 xRotMatrix = mat4(row1, row2, row3, row4);


		return MathlibMat4::Transpose(xRotMatrix);
	}

	inline mat4 RotationY(float const rad) {
		vec4 row1 = vec4({ cos(rad),0,sin(rad),0 });
		vec4 row2 = vec4({ 0,1,0,0 });
		vec4 row3 = vec4({ -sin(rad),0,cos(rad),0 });
		vec4 row4 = vec4({ 0,0,0,1 });

		mat4 yRotMatrix = mat4(row1, row2, row3, row4);


		return MathlibMat4::Transpose(yRotMatrix);
	}

	inline mat4 RotationZ(float const rad) {
		vec4 row1 = vec4({ cos(rad),-sin(rad),0,0 });
		vec4 row2 = vec4({ sin(rad),cos(rad),0,0 });
		vec4 row3 = vec4({ 0,0,1,0 });
		vec4 row4 = vec4({ 0,0,0,1 });

		mat4 zRotMatrix = mat4(row1, row2, row3, row4);


		return MathlibMat4::Transpose(zRotMatrix);
	}

	inline mat4 RotationAxis(vec3 const& v, float const rad) {
		vec4 row1 = vec4({
			cos(rad) + float(pow(v.x,2)) * (1 - cos(rad)),
			(v.x * v.y) * (1 - cos(rad)) - (v.z * sin(rad)),
			(v.x * v.z) * (1 - cos(rad)) + v.y * sin(rad),0 });
		vec4 row2 = vec4({
			(v.y * v.x) * (1 - cos(rad)) + v.z * sin(rad),
			cos(rad) + float(pow(v.y,2)) * (1 - cos(rad)),
			(v.y * v.z) * (1 - cos(rad)) - v.x * sin(rad),0 });
		vec4 row3 = vec4({
			(v.z * v.x) * (1 - cos(rad)) - v.y * sin(rad),
			(v.z * v.y) * (1 - cos(rad)) + v.x * sin(rad),
			cos(rad) + float(pow(v.z,2)) * (1 - cos(rad)),0 });
		vec4 row4 = vec4({ 0,0,0,1 });

		mat4 zRotMatrix = mat4(row1, row2, row3, row4);

		return MathlibMat4::Transpose(zRotMatrix);
}
}