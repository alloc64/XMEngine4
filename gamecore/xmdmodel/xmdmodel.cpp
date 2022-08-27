/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../../main.h"

extern Gui gui;

//#define _info

extern xmLevelShaders xmlLighting;


bool xmdModel::Load(char *path) {
    FILE *pFile = fopen(path, "rb");
    if (!pFile) {
        printf("error: could not open file %s!\n", path);
        return false;
    }

    char headerTitle[75];
    char header[7];

    fread(headerTitle, 1, 74, pFile);
    fread(header, 1, 7, pFile);

    if (!strstr(header, "LXMD19")) {
        printf("error: file %s is not valid xmdModel file!\n", path);
        return false;
    }

    for (int i = 0; i < 3; i++) {
        short int type = 0;
        fread(&type, 1, sizeof(short int), pFile);
        switch (type) {
            case 0:
                fread(&weaponBulletPosition, 1, sizeof(vec3), pFile);
                break;

            case 1:
                fread(&weaponSpritePosition, 1, sizeof(vec3), pFile);
                break;

            case 2:
                fread(&weaponHandlePosition, 1, sizeof(vec3), pFile);
                break;

            default:
                printf("error: undefined index! ::xmdModel");
                return false;
                break;
        }
    }

    fread(&iNumObjects, 1, sizeof(short int), pFile);

#ifdef _info
    printf("%s\n", headerTitle);
    printf("iNumObjects: %d\n", iNumObjects);
#endif

    if (!glGenBuffersARB || !glBindBufferARB || !glBufferDataARB ||
        !glDeleteBuffersARB && CheckExtension("GL_ARB_vertex_buffer_object")) {
        glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
        glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
        glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
        glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");

        if (!glGenBuffersARB || !glBindBufferARB || !glBufferDataARB || !glDeleteBuffersARB) {
            printf("error: LOL cannot load GL_ARB_vertex_buffer_object functions, its awesome ! :(\n");
            return false;
        }
    }

    if (iNumObjects <= 0) return false;
    object = new xmdObject[iNumObjects];
    if (!object) return false;

    for (int o = 0; o < iNumObjects; o++) {
        short int lnght = 0;
        fread(&lnght, 1, sizeof(short int), pFile);
        fread(object[o].name, 1, lnght + 1, pFile);
#ifdef _info
        printf("%s %d %d\n",object[o].name, strlen(object[o].name), o);
#endif
        fread(&object[o].iNumVertices, 1, sizeof(long int), pFile);
        fread(&object[o].iNumFaces, 1, sizeof(long int), pFile);

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
            printf("error: cannot alloc. memory for vertex, texcoords and things about it for xmdModel!\n");
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
#ifdef _info
        for(int f=0; f < object[o].iNumFaces; f++) printf("%d %d %d\n",object[o].face[f].a, object[o].face[f].b, object[o].face[f].c);
#endif
        fread(&lnght, 1, sizeof(short int), pFile);
        fread(object[o].texturePath, 1, lnght + 1, pFile);
        fread(&object[o].textureFilter, 1, sizeof(short int), pFile);
        fread(&object[o].opacity, 1, sizeof(float), pFile);
#ifdef _info
        printf("%s %d %f\n",object[o].texturePath, object[o].textureFilter, object[o].opacity);
#endif

        object[o].uDiffuseMap = texture.LoadTexture((char *) object[o].texturePath, o, (int) object[o].textureFilter);
        if (object[o].uDiffuseMap == -1)
            object[o].uDiffuseMap = texture.LoadTexture((char *) XM_DEFAULT_DIFFUSE_MAP, o,
                                                        (int) object[o].textureFilter);

        if (gui.bUseBumpMapping) {
            char *name = strtok(object[o].texturePath, ".");
            if (name) {
                char path[1024];
                sprintf(path, "%s_normal.dds", name);

                object[o].uNormalMap = texture.LoadTexture((char *) path, o, (int) object[o].textureFilter);
                if (object[o].uNormalMap == -1) {
                    object[o].uNormalMap = texture.LoadTexture((char *) XM_DEFAULT_NORMAL_MAP, o,
                                                               (int) object[o].textureFilter);
                }

                sprintf(path, "%s_specular.dds", name);

                object[o].uSpecularMap = texture.LoadTexture((char *) path, o, (int) object[o].textureFilter);
                if (object[o].uSpecularMap == -1) {
                    object[o].uSpecularMap = texture.LoadTexture((char *) XM_DEFAULT_SPECULAR_MAP, o,
                                                                 (int) object[o].textureFilter);
                }
            }
        }

        min = max = vec3(0, 0, 0);

        for (int i = 0; i < object[o].iNumFaces; i++) {
            xmCalculateNTB(object[o].vertex[object[o].face[i].a], object[o].vertex[object[o].face[i].b],
                           object[o].vertex[object[o].face[i].c], object[o].texcoord[object[o].face[i].a],
                           object[o].texcoord[object[o].face[i].b], object[o].texcoord[object[o].face[i].c],
                           object[o].normal, object[o].bitangent, object[o].tangent, object[o].face[i]);

            if (min.x == 0) min.x = 999999.9f;
            if (max.x == 0) max.x = -999999.9f;
            if (min.z == 0) min.z = 999999.9f;
            if (max.z == 0) max.z = -999999.9f;
            if (min.y == 0) min.y = 999999.9f;
            if (max.y == 0) max.y = -999999.9f;

            int ind[3];
            ind[0] = object[o].face[i].a;
            ind[1] = object[o].face[i].b;
            ind[2] = object[o].face[i].c;

            for (int v = 0; v < 3; v++) {
                if (object[o].vertex[ind[v]].x < min.x) min.x = object[o].vertex[ind[v]].x - 2.0f;
                if (object[o].vertex[ind[v]].y < min.y) min.y = object[o].vertex[ind[v]].y - 2.0f;
                if (object[o].vertex[ind[v]].z < min.z) min.z = object[o].vertex[ind[v]].z - 2.0f;

                if (object[o].vertex[ind[v]].x > max.x) max.x = object[o].vertex[ind[v]].x + 2.0f;
                if (object[o].vertex[ind[v]].y > max.y) max.y = object[o].vertex[ind[v]].y + 2.0f;
                if (object[o].vertex[ind[v]].z > max.z) max.z = object[o].vertex[ind[v]].z + 2.0f;
            }
        }

        glGenBuffersARB(1, &object[o].uVertexVBO);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uVertexVBO);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, object[o].iNumVertices * 3 * sizeof(float), object[o].vertex,
                        GL_STATIC_DRAW_ARB);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glGenBuffersARB(1, &object[o].uTexcoordVBO);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uTexcoordVBO);
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

        if (object[o].texcoord) delete[] object[o].texcoord;
        if (object[o].normal) delete[] object[o].normal;
        if (object[o].tangent) delete[] object[o].tangent;
        if (object[o].texcoord) object[o].texcoord = NULL;
        if (object[o].normal) object[o].normal = NULL;


    }

    center = ((max + min) / 2);
    radius = Distance(max, center);

    fclose(pFile);

    return true;
}

void xmdModel::Draw() {
    if (!object) return;

    for (int o = 0; o < iNumObjects; o++) {
        xmlLighting.BindTextures(object[o].uDiffuseMap, object[o].uNormalMap, object[o].uSpecularMap);

        if (gui.bUseBumpMapping) {
            glEnableVertexAttribArrayARB(xmlLighting.iUniformTangent);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uTangentVBO);
            glVertexAttribPointerARB(xmlLighting.iUniformTangent, 3, GL_FLOAT, 0, 0, 0);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        }

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uTexcoordVBO);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uVertexVBO);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glEnableClientState(GL_NORMAL_ARRAY);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uNormalVBO);
        glNormalPointer(GL_FLOAT, 0, 0);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, object[o].uFaceVBO);
        glDrawElements(GL_TRIANGLES, object[o].iNumFaces * 3, GL_UNSIGNED_INT, NULL);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

        if (gui.bUseBumpMapping) {
            glDisableVertexAttribArrayARB(xmlLighting.iUniformTangent);
        }

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}
