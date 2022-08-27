/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../../main.h"

extern Camera camera;

bool IsPolygonBackFacing(vec3 v0, vec3 normal) {
    vec3 viewVec = v0 - camera.GetPosition();
    float a = Dot(viewVec, normal);

    return (a > 0.0f);
}

float PointTriangleDistanceSquared(vec3 P, vec3 v0, vec3 v1, vec3 v2, vec3 *ClosestPoint) {

    vec3 E0 = (v1 - v0);
    vec3 E1 = (v2 - v0);
    vec3 kDiff = (v0 - P);

    float fA00 = Dot(E0, E0);
    float fA01 = Dot(E0, E1);
    float fA11 = Dot(E1, E1);
    float fB0 = Dot(kDiff, E0);
    float fB1 = Dot(kDiff, E1);
    float fC = Dot(kDiff, kDiff);

    float fDet = (float) fabs(fA00 * fA11 - fA01 * fA01);
    float fS = fA01 * fB1 - fA11 * fB0;
    float fT = fA01 * fB0 - fA00 * fB1;
    float fSqrDist;


    if (fabs(fDet) < 0.000000001f)
        return 100000000.0f;

    if (fS + fT <= fDet) {
        if (fS < (float) 0.0) {
            if (fT < (float) 0.0)  // region 4
            {
                if (fB0 < (float) 0.0) {
                    fT = (float) 0.0;
                    if (-fB0 >= fA00) {
                        fS = (float) 1.0;
                        fSqrDist = fA00 + ((float) 2.0) * fB0 + fC;
                    } else {
                        fS = -fB0 / fA00;
                        fSqrDist = fB0 * fS + fC;
                    }
                } else {
                    fS = (float) 0.0;
                    if (fB1 >= (float) 0.0) {
                        fT = (float) 0.0;
                        fSqrDist = fC;
                    } else if (-fB1 >= fA11) {
                        fT = (float) 1.0;
                        fSqrDist = fA11 + ((float) 2.0) * fB1 + fC;
                    } else {
                        fT = -fB1 / fA11;
                        fSqrDist = fB1 * fT + fC;
                    }
                }
            } else  // region 3
            {
                fS = (float) 0.0;
                if (fB1 >= (float) 0.0) {
                    fT = (float) 0.0;
                    fSqrDist = fC;
                } else if (-fB1 >= fA11) {
                    fT = (float) 1.0;
                    fSqrDist = fA11 + ((float) 2.0) * fB1 + fC;
                } else {
                    fT = -fB1 / fA11;
                    fSqrDist = fB1 * fT + fC;
                }
            }
        } else if (fT < (float) 0.0)  // region 5
        {
            fT = (float) 0.0;
            if (fB0 >= (float) 0.0) {
                fS = (float) 0.0;
                fSqrDist = fC;
            } else if (-fB0 >= fA00) {
                fS = (float) 1.0;
                fSqrDist = fA00 + ((float) 2.0) * fB0 + fC;
            } else {
                fS = -fB0 / fA00;
                fSqrDist = fB0 * fS + fC;
            }
        } else  // region 0
        {
            // minimum at interior point
            float fInvDet = ((float) 1.0) / fDet;
            fS *= fInvDet;
            fT *= fInvDet;
            fSqrDist = fS * (fA00 * fS + fA01 * fT + ((float) 2.0) * fB0) +
                       fT * (fA01 * fS + fA11 * fT + ((float) 2.0) * fB1) + fC;
        }
    } else {
        float fTmp0, fTmp1, fNumer, fDenom;

        if (fS < (float) 0.0)  // region 2
        {
            fTmp0 = fA01 + fB0;
            fTmp1 = fA11 + fB1;
            if (fTmp1 > fTmp0) {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00 - 2.0f * fA01 + fA11;
                if (fNumer >= fDenom) {
                    fS = (float) 1.0;
                    fT = (float) 0.0;
                    fSqrDist = fA00 + ((float) 2.0) * fB0 + fC;
                } else {
                    fS = fNumer / fDenom;
                    fT = (float) 1.0 - fS;
                    fSqrDist = fS * (fA00 * fS + fA01 * fT + 2.0f * fB0) +
                               fT * (fA01 * fS + fA11 * fT + ((float) 2.0) * fB1) + fC;
                }
            } else {
                fS = (float) 0.0;
                if (fTmp1 <= (float) 0.0) {
                    fT = (float) 1.0;
                    fSqrDist = fA11 + ((float) 2.0) * fB1 + fC;
                } else if (fB1 >= (float) 0.0) {
                    fT = (float) 0.0;
                    fSqrDist = fC;
                } else {
                    fT = -fB1 / fA11;
                    fSqrDist = fB1 * fT + fC;
                }
            }
        } else if (fT < (float) 0.0)  // region 6
        {
            fTmp0 = fA01 + fB1;
            fTmp1 = fA00 + fB0;
            if (fTmp1 > fTmp0) {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00 - ((float) 2.0) * fA01 + fA11;
                if (fNumer >= fDenom) {
                    fT = (float) 1.0;
                    fS = (float) 0.0;
                    fSqrDist = fA11 + ((float) 2.0) * fB1 + fC;
                } else {
                    fT = fNumer / fDenom;
                    fS = (float) 1.0 - fT;
                    fSqrDist = fS * (fA00 * fS + fA01 * fT + ((float) 2.0) * fB0) +
                               fT * (fA01 * fS + fA11 * fT + ((float) 2.0) * fB1) + fC;
                }
            } else {
                fT = (float) 0.0;
                if (fTmp1 <= (float) 0.0) {
                    fS = (float) 1.0;
                    fSqrDist = fA00 + ((float) 2.0) * fB0 + fC;
                } else if (fB0 >= (float) 0.0) {
                    fS = (float) 0.0;
                    fSqrDist = fC;
                } else {
                    fS = -fB0 / fA00;
                    fSqrDist = fB0 * fS + fC;
                }
            }
        } else  // region 1
        {
            fNumer = fA11 + fB1 - fA01 - fB0;
            if (fNumer <= (float) 0.0) {
                fS = (float) 0.0;
                fT = (float) 1.0;
                fSqrDist = fA11 + ((float) 2.0) * fB1 + fC;
            } else {
                fDenom = fA00 - 2.0f * fA01 + fA11;
                if (fNumer >= fDenom) {
                    fS = (float) 1.0;
                    fT = (float) 0.0;
                    fSqrDist = fA00 + ((float) 2.0) * fB0 + fC;
                } else {
                    fS = fNumer / fDenom;
                    fT = (float) 1.0 - fS;
                    fSqrDist = fS * (fA00 * fS + fA01 * fT + ((float) 2.0) * fB0) +
                               fT * (fA01 * fS + fA11 * fT + ((float) 2.0) * fB1) + fC;
                }
            }
        }
    }


    if (ClosestPoint) *ClosestPoint = v0 + (fS * E0) + (fT * E1);

    return (float) fabs(fSqrDist);
}


vec3
CollisionWithLevel(vec3 v0, vec3 v1, vec3 v2, char *objectName, float objectRadius, vec3 objectCenter, vec3 position,
                   float radius, vec3 playerSize) {
    //normala z ClosestPointu :/ it sux
    //(Normalize(position - cp))
    //krasnej kod pocitani normaly facu objektu
    //uplne nahovno kdyz je grafik pica a otaci vrcholy //mirriojojor
    //XD
    //vec3 normal = Normalize(Cross(v0 - v1, v1 - v2));
    vec3 pos[3];
    vec3 cp[3];
    float distance[3];

    //if(camera.bCrouch)
    //{
    //    vec3 p = vec3(position.x, position.y-2,position.z);
    //    vec3 cl;
    //    float dist = PointTriangleDistanceSquared(p, v0, v1, v2, &cl);
    //    vec3 normal = (Normalize(p - cl));
    //
    //    if(dist < pow(radius))// && normal.y >= 0.9f)
    //    {
    //        camera.bCrouch = true;
    //        //mainPlayer.UpdatePlayerSize(vec3(15,20,15));
    //    }else{
    //        camera.bCrouch = false;
    //    }
    //}

    pos[0] = vec3(position.x, position.y - 2, position.z);
    pos[1] = vec3(position.x, position.y - (playerSize.y / 2) + 2, position.z);
    pos[2] = vec3(position.x, position.y - playerSize.y + 4, position.z);

    for (int i = 0; i < 3; i++) {
        if (!SphereInSphere(pos[i], radius - (i * 2), objectCenter, objectRadius)) continue;
        distance[i] = PointTriangleDistanceSquared(pos[i], v0, v1, v2, &cp[i]);

        if (distance[i] < pow(radius - (i * 2))) {
            vec3 normal = (Normalize(pos[i] - cp[i]));
            if (normal.y >= 0.99f && i == 2) {
                camera.bColliding = true;
                camera.bJump = false;
            } else {
                camera.bColliding = false;
            }
            //nasel sem zebrik, tak proste pokracuju podle slidu a pricitam vysku na Y
            //if(strstr(objectName,"ladder_")!=NULL && normal.x >= 1.0f || normal.z >= 1.0f)
            //{
            //        position += (normal* ((radius-(i*2)) - sqrt(distance[i])));
            //        position.y++;
            //
            //}else{
            position += (normal * ((radius - (i * 2)) - sqrt(distance[i])));
            return position;
            //}
        } else {
            if (i == 2) {
                //ceknu jestli kdyz nekoliduju tak pustim cykl ve kterym zkousim jestli ve vysce height nenarazim na kolizi
                //kdyz jo, posunu tam pozici, kdyz ne tak to mrdam a returnu tu pozici kterou potrebuju :P
                for (float height = 0; height < 4.0f; height += 1.0) {
                    vec3 stepCP;
                    vec3 pos = position;
                    float stepDis;
                    stepDis = PointTriangleDistanceSquared(vec3(pos.x, (pos.y - playerSize.y) + height + 2.0, pos.z),
                                                           v0, v1, v2, &stepCP);
                    vec3 normal = (Normalize(pos - stepCP));
                    if (stepDis < pow(radius)) {
                        if (normal.y >= 0.99f) {
                            camera.bColliding = true;
                            camera.bJump = false;
                        } else {
                            camera.bColliding = false;
                        }
                        pos += (normal * ((radius) - sqrt(stepDis)));
                        return vec3(pos.x, pos.y, pos.z);
                    } else {
                        camera.bColliding = false;
                    }
                }
            }
        }
    }
    return position;
}

/*
vec3 CollisionWithLevel(vec3 v0, vec3 v1, vec3 v2, vec3 playerSize, vec3 position)
{
	float t;
	float s1, s2, mscale;
	vec3 vR, vQ1, vQ2;
	vec3 vIntersect;
	vec3 cV;

	vec3 cdA[100];

    vec3 pl = playerSize;
	cdA[0] = vec3(0.0f, pl.y, 0.0);
	cdA[1] = vec3( pl.x,  pl.y-25,   0.0f);
	cdA[2] = vec3(-pl.x,  pl.y-25,   0.0f);
	cdA[3] = vec3(0.0f,   pl.y-25,   pl.z);
	cdA[4] = vec3(0.0f,   pl.y-25,  -pl.z);

	cdA[5] = vec3( pl.x,  pl.y-25,    pl.z);
	cdA[6] = vec3(-pl.x,  pl.y-25,    pl.z);
	cdA[7] = vec3(-pl.x,  pl.y-25,   -pl.z);
	cdA[8] = vec3( pl.x,  pl.y-25,   -pl.z);

	cdA[9] = vec3( pl.x,  pl.y-35,   0.0f);
	cdA[10] = vec3(-pl.x,  pl.y-35,   0.0f);
	cdA[11] = vec3(0.0f,   pl.y-35,   pl.z);
	cdA[12] = vec3(0.0f,   pl.y-35,  -pl.z);

	cdA[13] = vec3( pl.x,  pl.y-35,    pl.z);
	cdA[14] = vec3(-pl.x,  pl.y-35,    pl.z);
	cdA[15] = vec3(-pl.x,  pl.y-35,   -pl.z);
	cdA[16] = vec3( pl.x,  pl.y-35,   -pl.z);

		vec3 normal = Normalize(Cross(v1-v0, v2-v0));
        float dist = -Dot(normal, v0);
        for(int k=0; k < 17; k++)
        {
        	vec3 vS = position + cdA[k];
        	vec3 vV = position - cdA[k];
        	vec3 vTV = vV - vS;
        	s1 = Dot(normal, vS) + dist;
        	s2 = Dot(normal, vV) + dist;

        	if( signum(s1) != signum(s2) )
        	{
        		t = -s1 / Dot(normal, vTV);
        		vIntersect = vS + (vTV*t);
                vR  = vIntersect - v0;
                vQ1 = v1 - v0;
                vQ2 = v2 - v0;

                float detp1 = Dot(vQ1, vQ1)*Dot(vQ2, vQ2);
                float detp2 = Dot(vQ1,vQ2)*Dot(vQ1,vQ2);

                mscale = 1/( detp1 - detp2);
                float matrixA[4] = { Dot(vQ2, vQ2), -Dot(vQ1, vQ2), -Dot(vQ1, vQ2),Dot(vQ1, vQ1) };
                float matrixB[2] = {Dot(vR, vQ1), Dot(vR, vQ2)};

                for(int j=0; j<4;j++) matrixA[j] *= mscale;
                float w1= matrixA[0]*matrixB[0] + matrixA[1]*matrixB[1];
                float w2= matrixA[2]*matrixB[0] + matrixA[3]*matrixB[1];
                float w0 = 1.0f - w1 -w2;

                if(signum(w0) == 0 && signum(w1) == 0 && signum(w2) == 0)
                {
                	vec3 offset;
                	if(t>=0.5f) offset = vIntersect - vV;
                	else 	    offset = vIntersect - vS;

                	position = position + offset;
                }
			}
	}

	return position;
}
*/
