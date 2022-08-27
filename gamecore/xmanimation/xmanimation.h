/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _XMANIMATION_H
#define _XMANIMATION_H

#include "../../math/vector.h"
#include "../../math/mat4.h"
#include <gl/gl.h>

enum {
    HEAD = 0,
    BODY = 1,
    FEET = 2
};

struct xmaAnimation {
    int id;
    char animName[256];
    int animNumBoneAffectNames;
    char boneAffectName[256][256];
    int animType;

    int animStartFrame;
    int animEndFrame;
    int animFramerate;
};

struct xmaBoundingSphere {
    xmaBoundingSphere() {
        type = -1;
    }

    int type;
    vec3 center;
    float radius;
};


struct xmaRotationKeyFrame {
    xmaRotationKeyFrame() {
        time = 0.0f;
        rotation = vec3(0, 0, 0);
    }

    float time;
    vec3 rotation;
};

struct xmaPositionKeyFrame {
    xmaPositionKeyFrame() {
        time = 0.0f;
        position = vec3(0, 0, 0);
    }

    float time;
    vec3 position;
};

struct xmaBone {
    xmaBone() {
        isBarrel = false;
        isBullet = false;
        isWeaponPosition = false;
        isSpinePosition = false;
    };

    short int parent;
    char boneName[256];
    char boneParent[256];
    vec3 rotation;
    vec3 position;
    mat4 abs;
    mat4 rel;
    mat4 final;
    bool isBarrel;
    bool isBullet;
    bool isWeaponPosition;
    bool isSpinePosition;

    unsigned short int iNumPositionKeys;
    unsigned short int iNumRotationKeys;
    xmaPositionKeyFrame *positionKey;
    xmaRotationKeyFrame *rotationKey;
};

struct xmaObject {
    vec3 *vertex;
    vec3 *tangent;
    vec3 *bitangent;
    vec3 *normal;
    vec2 *texcoord;
    int *boneid;

    vec3 *animverts;
    vec3 *animnormals;
    vec3 *animtangents;
    vec3 *animbitangents;

    vec3 min;
    vec3 max;
    vec3 center;
    float radius;
    int iNumVertices;
    char name[256];
    char texturePath[1024];
    short int textureFilter;
    float opacity;
    bool bReflectable;
    GLuint uDiffuseMap;
    GLuint uNormalMap;
    GLuint uSpecularMap;

    GLuint uVertexVBO;
    GLuint uTexCoordVBO;
    GLuint uTangentVBO;
    GLuint uNormalVBO;
};

class xmAnimation {

public:
    xmAnimation();

    ~xmAnimation();

    bool Load(char *path, bool isCharacter = false);

    bool setAnimation(int FirstFrame, int LastFrame, int AnimFPS, bool playAnimation);

    bool setAnimationByID(int id, bool bPlay);

    bool setBlendedAnimation(int animID1, int animID2, int AnimFPS, bool playAnimation);

    void drawSpark();

    void FPSPerson();

    void draw();

    mat4 GetWeaponPosition() {
        return weaponPosition;
    }

    mat4 GetSpine2Position() {
        return spine2Position;
    }

    vec3 GetBulletPosition() {
        return bulletPosition;
    }

    vec3 GetBarrelPosition() {
        return barrelPosition;
    }

    int GetCurrentAnimationID(void) {
        return iCurrentAnimationID;
    }

    bool isAnimationInHalf(void) {
        return fAnimationTime >= anim[iCurrentAnimationID].animEndFrame - anim[iCurrentAnimationID].animStartFrame / 2;
    }

    void resetToAnimation(int id) {
        //miluju kdyz prsi..
        //netrapi me potom, ze sedim za komplem a programuju jako nejakej nerd ..
        if (id < 0 || id > iNumAnimations) return;
        printf("%d\n", id);
        bPlay = false;
        fAnimationTime = (float) anim[id].animStartFrame;
    }

    void RotateSpine(vec3 angle);

    void RotatePelvis(vec3 angle);

    xmaObject *object;
    xmaBone *bone;
    xmaAnimation *anim;

    xmaBoundingSphere *bbSphere;
    int bBones[32];
    int iNumIndexedBBs;
    mat4 finalMatrix;

    int iNumObjects;
    int iNumAnimations;
    int iNumBones;
    int iLastFrame;
    int iFirstFrame;
    int iCurrentAnimationID;
    float iFramerate;
    float fAnimationTime;
    float BlendedAnimationTime[2];
    bool bFirstTime;
    bool bPlay;
    int iPrevFrame;
    int iCurrFrame;
    float fInterpolation;
    GLuint spark;
    int counter;
    float sparkTime;

    int iSpineIndex;
    int iPelvisIndex;

    vec3 SpineRotation;
    vec3 PelvisRotation;

    vec3 bulletPosition;
    vec3 barrelPosition;
    mat4 weaponPosition;
    mat4 spine2Position;


private:
    char headerTitle[255];
    char header[7];

};


#endif
