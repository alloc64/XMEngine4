/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../../main.h"

#define glTransformf(x, y, z, angle) glPushMatrix();\
                                     glTranslatef(x, y, z);\
                                     glRotatef(angle, 0, 1, 0);\
                                     glMatrixMode(GL_TEXTURE);\
                                     glActiveTextureARB(GL_TEXTURE7_ARB);\
                                     glPushMatrix();\
                                     glTranslatef(x, y, z);\
                                     glRotatef(angle, 0, 1, 0);\

#define glEndTransformf() glPopMatrix();\
                          glMatrixMode(GL_MODELVIEW);\
                          glPopMatrix();
//#define _drawbbs

extern xmAnimation character[4];
extern Network netw;
extern Gui gui;
extern Frustum frustum;
extern Bullet bullet;
extern xmWeapon weapon;
extern Camera camera;
extern HUD hud;

bool Characters::Load(void) {
    if (!character) return false;
    char *defaultPath = "gamecore/characters/w_character0%d.xma";
    for (int i = 0; i < 4; i++) {
        char path[512];
        sprintf(path, defaultPath, i);
        if (!character[i].Load(path, true)) {
            printf("error: cannot load character %s - number %d! ::Characters!\n", path, i);
            return false;
        }
    }

    bSetSpawn = false;
    return true;
}

void Characters::Interpolate(int pid, vec3 &pos, vec3 &angle) {
    float time = netw.playerPositions[pid].packetPing;// * 0.5;

    float d = 10.0;
    if (time >= 10) d = 100;
    if (time >= 100) d = 1000;
    if (time >= 1000) d = 10000;

    t += timer.GetDT();
    if (t >= 0.05) {
        currTime /= cnt;
        cnt = 0;
        t = 0;
    } else {
        currTime += time;
        cnt++;
    }

    time = currTime;
    float dt = 1.0;
    if (time >= 1.0f) time /= d;

    if (time > 0.0 && time <= 1.0) {
        dt = time;
        oldDt = dt;
    } else {
        dt = oldDt;
    }
    if (dt == 0.0) dt = 1.0;

    interpolatedCharEntity[pid].setNewTarget(netw.playerPositions[pid].lastPos, netw.playerPositions[pid].angle, dt);
    interpolatedCharEntity[pid].update(timer.GetDT());

    pos = interpolatedCharEntity[pid].getPosition();
    angle = interpolatedCharEntity[pid].getAngle();
}

void Characters::Draw(bool animate) {
    int charType = 0;
    vec3 inPos, angle;

    if (gui.IsPlayerDead()) {
        if (netw.iNumUsedPlayerSlots > 1) {
            camera.SetCameraFreezed();
            if (GetAsyncKeyState(VK_LBUTTON) && !bClicked[0] && !hud.IsBuyMenuShown()) {
                iCurrentWatchedPlayer--;
                bClicked[0] = true;
            }
            if (GetAsyncKeyState(VK_RBUTTON) && !bClicked[1] && !hud.IsBuyMenuShown()) {
                iCurrentWatchedPlayer++;
                bClicked[1] = true;
            }
            if (GetAsyncKeyState(VK_SPACE) && !bClicked[2] && !hud.IsBuyMenuShown()) {
                camType = !camType;
                bClicked[2] = true;
            }

            if (iCurrentWatchedPlayer == gui.GetPlayerID()) {
                if (iCurrentWatchedPlayer + 1 >= netw.iNumUsedPlayerSlots) iCurrentWatchedPlayer = 0;
                if (iCurrentWatchedPlayer - 1 < 0) iCurrentWatchedPlayer = netw.iNumUsedPlayerSlots;
            } else {
                if (iCurrentWatchedPlayer >= netw.iNumUsedPlayerSlots) {
                    iCurrentWatchedPlayer = 0;
                }

                if (iCurrentWatchedPlayer < 0) {
                    iCurrentWatchedPlayer = netw.iNumUsedPlayerSlots;
                }
            }

            int k = netw.usedPlayerSlotID[iCurrentWatchedPlayer];
            iCurrentWatchedPlayerRealID = k;

            netw.playerPositions[k].isWatched = true;
            for (int i = 0; i < gui.GetMaxPlayersOnServer(); i++) {
                vec3 pos, ang;
                if (i == k || !netw.playerPositions[i].slotUsed) continue;
                Interpolate(i, pos, ang);
                if (!frustum.SphereInFrustum(-netw.playerPositions[i].pos, charHeight)) continue;
                DrawChar(i, charType, pos, ang, animate);
                DrawWeapon(netw.playerPositions[i].wid, charType, pos, angle);
            }

            Interpolate(k, inPos, angle);

            switch (camType) {
                case FIRST_PERSON: {
                    camera.SetType(FIRST_PERSON);
                    camera.SetPosition(inPos);
                    camera.SetAngle(angle);
                }
                    break;

                case THIRD_PERSON: {
                    camera.SetType(THIRD_PERSON);
                    camera.SetPosition(inPos);

                    DrawChar(k, charType, inPos, angle, animate);
                    DrawWeapon(netw.playerPositions[k].wid, charType, inPos, angle);
                }
                    break;
            }

            if (!GetAsyncKeyState(VK_LBUTTON) && bClicked[0] && !hud.IsBuyMenuShown()) bClicked[0] = false;
            if (!GetAsyncKeyState(VK_RBUTTON) && bClicked[1] && !hud.IsBuyMenuShown()) bClicked[1] = false;
            if (!GetAsyncKeyState(VK_SPACE) && bClicked[2] && !hud.IsBuyMenuShown()) bClicked[2] = false;
        } else {
            //bylo by vhodny respawnout rovnou usera na miste nebo nastavit nejakou spesl posmrtnou kamerku
            if (hud.IsBuyMenuShown()) {
                camera.SetCameraFreezed();
                camera.SetType(FREE_LOOK);
                if (!bSetSpawn) {
                    camera.SetPosition(level.GetRandomPlayerSpawn());
                    bSetSpawn = true;
                }
            }
        }

    } else {
        for (int i = 0; i < gui.GetMaxPlayersOnServer(); i++) {
            if (i == gui.GetPlayerID()) continue;
            Interpolate(i, inPos, angle);

            if (netw.playerPositions[i].slotUsed && !gui.IsPlayerDead()) {
                if (frustum.SphereInFrustum(-inPos, 60)) {
                    if (!netw.playerPositions[i].dead) {
                        bDeathAnimated = false;
                        //bbOrientation.Rotate(0, angle.y, 0);
                        bbOrientation.Translate(vec3(inPos.x, inPos.y - charHeight,
                                                     inPos.z)); //Tady musi bejt interpolovana pozice hrace ;D
                        for (int j = 0; j < character[charType].iNumIndexedBBs; j++) {
                            vec3 bboxpos = bbOrientation * -character[charType].bbSphere[j].center;

#ifdef _drawbbs
                            glPushMatrix();
                                glTranslatef(-bboxpos.x, -bboxpos.y, -bboxpos.z);
                                glutWireSphere(character[charType].bbSphere[j].radius, 12, 12);
                            glPopMatrix();
#endif

                            int nHits = bullet.CheckBulletsForCollision(bboxpos,
                                                                        character[charType].bbSphere[j].radius);
                            if (nHits > 0) {
                                int type = character[charType].bbSphere[j].type;
                                int dmg = 0;
                                switch (type) {
                                    case 0: {
                                        //if(delka > nezUrcitaVzdalenost)
                                        //{

                                        //}else{
                                        dmg = weapon.weapon[weapon.wid].damage.head;
                                        printf("head damage %d\n", dmg);
                                        //}
                                    }
                                        break;

                                    case 1: {
                                        //if(delka > nezUrcitaVzdalenost)
                                        //{

                                        //}else{
                                        dmg = weapon.weapon[weapon.wid].damage.body;
                                        printf("body damage %d\n", dmg);
                                        //}
                                    }
                                        break;

                                    case 2: {
                                        //if(delka > nezUrcitaVzdalenost)
                                        //{

                                        //}else{
                                        dmg = weapon.weapon[weapon.wid].damage.feet;
                                        printf("foot damage %d\n", dmg);
                                        //}
                                    }
                                        break;

                                    default:
                                        printf("bad sphere type %d\n", type);
                                }
                                iCausedDamage = dmg;
                                iHitPlayerID = i;
                                bHit = true;
                            }
                        }
                    } else {
                        if (!bDeathAnimated) {
                            bAnimateDeath = true;
                            bDeathAnimated = true;
                        }
                    }

                    // render
                    DrawChar(i, charType, inPos, angle, animate);
                    DrawWeapon(netw.playerPositions[i].wid, charType, inPos, angle);
                }
            } else {

            }
        }
    }
}


void Characters::DrawChar(int i, int charType, vec3 pos, vec3 angle, bool animate) {
    switch (0)//weapon.GetCurrentWeapon())
    {
        case MAIN_WEAPON: //main equip
        {
            glPushMatrix();
            {
                if (animate) {
                    //FIXME: animace + interpolace potrebuji dodelat
                    //if(!bAnimateDeath)
                    //{
                    if (netw.playerPositions[i].isMoving[OLD_STATE]) {
                        if (netw.playerPositions[i].isJumping[OLD_STATE]) {
                            character[charType].setAnimationByID(2, true);
                        } else {
                            if (netw.playerPositions[i].isWalking[OLD_STATE]) {
                                if (netw.playerPositions[i].isCrouched[OLD_STATE]) {
                                    character[charType].setAnimationByID(3, true);
                                } else {
                                    character[charType].setAnimationByID(0, true);
                                }
                            } else {
                                character[charType].setAnimationByID(1, true);
                            }
                        }
                    } else {
                        character[charType].setAnimationByID(4, true);
                    }
                    //}else{
                    //    bAnimateDeath = character[charType].setAnimationByID(3, bAnimateDeath);
                    //}
                }

                glTransformf(-pos.x, -pos.y + charHeight, -pos.z, -angle.y);
                //angle.x = DegreesToRadians(angle.x-180);
                //angle.y = DegreesToRadians(angle.y);
                //if(angle.x > M_PI/2 || angle.x < -M_PI/2) angle.x = M_PI/2;
                //character[charType].RotateSpine(vec3(0, 0, angle.x));
                //character[charType].RotatePelvis(vec3(angle.y, 0, 0));
                glEnable(GL_CULL_FACE);
                glCullFace(animate ? GL_BACK : GL_FRONT);
                character[charType].draw();
                glEndTransformf();
            }
            glPopMatrix();
        }
            break;

        case SECONDARY_WEAPON: {

        }
            break;

        case OTHER_WEAPON: {

        }
            break;
    }
}

void Characters::DrawWeapon(int wid, int charType, vec3 pos, vec3 angle) {
    if (wid >= 0 && wid <= weapon.iNumWeapons && weapon.IsEntityLoaded(wid)) {
        vec3 cartridge = weapon.GetWeaponCartrigePosition(wid);
        vec3 sprite = weapon.GetWeaponSpritePosition(wid);
        vec3 handle = weapon.GetWeaponHandlePosition(wid);

        switch (weapon.weapon[wid].type) {
            case ASSAULT_RIFLE:
            case SNIPER:
            case SHOTGUN:
            case SMG: {
                mat4 handPosition = GetWeaponPosition(charType);
                glPushMatrix();
                {
                    glTranslatef(-pos.x, -pos.y + charHeight, -pos.z);
                    glRotatef(-angle.y, 0, 1, 0);
                    glMultMatrixf((float *) &handPosition.mat);
                    glRotatef(90, 0, 1, 0);
                    glRotatef(90, 0, 0, 1);
                    glTranslatef(-handle.x, -handle.y, -handle.z);

                    glMatrixMode(GL_TEXTURE);
                    glActiveTextureARB(GL_TEXTURE7);
                    glPushMatrix();

                    glTranslatef(-pos.x, -pos.y + charHeight, -pos.z);
                    glRotatef(-angle.y, 0, 1, 0);
                    glMultMatrixf((float *) &handPosition.mat);
                    glRotatef(90, 0, 1, 0);
                    glRotatef(90, 0, 0, 1);
                    glTranslatef(-handle.x, -handle.y, -handle.z);

                    glCullFace(GL_FRONT);
                    weapon.DrawWeapon(wid);

                    glPopMatrix();
                    glMatrixMode(GL_MODELVIEW);
                }
                glPopMatrix();
            }
                break;

            case KNIFE:
            case PISTOL:
            case GRENADE: {
                mat4 spinePosition = GetSpinePosition(charType);
                glPushMatrix();
                {
                    glTranslatef(-pos.x, -pos.y + charHeight, -pos.z);
                    glRotatef(-angle.y, 0, 1, 0);
                    glMultMatrixf((float *) &spinePosition.mat);
                    glRotatef(0, 1, 0, 0);
                    glRotatef(180, 0, 0, 1);
                    glTranslatef(-handle.x, -handle.y, -handle.z);

                    glMatrixMode(GL_TEXTURE);
                    glActiveTextureARB(GL_TEXTURE7);
                    glPushMatrix();

                    glTranslatef(-pos.x, -pos.y + charHeight, -pos.z);
                    glRotatef(-angle.y, 0, 1, 0);
                    glMultMatrixf((float *) &spinePosition.mat);
                    glRotatef(90, 0, 1, 0);
                    glRotatef(90, 0, 0, 1);
                    glTranslatef(-handle.x, -handle.y, -handle.z);

                    glCullFace(GL_FRONT);
                    weapon.DrawWeapon(wid);

                    glPopMatrix();
                    glMatrixMode(GL_MODELVIEW);
                }
                glPopMatrix();
            }
                break;

            default: {
                printf("error: weapon wid %d is not a valid MAIN_WEAPON or SECONDARY_WEAPON or OTHER_WEAPON!\n", wid);
            }
                break;
        }
    } else {
        printf("error: undefined wid %d\n", wid);
    }
}
