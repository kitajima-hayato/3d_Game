#include"MakeMatrix.h"

#include<cmath>
#include <cassert>


Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
    Matrix4x4 result;
    result.m[0][0] = 1 / aspectRatio * (1 / std::tan(fovY / 2));   result.m[0][1] = 0;                        result.m[0][2] = 0;                                                               result.m[0][3] = 0;
    result.m[1][0] = 0;                                            result.m[1][1] = 1 / std::tan(fovY / 2);   result.m[1][2] = 0;                                                               result.m[1][3] = 0;
    result.m[2][0] = 0;                                            result.m[2][1] = 0;                        result.m[2][2] = farClip / (farClip - nearClip);                                  result.m[2][3] = 1;
    result.m[3][0] = 0;                                            result.m[3][1] = 0;                        result.m[3][2] = (-nearClip * farClip * std::cos(0.0f)) / (farClip - nearClip);   result.m[3][3] = 0;

    return result;
}

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 ans;

	ans.m[0][0] = 1;
	ans.m[0][1] = 0;
	ans.m[0][2] = 0;
	ans.m[0][3] = 0;

	ans.m[1][0] = 0;
	ans.m[1][1] = 1;
	ans.m[1][2] = 0;
	ans.m[1][3] = 0;

	ans.m[2][0] = 0;
	ans.m[2][1] = 0;
	ans.m[2][2] = 1;
	ans.m[2][3] = 0;

	ans.m[3][0] = translate.x;
	ans.m[3][1] = translate.y;
	ans.m[3][2] = translate.z;
	ans.m[3][3] = 1;

	return ans;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 ans;

	ans.m[0][0] = scale.x;
	ans.m[0][1] = 0;
	ans.m[0][2] = 0;
	ans.m[0][3] = 0;

	ans.m[1][0] = 0;
	ans.m[1][1] = scale.y;
	ans.m[1][2] = 0;
	ans.m[1][3] = 0;

	ans.m[2][0] = 0;
	ans.m[2][1] = 0;
	ans.m[2][2] = scale.z;
	ans.m[2][3] = 0;

	ans.m[3][0] = 0;
	ans.m[3][1] = 0;
	ans.m[3][2] = 0;
	ans.m[3][3] = 1;
	return ans;
}



Matrix4x4 MakeRotateXMatrix(float radian) {

	Matrix4x4 ans;
	ans.m[0][0] = 1;
	ans.m[0][1] = 0;
	ans.m[0][2] = 0;
	ans.m[0][3] = 0;

	ans.m[1][0] = 0;
	ans.m[1][1] = std::cos(radian);
	;
	ans.m[1][2] = std::sin(radian);
	;
	ans.m[1][3] = 0;

	ans.m[2][0] = 0;
	ans.m[2][1] = -std::sin(radian);
	;
	ans.m[2][2] = std::cos(radian);
	;
	ans.m[2][3] = 0;

	ans.m[3][0] = 0;
	ans.m[3][1] = 0;
	ans.m[3][2] = 0;
	ans.m[3][3] = 1;

	return ans;
}

Matrix4x4 MakeRotateYMatrix(float radian) {

	Matrix4x4 ans;
	ans.m[0][0] = std::cos(radian);
	ans.m[0][1] = 0;
	ans.m[0][2] = -std::sin(radian);
	ans.m[0][3] = 0;

	ans.m[1][0] = 0;
	ans.m[1][1] = 1;
	ans.m[1][2] = 0;
	ans.m[1][3] = 0;

	ans.m[2][0] = std::sin(radian);
	;
	ans.m[2][1] = 0;
	ans.m[2][2] = std::cos(radian);
	;
	ans.m[2][3] = 0;

	ans.m[3][0] = 0;
	ans.m[3][1] = 0;
	ans.m[3][2] = 0;
	ans.m[3][3] = 1;

	return ans;
}

Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 ans;
	ans.m[0][0] = std::cos(radian);
	ans.m[0][1] = std::sin(radian);
	ans.m[0][2] = 0;
	ans.m[0][3] = 0;

	ans.m[1][0] = -std::sin(radian);
	ans.m[1][1] = std::cos(radian);
	ans.m[1][2] = 0;
	ans.m[1][3] = 0;

	ans.m[2][0] = 0;
	ans.m[2][1] = 0;
	ans.m[2][2] = 1;
	ans.m[2][3] = 0;

	ans.m[3][0] = 0;
	ans.m[3][1] = 0;
	ans.m[3][2] = 0;
	ans.m[3][3] = 1;

	return ans;
}


Matrix4x4 Add(const Matrix4x4& mt1, const Matrix4x4& mt2) {

	Matrix4x4 ans;

	ans.m[0][0] = mt1.m[0][0] + mt2.m[0][0];
	ans.m[0][1] = mt1.m[0][1] + mt2.m[0][1];
	ans.m[0][2] = mt1.m[0][2] + mt2.m[0][2];
	ans.m[0][3] = mt1.m[0][3] + mt2.m[0][3];

	ans.m[1][0] = mt1.m[1][0] + mt2.m[1][0];
	ans.m[1][1] = mt1.m[1][1] + mt2.m[1][1];
	ans.m[1][2] = mt1.m[1][2] + mt2.m[1][2];
	ans.m[1][3] = mt1.m[1][3] + mt2.m[1][3];

	ans.m[2][0] = mt1.m[2][0] + mt2.m[2][0];
	ans.m[2][1] = mt1.m[2][1] + mt2.m[2][1];
	ans.m[2][2] = mt1.m[2][2] + mt2.m[2][2];
	ans.m[2][3] = mt1.m[2][3] + mt2.m[2][3];

	ans.m[3][0] = mt1.m[3][0] + mt2.m[3][0];
	ans.m[3][1] = mt1.m[3][1] + mt2.m[3][1];
	ans.m[3][2] = mt1.m[3][2] + mt2.m[3][2];
	ans.m[3][3] = mt1.m[3][3] + mt2.m[3][3];

	return ans;
}

Matrix4x4 Subtract(const Matrix4x4& mt1, const Matrix4x4& mt2) {

	Matrix4x4 ans;

	ans.m[0][0] = mt1.m[0][0] - mt2.m[0][0];
	ans.m[0][1] = mt1.m[0][1] - mt2.m[0][1];
	ans.m[0][2] = mt1.m[0][2] - mt2.m[0][2];
	ans.m[0][3] = mt1.m[0][3] - mt2.m[0][3];

	ans.m[1][0] = mt1.m[1][0] - mt2.m[1][0];
	ans.m[1][1] = mt1.m[1][1] - mt2.m[1][1];
	ans.m[1][2] = mt1.m[1][2] - mt2.m[1][2];
	ans.m[1][3] = mt1.m[1][3] - mt2.m[1][3];

	ans.m[2][0] = mt1.m[2][0] - mt2.m[2][0];
	ans.m[2][1] = mt1.m[2][1] - mt2.m[2][1];
	ans.m[2][2] = mt1.m[2][2] - mt2.m[2][2];
	ans.m[2][3] = mt1.m[2][3] - mt2.m[2][3];

	ans.m[3][0] = mt1.m[3][0] - mt2.m[3][0];
	ans.m[3][1] = mt1.m[3][1] - mt2.m[3][1];
	ans.m[3][2] = mt1.m[3][2] - mt2.m[3][2];
	ans.m[3][3] = mt1.m[3][3] - mt2.m[3][3];

	return ans;
}

Matrix4x4 Multiply(const Matrix4x4& mt1, const Matrix4x4& mt2) {

	Matrix4x4 ans = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {

				ans.m[i][j] += mt1.m[i][k] * mt2.m[k][j];
			}
		}
	}
	return ans;
}

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	float A = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]
		- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
		+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	Matrix4x4 result = {};
	result.m[0][0] = 1 / A * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]);
	result.m[0][1] = 1 / A * (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);
	result.m[0][2] = 1 / A * (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);
	result.m[0][3] = 1 / A * (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

	result.m[1][0] = 1 / A * (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]);
	result.m[1][1] = 1 / A * (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]);
	result.m[1][2] = 1 / A * (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]);
	result.m[1][3] = 1 / A * (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]);

	result.m[2][0] = 1 / A * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]);
	result.m[2][1] = 1 / A * (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);
	result.m[2][2] = 1 / A * (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);
	result.m[2][3] = 1 / A * (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);

	result.m[3][0] = 1 / A * (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]);
	result.m[3][1] = 1 / A * (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]);
	result.m[3][2] = 1 / A * (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]);
	result.m[3][3] = 1 / A * (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]);

	return result;
}

Matrix4x4 Transpose(const Matrix4x4& mt1) {

	Matrix4x4 ans;

	ans.m[0][0] = mt1.m[0][0];
	ans.m[0][1] = mt1.m[1][0];
	ans.m[0][2] = mt1.m[2][0];
	ans.m[0][3] = mt1.m[3][0];

	ans.m[1][0] = mt1.m[0][1];
	ans.m[1][1] = mt1.m[1][1];
	ans.m[1][2] = mt1.m[2][1];
	ans.m[1][3] = mt1.m[3][1];

	ans.m[2][0] = mt1.m[0][2];
	ans.m[2][1] = mt1.m[1][2];
	ans.m[2][2] = mt1.m[2][2];
	ans.m[2][3] = mt1.m[3][2];

	ans.m[3][0] = mt1.m[0][3];
	ans.m[3][1] = mt1.m[1][3];
	ans.m[3][2] = mt1.m[2][3];
	ans.m[3][3] = mt1.m[3][3];

	return ans;
}

Matrix4x4 MakeIdentity4x4() {

	Matrix4x4 ans;

	ans.m[0][0] = 1;
	ans.m[0][1] = 0;
	ans.m[0][2] = 0;
	ans.m[0][3] = 0;

	ans.m[1][0] = 0;
	ans.m[1][1] = 1;
	ans.m[1][2] = 0;
	ans.m[1][3] = 0;

	ans.m[2][0] = 0;
	ans.m[2][1] = 0;
	ans.m[2][2] = 1;
	ans.m[2][3] = 0;

	ans.m[3][0] = 0;
	ans.m[3][1] = 0;
	ans.m[3][2] = 0;
	ans.m[3][3] = 1;

	return ans;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 rotateXYZ = Multiply(Multiply(MakeRotateXMatrix(rotate.x), MakeRotateYMatrix(rotate.y)), MakeRotateZMatrix(rotate.z));
	return Multiply(Multiply(MakeScaleMatrix(scale), rotateXYZ), MakeTranslateMatrix(translate));
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result;
	result.m[0][0] = 2 / right; result.m[0][1] = 0;						result.m[0][2] = 0;						 result.m[0][3] = 0;
	result.m[1][0] = 0;			result.m[1][1] = 2 / (top - bottom);		result.m[1][2] = 0;						 result.m[1][3] = 0;
	result.m[2][0] = 0;			result.m[2][1] = 0;						result.m[2][2] = 1 / (farClip - nearClip); result.m[2][3] = 0;
	result.m[3][0] = (left + right) / (left - right); result.m[3][1] = (top + bottom) / (bottom - top); result.m[3][2] = nearClip / (nearClip - farClip); result.m[3][3] = 1;
	return result;
}

//Quaternion LookAtQuaternion(const Vector3& from, const Vector3& to, const Vector3& up) {
//	// from から to への方向ベクトルを計算
//	Vector3 forward = Normalize(to - from);
//
//	// right ベクトルを計算（up と forward の外積）
//	Vector3 right = Normalize(Cross(up, forward));
//
//	// 新しい up ベクトルを計算
//	Vector3 newUp = Cross(forward, right);
//
//	// LookAt 行列を設定
//	Matrix4x4 lookAtMatrix;
//	lookAtMatrix.m[0][0] = right.x;
//	lookAtMatrix.m[0][1] = right.y;
//	lookAtMatrix.m[0][2] = right.z;
//	lookAtMatrix.m[0][3] = 0.0f;
//
//	lookAtMatrix.m[1][0] = newUp.x;
//	lookAtMatrix.m[1][1] = newUp.y;
//	lookAtMatrix.m[1][2] = newUp.z;
//	lookAtMatrix.m[1][3] = 0.0f;
//
//	lookAtMatrix.m[2][0] = forward.x;
//	lookAtMatrix.m[2][1] = forward.y;
//	lookAtMatrix.m[2][2] = forward.z;
//	lookAtMatrix.m[2][3] = 0.0f;
//
//	lookAtMatrix.m[3][0] = 0.0f;
//	lookAtMatrix.m[3][1] = 0.0f;
//	lookAtMatrix.m[3][2] = 0.0f;
//	lookAtMatrix.m[3][3] = 1.0f;
//
//	// 行列をクォータニオンに変換
//	return MatrixToQuaternion(lookAtMatrix);
//}
//Quaternion Slerp(Quaternion q1, Quaternion q2, float t)
//{
//	// クォータニオンの内積を計算
//	float dot = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
//
//	// ドット積が負の場合、逆の方向に補間するために q2 を反転
//	if (dot < 0.0f) {
//		q2.x = -q2.x;
//		q2.y = -q2.y;
//		q2.z = -q2.z;
//		q2.w = -q2.w;
//		dot = -dot;
//	}
//
//	// 補間係数を使った係数の計算
//	const float threshold = 0.9995f;
//	if (dot > threshold) {
//		// ドット積が閾値を超えた場合、線形補間を実行（角度が小さいため）
//		Quaternion result = {
//			q1.x + t * (q2.x - q1.x),
//			q1.y + t * (q2.y - q1.y),
//			q1.z + t * (q2.z - q1.z),
//			q1.w + t * (q2.w - q1.w)
//		};
//		return Normalize(result); // 結果を正規化
//	}
//
//	// 角度の計算
//	float theta_0 = std::acos(dot);        // θ0 = q1 と q2 間の角度
//	float theta = theta_0 * t;             // θ = t に対応する角度
//
//	// 係数の計算
//	float sin_theta = std::sin(theta);
//	float sin_theta_0 = std::sin(theta_0);
//
//	float s1 = std::cos(theta) - dot * sin_theta / sin_theta_0;
//	float s2 = sin_theta / sin_theta_0;
//
//	// 補間結果の計算
//	Quaternion result = {
//		s1 * q1.x + s2 * q2.x,
//		s1 * q1.y + s2 * q2.y,
//		s1 * q1.z + s2 * q2.z,
//		s1 * q1.w + s2 * q2.w
//	};
//	return result;
//}