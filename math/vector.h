/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>


#define PI    3.1415926535897932384626433832795
#define DEG    PI/180

struct color3f {
    float r, g, b;
};

class color4f {
public:
    color4f() {
        r = 0;
        g = 0;
        b = 0;
        a = 0;
    }

    color4f(float red, float green, float blue, float alfa) {
        r = red;
        g = green;
        b = blue;
        a = alfa;
    }

    float r, g, b, a;
};

class vec2 {
public:

    vec2() {}

    vec2(float X, float Y) {
        x = X;
        y = Y;
    }

    vec2(float num) {
        x = num;
        y = num;
    }

    vec2 operator+(vec2 vec) {
        return vec2(vec.x + x, vec.y + y);
    }

    vec2 operator-(vec2 vec) {
        return vec2(x - vec.x, y - vec.y);
    }

    vec2 operator*(float num) {
        return vec2(x * num, y * num);
    }

    vec2 operator/(float num) {
        return vec2(x / num, y / num);
    }

    float x, y;
};

class vec3 {
public:
    float x, y, z;

public:
    inline vec3(void) {}

    vec3(float X, float Y, float Z) {
        x = X;
        y = Y;
        z = Z;
    }

    vec3(float num) {
        x = y = z = num;
    }

    friend vec3 operator*(float k, const vec3 V) {
        return vec3(V.x * k, V.y * k, V.z * k);
    }

    inline float operator*(vec3 V) const {
        return (x * V.x) + (y * V.y) + (z * V.z);
    }

    inline vec3 operator*(float Scalar) const {
        return vec3(x * Scalar, y * Scalar, z * Scalar);
    }


    vec3 operator-() {
        return vec3(-x, -y, -z);
    }

    inline vec3 operator+=(vec3 Other) {
        x += Other.x;
        y += Other.y;
        z += Other.z;

        return *this;
    }

    inline vec3 operator+=(float Other) {
        x += Other;
        y += Other;
        z += Other;

        return *this;
    }

    inline vec3 operator-=(vec3 Other) {
        x -= Other.x;
        y -= Other.y;
        z -= Other.z;

        return *this;
    }

    inline vec3 operator-=(float Other) {
        x -= Other;
        y -= Other;
        z -= Other;

        return *this;
    }

    inline vec3 operator*=(float Scalar) {
        x *= Scalar;
        y *= Scalar;
        z *= Scalar;

        return *this;
    }

    inline float GetLengthSquared(void) {
        return (*this) * (*this);
    }

    inline float length(void) {
        return sqrt(GetLengthSquared());
    }


    bool Normalise(bool AllowFatal = false) {
        float Length = length();
        if (Length == 0.0f) return false;
        (*this) *= (1.0f / Length);

        return true;
    }


    vec3 Interpolate(vec3 Vec1, vec3 Vec2, float Fraction) {
        x = Vec1.x * Fraction;
        y = Vec1.y * Fraction;
        z = Vec1.z * Fraction;

        float OneMFrac = 1.0f - Fraction;

        x += Vec2.x * OneMFrac;
        y += Vec2.y * OneMFrac;
        z += Vec2.z * OneMFrac;

        return *this;
    }

    inline vec3 operator/=(float Scalar) {
        *this *= (1.0f / Scalar);

        return *this;
    }

    inline vec3 Scale(vec3 Scalar) {
        x *= Scalar.x;
        y *= Scalar.y;
        z *= Scalar.z;

        return *this;
    }


    inline bool operator!=(vec3 Other) const {
        float Epsilon = 0.0001f;

        if ((fabs(Other.x - x) > Epsilon) ||
            (fabs(Other.y - y) > Epsilon) ||
            (fabs(Other.z - z) > Epsilon)) {
            return true;
        }

        return false;
    }

    inline bool operator==(vec3 Other) const {
        return !((*this) != Other);
    }


    inline vec3 operator+(vec3 fromV) {
        return vec3(x + fromV.x, y + fromV.y, z + fromV.z);
    }

    inline vec3 operator-(vec3 fromV) const {
        return vec3(x - fromV.x, y - fromV.y, z - fromV.z);
    }

    inline vec3 operator/(float Scalar) const {
        return (*this) * (1.0f / Scalar);
    }


    inline vec3 SetAndScale(float Scalar, vec3 Vec) {
        x = Scalar * Vec.x;
        y = Scalar * Vec.y;
        z = Scalar * Vec.z;

        return *this;
    }

};


inline float Pi(void) {
    static float gPi = (float) atan(1.0f) * 4.0f;
    return gPi;
}

inline float TwoPi(void) {
    static float gTwoPi = (float) atan(1.0f) * 8.0f;
    return gTwoPi;
}

inline float Modulo(float x, float div) { return (float) fmod((double) x, (double) div); }

inline float DegreesToRadians(float Degrees) { return Degrees * (Pi() / 180.0f); }

inline float RadiansToDegrees(float Radians) { return Radians * (180.0f / Pi()); }

inline float Sine(float Radians) { return (float) sin(Radians); }

inline float Cosine(float Radians) { return (float) cos(Radians); }

inline float Tangent(float Radians) { return (float) tan((double) Radians); }

inline float ArcTangent(float X) { return (float) atan((double) X); }

inline float ArcTangent(float X, float Y) { return (float) atan((double) X); }

inline float ArcSine(float X) { return (float) asin((double) X); }

inline float ArcCosine(float X) { return (float) acos((double) X); }


float Absolute(float num);

float pow(float pow);

vec3 Compute2Vectors(vec3 vStart, vec3 vEnd);

vec3 Cross(vec3 vec1, vec3 vec2);

float VectorLength(vec3 vNormal);

vec3 Normalize(vec3 vec);

vec3 Normal(vec3 vPolygon[]);

float Distance(vec3 vPoint1, vec3 vPoint2);

float Dot(vec3 vec1, vec3 vec2);

bool isZeroVector(vec3 v);

bool signum(float value);

vec3 reflect(vec3 vec, vec3 normal);


struct xmFace {
    unsigned int a, b, c;
};

inline void
xmCalculateNTB(vec3 VertA, vec3 VertB, vec3 VertC, vec2 UvA, vec2 UvB, vec2 UvC, vec3 *vNormal, vec3 *vBitangent,
               vec3 *vTangent, xmFace face) {
    vec3 tangent;
    vec3 bitangent;
    vec3 normal;

    vec3 v_Edge1 = VertB - VertA;
    vec3 v_Edge2 = VertC - VertA;
    vec2 v_Edge1uvs = UvB - UvA;
    vec2 v_Edge2uvs = UvC - UvA;

    float cp = v_Edge1uvs.y * v_Edge2uvs.x - v_Edge1uvs.x * v_Edge2uvs.y;

    if (cp != 0.0f) {
        float mul = 1.0f / cp;
        tangent = Normalize((v_Edge1 * -v_Edge2uvs.y + v_Edge2 * v_Edge1uvs.y) * mul);
        bitangent = Normalize((v_Edge1 * -v_Edge2uvs.x + v_Edge2 * v_Edge1uvs.x) * mul);
        normal = Normalize(Cross(v_Edge1, v_Edge2));

        vNormal[face.a].x = normal.x;
        vNormal[face.a].y = normal.y;
        vNormal[face.a].z = normal.z;

        vNormal[face.b].x = normal.x;
        vNormal[face.b].y = normal.y;
        vNormal[face.b].z = normal.z;

        vNormal[face.c].x = normal.x;
        vNormal[face.c].y = normal.y;
        vNormal[face.c].z = normal.z;

        vBitangent[face.a].x = bitangent.x;
        vBitangent[face.a].y = bitangent.y;
        vBitangent[face.a].z = bitangent.z;

        vBitangent[face.b].x = bitangent.x;
        vBitangent[face.b].y = bitangent.y;
        vBitangent[face.b].z = bitangent.z;

        vBitangent[face.c].x = bitangent.x;
        vBitangent[face.c].y = bitangent.y;
        vBitangent[face.c].z = bitangent.z;

        vTangent[face.a].x = tangent.x;
        vTangent[face.a].y = tangent.y;
        vTangent[face.a].z = tangent.z;

        vTangent[face.b].x = tangent.x;
        vTangent[face.b].y = tangent.y;
        vTangent[face.b].z = tangent.z;

        vTangent[face.c].x = tangent.x;
        vTangent[face.c].y = tangent.y;
        vTangent[face.c].z = tangent.z;
    }
}


inline void
CalculateTangent(vec3 VertA, vec3 VertB, vec3 VertC, vec2 UvA, vec2 UvB, vec2 UvC, vec3 *vTangent, xmFace face) {
    vec3 v_Edge1 = VertB - VertA;
    vec3 v_Edge2 = VertC - VertA;
    vec2 v_Edge1uvs = UvB - UvA;
    vec2 v_Edge2uvs = UvC - UvA;

    float cp = v_Edge1uvs.y * v_Edge2uvs.x - v_Edge1uvs.x * v_Edge2uvs.y;

    if (cp != 0.0f) {
        float mul = 1.0f / cp;
        vec3 tangent = Normalize((v_Edge1 * -v_Edge2uvs.y + v_Edge2 * v_Edge1uvs.y) * mul);

        vTangent[face.a].x = tangent.x;
        vTangent[face.a].y = tangent.y;
        vTangent[face.a].z = tangent.z;

        vTangent[face.b].x = tangent.x;
        vTangent[face.b].y = tangent.y;
        vTangent[face.b].z = tangent.z;

        vTangent[face.c].x = tangent.x;
        vTangent[face.c].y = tangent.y;
        vTangent[face.c].z = tangent.z;
    }
}


#endif
