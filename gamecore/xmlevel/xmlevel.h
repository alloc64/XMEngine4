/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _XMLEVEL_H
#define _XMLEVEL_H

#include "..\..\math\vector.h"
#include <gl\gl.h>

#define XM_STATIC_ENTITY 0
#define XM_DYNAMIC_ENTITY 1

#define SPOT_LIGHT 0
#define POINT_LIGHT 1

struct xmlPlayerSpawn {
    char name[256];
    vec3 center;
};

struct xmlEntity {
    char name[256];
    vec3 rotation;
    vec3 center;
    vec3 min;
    vec3 max;
    float radius;
    int id;
    int wid;
    int damage;
    bool entityRisen;
    bool damaged;
    float entityWeight;
};

struct xmlLight // 52B
{
    //point light specific
    float radius;

    //spotlight specific
    float falloff;
    float hotspot;
    vec3 target;

    short int type;
    color3f color;
    vec3 center;
};


struct xmlObject {
    xmlObject() {
        bVisible = true;
    }

    vec3 *vertex;
    vec3 BoxVertices[36];
    vec3 BoxNormals[12];
    vec3 *tangent;
    vec3 *bitangent;
    vec3 *normal;
    vec2 *texcoord;
    xmFace *face;
    vec3 min;
    vec3 max;
    vec3 center;

    long int iNumFaces;
    long int iNumVertices;

    char name[256];
    char texturePath[1024];
    char normalMapPath[1024];
    char specularMapPath[1024];
    short int textureFilter;

    float opacity;
    float shininess;
    float radius;

    bool bUseAlphaTest;
    bool bUseCollBox;
    bool bNoColl;
    bool bReflective;
    bool bRefractive;
    bool bVisible;

    GLuint uDiffuseMap;
    GLuint uNormalMap;
    GLuint uSpecularMap;
    GLuint uCubeMap;

    GLuint uVertexVBO;
    GLuint uTexCoordVBO;
    GLuint uNormalVBO;
    GLuint uTangentVBO;
    GLuint uFaceVBO;
    GLuint uCBoxVBO;
    GLuint uOcclusionID;

};


class xmLevel {
public:
    xmLevel() {
        dt = 0.0f;
        bAnimateEntity = false;
        bDoorsOpened = false;
        bShowIHand = false;
        iCurrAnimation = 1;
    };

    bool Load(char *path);

    void Draw(void);

    bool CreateShadowMap(void);

    void RenderToShadowMap(void);

    void RenderToCubeMap(void);

    void RenderLevel(void);

    void RenderEntities(bool bCollide);

    void CalculateFlashlightMatrix(void);

    void SetLights(void);


    bool CreateSkybox(char **paths, float size);

    void RenderSkybox(void);

    bool isOccluded(int o);

    short int GetNumObjectsInScene() {
        return iNumObjects + iNumEntities;
    }

    vec3 GetRandomPlayerSpawn() {
        int id = 0;
        if (iNumSpawns > 1) while (id == 0) id = rand() % iNumSpawns;
        return spawn[id].center;
    }

    int iCurrFrame;
    float dt;
    xmlObject *object;
    xmlEntity *entity;
    xmlPlayerSpawn *spawn;
    xmlLight *areaLight;
    xmlLight mainLight;
    short int iNumObjects;
    short int iNumEntities;
    short int iNumSpawns;
    short int iNumLights;
    short int iSkyNum;
    int iNVObjects;
    int iNVEntities;
    int iNVWEntities;
    int iNCObjects;
    int iNOObjects;
    int iNumVisibleObjects;
    int iNumVisibleAreaLights;
    vec3 worldMin;
    vec3 worldMax;
    vec3 worldCenter;
    vec2 worldLightPos;
    float worldRadius;

    GLuint FBOid;
    GLuint ShadowMapID;
    GLuint CubeMapID;
    GLuint VolShadowMapID;
    int ShadowMapSize;

    //bone based entity
    bool bAnimateEntity;
    bool bDoorsOpened;
    bool bWannaOpenDoors;
    bool bShowIHand;
    int iCurrAnimation;
    float ModelViewMat[16];

private:
    char headerTitle[255];
    char header[8];

};

inline char *makeOptionalTexturePath(char *fullPath) {
    char *texturePaths[3] =
            {
                    "low",
                    "medium",
                    "high"
            };

    char finalpath[1024];
    sprintf(finalpath, "textures/%s/%s", texturePaths[2/*gui.iTexDetail*/], fullPath);
    return finalpath;
}

inline char *makeCubeMapPath(char *path, char *objectName) {
    char *file = new char[1024];
    strcpy(file, path);

    while (strstr(file, "\\")) {
        file = strstr(file, "\\");
        if (file) file += 1;
    }

    file = strtok(file, ".");
    strcat(file, "/");
    strcat(file, objectName);
    strcat(file, "_cubemap.xmc");
    return file;
}

inline vec2 GetLightScreenCoord(vec3 light) {
    double modelView[16];
    double projection[16];
    int viewport[4];
    double winX = 0.0;
    double winY = 0.0;
    double winZ = 0.0;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    gluProject(light.x, light.y, light.z, modelView, projection, viewport, &winX, &winY, &winZ);
    //uniformLightX = winX/((float)renderWidth/OFF_SCREEN_RENDER_RATIO);
    //uniformLightY = winY/((float)renderHeight/OFF_SCREEN_RENDER_RATIO) ;
    //printf("Light position zz xo=%f, yo=%f, x=%f, y=%f\n",winX,winY,uniformLightX,uniformLightY);

    return vec2(winX, winY);
}

inline vec3 *GetBoundingBox(vec3 min, vec3 max) {
    vec3 BoxVertices[36];
    BoxVertices[0] = vec3(min.x, min.y, min.z);
    BoxVertices[1] = vec3(min.x, max.y, min.z);
    BoxVertices[2] = vec3(max.x, max.y, min.z);

    BoxVertices[3] = vec3(min.x, min.y, min.z);
    BoxVertices[4] = vec3(max.x, max.y, min.z);
    BoxVertices[5] = vec3(max.x, min.y, min.z);

    BoxVertices[6] = vec3(min.x, min.y, max.z);
    BoxVertices[7] = vec3(max.x, max.y, max.z);
    BoxVertices[8] = vec3(min.x, max.y, max.z);

    BoxVertices[9] = vec3(min.x, min.y, max.z);
    BoxVertices[10] = vec3(max.x, min.y, max.z);
    BoxVertices[11] = vec3(max.x, max.y, max.z);

    BoxVertices[12] = vec3(min.x, min.y, min.z);
    BoxVertices[13] = vec3(max.x, min.y, max.z);
    BoxVertices[14] = vec3(min.x, min.y, max.z);

    BoxVertices[15] = vec3(min.x, min.y, min.z);
    BoxVertices[16] = vec3(max.x, min.y, min.z);
    BoxVertices[17] = vec3(max.x, min.y, max.z);

    BoxVertices[18] = vec3(min.x, max.y, min.z);
    BoxVertices[19] = vec3(min.x, max.y, max.z);
    BoxVertices[20] = vec3(max.x, max.y, max.z);

    BoxVertices[21] = vec3(min.x, max.y, min.z);
    BoxVertices[22] = vec3(max.x, max.y, max.z);
    BoxVertices[23] = vec3(max.x, max.y, min.z);

    BoxVertices[24] = vec3(min.x, min.y, min.z);
    BoxVertices[25] = vec3(min.x, min.y, max.z);
    BoxVertices[26] = vec3(min.x, max.y, max.z);

    BoxVertices[27] = vec3(min.x, min.y, min.z);
    BoxVertices[28] = vec3(min.x, max.y, max.z);
    BoxVertices[29] = vec3(min.x, max.y, min.z);

    BoxVertices[30] = vec3(max.x, min.y, min.z);
    BoxVertices[31] = vec3(max.x, max.y, max.z);
    BoxVertices[32] = vec3(max.x, min.y, max.z);

    BoxVertices[33] = vec3(max.x, min.y, min.z);
    BoxVertices[34] = vec3(max.x, max.y, min.z);
    BoxVertices[35] = vec3(max.x, max.y, max.z);

    return BoxVertices;
}

#endif
