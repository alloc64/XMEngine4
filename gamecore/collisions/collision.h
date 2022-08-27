/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _COLLISION_H
#define _COLLISION_H

vec3
CollisionWithLevel(vec3 v0, vec3 v1, vec3 v2, char *objectName, float objectRadius, vec3 objectCenter, vec3 position,
                   float radius, vec3 playerSize);

vec3 CollisionWithLevel(vec3 v0, vec3 v1, vec3 v2, vec3 playerSize, vec3 position);

float PointTriangleDistanceSquared(vec3 P, vec3 v0, vec3 v1, vec3 v2, vec3 *ClosestPoint);

bool IsPolygonBackFacing(vec3 v0, vec3 normal);


inline bool RaySphereIntersect(vec3 origin, vec3 direction, vec3 center, float radius, float &distance) {
    //Vilem Otte kod, diky ti ;D
    //direkce musi byt normalizovana + v tomhle enginu otocena :X
    vec3 rayToSphere = center - origin;
    float rayToSphereLength = Dot(rayToSphere, rayToSphere);
    float intersectPoint = Dot(rayToSphere, direction);
    if (intersectPoint < 0.0f) return false;
    float squaredPoint = (radius * radius) - rayToSphereLength + (intersectPoint * intersectPoint);
    if (squaredPoint < 0.0f) return false;
    distance = intersectPoint - sqrtf(squaredPoint);
    return distance > 0.0;
}

inline bool IntersectsRaySphere(vec3 p, vec3 d, vec3 cntr, float r) {
    vec3 diff = cntr - p;
    float sqrDist = Dot(diff, diff);
    float vDot = Dot(diff, d);

    if (vDot >= 0.0f) {
        float dP = r * r - sqrDist + vDot * vDot;
        if (dP < 0.0f) { return false; }
        float t = vDot - sqrt(dP);
        vec3 intersect = p + d * t;
        return true;
    }
    return false;
}

inline bool RayIntersectsTriangle(vec3 p, vec3 d, vec3 v0, vec3 v1, vec3 v2, float &t) {
    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;

    vec3 h = Cross(d, e2);
    float a = Dot(e1, h);

    //if (a > -0.00001 && a < 0.00001) return false;

    float f = 1.0f / a;
    vec3 s = p - v0;
    float u = f * (Dot(s, h));

    if (u < 0.0 || u > 1.0) return false;

    vec3 q = Cross(s, e1);
    float v = f * Dot(d, q);

    if (v < 0.0 || u + v > 1.0) return false;

    t = f * Dot(e2, q);

    return (t > 0.00001);
}

inline bool TestIntersionPlane(vec3 planeNormal, vec3 planePosition, vec3 position, vec3 direction) {
    float dot = Dot(direction, planeNormal);
    if ((dot < 0.0) && (dot > -0.0)) return false;
    float inter = (Dot(planeNormal, planePosition - position)) / dot;

    if (inter < -0.0) return 0;

    vec3 normal = planeNormal;
    float dist = inter;

    return true;
}


inline bool VertexInTriangle(vec3 vtxPoint, vec3 vtx0, vec3 vtx1, vec3 vtx2) {
    vec3 vec0 = Normalize(vtxPoint - vtx0);
    vec3 vec1 = Normalize(vtxPoint - vtx1);
    vec3 vec2 = Normalize(vtxPoint - vtx2);

    double dAngle = acos(Dot(vec0, vec1)) + acos(Dot(vec1, vec2)) + acos(Dot(vec2, vec0));
    if (fabs(dAngle - 2 * PI) < 0.5) return true; else return false;
}

inline char isInfPlus(const float &x) {
    float a;
    *(unsigned int *) &a = (unsigned int) 0x7f800000;
    return (x == a);
}

inline char isInfMinus(const float &x) {
    float a;
    *(unsigned int *) &a = (unsigned int) 0xff800000;
    return (x == a);
}

inline bool SameSide(vec3 p1, vec3 p2, vec3 a, vec3 b) {
    vec3 cp1 = Cross(b - a, p1 - a);
    vec3 cp2 = Cross(b - a, p2 - a);
    if (Dot(cp1, cp2) >= 0) return true; else return false;
}

inline bool PointInPoly(vec3 p, vec3 a, vec3 b, vec3 c) {
    if (SameSide(p, a, b, c) && SameSide(p, b, a, c) && SameSide(p, c, a, b)) return true; else return false;
}


inline bool PointInTriangle(vec3 P, vec3 A, vec3 B, vec3 C) {
    vec3 v0 = C - A;
    vec3 v1 = B - A;
    vec3 v2 = P - A;

    float Dot00 = Dot(v0, v0);
    float Dot01 = Dot(v0, v1);
    float Dot02 = Dot(v0, v2);
    float Dot11 = Dot(v1, v1);
    float Dot12 = Dot(v1, v2);

    float invDenom = 1 / (Dot00 * Dot11 - Dot01 * Dot01);
    float u = (Dot11 * Dot02 - Dot01 * Dot12) * invDenom;
    float v = (Dot00 * Dot12 - Dot01 * Dot02) * invDenom;

    if ((u > 0) && (v > 0) && (u + v < 1)) return false; else return true;
}

inline double AngleBetweenVectors(vec3 a, vec3 b) {
    double angle = acos(Dot(a, b) / (VectorLength(a) * VectorLength(b)));
    if (isInfPlus((float) angle) || isInfMinus((float) angle))return 0;
    return (angle);
}

inline bool PointInsidePolygon(vec3 point, vec3 a, vec3 b, vec3 c) {
    vec3 pa = a - point;
    vec3 pb = b - point;
    vec3 pc = c - point;
    double angle = AngleBetweenVectors(pa, pb);
    angle += AngleBetweenVectors(pb, pc);
    angle += AngleBetweenVectors(pc, pa);
    const double MATCH_FACTOR = 0.99;
    if (angle >= (MATCH_FACTOR * (2.0 * PI))) return true;
    return false;
}

inline bool PointInSphere(vec3 P, float r, vec3 Q) {
    vec3 PQ = Q - P;
    float pq2 = Dot(PQ, PQ);
    if (pq2 > pow(r)) return false; else return true;
}

inline bool SphereInSphere(vec3 vSphereCenter1, float fSphereRadius1, vec3 vSphereCenter2, float fSphereRadius2) {
    if (VectorLength(vSphereCenter1 + vSphereCenter2) < fSphereRadius1 + fSphereRadius2) return true;
    else return false;
}

inline vec3 ClosestPointOnLine(vec3 a, vec3 b, vec3 point) {
    vec3 ap = point - a;            // vector from a to point
    vec3 ab = b - a;                // vector from a to b
    float d = Distance(a, b);        // = Distance( a, b);  // length Line
    Normalize(a - b);                // normalized direction vector from a to b
    float t = Dot(ab, ap);    // ab is unit vector, t is distance from a to point projected on line ab
    if (t <= 0) return a;        // point projected on line ab is out of line, closest point on line is a
    if (t >= d) return b;        // point projected on line ab is out of line, closest point on line is b
    return a + t * ab;                // point away from a on t length in direction ab
}

inline bool EdgeSphereCollision(vec3 center, float radius, vec3 a, vec3 b, vec3 c) {
    if (Distance(center, ClosestPointOnLine(a, b, center)) < radius) return true;
    if (Distance(center, ClosestPointOnLine(b, c, center)) < radius) return true;
    if (Distance(center, ClosestPointOnLine(c, a, center)) < radius) return true;
    return false;
}

/*
inline bool RayIntersectTriangle(vec3 rayDirection, vec3 rayPosition, vec3 v0, vec3 v1, vec3 v2)
{
    vec3 Edge1 = v1 - v0;
    vec3 Edge2 = v2 - v0;
    vec3 Normal = Cross(Edge1, Edge2);
    float fGamma = -Dot(rayDirection, Normal);

    //if (fGamma < 0.00001) return false;
    
    vec3 b = rayPosition - v0;
    float fLambda = Dot(b, Normal) / fGamma;
    
    if (fLambda >= 0.0f && fLambda <= 1.0f)
    {
        vec3 u = Cross(b, rayDirection);
        float u1 = Dot(Edge2, u) / fGamma;
        float u2 = -Dot(Edge1, u) / fGamma;
        
        if (u1 + u2 <= 1.0f && u1 >= 0.0f && u2 >= 0.0f)
        {
            return true;
        }
    }
    
    return false;
}*/

inline bool RayTriangleIntersection(vec3 rayOrigin, vec3 rayDirection, vec3 v0, vec3 v1, vec3 v2) {
    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;

    vec3 s1 = Cross(rayDirection, edge2);

    float divisor = Dot(s1, edge1);
    if (divisor == 0.0) return false;
    float invDivisor = 1 / divisor;

    vec3 distance = rayOrigin - v0;
    float barycCoord_1 = Dot(distance, s1) * invDivisor;
    if (barycCoord_1 < 0.0 || barycCoord_1 > 1.0) return false;

    vec3 s2 = Cross(distance, edge1);
    float barycCoord_2 = Dot(rayDirection, s2) * invDivisor;
    if (barycCoord_2 < 0.0 || (barycCoord_1 + barycCoord_2) > 1.0) return false;

    float intersectionDistance = Dot(edge2, s2) * invDivisor;

    //res.distance = intersectionDistance;
    return true;
}


inline bool AABBIntersectsPlane(vec3 min, vec3 max, vec3 normal, float distanceFromOrigin) {
    vec3 diagMin, diagMax;

    if (normal.x >= 0) {
        diagMin.x = min.x;
        diagMax.x = max.x;
    } else {
        diagMin.x = max.x;
        diagMax.x = min.x;
    }

    if (normal.y >= 0) {
        diagMin.y = min.y;
        diagMax.y = max.y;
    } else {
        diagMin.y = max.y;
        diagMax.y = min.y;
    }

    if (normal.z >= 0) {
        diagMin.z = min.z;
        diagMax.z = max.z;
    } else {
        diagMin.z = max.z;
        diagMax.z = min.z;
    }

    float test = Dot(normal, diagMin) + distanceFromOrigin;
    if (test > 0.0f) return false;

    test = Dot(normal, diagMax) + distanceFromOrigin;
    if (test >= 0.0f) return true; else return false;
}

#endif
