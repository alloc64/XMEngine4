/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../../main.h"

//#define _info
#define _basic
#define _loadtex

Shader shd;
extern Timer timer;
extern Gui gui;

bool xmMenu::Load(char *path) {
    FILE *pFile = fopen(path, "rb");
    if (!pFile) {
        printf("error: %s not found! ::xmMenu\n", path);
        return false;
    }

    fread(headerTitle, 1, 77, pFile);
    fread(header, 1, 7, pFile);

    if (strcmp(header, "LXMM21")) {
        printf("error: %s is not valid xmMenu file!\n");
        return false;
    }

    fread(&iNumLights, 1, sizeof(short int), pFile);
    light = new xmmLight[iNumLights];
    if (!light) return false;

    fread(light, iNumLights, sizeof(xmmLight), pFile);

    fread(&iNumCameras, 1, sizeof(short int), pFile);
    camera = new xmmCamera[iNumCameras];
    if (!camera) return false;

    fread(camera, iNumCameras, sizeof(xmmCamera), pFile);
    fread(&iNumObjects, 1, sizeof(short int), pFile);

    if (iNumObjects <= 0) return false;
    object = new xmmObject[iNumObjects];
    if (!object) return false;

    for (int o = 0; o < iNumObjects; o++) {
        short int size = 0;
        fread(&size, 1, sizeof(short int), pFile);
        fread(object[o].name, 1, size, pFile);
#ifdef _info
        printf("%s %d %d\n",object[o].name, size, o);
#endif
        fread(&object[o].iNumVertices, 1, sizeof(long int), pFile);
        fread(&object[o].iNumFaces, 1, sizeof(long int), pFile);

        if (object[o].iNumFaces <= 0 || object[o].iNumVertices <= 0) {
            printf("warning: empty object on index %d\n", o);
            iNumObjects -= 1;
            continue;
        }

#ifdef _info
        printf("%d %d\n",object[o].iNumVertices, object[o].iNumFaces);
#endif

        object[o].vertex = new vec3[object[o].iNumVertices];
        object[o].texcoord = new vec2[object[o].iNumVertices];
        object[o].normal = new vec3[object[o].iNumVertices];
        object[o].tangent = new vec3[object[o].iNumVertices];
        object[o].bitangent = new vec3[object[o].iNumVertices];
        object[o].face = new xmFace[object[o].iNumFaces];

        if (!object[o].vertex || !object[o].texcoord || !object[o].face || !object[o].normal || !object[o].tangent ||
            !object[o].bitangent) {
            printf("error: cannot alloc. memory for vertex, texcoords and things about it for xmLevel!\n");
            return false;
        }

        for (int v = 0; v < object[o].iNumVertices; v++) {
            fread(&object[o].vertex[v], 1, sizeof(vec3), pFile);
            fread(&object[o].texcoord[v], 1, sizeof(vec2), pFile);
#ifdef _info
            printf("%f %f %f  %f %f\n", object[o].vertex[v].x, object[o].vertex[v].y, object[o].vertex[v].z, object[o].texcoord[v].x, object[o].texcoord[v].y);
#endif
        }

        fread(object[o].face, object[o].iNumFaces, sizeof(xmFace), pFile);
        fread(object[o].normal, object[o].iNumVertices, sizeof(vec3), pFile);
        fread(object[o].tangent, object[o].iNumVertices, sizeof(vec3), pFile);
        fread(object[o].bitangent, object[o].iNumVertices, sizeof(vec3), pFile);

#ifdef _info
        for(int f=0; f < object[o].iNumFaces; f++)
        {
             printf("%d %d %d\n",object[o].face[f].a, object[o].face[f].b, object[o].face[f].c);
        }
#endif

        fread(&size, 1, sizeof(short int), pFile);
        fread(object[o].texturePath, 1, size, pFile);

        fread(&object[o].textureFilter, 1, sizeof(short int), pFile);
        fread(&object[o].opacity, 1, sizeof(float), pFile);

#ifdef _info
        printf("%s %d %f\n",object[o].texturePath, object[o].textureFilter, object[o].opacity);
#endif
        char path[1024];
        sprintf(path, "textures/%s", object[o].texturePath);
#ifdef _loadtex
        if ((object[o].uDiffuseMap = texture.LoadTexture(path, o, (int) object[o].textureFilter)) == -1) {
            object[o].uDiffuseMap = texture.LoadTexture(XM_DEFAULT_DIFFUSE_MAP, o, (int) object[o].textureFilter);
        }

        char *name = strtok(path, ".");
        sprintf(path, "%s_normal.dds", name);

        if ((object[o].uNormalMap = texture.LoadTexture(path, o, (int) object[o].textureFilter)) == -1) {
            object[o].uNormalMap = texture.LoadTexture(XM_DEFAULT_NORMAL_MAP, o, (int) object[o].textureFilter);
        }

        name = strtok(object[o].texturePath, ".");
        sprintf(path, "textures/%s_specular.dds", name);

        if ((object[o].uSpecularMap = texture.LoadTexture(path, o, (int) object[o].textureFilter)) == -1) {
            object[o].uSpecularMap = texture.LoadTexture(XM_DEFAULT_SPECULAR_MAP, o, (int) object[o].textureFilter);
        }
#endif

        glGenBuffersARB(1, &object[o].uVertexVBO);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uVertexVBO);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, object[o].iNumVertices * 3 * sizeof(float), object[o].vertex,
                        GL_STATIC_DRAW_ARB);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glGenBuffersARB(1, &object[o].uTexCoordVBO);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uTexCoordVBO);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, object[o].iNumVertices * 2 * sizeof(float), object[o].texcoord,
                        GL_STATIC_DRAW_ARB);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glGenBuffersARB(1, &object[o].uNormalVBO);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uNormalVBO);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, object[o].iNumVertices * 3 * sizeof(float), object[o].normal,
                        GL_STATIC_DRAW_ARB);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glGenBuffersARB(1, &object[o].uTangentVBO);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uTangentVBO);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, object[o].iNumVertices * 3 * sizeof(float), object[o].tangent,
                        GL_STATIC_DRAW_ARB);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glGenBuffersARB(1, &object[o].uFaceVBO);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, object[o].uFaceVBO);
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, object[o].iNumFaces * 3 * sizeof(unsigned int), object[o].face,
                        GL_STATIC_DRAW_ARB);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

        if (object[o].vertex) delete[] object[o].vertex;
        if (object[o].face) delete[] object[o].face;
        if (object[o].texcoord) delete[] object[o].texcoord;
        if (object[o].normal) delete[] object[o].normal;
        if (object[o].tangent) delete[] object[o].tangent;
        if (object[o].bitangent) delete[] object[o].bitangent;
    }
    fclose(pFile);

#ifdef _basic
    printf("Svetel: %d Objektu: %d\n", iNumLights, iNumObjects);
#endif
    return true;
}

void xmMenu::Draw(void) {
    if (!object) return;

    glPushMatrix();
    {
        //glRotatef(camera[0].rotation.z, 0,0,1);
        glRotatef(-11, 0, 0, 1);
        glRotatef(camera[0].rotation.y, 0, 1, 0);
        glRotatef(camera[0].rotation.x + 6, 1, 0, 0);
        glTranslatef(camera[0].center.x, camera[0].center.y, camera[0].center.z);

        for (int l = 0; l < iNumLights; l++) {
            float diff[] = {light[l].color.r, light[l].color.g, light[l].color.b, 1.0f};
            float spec[] = {0.5, 0.5, 0.5, 1.0f};
            float pos[] = {light[l].center.x, light[l].center.y, light[l].center.z, 1.0f};

            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diff);

            glLightfv(GL_LIGHT0 + l, GL_DIFFUSE, diff);
            glLightfv(GL_LIGHT0 + l, GL_SPECULAR, spec);
            glLightfv(GL_LIGHT0 + l, GL_POSITION, pos);
        }

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        EnableLightingShader();
        for (int o = 0; o < iNumObjects; o++) {
            BindTextures(object[o].uDiffuseMap, object[o].uNormalMap, object[o].uSpecularMap);

            glEnableVertexAttribArrayARB(iUniformTangent);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uTangentVBO);
            glVertexAttribPointerARB(iUniformTangent, 3, GL_FLOAT, 0, 0, 0);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uTexCoordVBO);
            glTexCoordPointer(2, GL_FLOAT, 0, 0);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

            glEnableClientState(GL_NORMAL_ARRAY);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uNormalVBO);
            glNormalPointer(GL_FLOAT, 0, 0);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

            glEnableClientState(GL_VERTEX_ARRAY);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uVertexVBO);
            glVertexPointer(3, GL_FLOAT, 0, 0);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, object[o].uFaceVBO);
            glDrawElements(GL_TRIANGLES, object[o].iNumFaces * 3, GL_UNSIGNED_INT, NULL);
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

            glDisableVertexAttribArrayARB(iUniformTangent);

            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
        }

        DisableLightingShader();
        glDisable(GL_CULL_FACE);
    }
    glPopMatrix();
}

void xmMenu::EnableLightingShader() {
    glUseProgramObjectARB(LightingPH);
}

void xmMenu::BindTextures(GLuint tex, GLuint normalTex, GLuint specularTex) {
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1iARB(iUniformDiffuseMap, 0);

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glBindTexture(GL_TEXTURE_2D, normalTex);
    glUniform1iARB(iUniformNormalMap, 1);

    glActiveTextureARB(GL_TEXTURE2_ARB);
    glBindTexture(GL_TEXTURE_2D, specularTex);
    glUniform1iARB(iUniformSpecularMap, 2);
}

void xmMenu::DisableLightingShader(void) {
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glUseProgramObjectARB(0);
}

bool xmMenu::CreateLightingShader(void) {
    char *vertexShader[1];
    char *fragmentShader[1];

    vertexShader[0] = "\n\
       \n\
       attribute vec3 tangent;\n\
       \n\
       varying vec3 lightVec;\n\
       varying vec3 viewVec;\n\
       varying vec3 halfVec;\n\
       varying float att;\n\
       \n\
       void main(void)\n\
       {\n\
            gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n\
            gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;\n\
            \n\
            vec3 mvTangent = gl_NormalMatrix * tangent;\n\
            vec3 mvNormal = gl_NormalMatrix * gl_Normal;\n\
            vec3 mvBitangent = cross(mvTangent, mvNormal);\n\
            vec3 mvVertex = vec3(gl_ModelViewMatrix * gl_Vertex);\n\
            vec3 mvDirToEye = -mvVertex;\n\
            \n\
            vec3 mvDirToLight = (gl_LightSource[0].position.xyz - mvVertex);\n\
            lightVec = normalize(vec3(dot(mvDirToEye, mvTangent), dot(mvDirToEye, mvBitangent), dot(mvDirToEye, mvNormal)));\n\
            viewVec = normalize(vec3(dot(mvDirToLight, mvTangent), dot(mvDirToLight, mvBitangent), dot(mvDirToLight, mvNormal)));\n\
            halfVec = normalize(lightVec + viewVec);\n\
            float dist = length(mvDirToLight);\n\
            att = 1.0 / (0.00055 * dist * dist);\n\
       }\n\
    ";

    fragmentShader[0] = "\n\
    uniform sampler2D DiffuseMap;\n\
    uniform sampler2D NormalMap;\n\
    uniform sampler2D SpecularMap;\n\
    \n\
    varying vec3 lightVec;\n\
    varying vec3 viewVec;\n\
    varying vec3 halfVec;\n\
    varying float att;\n\
    \n\
    void main(void)\n\
    {\n\
        vec4 base = texture2D(DiffuseMap, gl_TexCoord[0].xy);\n\
        vec3 normal = normalize(texture2D(NormalMap, gl_TexCoord[0].xy).xyz * 2.0 - 1.0);\n\
        float gloss = texture2D(SpecularMap, gl_TexCoord[0].xy).x;\n\
        \n\
        vec4 diffuse = gl_FrontLightProduct[0].diffuse * max(dot(viewVec, normal), 0.0) * base;\n\
        vec4 specular =  vec4(vec3(gloss * pow(max(dot(reflect(-viewVec, normal.xyz), lightVec), 0.0), 4.0)), 1.0);\n\
        //pow(max(dot(halfVec, normal), 0.0), 4.0);\n\
        \n\
	   	gl_FragColor = clamp((diffuse * att + specular), 0.0, 1.0);\n\
    }\n\
    ";

    GLhandleARB MenuVS = shd.LoadShaderFromArray("menu vertex shader", (const GLcharARB **) vertexShader,
                                                 GL_VERTEX_SHADER_ARB);
    GLhandleARB MenuFS = shd.LoadShaderFromArray("menu fragment shader", (const GLcharARB **) fragmentShader,
                                                 GL_FRAGMENT_SHADER_ARB);

    if (!MenuVS || !MenuFS) {
        printf("error: couldn't create vertex || fragment shader handles!\n");
        return false;
    }

    if (!(LightingPH = glCreateProgramObjectARB())) {
        printf("error: cannot create shader handle!\n");
        return false;
    }

    glAttachObjectARB(LightingPH, MenuVS);
    glAttachObjectARB(LightingPH, MenuFS);
    glLinkProgramARB(LightingPH);
    glValidateProgramARB(LightingPH);

    iUniformDiffuseMap = glGetUniformLocationARB(LightingPH, "DiffuseMap");
    iUniformNormalMap = glGetUniformLocationARB(LightingPH, "NormalMap");
    iUniformSpecularMap = glGetUniformLocationARB(LightingPH, "SpecularMap");
    iUniformTangent = glGetAttribLocationARB(LightingPH, "tangent");
    printf("%d %d %d %d\n", iUniformDiffuseMap, iUniformNormalMap, iUniformSpecularMap, iUniformTangent);

    return true;
}
