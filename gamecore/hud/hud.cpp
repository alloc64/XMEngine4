/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../../main.h"

extern Gui gui;
extern xmWeapon weapon;
extern Camera camera;

char *substr(char *original, char *replacement, int length) {
    char out[1024];
    strcpy(out, original);
    if (strlen(out) >= length) {
        out[length] = '\0';
        strcat(out, replacement);
    }
    return out;
}

char *getFileFromPath(char *path) {
    char *file = path;
    while (strstr(file, "\\")) {
        file = strstr(file, "\\");
        if (file) file += 1;
    }

    return file;
}

char *getPathOnly(char *path) {
    char fpath[1024];
    char *pch = strtok(path, "|");
    strcpy(fpath, "");

    while (pch) {
        strcat(fpath, pch);
        pch = strtok(NULL, "|");
    }

    return fpath;
}

char *getFileName(char *filename) {
    return strtok(filename, ".");
}

char *_strtok(char *string, char token) {
    char input[1024];
    char output[1024];
    int i;
    strcpy(input, string);
    for (i = 0; input[i] != token; i++) output[i] = input[i];
    output[i] = '\0';
    return output;
}

ProgressbarTextured pb;

bool HUD::Load(void) {
    Font fbFont;
    if (!inputBoxFont.Load("gui/baseFont.ttf", 12)) return false;
    if (!hudBold.Load("gui/hudb.ttf", 10)) return false;
    //if(!hudFont.Load("gui/hud.ttf", 10)) return false;
    if (!hudWeaponFont.Load("gui/hud.ttf", 14)) return false;
    if (!fbFont.Load("gui/hud.ttf", 13)) return false;

    if (!teamSpeakBG.Load("gui/loginTable.dds", vec2(25, window.h / 2 + 150), vec2(400, 80), color4f(1, 1, 1, 0.8),
                          false))
        return false;
    if (!teamSpeakBox.Load("gui/inputBox.dds", "gui/inputBox.dds", "gui/inputBox.dds", "", color4f(1, 1, 1, 1),
                           color4f(0, 0, 0, 1), vec2(300, 25), vec2(75, window.h / 2 - 205), inputBoxFont, 28, 33,
                           false, false))
        return false;

    if (!interactiveHand.Load("gamecore/hud/iHand.dds", vec2(0, window.h / 2 - 100), vec2(64, 64),
                              color4f(1, 1, 1, 0.8), true))
        return false;

    if (!scoreTableBG.Load("gamecore/hud/scoreTable.dds", vec2(0, 30),
                           vec2(window.w - window.w / 8, window.h - window.h / 8), color4f(1, 1, 1, 0.8), true))
        return false;
    if (!bigScoreFont.Load("gui/baseFont.ttf", 24)) return false;
    if (!infoFont.Load("gui/baseFont.ttf", 14)) return false;

    vignette[0].Load("gamecore/hud/vignette.dds", vec2(window.w, 0), vec2(256, 256), color4f(1, 1, 1, 1), false);
    vignette[1].Load("gamecore/hud/vignette.dds", vec2(window.w, window.h), vec2(256, 256), color4f(1, 1, 1, 1), false);
    vignette[2].Load("gamecore/hud/vignette.dds", vec2(0, 0), vec2(256, 256), color4f(1, 1, 1, 1), false);
    vignette[3].Load("gamecore/hud/vignette.dds", vec2(0, window.h), vec2(256, 256), color4f(1, 1, 1, 1), false);

    //select team a dlsi picoviny
    if (!selectBG.Load("gamecore/hud/bg.dds", color4f(1, 1, 1, 1.0))) return false;
    if (!primaryBuyBG.Load("gamecore/hud/buy_backg.dds", vec2(0, window.h / 2 - 225), vec2(650, 450),
                           color4f(1, 1, 1, 1), true))
        return false;
    if (!secondaryBuyBG.Load("gamecore/hud/buy_sec_backg.dds", vec2(0, window.h / 2 - 225), vec2(650, 450),
                             color4f(1, 1, 1, 1), true))
        return false;
    if (!OKButton.Load("gamecore/hud/buy_OK.dds", "gamecore/hud/buy_OK_onclick.dds", vec2(128, 128),
                       vec2(window.w / 2 + 300, window.h / 2 - 80)))
        return false;

    cartridgePB.Load("gamecore/hud/buy_pb_bg.dds", "gamecore/hud/buy_pb.dds", vec2(window.w / 2, window.h / 2 - 120),
                     fbFont, 50, vec2(150, 20), color4f(0.55, 0.55, 0.55, 1), "Cartridge: ",
                     vec2(window.w / 2 - 100, window.h / 2 - 115));
    damagePB.Load("gamecore/hud/buy_pb_bg.dds", "gamecore/hud/buy_pb.dds", vec2(window.w / 2, window.h / 2 - 150),
                  fbFont, 70, vec2(150, 20), color4f(0.55, 0.55, 0.55, 1), "Damage: ",
                  vec2(window.w / 2 - 100, window.h / 2 - 145));
    cadencyPB.Load("gamecore/hud/buy_pb_bg.dds", "gamecore/hud/buy_pb.dds", vec2(window.w / 2, window.h / 2 - 180),
                   fbFont, 20, vec2(150, 20), color4f(0.55, 0.55, 0.55, 1), "Cadence: ",
                   vec2(window.w / 2 - 100, window.h / 2 - 175));

    primaryWeaponButton = new ButtonFonted2[weapon.iNumWeapons];
    secondaryWeaponButton = new ButtonFonted2[weapon.iNumWeapons];
    primaryWeaponSprite = new GLuint[weapon.iNumWeapons];
    secondaryWeaponSprite = new GLuint[weapon.iNumWeapons];

    int j = 0, k = 0;
    for (int i = 0; i < weapon.iNumWeapons; i++) {
        char *path = weapon.weapon[i].spriteModelPath;
        switch (weapon.weapon[i].type) {
            case SNIPER:
            case ASSAULT_RIFLE:
            case SMG:
            case SHOTGUN: {
                primaryWeaponSprite[j] = texture.LoadTexture(path, 0, 0);
                if (!primaryWeaponButton[j].Load(vec2(window.w / 2 - 275, (window.h / 2 + 60) - j * 20),
                                                 substr(weapon.weapon[i].realName, "...", 15), hudBold,
                                                 color4f(0.8, 0.8, 0.8, 0.8), color4f(0.9, 0.9, 0.9, 0.9), false, i))
                    return false;
                iNumPrimaryWeaponButtons++;
                j++;
            }
                break;

            case PISTOL: {
                secondaryWeaponSprite[k] = texture.LoadTexture(path, 0, 0);
                if (!secondaryWeaponButton[k].Load(vec2(window.w / 2 - 275, (window.h / 2 + 60) - k * 20),
                                                   substr(weapon.weapon[i].realName, "...", 15), hudBold,
                                                   color4f(0.8, 0.8, 0.8, 0.8), color4f(0.9, 0.9, 0.9, 0.9), false, i))
                    return false;

                iNumSecondaryWeaponButtons++;
                k++;
            }
                break;
        }
    }

    return true;
}

void HUD::DrawCountdown(void) {
    //120 / 60 = 2 minuty
    //120 % 60 = 0 sekund
    int roundTime = GetRoundTime();
    glPrintn(1, 1, 1, 1, hudWeaponFont, window.w / 2, 65, "%d:%d", (int) roundTime / 60, (int) roundTime % 60);
}

void HUD::DrawBuyMenu(void) {
    if (!bShowBuyMenu) return;
    float depth[2];
    glGetFloatv(GL_DEPTH_RANGE, depth);
    glDepthRange(0, 0.1);
    selectBG.Draw();
    {

        switch (iNumHits) {
            case 0: //PRIMARY WEAPON
            {
                primaryBuyBG.Draw();
                for (int i = 0; i < iNumPrimaryWeaponButtons; i++) {
                    if (primaryWeaponButton[i].clicked() && !bWeaponClicked) {
                        //saddasdaasdas FAPFAPFAP..
                        iCurrSelectedWeaponID = primaryWeaponButton[i].id;
                        iPrimaryWeaponID = iCurrSelectedWeaponID;
                        iCurrSelectedWeapon = i;
                        bWeaponClicked = true;

                    }

                    if (!primaryWeaponButton[i].clicked() && bWeaponClicked) bWeaponClicked = false;
                }


                if (iCurrSelectedWeaponID != -1) {
                    //vykresleni obdelnicku se sprajtem a dalsi sracky..
                    glPushMatrix();
                    {
                        glAlphaFunc(GL_GREATER, 0.1f);
                        glEnable(GL_ALPHA_TEST);
                        glTranslatef(window.w / 2 - 50, window.h / 2 - 50, 0);
                        glBindTexture(GL_TEXTURE_2D, primaryWeaponSprite[iCurrSelectedWeapon]);

                        vec2 size = vec2(300, 200);
                        float quad[] =
                                {
                                        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                        1.0f, 0.0f, size.x, 0.0f, 0.0f,
                                        1.0f, -1.0f, size.x, size.y, 0.0f,
                                        0.0f, -1.0f, 0.0f, size.y, 0.0f
                                };

                        glInterleavedArrays(GL_T2F_V3F, 0, quad);
                        glDrawArrays(GL_QUADS, 0, 4);

                        glDisable(GL_ALPHA_TEST);
                    }
                    glPopMatrix();

                    cartridgePB.Draw();
                    damagePB.Draw();
                    cadencyPB.Draw();

                    glPrintn(0.7, 0.7, 0.7, 1, hudWeaponFont, window.w / 2 - 100, window.h / 2 - 60, "%s",
                             substr(weapon.weapon[iCurrSelectedWeaponID].realName, "...", 38));
                }
            }
                break;

            case 1: //SECONDARY WEAPON
            {
                secondaryBuyBG.Draw();
                for (int i = 0; i < iNumSecondaryWeaponButtons; i++) {
                    if (secondaryWeaponButton[i].clicked() && !bWeaponClicked) {
                        iCurrSelectedWeaponID = secondaryWeaponButton[i].id;
                        iSecondaryWeaponID = iCurrSelectedWeaponID;
                        iCurrSelectedWeapon = i;
                        bWeaponClicked = true;
                    }

                    if (!secondaryWeaponButton[i].clicked() && bWeaponClicked) bWeaponClicked = false;
                }

                if (iCurrSelectedWeaponID != -1) {
                    glPushMatrix();
                    {
                        glAlphaFunc(GL_GREATER, 0.1f);
                        glEnable(GL_ALPHA_TEST);
                        glTranslatef(window.w / 2 - 50, window.h / 2 - 50, 0);
                        glBindTexture(GL_TEXTURE_2D, secondaryWeaponSprite[iCurrSelectedWeapon]);

                        vec2 size = vec2(300, 200);
                        float quad[] =
                                {
                                        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                        1.0f, 0.0f, size.x, 0.0f, 0.0f,
                                        1.0f, -1.0f, size.x, size.y, 0.0f,
                                        0.0f, -1.0f, 0.0f, size.y, 0.0f
                                };

                        glInterleavedArrays(GL_T2F_V3F, 0, quad);
                        glDrawArrays(GL_QUADS, 0, 4);

                        glDisable(GL_ALPHA_TEST);
                    }
                    glPopMatrix();

                    cartridgePB.Draw();
                    damagePB.Draw();
                    cadencyPB.Draw();

                    glPrintn(0.7, 0.7, 0.7, 1, hudWeaponFont, window.w / 2 - 100, window.h / 2 - 60, "%s",
                             substr(weapon.weapon[iCurrSelectedWeaponID].realName, "...", 38));
                }
            }
                break;

            default:
                printf("error: invalid case ::HUD!\n");
                iNumHits = 0;
                break;
        }

        if (OKButton.clicked() && !bOKclicked) {
            bOKclicked = true;
            if (iNumHits == 0) {
                if (iCurrSelectedWeaponID != -1) iNumHits++;
                iCurrSelectedWeaponID = -1;
            } else {
                if (iNumHits >= 1 && iCurrSelectedWeaponID != -1) bShowBuyMenu = false;
                bWeaponClicked = false;
                iCurrSelectedWeaponID = -1;
            }
        }

        if (!OKButton.clicked() && bOKclicked) bOKclicked = false;
    }
    glDepthRange(depth[0], depth[1]);
}

void HUD::DrawWeaponHud(void) {
    int i = weapon.wid;
    glPrintn(1, 1, 1, 1, hudWeaponFont, window.w / 2 + 220, 65, "%d  |  %d", weapon.weapon[i].iCurrAmmoCount,
             weapon.weapon[i].supply);
    glPrintn(1, 1, 1, 1, hudWeaponFont, window.w / 2 + 180, 45, "%s", weapon.weapon[i].realName);
}

void HUD::Draw(void) {
    //for(int i=0; i < 4; i++) vignette[i].Draw();
    DrawMessages();
    if (camera.GetType() != THIRD_PERSON) DrawWeaponHud();
    ShowInteractiveHand();
    DrawCountdown();

    if (GetAsyncKeyState('T') && !bTClicked && !bWrite2Chat) {
        bWrite2Chat = true;
        bTClicked = true;
        bSendChatMessage = false;
        teamSpeakBox.SetText("");
    }

    if (!GetAsyncKeyState('T') && bTClicked) bTClicked = false;

    if (bWrite2Chat) {
        if (GetAsyncKeyState(VK_RETURN)) {
            chatMessage = teamSpeakBox.GetText();
            bWrite2Chat = false;
            bSendChatMessage = true;
            return;
        }
        float depth[2];
        glGetFloatv(GL_DEPTH_RANGE, depth);
        glDepthRange(0, 0.1);
        teamSpeakBG.Draw();
        teamSpeakBox.Draw();
        teamSpeakBox.SetActive();
        glDepthRange(depth[0], depth[1]);
    }

    DrawBuyMenu();

    if (GetAsyncKeyState(VK_TAB)) {
        float depth[2];
        glGetFloatv(GL_DEPTH_RANGE, depth);
        scoreTableBG.Draw();
        //vrsek
        glPrintn(1, 1, 1, 1, infoFont, window.w / 2 - 320, window.h - window.h / 10, "#0 Deathmatch::dm_ufomap");
        char score[128];
        sprintf(score, "%d:%d", 10, 10);
        float pos = strlen(score) * 7.25f;
        glPrintn(1, 1, 1, 1, bigScoreFont, window.w / 2 - pos, window.h - window.h / 10, score);
        glPrintn(1, 1, 1, 1, infoFont, window.w / 2 + 180, window.h - window.h / 10, "Time Left: %d:%d", 19, 20);
        for (int i = 0; i < gui.GetMaxPlayersOnServer(); i++) {
            if (netw.playerPositions[i].slotUsed) {
                char *nick = netw.playerPositions[i].name;
                int deaths = netw.playerPositions[i].deaths;
                int kills = netw.playerPositions[i].kills;
                int ping = netw.playerPositions[i].ping;

                if (netw.playerPositions[i].team == BLUE_TEAM) {
                    int y = (window.h / 2 + 175) + i * 10;
                    glPrintn(1, 1, 1, 1, inputBoxFont, window.w / 2 - 300, y, nick);
                    glPrintn(1, 1, 1, 1, inputBoxFont, window.w / 2 - 100, y, "%d", deaths);
                    glPrintn(1, 1, 1, 1, inputBoxFont, window.w / 2 - 70, y, "%d", kills);
                    glPrintn(1, 1, 1, 1, inputBoxFont, window.w / 2 - 40, y, "%d", ping);
                }

                if (netw.playerPositions[i].team == RED_TEAM) {
                    //glPrintn(1,1,1,1, inputBoxFont, window.w/2-pos, window.h-window.h/10, score);
                }
            }
        }

        //spodek s hracema


        glDepthRange(depth[0], depth[1]);
    }
}
