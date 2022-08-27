/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef QUAT_H
#define QUAT_H

#include "mat3.h"

class Quaternion {
private:
    vec3 V;
    float R;

public:
    Quaternion(void) {

    };

    Quaternion(vec3 InitV, float InitR) : V(InitV), R(InitR) {

    };

    Quaternion(vec3 InitRPY) {
        float SinX = Sine(InitRPY.x * 0.5f);
        float CosX = Cosine(InitRPY.x * 0.5f);
        float SinY = Sine(InitRPY.y * 0.5f);
        float CosY = Cosine(InitRPY.y * 0.5f);
        float SinZ = Sine(InitRPY.z * 0.5f);
        float CosZ = Cosine(InitRPY.z * 0.5f);

        V.x = CosZ * SinX * CosY + SinZ * CosX * SinY;
        V.y = CosZ * CosX * SinY - SinZ * SinX * CosY;
        V.z = SinZ * CosX * CosY - CosZ * SinX * SinY;

        R = CosZ * CosX * CosY + SinZ * SinX * SinY;
    }

    Quaternion operator*=(float Scalar) {
        V *= Scalar;
        R *= Scalar;

        return *this;
    }

    Quaternion operator+=(Quaternion OtherQuat) {
        V.x += OtherQuat.V.x;
        V.y += OtherQuat.V.y;
        V.z += OtherQuat.V.z;
        R += OtherQuat.R;

        return *this;
    }

    Quaternion operator*=(Quaternion OtherQuat) {
        vec3 D1 = OtherQuat.V;
        vec3 D2 = V;

        D1 *= R;
        D2 *= OtherQuat.R;

        R = (R * OtherQuat.R) - (V * OtherQuat.V);

        V = Cross(V, OtherQuat.V);
        V += D1;
        V += D2;

        return *this;
    }

    Quaternion operator=(Matrix Mat) {
        float S, Tr = Mat.GetElement(0, 0) + Mat.GetElement(1, 1) + Mat.GetElement(2, 2);

        if (Tr >= 0.0f) {
            S = sqrt(Tr + 1.0f);
            R = S * 0.5f;
            S = 0.5f / S;
            V.x = (Mat.GetElement(2, 1) - Mat.GetElement(1, 2)) * S;
            V.y = (Mat.GetElement(0, 2) - Mat.GetElement(2, 0)) * S;
            V.z = (Mat.GetElement(1, 0) - Mat.GetElement(0, 1)) * S;
        } else {
            int i = 0;
            if (Mat.GetElement(1, 1) > Mat.GetElement(0, 0)) i = 1;
            if (Mat.GetElement(2, 2) > Mat.GetElement(i, i)) i = 2;

            switch (i) {
                case 0:
                    S = (Mat.GetElement(0, 0) - (Mat.GetElement(1, 1) + Mat.GetElement(2, 2))) + 1.0f;
                    S = sqrt(S);
                    V.x = 0.5f * S;
                    S = 0.5f / S;
                    V.y = (Mat.GetElement(0, 1) + Mat.GetElement(1, 0)) * S;
                    V.z = (Mat.GetElement(2, 0) + Mat.GetElement(0, 2)) * S;
                    R = (Mat.GetElement(2, 1) - Mat.GetElement(1, 2)) * S;
                    break;

                case 1:
                    S = (Mat.GetElement(1, 1) - (Mat.GetElement(2, 2) + Mat.GetElement(0, 0))) + 1.0f;
                    S = sqrt(S);
                    V.y = 0.5f * S;
                    S = 0.5f / S;
                    V.z = (Mat.GetElement(1, 2) + Mat.GetElement(2, 1)) * S;
                    V.x = (Mat.GetElement(0, 1) + Mat.GetElement(1, 0)) * S;
                    R = (Mat.GetElement(0, 2) - Mat.GetElement(2, 0)) * S;
                    break;

                case 2:
                    S = (Mat.GetElement(2, 2) - (Mat.GetElement(0, 0) + Mat.GetElement(1, 1))) + 1.0f;
                    S = sqrt(S);
                    V.z = 0.5f * S;
                    S = 0.5f / S;
                    V.x = (Mat.GetElement(2, 0) + Mat.GetElement(0, 2)) * S;
                    V.y = (Mat.GetElement(1, 2) + Mat.GetElement(2, 1)) * S;
                    R = (Mat.GetElement(1, 0) - Mat.GetElement(0, 1)) * S;
                    break;
            }
        }

        return *this;
    }


    friend Matrix Matrix::operator=(Quaternion Quat);


    Quaternion operator*(Quaternion OtherQuat)    //4 dimensional dot product
    {
        Quaternion Temp = *this;

        return Temp *= OtherQuat;
    }

    Quaternion operator*(float k) {
        Quaternion Temp = *this;

        return Temp *= k;
    }

    float GetLengthSquared(void) {
        float ret = (V.GetLengthSquared() + (R * R));

        return ret;
    }

    float GetLength(void) {
        return sqrt(GetLengthSquared());
    }

    bool Normalise(void) {
        float Length = GetLength();

        if (Length == 0.0f)
            return false;

        (*this) *= (1.0f / Length);            //divide by length

        return true;
    }

    void Orientate(vec3 NormStartVec, vec3 NormEndVec) {
        float CosTheta = NormStartVec * NormEndVec;
        float CosTd2Sq = (CosTheta + 1.0f) * 0.5f;
        float CosTd2 = sqrt(CosTd2Sq);
        float SinTd2 = sqrt(1.0f - CosTd2Sq);

        vec3 RotAxis;
        if (CosTheta < -0.999f)                                //very nearly opposite
        {
            vec3 NonParraVec(0.0f, 1.0f, 0.0f);
            RotAxis = Cross(NonParraVec, NormStartVec);

            if (RotAxis.GetLengthSquared() < 0.001f)        //Still aligned
            {
                NonParraVec = vec3(1.0f, 0.0f, 0.0f);
                RotAxis = Cross(NonParraVec, NormStartVec);
            }
        } else {
            RotAxis = Cross(NormStartVec, NormEndVec);
        }

        if (SinTd2 < 0.0004f)                                //vec3s lined up, no need for an axis
        {
            RotAxis = vec3(0.0f, 0.0f, 0.0f);                //No axis needed if no rotation
        } else {
            RotAxis.Normalise();

        }

        R = CosTd2;
        V.SetAndScale(SinTd2, RotAxis);
    }

    void Quaternion::CreateRotation(vec3 RotAxis, float AngleInRads) {
        float Thetad2 = AngleInRads * 0.5f;
        float SinTd2 = Sine(Thetad2);
        float CosTd2 = Cosine(Thetad2);

        R = CosTd2;
        V.SetAndScale(SinTd2, RotAxis);
    }

    float GetR(void) { return R; }

    vec3 GetV(void) { return V; }

};


#endif



