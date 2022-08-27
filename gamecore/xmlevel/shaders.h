/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _XMLSHADERS_H
#define _XMLSHADERS_H

class xmLevelShaders {
public:
    bool CreateLevelLighting();

    void EnableShader(GLuint shadowMapID, int numVisibleLights);

    void BindTextures(GLuint tex, GLuint normalTex, GLuint specularTex, GLuint cubeMap = -1);

    void SetMaterialType(int type = 0) {
        glUniform1iARB(iUniformMaterialType, type);
    }

    void HideShadows() {
        glUniform1iARB(iUniformHideShadows, 1);
    }

    void ShowShadows() {
        glUniform1iARB(iUniformHideShadows, 0);
    }

    void DisableShader(void);

    GLuint iUniformTangent;

    GLhandleARB LightingPH;
    GLuint iUniformShadowMap;
    GLuint iUniformDiffuseMap;
    GLuint iUniformNormalMap;
    GLuint iUniformSpecularMap;
    GLuint iUniformCubeMap;
    GLuint iUniformNumVisibleLights;
    GLuint iUniformFlashLightMap;
    GLuint iUniformIsFlashlightActive;
    GLuint iUniformHideShadows;
    GLuint iUniformMaterialType;

    GLuint FlashLightMapID;
private:

};

#endif
