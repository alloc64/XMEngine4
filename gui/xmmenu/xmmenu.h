/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _XMMENU_H
#define _XMMENU_H

#include "../../math/vector.h"
#include <gl\gl.h>

#define XM_DEFAULT_DIFFUSE_MAP "textures/default/notfound.dds"
#define XM_DEFAULT_NORMAL_MAP "textures/default/notfound_normal.dds"
#define XM_DEFAULT_SPECULAR_MAP "textures/default/notfound_specular.dds"

struct xmmLight {
    color3f color;
    vec3 center;
};

struct xmmCamera {
    vec3 rotation;
    vec3 center;
};

struct xmmObject {
    vec3 *vertex;
    vec3 *tangent;
    vec3 *bitangent;
    vec3 *normal;
    vec2 *texcoord;
    xmFace *face;
    vec3 min;
    vec3 max;
    vec3 center;
    float radius;
    long int iNumFaces;
    long int iNumVertices;
    char name[256];
    char texturePath[1024];
    char heightMapPath[1024];
    char specularMapPath[1024];
    short int textureFilter;
    float opacity;
    bool bUseCollBox;
    GLuint uDiffuseMap;
    GLuint uNormalMap;
    GLuint uSpecularMap;

    GLuint uVertexVBO;
    GLuint uTexCoordVBO;
    GLuint uNormalVBO;
    GLuint uTangentVBO;
    GLuint uBiTangentVBO;
    GLuint uFaceVBO;

};


class xmMenu {
public:
    xmMenu() {

    };

    bool Load(char *path);

    void Draw(void);

    bool CreateLightingShader(void);

    void EnableLightingShader();

    void BindTextures(GLuint tex, GLuint normalTex, GLuint specularTex);

    void DisableLightingShader(void);

    xmmObject *object;
    xmmLight *light;
    xmmCamera *camera;

    short int iNumCameras;
    short int iNumObjects;
    short int iNumLights;

    GLuint FBOid;

    GLhandleARB LightingPH;
    GLuint iUniformDiffuseMap;
    GLuint iUniformNormalMap;
    GLuint iUniformSpecularMap;

    GLuint iUniformTangent;
    GLuint iUniformBiTangent;

private:
    char headerTitle[255];
    char header[8];


};

inline void GetTextureMatrix(GLuint texUint) {
    static double modelView[16];
    static double projection[16];

    const GLdouble bias[16] =
            {
                    0.5, 0.0, 0.0, 0.0,
                    0.0, 0.5, 0.0, 0.0,
                    0.0, 0.0, 0.5, 0.0,
                    0.5, 0.5, 0.5, 1.0
            };

    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    glMatrixMode(GL_TEXTURE);
    glActiveTextureARB(texUint);

    glLoadIdentity();
    glLoadMatrixd(bias);

    glMultMatrixd(projection);
    glMultMatrixd(modelView);

    glMatrixMode(GL_MODELVIEW);
    glActiveTextureARB(GL_TEXTURE0_ARB);
}


#endif
