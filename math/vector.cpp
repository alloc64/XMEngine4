/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "vector.h"

float Absolute(float num) {
    if (num < 0) return (0 - num);
    return num;
}


vec3 Compute2Vectors(vec3 vStart, vec3 vEnd) {
    vec3 vVector;

    vVector.x = vEnd.x - vStart.x;
    vVector.y = vEnd.y - vStart.y;
    vVector.z = vEnd.z - vStart.z;

    Normalize(vVector);
    return vVector;
}

vec3 Cross(vec3 vVector1, vec3 vVector2) {
    vec3 vNormal;

    vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
    vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
    vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

    return vNormal;
}

float Dot(vec3 vVector1, vec3 vVector2) {
    return (float) ((vVector1.x * vVector2.x) + (vVector1.y * vVector2.y) + (vVector1.z * vVector2.z));
}

float VectorLength(vec3 vNormal) {
    return (float) sqrt((vNormal.x * vNormal.x) + (vNormal.y * vNormal.y) + (vNormal.z * vNormal.z));
}

float pow(float pow) {
    return pow * pow;
}

vec3 Normalize(vec3 vVector) {
    float magnitude = VectorLength(vVector);
    vVector = vVector / magnitude;

    return vVector;
}

vec3 Normal(vec3 vPolygon[]) {
    vec3 vVector1 = vPolygon[2] - vPolygon[0];
    vec3 vVector2 = vPolygon[1] - vPolygon[0];

    vec3 vNormal = Cross(vVector1, vVector2);
    vNormal = Normalize(vNormal);
    return vNormal;
}


float Distance(vec3 vPoint1, vec3 vPoint2) {
    double distance = sqrt((vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
                           (vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) +
                           (vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z));

    return (float) distance;
}

bool isZeroVector(vec3 v) {
    if ((v.x == 0.0f) && (v.y == 0.0f) && (v.z == 0.0f)) return true;
    return false;
}

bool signum(float value) {
    if (value < 0.0f) return 1; else return 0;
}

vec3 reflect(vec3 vec, vec3 normal) {
    return vec - 2.f * Dot(vec, normal) * normal;
};


