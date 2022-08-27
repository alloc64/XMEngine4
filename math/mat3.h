/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef MAT3_H
#define MAT3_H

#include <string.h>

class Matrix {
    friend class vec3;

    friend class Quaternion;

private:
    float Mat[3][3];

public:
    inline Matrix(float a00, float a01, float a02,
                  float a10, float a11, float a12,
                  float a20, float a21, float a22) {
        Mat[0][0] = a00;
        Mat[0][1] = a01;
        Mat[0][2] = a02;
        Mat[1][0] = a10;
        Mat[1][1] = a11;
        Mat[1][2] = a12;
        Mat[2][0] = a20;
        Mat[2][1] = a21;
        Mat[2][2] = a22;
    }

    inline Matrix(void) {

    }

    inline Matrix(float fscale) {
        SetToScaledIdentity(fscale);
    };

    friend vec3 &vec3::operator*=(const Matrix &Mat);

    Matrix operator=(Quaternion Quat);

    Matrix operator*=(float Scale);

    Matrix operator*=(Matrix Mat) {
        Matrix Temp(*this);
        this->GetMatMul(Temp, Mat);
        return *this;
    }

    Matrix Invert(void) {
        Matrix Source = *this;
        float Determinant = Source.Determinant();
        float InvDet = 1.0f / Determinant;
        int PlusArray[5] = {0, 1, 2, 0, 1};

        for (int i = 0; i < 3; i++) {
            int ip1 = PlusArray[i + 1], ip2 = PlusArray[i + 2];

            for (int j = 0; j < 3; j++) {
                int jp1 = PlusArray[j + 1], jp2 = PlusArray[j + 2];

                Mat[j][i] =
                        (Source.Mat[ip1][jp1] * Source.Mat[ip2][jp2]) - (Source.Mat[ip1][jp2] * Source.Mat[ip2][jp1]);
                Mat[j][i] *= InvDet;                //remember this is the transpose of the adjoint matrix
            }
        }
        return *this;
    }

    float Determinant(void) {
        return ((Mat[1][1] * Mat[2][2]) - (Mat[1][2] * Mat[2][1])) * Mat[0][0] +
               ((Mat[1][2] * Mat[2][0]) - (Mat[1][0] * Mat[2][2])) * Mat[0][1] +
               ((Mat[1][0] * Mat[2][1]) - (Mat[1][1] * Mat[2][0])) * Mat[0][2];
    }

    Matrix GetMatMul(Matrix Mat1, Matrix Mat2) {
        for (int Row = 0; Row < 3; Row++) {
            float MatVal0 = Mat1.Mat[Row][0];
            float MatVal1 = Mat1.Mat[Row][1];
            float MatVal2 = Mat1.Mat[Row][2];


            for (int Col = 0; Col < 3; Col++) {
                float Result;

                Result = MatVal0 * Mat2.Mat[0][Col];
                Result += MatVal1 * Mat2.Mat[1][Col];
                Result += MatVal2 * Mat2.Mat[2][Col];

                Mat[Row][Col] = Result;
            }
        }

        return *this;
    }

    Matrix Inverse(void) {
        Matrix Temp = *this;

        return Temp.Invert();
    }

    Matrix operator*(Matrix M) const {
        Matrix Temp = *this;

        return Temp *= M;
    }

    float *GetMat(void) { return &Mat[0][0]; }

    float GetElement(int i, int j) { return Mat[i][j]; }

    void SetElement(int i, int j, float Value) { Mat[i][j] = Value; }

    void SetToScaledIdentity(float Scale) {
        *this = Matrix(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

        Mat[0][0] = Mat[1][1] = Mat[1][1] = Scale;
    }


};

#include "quat.h"

inline Matrix Matrix::operator=(Quaternion Quat) {
    float D1, D2, D3, D4, D5, D6, D7, D8, D9;

    D1 = (Quat.V.x * Quat.V.x) * 2.0f;
    D2 = (Quat.V.y * Quat.V.y) * 2.0f;
    D3 = (Quat.V.z * Quat.V.z) * 2.0f;

    float RTimesTwo = Quat.R * 2.0f;
    D4 = Quat.V.x * RTimesTwo;
    D5 = Quat.V.y * RTimesTwo;
    D6 = Quat.V.z * RTimesTwo;

    D7 = (Quat.V.x * Quat.V.y) * 2.0f;
    D8 = (Quat.V.x * Quat.V.z) * 2.0f;
    D9 = (Quat.V.y * Quat.V.z) * 2.0f;

    Mat[0][0] = 1.0f - D2 - D3;
    Mat[0][1] = D7 - D6;
    Mat[0][2] = D8 + D5;
    Mat[1][0] = D7 + D6;
    Mat[1][1] = 1.0f - D1 - D3;
    Mat[1][2] = D9 - D4;
    Mat[2][0] = D8 - D5;
    Mat[2][1] = D9 + D4;
    Mat[2][2] = 1.0f - D1 - D2;

    return *this;
}

inline float *glMatrix(Matrix M) {
    static float mat[16];
    float m[3][3];

    memcpy(m, M.GetMat(), sizeof(m));
    memset(mat, 0, sizeof(mat));

    mat[0] = m[0][0];
    mat[1] = m[1][0];
    mat[2] = m[2][0];
    mat[3] = 0.0f;
    mat[4] = m[0][1];
    mat[5] = m[1][1];
    mat[6] = m[2][1];
    mat[7] = 0.0f;
    mat[8] = m[0][2];
    mat[9] = m[1][2];
    mat[10] = m[2][2];
    mat[11] = 0.0f;
    mat[12] = 0.0f;
    mat[13] = 0.0f;
    mat[14] = 0.0f;
    mat[15] = 1.0f;

    return mat;
}

#endif
