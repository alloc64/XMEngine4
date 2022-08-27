/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "..\..\main.h"
//#define _notex
#define _basic
#define _collision
#define _weapons
//#define _drawbbs

FILE *fp;

extern Gui gui;
extern Timer timer;
extern Frustum frustum;
extern Camera camera;
extern Network netw;
extern Cartridge cartridges;
extern Bullet bullet;
extern HUD hud;
extern Characters characters;

//shaders
Texture3D tex3D;
PostProcess post;
xmLevelShaders xmlLighting;
//weapons
xmWeapon weapon;
xmdModel *weaponModel;
xmAnimation *fpsWeaponModel;

xmVertexAnimation *levelVertexEntity;
xmAnimation *levelBonedEntity;

//characters
xmAnimation character[4];

//skyboxiQ
Skybox skybox;

bool xmLevel::CreateShadowMap(void) {
    glGenTextures(1, &ShadowMapID);
    glBindTexture(GL_TEXTURE_2D, ShadowMapID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    float ClampColor[] = {1.0, 1.0, 1.0, 1.0};

    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, ClampColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, ShadowMapSize, ShadowMapSize, 0, GL_DEPTH_COMPONENT,
                 GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffersEXT(1, &FBOid);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOid);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, ShadowMapID, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        printf("FBO ERROR %d", status);
        return false;
    }

    return true;
}

bool xmLevel::Load(char *path) {
    window.SetFOV(50.0f);
    FILE *pFile = fopen(path, "rb");
    if (!pFile) {
        printf("error: file %s not found! can't open xmlevel file!\n", path);
        return false;
    }

    fread(headerTitle, 1, 74, pFile);
    fread(header, 1, 7, pFile);

    if (strcmp(header, "LXML19")) {
        printf("error: file %s is not valid xmLevel file!\n");
        return false;
    }

    gui.mainProgressBar.SetMessage("Loading lights ...");
    fread(&iNumLights, 1, sizeof(short int), pFile);
    fread(&mainLight, 1, sizeof(xmlLight), pFile);

    areaLight = new xmlLight[iNumLights];
    if (!areaLight) return false;
    fread(areaLight, iNumLights, sizeof(xmlLight), pFile);

    gui.mainProgressBar.SetPercents(3);
    gui.mainProgressBar.SetMessage("Loading spawns ...");

    fread(&iNumSpawns, 1, sizeof(short int), pFile);

    spawn = new xmlPlayerSpawn[iNumSpawns];
    if (!spawn) return false;

    if (iNumSpawns > 0) {
        for (int s = 0; s < iNumSpawns; s++) {
            short int size = strlen(spawn[s].name);
            fread(&size, 1, sizeof(short int), pFile);
            fread(spawn[s].name, 1, size, pFile);
            fread(&spawn[s].center, 1, sizeof(vec3), pFile);
        }
    } else {
        iNumSpawns = 1;
        printf("warning: creating default spawn!\n");
        spawn = new xmlPlayerSpawn[1];
        if (!spawn) return false;
        strcpy(spawn[0].name, "default_Spawn");
        spawn[0].center = vec3(-240, 400, -400);
    }

    gui.mainProgressBar.SetPercents(4);
    gui.mainProgressBar.SetMessage("Loading weapons ...");


    if (!weapon.Load("gamecore/weapons/weapons.wea")) {
        //o_O
    }


    gui.mainProgressBar.SetPercents(5);
    gui.mainProgressBar.SetMessage("Loading entities ...");

    fread(&iNumEntities, 1, sizeof(short int), pFile);
    entity = new xmlEntity[iNumEntities];
    if (!entity) return false;

    for (int e = 0; e < iNumEntities; e++) {
        short int size = 0;
        fread(&size, 1, sizeof(short int), pFile);
        fread(entity[e].name, 1, size, pFile);
        fread(&entity[e].rotation, 1, sizeof(vec3), pFile);
        fread(&entity[e].center, 1, sizeof(vec3), pFile);
        fread(&entity[e].min, 1, sizeof(vec3), pFile);
        fread(&entity[e].max, 1, sizeof(vec3), pFile);
        entity[e].max += entity[e].center;
        entity[e].min += entity[e].center;
        fread(&entity[e].radius, 1, sizeof(float), pFile);
        entity[e].id = classifyWeapon(entity[e].name);
        entity[e].wid = getWeaponID(entity[e].name);
        entity[e].damage = 0;
        entity[e].damaged = false;
        printf("%s %d\n", entity[e].name, entity[e].id);
    }

    gui.mainProgressBar.SetPercents(9);

    fread(&iNumObjects, 1, sizeof(short int), pFile);
    fread(&iSkyNum, 1, sizeof(short int), pFile);

    fread(&worldMin, 1, sizeof(vec3), pFile);
    fread(&worldMax, 1, sizeof(vec3), pFile);
    fread(&worldCenter, 1, sizeof(vec3), pFile);
    fread(&worldRadius, 1, sizeof(float), pFile);

    if (iNumObjects <= 0) return false;
    object = new xmlObject[iNumObjects];
    if (!object) return false;

    gui.mainProgressBar.SetPercents(10);
    gui.mainProgressBar.SetMessage("Loading level ...");

    for (int o = 0; o < iNumObjects; o++) {
        short int size = 0;
        fread(&size, 1, sizeof(short int), pFile);
        fread(object[o].name, 1, size, pFile);
        fread(&object[o].bUseAlphaTest, 1, sizeof(bool), pFile);
        fread(&object[o].bUseCollBox, 1, sizeof(bool), pFile);
        fread(&object[o].bNoColl, 1, sizeof(bool), pFile);
        fread(&object[o].bReflective, 1, sizeof(bool), pFile);
        fread(&object[o].bRefractive, 1, sizeof(bool), pFile);
        fread(&object[o].iNumVertices, 1, sizeof(long int), pFile);
        fread(&object[o].iNumFaces, 1, sizeof(long int), pFile);

        if (object[o].iNumVertices <= 0 || object[o].iNumFaces <= 0) {
            printf("warning: empty object %d\n", o);
            iNumObjects--;
            continue;
        }

        object[o].vertex = new vec3[object[o].iNumVertices];
        object[o].texcoord = new vec2[object[o].iNumVertices];
        object[o].normal = new vec3[object[o].iNumVertices];
        object[o].tangent = new vec3[object[o].iNumVertices];
        object[o].face = new xmFace[object[o].iNumFaces];

        if (!object[o].vertex || !object[o].texcoord || !object[o].face || !object[o].normal || !object[o].tangent) {
            printf("error: cannot alloc. memory for vertex, texcoords and things about it for xmLevel!\n");
            return false;
        }

        for (int v = 0; v < object[o].iNumVertices; v++) {
            fread(&object[o].vertex[v], 1, sizeof(vec3), pFile);
            fread(&object[o].texcoord[v], 1, sizeof(vec2), pFile);
        }

        fread(object[o].face, object[o].iNumFaces, sizeof(xmFace), pFile);
        fread(object[o].normal, object[o].iNumVertices, sizeof(vec3), pFile);
        fread(object[o].tangent, object[o].iNumVertices, sizeof(vec3), pFile);

        fread(&object[o].min, 1, sizeof(vec3), pFile);
        fread(&object[o].max, 1, sizeof(vec3), pFile);
        fread(&object[o].center, 1, sizeof(vec3), pFile);
        fread(&object[o].radius, 1, sizeof(float), pFile);


        fread(&size, 1, sizeof(short int), pFile);
        if (size > 0) {
            fwrite(&object[o].BoxVertices, size, sizeof(vec3), pFile);
            fwrite(&object[o].BoxNormals, size / 3, sizeof(vec3), pFile);
        }

        fread(&size, 1, sizeof(short int), pFile);
        fread(object[o].texturePath, 1, size, pFile);

        fread(&size, 1, sizeof(short int), pFile);
        fread(object[o].normalMapPath, 1, size, pFile);

        fread(&size, 1, sizeof(short int), pFile);
        fread(object[o].specularMapPath, 1, size, pFile);

        fread(&object[o].textureFilter, 1, sizeof(short int), pFile);
        fread(&object[o].opacity, 1, sizeof(float), pFile);

        strcpy(object[o].texturePath, makeOptionalTexturePath((char *) &object[o].texturePath));

        if (gui.bUseBumpMapping) {
            strcpy(object[o].normalMapPath, makeOptionalTexturePath((char *) object[o].normalMapPath));
            object[o].uNormalMap = texture.LoadTexture((char *) object[o].normalMapPath, o,
                                                       (int) object[o].textureFilter);
            if (object[o].uNormalMap == -1) {
                object[o].uNormalMap = texture.LoadTexture((char *) XM_DEFAULT_NORMAL_MAP, o,
                                                           (int) object[o].textureFilter);
            }

            strcpy(object[o].specularMapPath, makeOptionalTexturePath((char *) object[o].specularMapPath));
            object[o].uSpecularMap = texture.LoadTexture((char *) object[o].specularMapPath, o,
                                                         (int) object[o].textureFilter);
            if (object[o].uSpecularMap == -1) {
                object[o].uSpecularMap = texture.LoadTexture((char *) XM_DEFAULT_SPECULAR_MAP, o,
                                                             (int) object[o].textureFilter);
            }
        }

#ifdef _notex
        object[o].uDiffuseMap = texture.LoadTexture((char*)XM_DEFAULT_DIFFUSE_MAP, o, (int)object[o].textureFilter);
#else
        object[o].uDiffuseMap = texture.LoadTexture((char *) object[o].texturePath, o, (int) object[o].textureFilter);
        if (object[o].uDiffuseMap == -1)
            object[o].uDiffuseMap = texture.LoadTexture((char *) XM_DEFAULT_DIFFUSE_MAP, o,
                                                        (int) object[o].textureFilter);
#endif

        if (object[o].bReflective || object[o].bRefractive) {
            object[o].uCubeMap = texture.LoadXMCubemap(makeCubeMapPath(path, object[o].name));
        }

        glGenBuffersARB(1, &object[o].uVertexVBO);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uVertexVBO);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, object[o].iNumVertices * 3 * sizeof(float), object[o].vertex,
                        GL_STATIC_DRAW_ARB);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glGenBuffersARB(1, &object[o].uCBoxVBO);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uCBoxVBO);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, 36 * sizeof(float), object[o].BoxVertices, GL_STATIC_DRAW_ARB);
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

        if (gui.bUseBumpMapping) {
            glGenBuffersARB(1, &object[o].uTangentVBO);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uTangentVBO);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, object[o].iNumVertices * 3 * sizeof(float), object[o].tangent,
                            GL_STATIC_DRAW_ARB);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        }

        glGenBuffersARB(1, &object[o].uFaceVBO);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, object[o].uFaceVBO);
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, object[o].iNumFaces * 3 * sizeof(unsigned int), object[o].face,
                        GL_STATIC_DRAW_ARB);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

        if (object[o].texcoord) delete[] object[o].texcoord;
        if (object[o].tangent) delete[] object[o].tangent;
        if (object[o].normal) delete[] object[o].normal;

        if (o > 200)
            gui.mainProgressBar.SetPercents(13);
        if (o > 500)
            gui.mainProgressBar.SetPercents(17);
        if (o > 700)
            gui.mainProgressBar.SetPercents(20);
        if (o > 900)
            gui.mainProgressBar.SetPercents(24);
        if (o > 1100)
            gui.mainProgressBar.SetPercents(27);
        if (o > 1300)
            gui.mainProgressBar.SetPercents(30);
        if (o > 1500)
            gui.mainProgressBar.SetPercents(34);
        if (o > 1700)
            gui.mainProgressBar.SetPercents(39);
        if (o > 1900)
            gui.mainProgressBar.SetPercents(43);
        if (o > 2200)
            gui.mainProgressBar.SetPercents(48);
        if (o > 2400)
            gui.mainProgressBar.SetPercents(56);
        if (o > 2800)
            gui.mainProgressBar.SetPercents(63);
        if (o > 2900)
            gui.mainProgressBar.SetPercents(67);
        if (o > 3100)
            gui.mainProgressBar.SetPercents(72);
        if (o > 3300)
            gui.mainProgressBar.SetPercents(77);
        if (o > 3500)
            gui.mainProgressBar.SetPercents(82);
        if (o > 3600)
            gui.mainProgressBar.SetPercents(85);
        if (o > 3800)
            gui.mainProgressBar.SetPercents(86);
    }
    fclose(pFile);
    //glGenTextures(1, &CubeMapID);

    //tady nekde, bych mel hodit nacteni skybox textur
    //predni
    //zadni
    //spodni
    //vrchni
    //prava
    //leva

    gui.mainProgressBar.SetPercents(88);
    gui.mainProgressBar.SetMessage("Loading skybox ...");

    char *arrTextures[6] =
            {
                    "textures/skybox/sky2/highres_px.dds",
                    "textures/skybox/sky2/highres_nx.dds",
                    "textures/skybox/sky2/highres_py.dds",
                    "textures/skybox/sky2/highres_ny.dds",
                    "textures/skybox/sky2/highres_pz.dds",
                    "textures/skybox/sky2/highres_nz.dds"
            };

    if (!skybox.Create(arrTextures, worldRadius * 2)) {
        printf("error: fatal error::Skybox!\n");
        return false;
    }


    gui.mainProgressBar.SetPercents(98);
    gui.mainProgressBar.SetMessage("Loading shaders ...");

    if (!xmlLighting.CreateLevelLighting()) {
        printf("error: cannot create xml lighting shaders!\n");
        return false;
    }

    if (!post.CreateFBO()) {
        printf("error: cannot create main FBO!\n");
        return false;
    }

    if (gui.bUseShadows) {
        if (!CreateShadowMap()) {
            printf("error: cannot create shadow map!\n");
            system("pause");
            return false;
        }
    }

#ifdef _basic
    printf("Svetel: %d Objektu: %d Entit: %d ", iNumLights, iNumObjects, iNumEntities);
#endif

    characters.Load();
    camera.Create(GetRandomPlayerSpawn(), vec3(15, 48, 15), vec3(180, 90, 0), 5.0f, 30.0f, 100.0, FIRST_PERSON);

    gui.mainProgressBar.SetPercents(100);
    gui.mainProgressBar.SetMessage("o_O!");
    if (!netw.SetSlotLoaded()) {
        printf("error: cannot set slot loaded!");
        return false;
    }

    fp = fopen("pozice.txt", "w");

    return true;
}

void xmLevel::Draw() {
    glPushMatrix();
    {
        if (!gui.IsPlayerDead()) {
            fprintf(fp, "pos %f %f %f %f %f cislo %d %d %d %d %d %d %d %d %d %d imprint\n",
                    camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z,
                    camera.GetAngle().x, camera.GetAngle().y,
                    camera.IsCameraWalking(),
                    camera.IsCameraMoving(),
                    camera.IsCameraStrafingLeft(),
                    camera.IsCameraStrafingRight(),
                    camera.IsJumping(),
                    camera.IsCrouched(),
                    weapon.GetCurrentWeaponID(),
                    weapon.GetCurrentWeaponAnimID(),
                    weapon.GetCurrentWeaponFrame(),
                    0);
        }
        SetLights();
        if (GetAsyncKeyState('R') && !hud.IsChatActive()) camera.SetPosition(GetRandomPlayerSpawn());

        skybox.Render();
        skybox.RenderSun();

        xmlLighting.EnableShader(ShadowMapID, iNumVisibleAreaLights);
        xmlLighting.SetMaterialType();

        xmlLighting.HideShadows();
#ifdef _weapons
        weapon.DrawFPSWeapon();
#endif
        xmlLighting.ShowShadows();

        if (weapon.isFlashLightActive()) CalculateFlashlightMatrix();

        RenderEntities(true);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        iNVObjects = 0;

        for (int o = 0; o < iNumObjects; o++) {
#ifdef _collision
            if (SphereInSphere(camera.GetPosition(), camera.GetRadius(), object[o].center, object[o].radius) &&
                object[o].bNoColl && camera.GetType() != FREE_LOOK) {
                if (object[o].bUseCollBox) {
                    for (int i = 0; i < 36; i++) {
                        vec3 v0 = object[o].BoxVertices[i];
                        vec3 v1 = object[o].BoxVertices[i + 1];
                        vec3 v2 = object[o].BoxVertices[i + 2];
                        if (!frustum.PolygonInFrustum(v0, v1, v2)) continue;

                        //cartridges.CheckCollision(-v0,-v1,-v2);
                        camera.SetPosition(CollisionWithLevel(-v0, -v1, -v2, (char *) object[o].name, object[o].radius,
                                                              object[o].center, camera.GetPosition(), 8.0f,
                                                              camera.GetCameraSize()));
                    }
                } else {
                    for (int i = 0; i < object[o].iNumFaces; i++) {
                        vec3 v0 = object[o].vertex[object[o].face[i].a];
                        vec3 v1 = object[o].vertex[object[o].face[i].b];
                        vec3 v2 = object[o].vertex[object[o].face[i].c];
                        if (!frustum.PolygonInFrustum(v0, v1, v2)) continue;

                        //cartridges.CheckCollision(-v0,-v1,-v2);
                        camera.SetPosition(CollisionWithLevel(-v0, -v1, -v2, (char *) object[o].name, object[o].radius,
                                                              object[o].center, camera.GetPosition(), 8.0f,
                                                              camera.GetCameraSize()));
                    }
                }
            }

#endif
            if (!frustum.SphereInFrustum(object[o].center, object[o].radius)) continue;

            /*if(camera.GetType() == THIRD_PERSON)
            {
                if(SphereInSphere(camera.GetPosition(), 10.0, object[o].center, object[o].radius) && object[o].bNoColl)
                {
                    for(int i=0; i < object[o].iNumFaces; i++)
                    {
                        vec3 v0 = object[o].vertex[object[o].face[i].a];
                        vec3 v1 = object[o].vertex[object[o].face[i].b];
                        vec3 v2 = object[o].vertex[object[o].face[i].c];

                        //cartridges.CheckCollision(-v0,-v1,-v2);
                        camera.SetPosition(CollisionWithLevel(-v0,-v1,-v2, (char*)object[o].name, object[o].radius, object[o].center, camera.GetPosition(), 8.0f, camera.GetCameraSize()));
                    }
                }
            }*/

            if (weapon.isShooting() && 1 != 1) {
                for (int i = 0; i < bullet.iNumFiredBullets; i++) {
                    if (bullet.bullet[i].fired && !bullet.bullet[i].collided) {
                        float d;
                        //if(RaySphereIntersect(bullet.bullet[i].start, bullet.bullet[i].direction, object[o].center, object[o].radius, d))
                        {
                            for (int j = 0; j < object[o].iNumFaces; j++) {
                                vec3 v0 = object[o].vertex[object[o].face[j].a];
                                vec3 v1 = object[o].vertex[object[o].face[j].b];
                                vec3 v2 = object[o].vertex[object[o].face[j].c];
                                if (!frustum.PolygonInFrustum(v0, v1, v2)) continue;

                                if (RayIntersectsTriangle(bullet.bullet[i].start, bullet.bullet[i].direction, -v0, -v1,
                                                          -v2, d)) {
                                    vec3 normal = Normalize(Cross(v0 - v1, v1 - v2));
                                    bullet.bullet[i].fired = false;
                                    bullet.bullet[i].collided = true;
                                    bullet.bullet[i].collPoints[0] = bullet.bullet[i].start;
                                    bullet.bullet[i].collPoints[1] =
                                            bullet.bullet[i].start + bullet.bullet[i].direction * d;
                                    weapon.AddDecal(bullet.bullet[i].collPoints[1], normal, rand() % 4);
                                }
                            }
                        }
                    }
                }
            }
            iNVObjects++;


            if (object[o].bUseAlphaTest) {
                //glAlphaFunc(GL_GREATER,0.1f);
                //glEnable(GL_ALPHA_TEST);
                //glEnable(GL_BLEND);
            }

            xmlLighting.BindTextures(object[o].uDiffuseMap, object[o].uNormalMap, object[o].uSpecularMap,
                                     object[o].uCubeMap);

            int matType = 0;
            if (object[o].bReflective) matType = 1;
            if (object[o].bRefractive) matType = 2;
            if (object[o].bReflective && object[o].bRefractive) matType = 3;

            xmlLighting.SetMaterialType(matType);
            //glMaterialf(GL_FRONT, GL_SHININESS, object[o].shininess);

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

            //if(object[o].bUseAlphaTest)
            //{
            //glDisable(GL_BLEND);
            //glDisable(GL_ALPHA_TEST);
            //}

        }

        //weapon.DropWeapon(2);

        characters.Draw(true);
        glDisable(GL_CULL_FACE);
        xmlLighting.DisableShader();
    }

    weapon.DrawDecals();
    weapon.DrawSpark();

    glPopMatrix();

}


void xmLevel::SetLights(void) {
    glPushMatrix();
    {
        float position[] = {-mainLight.center.x, -mainLight.center.y, -mainLight.center.z, 1.0};
        glLightfv(GL_LIGHT0, GL_POSITION, position);

        int j = 0;
        iNumVisibleAreaLights = 0;
        for (int i = 0; i < iNumLights; i++) {
            short int type = areaLight[i].type;
            vec3 center = areaLight[i].center;
            vec3 dir = areaLight[i].target;
            color3f color = areaLight[i].color;
            float radius = areaLight[i].radius;

            if (frustum.SphereInFrustum(center, radius) ||
                SphereInSphere(camera.GetPosition(), camera.GetRadius(), center, radius)) {
                switch (type) {
                    case SPOT_LIGHT: {
                        float falloff = areaLight[i].falloff;
                        float hotspot = areaLight[i].hotspot;
                        float ambient[4] = {(float) type, falloff, hotspot, 1.0};
                        float position[4] = {center.x, center.y, center.z, 1.0};
                        float diffuse[4] = {color.r, color.g, color.b, 1.0};
                        float direction[4] = {dir.x, dir.y, dir.z, 1.0};
                        glLightfv(GL_LIGHT1 + j, GL_AMBIENT, ambient);
                        glLightfv(GL_LIGHT1 + j, GL_DIFFUSE, diffuse);
                        glLightfv(GL_LIGHT1 + j, GL_POSITION, position);
                        glLightfv(GL_LIGHT1 + j, GL_SPOT_DIRECTION, direction);
                    }
                        break;

                    case POINT_LIGHT: {
                        float ambient[4] = {(float) type, 1.0, 1.0, 1.0};
                        float position[4] = {center.x, center.y, center.z, 1.0};
                        float diffuse[4] = {color.r, color.g, color.b, radius};
                        glLightfv(GL_LIGHT1 + j, GL_AMBIENT, ambient);
                        glLightfv(GL_LIGHT1 + j, GL_DIFFUSE, diffuse);
                        glLightfv(GL_LIGHT1 + j, GL_POSITION, position);
                    }
                        break;
                }
                j++;
                iNumVisibleAreaLights++;
            }
        }
    }
    glPopMatrix();
}

void xmLevel::RenderEntities(bool bCollide) {
    for (int i = 0; i < iNumEntities; i++) {
        if (entity[i].id != -1) {
            if (weapon.weapon[entity[i].wid].type == VERTEXENTITY || weapon.weapon[entity[i].wid].type == BONEDENTITY) {
                if (weapon.weapon[entity[i].wid].type == VERTEXENTITY) {
                    bool bDestroyEntity = false;
                    if (entity[i].damage < 100.0) {
                        if (bCollide) {
                            vec3 entcenter = entity[i].center - levelVertexEntity[entity[i].id].center;
                            float radius = levelVertexEntity[entity[i].id].radius;
                            mat4 bbOrientation;
                            bbOrientation.Rotate(-entity[i].rotation);
                            bbOrientation.Translate(entity[i].center);

                            if (SphereInSphere(camera.GetPosition(), camera.GetRadius(), entcenter, radius)) {
                                for (int k = 0; k < 36; k++) {
                                    vec3 v0 = bbOrientation * levelVertexEntity[entity[i].id].BoxVertices[k];
                                    vec3 v1 = bbOrientation * levelVertexEntity[entity[i].id].BoxVertices[k + 1];
                                    vec3 v2 = bbOrientation * levelVertexEntity[entity[i].id].BoxVertices[k + 2];

                                    if (!frustum.PolygonInFrustum(v0, v1, v2)) continue;
                                    camera.SetPosition(CollisionWithLevel(-v0, -v1, -v2, "", radius, entcenter,
                                                                          camera.GetPosition(), 8.0f,
                                                                          camera.GetCameraSize()));
                                }
                            }

                            if (!frustum.SphereInFrustum(entcenter, radius)) continue;
                            for (int o = 0; o < levelVertexEntity[entity[i].id].iNumObjects; o++) {
                                mat4 bbOrientation;
                                bbOrientation.Translate(-entity[i].center);
                                bbOrientation.Rotate(-entity[i].rotation);
                                vec3 center = bbOrientation *
                                              levelVertexEntity[entity[i].id].object[o].frame[levelVertexEntity[entity[i].id].iCurrFrame].center;
                                float radius = 10.0;//levelVertexEntity[entity[i].id].object[o].radius;
#ifdef _drawbbs
                                glPushMatrix();
                                glTranslatef(-center.x, -center.y, -center.z);
                                glutWireSphere(radius, 12, 12);
                                glPopMatrix();
#endif

                                int nHits = bullet.CheckBulletsForCollision(center, radius);

                                if (nHits > 0) {
                                    entity[i].damage += weapon.weapon[entity[i].wid].damage.head;
                                    printf("ENTITY hit %d - %d - %f %f %f\n", entity[i].id, entity[i].damage, center.x,
                                           center.y, center.z);
                                    if (entity[i].damage > 100.0) {
                                        //rozbiju stoleceq
                                        entity[i].damaged = true;
                                        bDestroyEntity = true;
                                    }
                                }
                            }
                        }
                    }
                    glPushMatrix();
                    {
                        glTranslatef(entity[i].center.x, entity[i].center.y, entity[i].center.z);
                        //glutWireSphere(levelVertexEntity[entity[i].id].radius,12,12);

                        glRotatef(entity[i].rotation.z, 0, 0, 1);
                        glRotatef(entity[i].rotation.y, 0, 1, 0);
                        glRotatef(entity[i].rotation.x, 1, 0, 0);
                        bDestroyEntity = levelVertexEntity[entity[i].id].SetSequence(
                                weapon.weapon[entity[i].wid].animStart,
                                weapon.weapon[entity[i].wid].animEnd,
                                bDestroyEntity);
                        levelVertexEntity[entity[i].id].Draw();
                    }
                    glPopMatrix();
                } else {
                    if (bCollide) {
                        //kreslim kosterni XMA
                        mat4 bbOrientation;

                        bbOrientation.Translate(entity[i].center);
                        //bbOrientation.Rotate(entity[i].rotation.x, -entity[i].rotation.z, entity[i].rotation.y);

                        vec3 entcenter = entity[i].center;// + levelBonedEntity[entity[i].id].center;
                        float radius = 30;//levelBonedEntity[entity[i].id].radius;
                        if (!frustum.SphereInFrustum(entcenter, radius)) continue;
                        if (GetAsyncKeyState('E') && !bAnimateEntity &&
                            SphereInSphere(camera.GetPosition(), camera.GetRadius(), entcenter, radius)) {
                            if (!bDoorsOpened) {
                                iCurrAnimation = 1;
                                bAnimateEntity = true;
                                bWannaOpenDoors = true;
                            }

                            if (bDoorsOpened) {
                                iCurrAnimation = 2;
                                bAnimateEntity = true;
                                bWannaOpenDoors = false;
                            }
                        }


                        bShowIHand = SphereInSphere(camera.GetPosition(), camera.GetRadius(), entcenter, radius);

                        if (bShowIHand) {
                            for (int o = 0; o < levelBonedEntity[entity[i].id].iNumObjects; o++) {
                                for (int f = 0; f < levelBonedEntity[entity[i].id].object[o].iNumVertices; f++) {
                                    vec3 v0 = bbOrientation * levelBonedEntity[entity[i].id].object[o].animverts[f + 0];
                                    vec3 v1 = bbOrientation * levelBonedEntity[entity[i].id].object[o].animverts[f + 1];
                                    vec3 v2 = bbOrientation * levelBonedEntity[entity[i].id].object[o].animverts[f + 2];
                                    vec3 normal = Normalize(Cross(v0 - v1, v1 - v2));
                                    if (normal.x >= -1.0 || normal.z >= -1.0 && bAnimateEntity) {
                                        if (!bDoorsOpened && bWannaOpenDoors) {
                                            iCurrAnimation = 3;
                                        } else {
                                            iCurrAnimation = 4;
                                        }
                                    }
                                    //glBegin(GL_LINES);
                                    //{
                                    //    glVertex3fv((float*)&v0);
                                    //    glVertex3fv((float*)&v1);
                                    //    glVertex3fv((float*)&v2);
                                    //}
                                    //glEnd();
                                    camera.SetPosition(CollisionWithLevel(-v0, -v1, -v2, "", radius, entcenter,
                                                                          camera.GetPosition(), 8.0f,
                                                                          camera.GetCameraSize()));
                                }
                            }
                        }


                        bAnimateEntity = levelBonedEntity[entity[i].id].setAnimationByID(iCurrAnimation,
                                                                                         bAnimateEntity);
                        if (!bAnimateEntity) {
                            if (bWannaOpenDoors) {
                                bDoorsOpened = true;
                            } else {
                                bDoorsOpened = false;
                            }
                        }
                    }

                    glPushMatrix();
                    {
                        glCullFace(GL_BACK);
                        glTranslatef(entity[i].center.x, entity[i].center.y, entity[i].center.z);
                        glRotatef(entity[i].rotation.z, 0, 0, 1);
                        glRotatef(entity[i].rotation.y, 0, 1, 0);
                        glRotatef(entity[i].rotation.x, 1, 0, 0);

                        levelBonedEntity[entity[i].id].draw();
                    }
                    glPopMatrix();
                }
            } else {
                if (!frustum.SphereInFrustum(entity[i].center + weaponModel[entity[i].id].center,
                                             weaponModel[entity[i].id].radius))
                    continue;

                glPushMatrix();
                {
                    glCullFace(GL_FRONT);
                    glTranslatef(entity[i].center.x, entity[i].center.y, entity[i].center.z);
                    glRotatef(entity[i].rotation.z, 0, 0, 1);
                    glRotatef(entity[i].rotation.y, 0, 1, 0);
                    glRotatef(entity[i].rotation.x, 1, 0, 0);
                    weaponModel[entity[i].id].Draw();
                }
                glPopMatrix();
            }
        }
    }
}

bool xmLevel::isOccluded(int o) {
    GLuint queryID;
    GLint pixelCount;

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

    glGenOcclusionQueriesNV(1, &queryID);
    glBeginOcclusionQueryNV(queryID);
    {
        //glPushMatrix();
        //    glTranslatef(object[o].center.x, object[o].center.y, object[o].center.z);
        //    glutSolidSphere(14, 14, object[o].radius);
        //glPopMatrix();
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uVertexVBO);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, object[o].uFaceVBO);
        glDrawElements(GL_TRIANGLES, object[o].iNumFaces * 3, GL_UNSIGNED_INT, NULL);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }
    glEndOcclusionQueryNV();
    glFlush();

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);

    glGetOcclusionQueryivNV(queryID, GL_PIXEL_COUNT_NV, &pixelCount);
    return (pixelCount > 1);
}


void xmLevel::RenderToCubeMap(void) {
    glPushMatrix();
    {
        unsigned int pos = 0, px = 0, py = 0, renderableTexture = 256;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(90.0, 1.0, 1.0, 10000.0);
        glMatrixMode(GL_MODELVIEW);
        if (4 * renderableTexture <= window.w || 3 * renderableTexture <= window.h) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        for (int i = 0; i < 6; i++) {
            switch (i) {
                case 0:
                    px = 3 * renderableTexture;
                    py = renderableTexture;
                    break;    // pozerame na -z
                case 1:
                    px = 2 * renderableTexture;
                    py = renderableTexture;
                    break;    // pozerame na +x
                case 2:
                    px = renderableTexture;
                    py = renderableTexture;
                    break;    // pozerame na +z
                case 3:
                    px = 0;
                    py = renderableTexture;
                    break;    // pozerame na -x
                case 4:
                    px = renderableTexture;
                    py = 0;
                    break;    // pozerame na +y
                case 5:
                    px = renderableTexture;
                    py = 2 * renderableTexture;
                    break;    // pozerame na -y
            }

            if (4 * renderableTexture > window.w || 3 * renderableTexture > window.h) {
                px = 0;
                py = 0;
            }

            glViewport(px, py, renderableTexture, renderableTexture);
            if (4 * renderableTexture > window.w || 3 * renderableTexture > window.h)
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            {
                if (i < 4) glRotatef(180, 0, 0, 1);

                switch (i) {
                    case 0:
                        break;                            // pozerame na -z
                    case 1:
                        glRotatef(90, 0, 1, 0);
                        break;        // pozerame na +x
                    case 2:
                        glRotatef(180, 0, 1, 0);
                        break;    // pozerame na +z
                    case 3:
                        glRotatef(270, 0, 1, 0);
                        break;    // pozerame na -x
                    case 4:
                        glRotatef(-90, 1, 0, 0);
                        break;    // pozerame na +y
                    case 5:
                        glRotatef(90, 1, 0, 0);
                        break;        // pozerame na -y
                }
                vec3 cam = camera.GetPosition();
                //glTranslatef(-cam.x, -cam.y, -cam.z);

                //kresleni objektu
                skybox.Render();

                switch (i) {
                    case 0:
                        pos = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
                        break;    // pozerame na -z
                    case 1:
                        pos = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
                        break;    // pozerame na +x
                    case 2:
                        pos = GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
                        break;    // pozerame na +z
                    case 3:
                        pos = GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
                        break;    // pozerame na -x
                    case 4:
                        pos = GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
                        break;    // pozerame na +y
                    case 5:
                        pos = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
                        break;    // pozerame na -y
                }

                glEnable(GL_TEXTURE_CUBE_MAP_ARB);
                glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, CubeMapID);
                glCopyTexSubImage2D(pos, 0, 0, 0, px, py, renderableTexture, renderableTexture);
                glDisable(GL_TEXTURE_CUBE_MAP_ARB);
            }
            glPopMatrix();
        }
        ResizeView(window.w, window.h);
    }
    glPopMatrix();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void xmLevel::RenderToShadowMap(void) {
    glPushMatrix();
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOid);
        glViewport(0, 0, ShadowMapSize, ShadowMapSize);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonOffset(1.0, 1.0);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        //gluPerspective(120.0, window.w / window.h, 600.0, 10000.0);
        glOrtho(-2048, 2048, -2048, 2048, 1.0, 100000.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        gluLookAt(0.0, -1549.301270, 373.978149, 0.0f, -0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        {
            for (int o = 0; o < iNumObjects; o++) {
                if (object[o].bUseAlphaTest) {
                    glAlphaFunc(GL_GREATER, 0.1f);
                    glEnable(GL_ALPHA_TEST);
                    glBindTexture(GL_TEXTURE_2D, object[o].uDiffuseMap);
                }

                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uTexCoordVBO);
                glTexCoordPointer(2, GL_FLOAT, 0, 0);
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

                glEnableClientState(GL_VERTEX_ARRAY);
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, object[o].uVertexVBO);
                glVertexPointer(3, GL_FLOAT, 0, (char *) NULL);
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
                glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, object[o].uFaceVBO);
                glDrawElements(GL_TRIANGLES, object[o].iNumFaces * 3, GL_UNSIGNED_INT, NULL);
                glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);

                if (object[o].bUseAlphaTest) {
                    glDisable(GL_ALPHA_TEST);
                }
            }

            //weapon.DrawFPSWeapon();
            RenderEntities(false);
            characters.Draw(false);
        }
        glDisable(GL_CULL_FACE);
        GetTextureMatrix(GL_TEXTURE7_ARB);
        glDisable(GL_POLYGON_OFFSET_FILL);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        ResizeView(window.w, window.h);
    }
    glPopMatrix();
}

void xmLevel::CalculateFlashlightMatrix(void) {
    glPushMatrix();
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 1.0, 1.0, 100.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(camera.GetAngle().x, 1, 0, 0);
        glRotatef(camera.GetAngle().y, 0, 1, 0);
        glTranslatef(camera.GetPosition().x,
                     camera.GetPosition().y,
                     camera.GetPosition().z);

        GetTextureMatrix(GL_TEXTURE1_ARB);
        ResizeView(window.w, window.h);
    }
    glPopMatrix();
}
