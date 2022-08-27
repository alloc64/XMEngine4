/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef MAT4_H
#define MAT4_H

//#include "vector.h"
#include <memory.h>
#include <math.h>
#include <assert.h>
#include <gl/gl.h>
#include "vector.h"

#define DEGTORAD(degree) ((degree) * (3.141592654f / 180.0f))
#define RADTODEG(radian) ((radian) * (180.0f / 3.141592654f))

class mat4 {
public:
    float mat[16];

    mat4() {
        Identity();
    }

    mat4 operator+(const mat4 &other);

    mat4 operator*(const float scalar);

    mat4 operator*(const mat4 &m2);

    vec3 operator*(vec3 vec);

    mat4 &operator*=(mat4 &m2) {
        *this = m2 * *this;
        return *this;
    }

    float operator[](int i) const { return mat[i]; }

    void operator()(int i, float x) { if (i >= 0 && i <= 15)mat[i] = x; }

    void Inverse();

    float Determinant();

    inline vec3 GetTranslate(void) {
        return vec3(mat[12], mat[13], mat[14]);
    }

    void Identity() {
        memset(mat, 0, sizeof(float) * 16);
        mat[0] = mat[5] = mat[10] = mat[15] = 1;
        assert(mat);
    }

    void operator()(float *m) {
        memcpy(mat, m, sizeof(float) * 16);
    }


    void Rotate(float x, float y, float z);

    void Rotate(vec3 rot);

    void Translate(float x, float y, float z) {
        assert(mat);
        mat[12] = x;
        mat[13] = y;
        mat[14] = z;
    }

    void Translate(vec3 vec) {
        assert(mat);
        mat[12] = vec.x;
        mat[13] = vec.y;
        mat[14] = vec.z;
    }

    void GetMatrix(float *mat) { memcpy(mat, mat, sizeof(float) * 16); }

private:

};


vec3 TransformVec3ByMat4(vec3 vector, mat4 matrix);


#endif
