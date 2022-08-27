/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../../main.h"
//#define _info
//#define _depthInfo

extern Timer timer;
extern Texture texture;
extern Camera camera;
extern xmLevelShaders xmlLighting;
extern Gui gui;

xmAnimation::xmAnimation() {
    bFirstTime = true;
    bPlay = false;
    fAnimationTime = 0.0f;
    sparkTime = 0.0f;
    iPrevFrame = 0;
    iCurrFrame = 0;
    fInterpolation = 0;
    counter = -1;
    iNumIndexedBBs = 0;
    iCurrentAnimationID = 0;
}

xmAnimation::~xmAnimation() {

}


bool xmAnimation::Load(char *path, bool isCharacter) {
    FILE *pFile = fopen(path, "rb");
    if (!pFile) {
        printf("error: could not open file %s! ::xmAnimation", path);
        return false;
    }

    fread(headerTitle, 1, 74, pFile);
    fread(header, 1, 7, pFile);

    if (strcmp(header, "LXMA19")) {
        printf("error: file %s is not valid xmAnimation file!\n");
        return false;
    }

    fread(&iNumObjects, 1, sizeof(int), pFile);
    fread(&iFramerate, 1, sizeof(float), pFile);
    fread(&iLastFrame, 1, sizeof(int), pFile);
    fread(&iNumBones, 1, sizeof(int), pFile);
    fread(&iNumAnimations, 1, sizeof(int), pFile);
#ifdef _info
    printf("%d %f %d %d %d\n", iNumObjects, iFramerate, iLastFrame, iNumBones, iNumAnimations);
#endif
    anim = new xmaAnimation[iNumAnimations];
    if (!anim) return false;

    for (int a = 0; a < iNumAnimations; a++) {
        short int size = 0;
        fread(&anim[a].id, sizeof(int), 1, pFile);
        fread(&size, sizeof(short int), 1, pFile);
        fread(&anim[a].animName, sizeof(char), size, pFile);
        fread(&anim[a].animNumBoneAffectNames, sizeof(int), 1, pFile);
        for (int b = 0; b < anim[a].animNumBoneAffectNames; b++) {
            fread(&size, sizeof(short int), 1, pFile);
            fread(&anim[a].boneAffectName[b], sizeof(char), size, pFile);
        }
        fread(&anim[a].animType, sizeof(int), 1, pFile);
        fread(&anim[a].animStartFrame, sizeof(int), 1, pFile);
        fread(&anim[a].animEndFrame, sizeof(int), 1, pFile);
        fread(&anim[a].animFramerate, sizeof(int), 1, pFile);
#ifdef _info
        printf("%d %s %d %d %d %d\n", anim[a].id, anim[a].animName, anim[a].animType, anim[a].animStartFrame, anim[a].animEndFrame, anim[a].animFramerate);
        printf("%d\n", anim[a].animNumBoneAffectNames);
        for(int b=0; b < anim[a].animNumBoneAffectNames; b++)
        {
            printf("%s\n", anim[a].boneAffectName[b]);
        }
#endif
    }

    bone = new xmaBone[iNumBones];
    if (!bone) return false;

#ifdef _info
    printf("iNumObjects: %d\n", iNumObjects);
#endif
    if (iNumObjects <= 0) return false;
    object = new xmaObject[iNumObjects];
    if (!object) return false;

    for (int o = 0; o < iNumObjects; o++) {
        short int size = 0;
        fread(&size, 1, sizeof(short int), pFile);
        fread(object[o].name, 1, size, pFile);
#ifdef _info
        printf("%s %d %d\n",object[o].name, strlen(object[o].name), o);
#endif
        object[o].bReflectable = !(strstr(object[o].name, "Hand") || strstr(object[o].name, "hand") ||
                                   strstr(object[o].name, "ent_"));
        fread(&object[o].iNumVertices, 1, sizeof(int), pFile);
#ifdef _info
        printf("%d %d %d %d %.1f\n",object[o].iNumVertices,
                                    iNumBones,
                                    iFirstFrame,
                                    iLastFrame,
                                    iFramerate);
#endif

        object[o].texcoord = new vec2[object[o].iNumVertices];
        object[o].vertex = new vec3[object[o].iNumVertices];
        object[o].animverts = new vec3[object[o].iNumVertices];
        object[o].boneid = new int[object[o].iNumVertices];
        object[o].normal = new vec3[object[o].iNumVertices];
        object[o].tangent = new vec3[object[o].iNumVertices];
        //object[o].bitangent           = new vec3[object[o].iNumVertices];

        if (!object[o].vertex || !object[o].texcoord || !object[o].normal || !object[o].tangent) {
            printf("error: cannot alloc. memory for vertex, texcoords and things about it for ::xmAnimation!\n");
            return false;
        }


        for (int i = 0; i < object[o].iNumVertices; i++) {
            fread(&object[o].vertex[i], 1, sizeof(vec3), pFile);
            fread(&object[o].normal[i], 1, sizeof(vec3), pFile);
            fread(&object[o].texcoord[i], 1, sizeof(vec2), pFile);
            fread(&object[o].boneid[i], 1, sizeof(int), pFile);
        }

        for (int i = 0; i < object[o].iNumVertices; i += 3) {
            int ind[3];
            ind[0] = i;
            ind[1] = i + 1;
            ind[2] = i + 2;

            vec3 Edge1 = object[o].vertex[i + 1] - object[o].vertex[i];
            vec3 Edge2 = object[o].vertex[i + 2] - object[o].vertex[i];
            vec2 Edge1uvs = object[o].texcoord[i + 1] - object[o].texcoord[i];
            vec2 Edge2uvs = object[o].texcoord[i + 2] - object[o].texcoord[i];

            float cp = Edge1uvs.y * Edge2uvs.x - Edge1uvs.x * Edge2uvs.y;
            vec3 tangent = vec3(0);
            (fabs(cp) < 1e-6f) ? tangent.x = 1.0f : (cp != 0.0f) ? tangent = Normalize(
                    (Edge1 * -Edge2uvs.y + Edge2 * Edge1uvs.y) * (1.0f / cp)) : tangent = vec3(0);

            object[o].tangent[ind[0]] = tangent;
            object[o].tangent[ind[1]] = tangent;
            object[o].tangent[ind[2]] = tangent;
        }


        for (int i = 0; i < object[o].iNumVertices; i++) {
            object[o].tangent[i] -= object[o].normal[i] * Dot(object[o].tangent[i], object[o].normal[i]);
            object[o].tangent[i] = Normalize(object[o].tangent[i]);

        }


        fread(&size, 1, sizeof(short int), pFile);
        fread(object[o].texturePath, 1, size, pFile);
        fread(&object[o].textureFilter, 1, sizeof(short int), pFile);

#ifdef _info
        printf("size %d texture path %s  filter %d\n", size, object[o].texturePath, object[o].textureFilter);
#endif

        object[o].uDiffuseMap = texture.LoadTexture((char *) object[o].texturePath, o, (int) object[o].textureFilter);
        if (object[o].uDiffuseMap == -1)
            object[o].uDiffuseMap = texture.LoadTexture((char *) XM_DEFAULT_DIFFUSE_MAP, o,
                                                        (int) object[o].textureFilter);

        if (gui.bUseBumpMapping) {
            char *name = strtok(object[o].texturePath, ".");
            if (name) {
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

        object[o].center = ((object[o].max + object[o].min) / 2);
        object[o].radius = Distance(object[o].max, object[o].center);

        //glGenBuffersARB(1, &object[o].uVertexVBO);
        //glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uVertexVBO);
        //glBufferDataARB(GL_ARRAY_BUFFER_ARB, object[o].iNumVertices*3 *sizeof(float), object[o].vertex, GL_STREAM_DRAW_ARB);
        //glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

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

        if (object[o].texcoord) delete[] object[o].texcoord;
        if (object[o].tangent) delete[] object[o].tangent;
        if (object[o].normal) delete[] object[o].normal;
    }
    for (int i = 0; i < iNumBones; i++) {
        short int size = 0;
        fread(&size, 1, sizeof(short int), pFile);
        fread(&bone[i].boneName, 1, size, pFile);
        //printf("%d %s\n",size,bone[i].boneName);

        fread(&size, 1, sizeof(short int), pFile);
        fread(&bone[i].boneParent, 1, size, pFile);
        //printf("%d %s\n",size,bone[i].boneParent);

        fread(&bone[i].rotation, 1, sizeof(vec3), pFile);
        fread(&bone[i].position, 1, sizeof(vec3), pFile);

        fread(&bone[i].iNumRotationKeys, sizeof(unsigned short), 1, pFile);
        fread(&bone[i].iNumPositionKeys, sizeof(unsigned short), 1, pFile);

        if (bone[i].iNumRotationKeys > 0) {
            bone[i].rotationKey = new xmaRotationKeyFrame[bone[i].iNumRotationKeys];
            fread(bone[i].rotationKey, sizeof(xmaRotationKeyFrame), bone[i].iNumRotationKeys, pFile);
        }

        if (bone[i].iNumPositionKeys > 0) {
            bone[i].positionKey = new xmaPositionKeyFrame[bone[i].iNumPositionKeys];
            fread(bone[i].positionKey, sizeof(xmaPositionKeyFrame), bone[i].iNumPositionKeys, pFile);
        }


        //printf("%s %s\n", bone[i].boneName, bone[i].boneParent);
#ifdef _info
        for(int r=0; r < bone[i].iNumRotationKeys; r++)
        {
            printf("rotation key bid %d rkid %d %f %f %f %f\n", i, r, bone[i].rotationKey[r].time,
                                bone[i].rotationKey[r].rotation.x,
                                bone[i].rotationKey[r].rotation.y,
                                bone[i].rotationKey[r].rotation.z);
        }

        for(int p=0; p < bone[i].iNumPositionKeys; p++)
        {
            printf("position key bid %d pkid %d %f %f %f %f\n", i, p, bone[i].positionKey[p].time,
                                bone[i].positionKey[p].position.x,
                                bone[i].positionKey[p].position.y,
                                bone[i].positionKey[p].position.z);
        }
        printf("%f %f %f    %f %f %f\n",
            bone[i].position.x,
            bone[i].position.y,
            bone[i].position.z,
            bone[i].rotation.x,
            bone[i].rotation.y,
            bone[i].rotation.z);
#endif
        if (!isCharacter) {
            if (strstr(bone[i].boneName, "bullet")
                || strstr(bone[i].boneName, "Bullet"))
                bone[i].isBullet = true;

            if (strstr(bone[i].boneName, "barrel")
                || strstr(bone[i].boneName, "Barrel")
                || strstr(bone[i].boneName, "FlashLight")
                || strstr(bone[i].boneName, "flashlight"))
                bone[i].isBarrel = true;
        } else {
            if (strstr(bone[i].boneName, "Bip01 weaponPosition")) bone[i].isWeaponPosition = true;
            if (strstr(bone[i].boneName, "Bip01_Spine2")) bone[i].isSpinePosition = true;
        }
    }

    for (int i = 0; i < iNumBones; i++) {
        bone[i].parent = -1;
        if (bone[i].boneParent[0] == '\0') continue;

        for (int n = i - 1; n >= 0; n--) {
            if (!strcmp(bone[n].boneName, bone[i].boneParent)) {
                bone[i].parent = n;
                break;
            }
        }
        if (isCharacter) {
            if (strstr(bone[i].boneName, "Forearm") ||   //predlokti
                strstr(bone[i].boneName, "UpperArm") ||  //nadlokti
                strstr(bone[i].boneName, "Clavicle") ||  //klicni kost
                strstr(bone[i].boneName, "Spine") ||     //pater
                strstr(bone[i].boneName, "Head") ||      //hlava
                strstr(bone[i].boneName, "Thigh") ||     //stehno
                strstr(bone[i].boneName, "Calf") ||      //lytko
                strstr(bone[i].boneName, "Hand") ||      //ruka
                strstr(bone[i].boneName, "Foot"))        //noha
            {
                bBones[iNumIndexedBBs] = i;
                iNumIndexedBBs++;
            }
        }
    }

    if (isCharacter) {
        bbSphere = new xmaBoundingSphere[iNumBones];
        if (!bbSphere) return false;

        int j = 0;
        for (int i = 0; i < iNumBones; i++) {
            //FIXME - chtelo by to spocitat radius z delky kosti abych predesel nakejm kokotinqam
            /*
            bone Bip01 Spine - type 1
            bone Bip01 Spine1 - type 1
            bone Bip01 Spine2 - type 1
            bone Bip01 Spine3 - type 1
            bone Bip01 Head - type 0
            bone Bip01 L Clavicle - type 0
            bone Bip01 L UpperArm - type 1
            bone Bip01 L Forearm - type 1
            bone Bip01 L Hand - type 1
            bone Bip01 R Clavicle - type 0
            bone Bip01 R UpperArm - type 1
            */
            bbSphere[i].radius = 8.0f;

            if (strstr(bone[i].boneName, "Head") ||      //hlava
                strstr(bone[i].boneName, "Clavicle"))  //klicni
            {

                bbSphere[j].type = HEAD;
#ifdef _depthInfo
                printf("bone %s - type %d\n", bone[i].boneName, bbSphere[j].type);
#endif
                j++;
                continue;
            }

            if (strstr(bone[i].boneName, "Forearm") ||   //predlokti
                strstr(bone[i].boneName, "UpperArm") ||  //nadlokti
                strstr(bone[i].boneName, "Hand") ||      //ruka
                strstr(bone[i].boneName, "Spine"))     //pater
            {
                bbSphere[j].type = BODY;
#ifdef _depthInfo
                printf("bone %s - type %d\n", bone[i].boneName, bbSphere[j].type);
#endif
                j++;
                continue;
            }

            if (strstr(bone[i].boneName, "Thigh") ||     //stehno
                strstr(bone[i].boneName, "Calf") ||      //lytko
                strstr(bone[i].boneName, "Foot"))        //noha
            {
                bbSphere[j].type = FEET;
#ifdef _depthInfo
                printf("bone %s - type %d\n", bone[i].boneName, bbSphere[j].type);
#endif
                j++;
                continue;
            }
        }
    }

    mat4 mat;
    for (int i = 0; i < iNumBones; i++) {
        mat.Translate(bone[i].position);
        mat.Rotate(bone[i].rotation);

        bone[i].rel = mat;

        if (bone[i].parent != -1) mat = mat * bone[bone[i].parent].abs;
        bone[i].abs = mat;
        mat.Identity();
    }

    for (int k = 0; k < iNumBones; k++) {
        if (isCharacter) {
            if (!strcmp(bone[k].boneName, "Bip01 Spine")) iSpineIndex = k;
            if (!strcmp(bone[k].boneName, "Bip01 Pelvis")) iPelvisIndex = k;
        }

        mat = bone[k].abs;
        mat.Inverse();
        for (int o = 0; o < iNumObjects; o++) {
            for (int j = 0; j < object[o].iNumVertices; j++) {
                if (object[o].boneid[j] == k) {
                    object[o].vertex[j] = mat * object[o].vertex[j];
                }
            }
        }
    }

#ifdef _depthInfo
    printf("%d %d\n", iSpineIndex, iPelvisIndex);
#endif

    //setAnimationByID(0, true); // pri smazani, je potreba zmenit counter na 0;
    setAnimation(0, 0, 1, true);
    spark = texture.LoadTexture("gamecore/weapons/sparks/0.dds", 0, 1);
    fclose(pFile);

    return true;
}

bool xmAnimation::setAnimationByID(int id, bool bPlay) {
    if (id > iNumAnimations) return false;
    iCurrentAnimationID = id;
    return setAnimation(anim[id].animStartFrame, anim[id].animEndFrame, anim[id].animFramerate, bPlay);
}

bool xmAnimation::setAnimation(int FirstFrame, int LastFrame, int AnimFPS, bool playAnimation) {
    {
        if (playAnimation) {
            if (AnimFPS < 0) AnimFPS = (int) iFramerate;
            if (!bPlay) fAnimationTime = (float) FirstFrame;
            //playAnimation = false;
            bPlay = true;
            //printf("%f\n ", fAnimationTime);
        }

        if (bPlay) {
            if (fAnimationTime < FirstFrame) {
                bPlay = false;
                fAnimationTime = (float) FirstFrame;
            }

            fAnimationTime += (float) AnimFPS * timer.fFrameInterval;
            //printf("%d %d %f\n", FirstFrame, LastFrame, fAnimationTime);

            if (fAnimationTime >= LastFrame) {
                bPlay = false;
                fAnimationTime = (float) LastFrame;
            }

            for (int i = 0; i < iNumBones; i++) {
                fInterpolation = 0;
                if (bone[i].iNumPositionKeys > 0) {
                    iPrevFrame = 0;
                    iCurrFrame = 0;
                    while (bone[i].positionKey[iCurrFrame].time < fAnimationTime &&
                           iCurrFrame < bone[i].iNumPositionKeys)
                        iCurrFrame++;
                    iPrevFrame = iCurrFrame;
                    if (iCurrFrame > 0) iPrevFrame--;
                    fInterpolation = (fAnimationTime - bone[i].positionKey[iPrevFrame].time);
                    if (iCurrFrame != iPrevFrame)
                        fInterpolation /= bone[i].positionKey[iCurrFrame].time - bone[i].positionKey[iPrevFrame].time;
                    vec3 vTransformation =
                            bone[i].positionKey[iCurrFrame].position - bone[i].positionKey[iPrevFrame].position;
                    if (iCurrFrame == bone[i].iNumPositionKeys) fInterpolation = 0;
                    vTransformation *= fInterpolation;
                    vTransformation += bone[i].positionKey[iPrevFrame].position;
                    finalMatrix.Translate(vTransformation);
                }

                if (bone[i].iNumRotationKeys > 0) {
                    iPrevFrame = 0;
                    iCurrFrame = 0;
                    while (bone[i].rotationKey[iCurrFrame].time < fAnimationTime &&
                           iCurrFrame < bone[i].iNumRotationKeys)
                        iCurrFrame++;
                    iPrevFrame = iCurrFrame;
                    if (iCurrFrame > 0)iPrevFrame--;
                    fInterpolation = (fAnimationTime - bone[i].rotationKey[iPrevFrame].time);
                    if (iCurrFrame != iPrevFrame)
                        fInterpolation /= bone[i].rotationKey[iCurrFrame].time - bone[i].rotationKey[iPrevFrame].time;
                    vec3 vRotation =
                            bone[i].rotationKey[iCurrFrame].rotation - bone[i].rotationKey[iPrevFrame].rotation;
                    if (iCurrFrame == bone[i].iNumRotationKeys) fInterpolation = 0;
                    vRotation *= fInterpolation;
                    vRotation += bone[i].rotationKey[iPrevFrame].rotation;

                    if (i == iSpineIndex) vRotation += SpineRotation;
                    if (i == iPelvisIndex) vRotation += PelvisRotation;

                    finalMatrix.Rotate(vRotation);
                }

                bone[i].abs = finalMatrix;

                finalMatrix = finalMatrix * bone[i].rel;

                if (bone[i].parent != -1) finalMatrix = finalMatrix * bone[bone[i].parent].final;
                bone[i].final = finalMatrix;

                if (bone[i].isBullet) bulletPosition = bone[i].final.GetTranslate();
                if (bone[i].isBarrel) barrelPosition = bone[i].final.GetTranslate();
                if (bone[i].isWeaponPosition) weaponPosition = bone[i].final;
                if (bone[i].isSpinePosition) spine2Position = bone[i].final;

                finalMatrix.Identity();
            }
        }
    }

    return bPlay;
}


bool xmAnimation::setBlendedAnimation(int animID1, int animID2, int AnimFPS, bool playAnimation) {
    if (animID1 > iNumAnimations || animID2 > iNumAnimations) return false;
    mat4 boneMatrix[128][2];
    glPushMatrix();
    {
        if (playAnimation) bPlay = true;
        if (bPlay) {
            int FirstFrame = anim[animID1].animStartFrame;
            int LastFrame = anim[animID1].animEndFrame;

            int FirstFrame1 = anim[animID2].animStartFrame;
            int LastFrame1 = anim[animID2].animEndFrame;

            if (AnimFPS < 0) AnimFPS = (int) iFramerate;

            if (bFirstTime) {
                BlendedAnimationTime[0] = FirstFrame;
                BlendedAnimationTime[1] = FirstFrame1;
            }

            BlendedAnimationTime[0] += (float) anim[animID1].animFramerate * timer.fFrameInterval;
            BlendedAnimationTime[1] += (float) anim[animID2].animFramerate * timer.fFrameInterval;

            if (BlendedAnimationTime[0] >= LastFrame) {
                BlendedAnimationTime[0] = FirstFrame;
            }

            if (BlendedAnimationTime[1] >= LastFrame1) {
                BlendedAnimationTime[1] = FirstFrame1;
            }


            for (int i = 0; i < iNumBones; i++) {

                iPrevFrame = 0;
                iCurrFrame = 0;
                fInterpolation = 0;

                while (bone[i].positionKey[iCurrFrame].time < BlendedAnimationTime[0] &&
                       iCurrFrame < bone[i].iNumPositionKeys)
                    iCurrFrame++;
                iPrevFrame = iCurrFrame;
                if (iCurrFrame > 0) iPrevFrame--;
                fInterpolation = (BlendedAnimationTime[0] - bone[i].positionKey[iPrevFrame].time);
                if (iCurrFrame != iPrevFrame)
                    fInterpolation /= bone[i].positionKey[iCurrFrame].time - bone[i].positionKey[iPrevFrame].time;
                vec3 vTransformation =
                        bone[i].positionKey[iCurrFrame].position - bone[i].positionKey[iPrevFrame].position;
                if (iCurrFrame == bone[i].iNumPositionKeys) fInterpolation = 0;
                vTransformation *= fInterpolation;
                vTransformation += bone[i].positionKey[iPrevFrame].position;
                boneMatrix[i][0].Translate(vTransformation);

                iPrevFrame = 0;
                iCurrFrame = 0;
                while (bone[i].rotationKey[iCurrFrame].time < BlendedAnimationTime[0] &&
                       iCurrFrame < bone[i].iNumRotationKeys)
                    iCurrFrame++;
                iPrevFrame = iCurrFrame;
                if (iCurrFrame > 0)iPrevFrame--;
                fInterpolation = (BlendedAnimationTime[0] - bone[i].rotationKey[iPrevFrame].time);
                if (iCurrFrame != iPrevFrame)
                    fInterpolation /= bone[i].rotationKey[iCurrFrame].time - bone[i].rotationKey[iPrevFrame].time;
                vec3 vRotation = bone[i].rotationKey[iCurrFrame].rotation - bone[i].rotationKey[iPrevFrame].rotation;
                if (iCurrFrame == bone[i].iNumRotationKeys) fInterpolation = 0;
                vRotation *= fInterpolation;
                vRotation += bone[i].rotationKey[iPrevFrame].rotation;
                boneMatrix[i][0].Rotate(vRotation);

                bone[i].abs = boneMatrix[i][0];
                boneMatrix[i][0] = boneMatrix[i][0] * bone[i].rel;
                if (bone[i].parent != -1) boneMatrix[i][0] = boneMatrix[i][0] * bone[bone[i].parent].final;
                bone[i].final = boneMatrix[i][0];
            }

            for (int i = 0; i < iNumBones; i++) {
                bool blend = false;
                for (int b = 0; b < anim[animID2].animNumBoneAffectNames; b++) {
                    //printf("%s %s\n", bone[i].boneName, anim[animID2].boneAffectName[b]);
                    if (!strcmp(bone[i].boneName, anim[animID2].boneAffectName[b])) blend = true;
                }

                if (blend) {
                    iPrevFrame = 0;
                    iCurrFrame = 0;
                    fInterpolation = 0;

                    while (bone[i].positionKey[iCurrFrame].time < BlendedAnimationTime[1] &&
                           iCurrFrame < bone[i].iNumPositionKeys)
                        iCurrFrame++;
                    iPrevFrame = iCurrFrame;
                    if (iCurrFrame > 0) iPrevFrame--;
                    fInterpolation = (BlendedAnimationTime[1] - bone[i].positionKey[iPrevFrame].time);
                    if (iCurrFrame != iPrevFrame)
                        fInterpolation /= bone[i].positionKey[iCurrFrame].time - bone[i].positionKey[iPrevFrame].time;
                    vec3 vTransformation =
                            bone[i].positionKey[iCurrFrame].position - bone[i].positionKey[iPrevFrame].position;
                    if (iCurrFrame == bone[i].iNumPositionKeys) fInterpolation = 0;
                    vTransformation *= fInterpolation;
                    vTransformation += bone[i].positionKey[iPrevFrame].position;
                    boneMatrix[i][1].Translate(vTransformation);

                    iPrevFrame = 0;
                    iCurrFrame = 0;
                    while (bone[i].rotationKey[iCurrFrame].time < BlendedAnimationTime[1] &&
                           iCurrFrame < bone[i].iNumRotationKeys)
                        iCurrFrame++;
                    iPrevFrame = iCurrFrame;
                    if (iCurrFrame > 0)iPrevFrame--;
                    fInterpolation = (BlendedAnimationTime[1] - bone[i].rotationKey[iPrevFrame].time);
                    if (iCurrFrame != iPrevFrame)
                        fInterpolation /= bone[i].rotationKey[iCurrFrame].time - bone[i].rotationKey[iPrevFrame].time;
                    vec3 vRotation =
                            bone[i].rotationKey[iCurrFrame].rotation - bone[i].rotationKey[iPrevFrame].rotation;
                    if (iCurrFrame == bone[i].iNumRotationKeys) fInterpolation = 0;
                    vRotation *= fInterpolation;
                    vRotation += bone[i].rotationKey[iPrevFrame].rotation;
                    boneMatrix[i][1].Rotate(vRotation);

                    bone[i].abs = boneMatrix[i][1];
                    boneMatrix[i][1] = boneMatrix[i][1] * bone[i].rel;
                    if (bone[i].parent != -1) boneMatrix[i][1] = boneMatrix[i][1] * bone[bone[i].parent].final;

                    float delta = 1.0f;
                    bone[i].final = boneMatrix[i][1] * delta + boneMatrix[i][1] * (1 - delta);
                }
            }

        }
    }
    if (playAnimation) bFirstTime = false;
    glPopMatrix();

    return bPlay;
}

void xmAnimation::RotateSpine(vec3 angle) {
    SpineRotation = angle;
}

void xmAnimation::RotatePelvis(vec3 angle) {
    PelvisRotation = angle;
}

void xmAnimation::draw() {
    //glPushMatrix();
    //    glTranslatef(bulletPosition.x, bulletPosition.y, bulletPosition.z);
    //    glutSolidSphere(1,12,12);
    //glPopMatrix();

    for (int i = 0; i < iNumIndexedBBs; i++) {
        bbSphere[i].center = bone[bBones[i]].final.GetTranslate();
        //    glPushMatrix();
        //        glTranslatef(BBoxPosition[i].x, BBoxPosition[i].y, BBoxPosition[i].z);
        //        glutWireSphere(8, 12, 12);
        //    glPopMatrix();
    }
    for (int o = 0; o < iNumObjects; o++) {
        //450FPS
        for (int i = 0; i < object[o].iNumVertices; i++) {
            object[o].animverts[i] = bone[object[o].boneid[i]].final * object[o].vertex[i];
        }

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, object[o].animverts);
        xmlLighting.BindTextures(object[o].uDiffuseMap, object[o].uNormalMap, object[o].uSpecularMap);

        if (gui.bUseBumpMapping) {
            glEnableVertexAttribArrayARB(xmlLighting.iUniformTangent);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uTangentVBO);
            glVertexAttribPointerARB(xmlLighting.iUniformTangent, 3, GL_FLOAT, 0, 0, 0);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        }

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uTexCoordVBO);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glEnableClientState(GL_NORMAL_ARRAY);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uNormalVBO);
        glNormalPointer(GL_FLOAT, 0, 0);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glDrawArrays(GL_TRIANGLES, 0, object[o].iNumVertices);

        if (gui.bUseBumpMapping) {
            glDisableVertexAttribArrayARB(xmlLighting.iUniformTangent);
        }

        glDisable(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_NORMAL_ARRAY);
        glDisable(GL_VERTEX_ARRAY);

    }
}


void xmAnimation::FPSPerson() {
    glPushMatrix();
    float depth[2];
    glGetFloatv(GL_DEPTH_RANGE, depth);
    glDepthRange(0, 0.1);

    glLoadIdentity();
    glRotatef(180, 0, 0, 1);
    draw();

    glDepthRange(depth[0], depth[1]);
    glPopMatrix();
}


void xmAnimation::drawSpark() {
    glPushMatrix();
    {
        glLoadIdentity();
        glRotatef(180, 0, 0, 1);
        glTranslatef(barrelPosition.x, barrelPosition.y, barrelPosition.z);
        //glActiveTextureARB(GL_TEXTURE1_ARB);
        glBindTexture(GL_TEXTURE_2D, spark);
        //glUniform1iARB(xmlLighting.iUniformDiffuseMap, 1);
        glAlphaFunc(GL_GREATER, 0.1f);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glColor4f(1, 1, 1, 0.9);
        float size = 3.0f;
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-size, -size, size);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(size, -size, size);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(size, size, size);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-size, size, size);
        glEnd();
        glColor4f(1, 1, 1, 1);
        glDisable(GL_BLEND);
        glDisable(GL_ALPHA_TEST);
    }
    glPopMatrix();
}
