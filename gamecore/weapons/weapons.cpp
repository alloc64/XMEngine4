/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../../main.h"

extern xmdModel *weaponModel;
extern xmAnimation *fpsWeaponModel;

extern xmVertexAnimation *levelVertexEntity;
extern xmAnimation *levelBonedEntity;

extern Camera camera;
extern HUD hud;
extern Characters characters;
Cartridge cartridges;
Bullet bullet;

float angle = 0;


void xmWeapon::DrawFPSWeaponAt(int wID, int startRange, int endRange, int rate) {
    fpsWeaponModel[wID].setAnimation(startRange, endRange, rate, true);
    fpsWeaponModel[wID].FPSPerson();
}

void xmWeapon::DrawFPSWeaponAt(int wID, int animID) {
    fpsWeaponModel[wID].setAnimationByID(animID, true);
    fpsWeaponModel[wID].FPSPerson();
}

vec3 sangle = vec3(0);

void xmWeapon::DrawFPSWeapon() {
    if (camera.GetType() == THIRD_PERSON || camera.GetType() == FREE_LOOK ||
        (camera.GetType() == FIRST_PERSON && hud.IsBuyMenuShown()))
        return;

    if (!characters.IsCurrentWatchedPlayerDead() && gui.IsPlayerDead()) {
        //vytahnout ze siti a prenastavovat dynamicky
        int id = characters.GetCurrentWatchedPlayerID();
        DrawFPSWeaponAt(netw.playerPositions[id].wid,
                        netw.playerPositions[id].currFrame,
                        netw.playerPositions[id].currFrame,
                        30);
        return;
    }

    if (GetAsyncKeyState('Z') && !bKnifeAnim && !hud.IsChatActive()) bKnifeAnim = true;
    if (GetAsyncKeyState('F') && !hud.IsChatActive()) {
        if (!bFlashLightAnim) {
            bFlashLightAnim = true;
            bWeaponChanged = true;
            bHideFlashlight = false;
        } else {
            bHideFlashlight = true;
        }
    }

    if (bKnifeAnim) {
        bKnifeAnim = fpsWeaponModel[0].setAnimationByID(0, bKnifeAnim);
        if (!bKnifeAnim) bWeaponChanged = true; else fpsWeaponModel[0].FPSPerson();
    } else {
        if (bFlashLightAnim) {
            wid = 1;
            if (bHideFlashlight) {
                bHideFlashlight = fpsWeaponModel[wid].setAnimationByID(2, true);
                if (!bHideFlashlight) {
                    bWeaponChanged = true;
                    bFlashLightAnim = false;
                }
                bFlashLightAnimated = false;
            } else {
                if (bWeaponChanged) {

                    bWeaponChanged = fpsWeaponModel[wid].setAnimationByID(3, true);
                    if (!bWeaponChanged) {
                        fpsWeaponModel[wid].setAnimationByID(0, true);
                        bFlashLightAnimated = true;
                    }
                } else {
                    if (camera.IsCameraMoving()) {
                        if (camera.IsCameraWalking()) {
                            fpsWeaponModel[wid].setAnimationByID(0, true);
                        } else {
                            fpsWeaponModel[wid].setAnimationByID(1, true);
                        }
                    }
                }
            }

            fpsWeaponModel[1].FPSPerson();
        } else {
            if (iCurrWeapon < 0 || iCurrWeapon > 2) iCurrWeapon = 0;

            switch (iCurrWeapon) {
                case MAIN_WEAPON: //main equip
                {
                    wid = 6;
                    if (weapon[wid].type == ASSAULT_RIFLE || weapon[wid].type == SMG || weapon[wid].type == SNIPER) {
                        if (bWeaponChanged) {
                            bWeaponChanged = fpsWeaponModel[wid].setAnimationByID(3, true);
                            if (!bWeaponChanged) {
                                fpsWeaponModel[wid].setAnimationByID(0, true);
                            }
                        } else {
                            if (GetAsyncKeyState(VK_LBUTTON) && !bShoot) {
                                bShoot = true;
                                cartridges.Fire();
                                bullet.Fire();
                                //weapon[wid].DecreaseMagazine();
                            }

                            if (GetAsyncKeyState(VK_RBUTTON) && !bZoomClicked && !bZoomAnimated) {
                                bZoomClicked = true;
                                bZoom = !bZoom;
                                bWasZoomed = true;
                                bZoomAnimated = true;
                            }

                            if (weapon[wid].ReloadWeapon()) {
                                bWasZoomed = true;
                                bZoom = false;
                                bAnimateReload = true;
                            }

                            if (bAnimateReload) {
                                //zanimuju riloud
                                //bAnimateReload = fpsWeaponModel[wid].setAnimationByID(9, true);
                                if (!bAnimateReload) {
                                    bReloadWeapon = false;
                                }
                            } else {
                                if (bZoom) {
                                    if (bZoomAnimated) {
                                        bZoomAnimated = fpsWeaponModel[wid].setAnimationByID(5, bZoomClicked);
                                    } else {
                                        if (bShoot) {
                                            bShoot = fpsWeaponModel[wid].setAnimationByID(7, bShoot);

                                            if (fpsWeaponModel[wid].isAnimationInHalf()) {
                                                sangle.x -= 4.0 * timer.GetDT();
                                            } else {
                                                sangle.x += 4.0 * timer.GetDT();
                                            }

                                            //for(int j=0; j < weapon[i].magazine; j++)
                                            //{
                                            //    cartridges[j].Create(fpsWeaponModel[i].GetBulletPosition(, 0);
                                            //}
                                        } else {
                                            sangle.x = 0.0;
                                            if (camera.IsCameraMoving()) {
                                                if (camera.IsCameraWalking()) {
                                                    fpsWeaponModel[wid].setAnimationByID(6, true);
                                                } else {
                                                    bWasZoomed = true;
                                                    bZoom = false;
                                                }
                                            }
                                        }
                                    }
                                } else {
                                    if (!bWasZoomed) {
                                        if (bShoot) {
                                            bShoot = fpsWeaponModel[wid].setAnimationByID(4, bShoot);

                                            if (fpsWeaponModel[wid].isAnimationInHalf()) {
                                                sangle.x += 4.0 * timer.GetDT();
                                            } else {
                                                sangle.x -= 4.0 * timer.GetDT();
                                            }
                                        } else {
                                            sangle.x = 0;
                                            if (camera.IsCameraMoving()) {
                                                if (camera.IsCameraWalking()) {
                                                    fpsWeaponModel[wid].setAnimationByID(0, true);
                                                } else {
                                                    fpsWeaponModel[wid].setAnimationByID(1, true);
                                                }
                                            }
                                        }
                                    }
                                    if (bWasZoomed) {
                                        bZoomAnimated = fpsWeaponModel[wid].setAnimationByID(8, bWasZoomed);
                                        if (!bZoomAnimated) bWasZoomed = false;
                                    }

                                }
                            }

                            if (!GetAsyncKeyState(VK_RBUTTON) && bZoomClicked && !bZoomAnimated) bZoomClicked = false;
                        }


                        camera.SetAngle(camera.GetAngle() + sangle);
                        cartridges.SetCenters(fpsWeaponModel[wid].GetBulletPosition());
                        cartridges.Draw(weapon[wid].cartridgeType);
                        //bullet.Draw();
                        fpsWeaponModel[wid].FPSPerson();
                    }
                }
                    break;

                case SECONDARY_WEAPON: // secondary equip
                {
                    wid = 3;
                    if (weapon[wid].type == PISTOL) {
                        if (bWeaponChanged) {
                            bWeaponChanged = fpsWeaponModel[wid].setAnimationByID(3, true);
                            if (!bWeaponChanged) fpsWeaponModel[wid].setAnimationByID(0, true);
                        } else {
                            if (GetAsyncKeyState(VK_LBUTTON) && !bShoot) {
                                bShoot = true;
                                cartridges.Fire();
                                bullet.Fire();
                                //weapon[wid].DecreaseMagazine();
                            }

                            if (GetAsyncKeyState(VK_RBUTTON) && !bZoomClicked && !bZoomAnimated) {
                                bZoomClicked = true;
                                bZoom = !bZoom;
                                bWasZoomed = true;
                                bZoomAnimated = true;
                            }

                            //if(weapon[wid].ReloadWeapon())
                            //{
                            //     bWasZoomed = true;
                            //     bZoom = false;
                            //     bAnimateReload = true;
                            //}

                            if (bAnimateReload) {
                                //zanimuju riloud
                                //bAnimateReload = fpsWeaponModel[wid].setAnimationByID(9, true);
                                if (!bAnimateReload) {
                                    bReloadWeapon = false;
                                }
                            } else {
                                if (bZoom) {
                                    if (bZoomAnimated) {
                                        bZoomAnimated = fpsWeaponModel[wid].setAnimationByID(5, bZoomClicked);
                                    } else {
                                        if (bShoot) {
                                            bShoot = fpsWeaponModel[wid].setAnimationByID(7, bShoot);

                                            if (fpsWeaponModel[wid].isAnimationInHalf()) {
                                                sangle.x -= 4.0 * timer.GetDT();
                                            } else {
                                                sangle.x += 4.0 * timer.GetDT();
                                            }
                                            //for(int j=0; j < weapon[i].magazine; j++)
                                            //{
                                            //    cartridges[j].Create(fpsWeaponModel[i].GetBulletPosition(, 0);
                                            //}
                                        } else {
                                            sangle.x = 0;
                                            if (camera.IsCameraMoving()) {
                                                if (camera.IsCameraWalking()) {
                                                    fpsWeaponModel[wid].setAnimationByID(6, true);
                                                } else {
                                                    bWasZoomed = true;
                                                    bZoom = false;
                                                }
                                            }
                                        }
                                    }
                                } else {
                                    if (!bWasZoomed) {
                                        if (bShoot) {
                                            bShoot = fpsWeaponModel[wid].setAnimationByID(4, bShoot);

                                            if (fpsWeaponModel[wid].isAnimationInHalf()) {
                                                sangle.x += 4.0 * timer.GetDT();
                                            } else {
                                                sangle.x -= 4.0 * timer.GetDT();
                                            }

                                        } else {
                                            sangle.x = 0;
                                            if (camera.IsCameraMoving()) {
                                                if (camera.IsCameraWalking()) {
                                                    fpsWeaponModel[wid].setAnimationByID(0, true);
                                                } else {
                                                    fpsWeaponModel[wid].setAnimationByID(1, true);
                                                }
                                            }
                                        }
                                    }
                                    if (bWasZoomed) {
                                        bZoomAnimated = fpsWeaponModel[wid].setAnimationByID(8, bWasZoomed);
                                        if (!bZoomAnimated) bWasZoomed = false;
                                    }

                                }
                            }

                            if (!GetAsyncKeyState(VK_RBUTTON) && bZoomClicked && !bZoomAnimated) bZoomClicked = false;
                        }

                        camera.SetAngle(camera.GetAngle() + sangle);
                        cartridges.SetCenters(fpsWeaponModel[wid].GetBulletPosition());
                        cartridges.Draw(weapon[wid].cartridgeType);
                        fpsWeaponModel[wid].FPSPerson();
                    }
                }
                    break;

                case OTHER_WEAPON: // grenades etc, fuck ...
                {
                    wid = 4;
                    if (weapon[wid].type == GRENADE) {

                        if (bWeaponChanged) {
                            bWeaponChanged = fpsWeaponModel[wid].setAnimationByID(3, true);
                            if (!bWeaponChanged) fpsWeaponModel[wid].setAnimationByID(0, true);
                        } else {
                            if (GetAsyncKeyState(VK_LBUTTON) && !bShoot) {
                                bShoot = true;
                                //cartridges.Fire();
                                //bullet.Fire();
                                //weapon[wid].DecreaseMagazine();
                            }
                            if (bShoot) {
                                bShoot = fpsWeaponModel[wid].setAnimationByID(4, bShoot);
                            } else {
                                if (camera.IsCameraMoving()) {
                                    if (camera.IsCameraWalking()) {
                                        fpsWeaponModel[wid].setAnimationByID(0, true);
                                    } else {
                                        fpsWeaponModel[wid].setAnimationByID(1, true);
                                    }
                                }
                            }
                        }
                        fpsWeaponModel[wid].FPSPerson();
                    }
                }
                    break;
            }
        }
    }
}

void xmWeapon::DrawSpark(void) {
    if (bShoot && !fpsWeaponModel[wid].isAnimationInHalf()) fpsWeaponModel[wid].drawSpark();
}

void xmWeapon::DrawDecals(void) {
    glAlphaFunc(GL_GREATER, 0.1f);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    glColor4f(1, 1, 1, 0.95);

    for (int i = 0; i < iNumDecals; i++) {
        glPushMatrix();
        {
            vec3 pos = -decal[i].position;
            vec3 normal = decal[i].normal;

            vec3 Right = Cross(normal, vec3(0.0, 1.0, 0.0));
            vec3 Up = Cross(normal, Right);

            float size = 2.0f;
            vec3 A = ((-Right - Up) * size);
            vec3 B = ((Right - Up) * size);
            vec3 C = ((Right + Up) * size);
            vec3 D = ((-Right + Up) * size);

            glTranslatef(pos.x - normal.x * 0.1, pos.y - normal.y * 0.1, pos.z - normal.z * 0.1);

            glBindTexture(GL_TEXTURE_2D, decalID[decal[i].texID]);
            glBegin(GL_QUADS);
            {
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(A.x, A.y, A.z);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(B.x, B.y, B.z);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(C.x, C.y, C.z);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(D.x, D.y, D.z);
            }
            glEnd();
        }
        glPopMatrix();
    }

    glColor4f(1, 1, 1, 1);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
}

void xmWeapon::DrawWeapon(int wid, vec3 pos) {
    glPushMatrix();
    {
        glTranslatef(pos.x, pos.y, pos.z);
        weaponModel[wid].Draw();
    }
    glPopMatrix();
}

void xmWeapon::DrawWeapon(int wid) {
    weaponModel[wid].Draw();
}

bool xmWeapon::DropWeapon(int wid) {
    if (GetAsyncKeyState('G'))// && !bDropWeapon)
    {
        bDropWeapon = true;
        weapPos = camera.GetPosition() - (camera.GetCameraSize() / 2);
        weapVel = vec3(-camera.GetCameraDirection().x * 600.0f, 100.0f, -camera.GetCameraDirection().z * 600.0f);
        weapForce = vec3(0);

        return true;
    } else {
        if (bDropWeapon) {
            float dt = timer.GetDT() / 8.0;
            float mass = weapon[wid].weight;
            weapRadius = weaponModel[weapon[wid].id].radius;
            weapForce += vec3(0, -9.81 * mass, 0);
            weapVel += weapForce * mass * dt;
            weapPos += weapVel * dt;

            DrawWeapon(wid, -weapPos);

            return true;
        }
    }

    return false;
}

bool xmWeapon::Load(char *path) {
    FILE *pFile = fopen(path, "rb");
    if (!pFile) return false;

    char headerTitle[74];
    char header[7];

    fread(headerTitle, 1, 74, pFile);
    fread(header, 1, 7, pFile);
    fread(&iNumWeapons, 1, sizeof(short int), pFile);
    fread(&iNumVEntities, 1, sizeof(short int), pFile);
    fread(&iNumBEntities, 1, sizeof(short int), pFile);

    weapon = new xmWeapon[iNumWeapons + iNumBEntities + iNumVEntities];
    if (!weapon) return false;

    weaponModel = new xmdModel[iNumWeapons];//+iNumBEntities+iNumVEntities];
    if (!weaponModel) return false;

    fpsWeaponModel = new xmAnimation[iNumWeapons];
    if (!fpsWeaponModel) return false;

    levelVertexEntity = new xmVertexAnimation[iNumVEntities];
    if (!levelVertexEntity) return false;

    levelBonedEntity = new xmAnimation[iNumBEntities];
    if (!levelBonedEntity) return false;

    if (!cartridges.SetCartridges(iNumWeapons)) return false;

    int j = 0, k = 0;
    iNumWeapons += iNumBEntities + iNumVEntities;

    for (int i = 0; i < iNumWeapons; i++) {
        short int size = strlen(weapon[i].name) + 1;
        fread(&size, 1, sizeof(short int), pFile);
        fread(&weapon[i].name, 1, size, pFile);

        size = strlen(weapon[i].realName) + 1;
        fread(&size, 1, sizeof(short int), pFile);
        fread(&weapon[i].realName, 1, size, pFile);

        fread(&weapon[i].weight, 1, sizeof(float), pFile);
        fread(&weapon[i].magazine, 1, sizeof(int), pFile);
        fread(&weapon[i].supply, 1, sizeof(int), pFile);
        weapon[i].iCurrAmmoCount = weapon[i].magazine;

        fread(&size, 1, sizeof(short int), pFile);
        fread(&weapon[i].animatedModelPath, 1, size, pFile);

        fread(&weapon[i].animStart, 1, sizeof(int), pFile);
        fread(&weapon[i].animEnd, 1, sizeof(int), pFile);
        fread(&weapon[i].animLoopable, 1, sizeof(int), pFile);

        fread(&size, 1, sizeof(short int), pFile);
        fread(&weapon[i].staticModelPath, 1, size, pFile);

        fread(&size, 1, sizeof(short int), pFile);
        fread(&weapon[i].cartridgeModelPath, 1, size, pFile);

        fread(&weapon[i].type, 1, sizeof(int), pFile);
        fread(&weapon[i].menuType, 1, sizeof(int), pFile);
        fread(&weapon[i].throwable, 1, sizeof(bool), pFile);
        fread(&weapon[i].damage, 1, sizeof(Damage), pFile);
        fread(&weapon[i].efficiency, 1, sizeof(int), pFile);
        fread(&weapon[i].effectiveRange, 1, sizeof(int), pFile);

        weapon[i].id = i;
        weapon[i].wid = i;
        if ((weapon[i].type != KNIFE || weapon[i].type != VERTEXENTITY || weapon[i].type != BONEDENTITY) &&
            fopen(weapon[i].cartridgeModelPath, "rb")) {
            weapon[i].cartridgeType = cartridges.LoadCartridge(weapon[i].cartridgeModelPath);
        }

        if (weapon[i].staticModelPath && fopen(weapon[i].staticModelPath, "rb")) {
            if (!weaponModel[i].Load(weapon[i].staticModelPath)) {
                printf("error: can't load %s ::xmdModel!\n", weapon[i].staticModelPath);
                return false;
            } else weapon[i].bEntityLoaded = true;
        }

        if (!fopen(weapon[i].animatedModelPath, "rb")) printf("err: %s\n", weapon[i].animatedModelPath);

        if (weapon[i].animatedModelPath && fopen(weapon[i].animatedModelPath, "rb")) {
            if (weapon[i].type == VERTEXENTITY || weapon[i].type == BONEDENTITY) {
                if (weapon[i].type == VERTEXENTITY) {
                    if (!levelVertexEntity[j].Load(weapon[i].animatedModelPath)) {
                        printf("error: can't load %s ::xmPerVertexAnimation!\n", weapon[i].animatedModelPath);
                        return false;
                    }
                    weapon[i].id = j;
                    j++;
                } else {
                    if (weapon[i].type == BONEDENTITY) {
                        if (!levelBonedEntity[k].Load(weapon[i].animatedModelPath)) {
                            printf("error: can't load %s ::xmAnimation::Bones!\n", weapon[i].animatedModelPath);
                            return false;
                        }
                        weapon[i].id = k;
                        k++;
                    }
                }
            } else {
                if (!fpsWeaponModel[i].Load(weapon[i].animatedModelPath)) {
                    printf("error: can't load %s ::xmAnimation!\n", weapon[i].animatedModelPath);
                    return false;
                }
            }
        }

        char *path = strtok(weapon[i].animatedModelPath, ".");
        sprintf(weapon[i].spriteModelPath, "%s_sprite.dds", path);
        weapon[i].cartridgePos = fpsWeaponModel[i].GetBulletPosition();

    }
    fclose(pFile);

    for (int i = 0; i < 5; i++) {
        char name[255];
        sprintf(name, "gamecore/decals/%d.dds", i);
        decalID[i] = texture.LoadTexture(name, 0, 1);
    }

    return true;
}
