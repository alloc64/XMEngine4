/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _FRUSTUM_H
#define _FRUSTUM_H

#include "../../main.h"

class Frustum {
public:
    void Update();

    bool PointInFrustum(vec3 vec);

    bool SphereInFrustum(vec3 Center, float Radius);

    bool PolygonInFrustum(vec3 v0, vec3 v1, vec3 v2);

    bool IsPointOccluded(vec3 center);

private:
    float FrustumMatrix[6][4];
    float ClipMatrix[16];
    float ProjectionMatrix[16];
    float ModelviewMatrix[16];
    float t;
};

#endif
