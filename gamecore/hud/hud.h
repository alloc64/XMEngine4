/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _HUD_H
#define _HUD_H

#include "../../main.h"

extern Gui gui;
extern xmLevel level;

struct message {
    char *msg;
    color4f color;
    bool bold;
};


class HUD {
public:
    HUD() {
        bWrite2Chat = false;
        bTClicked = false;
        bSendChatMessage = false;
        otherChatMessages = " ";
        iNumHits = 0;
        iCurrSelectedWeaponID = -1;
        iCurrSelectedWeapon = 0;
        bShowBuyMenu = true;
    }

    bool Load(void);

    void Draw(void);

    bool bSendChatMessage;
    int iCurrentTime;

    bool IsBuyMenuShown(void) {
        return bShowBuyMenu;
    }

    bool IsChatActive() {
        return bWrite2Chat;
    }

    bool WannaSendMessage() {
        return bSendChatMessage;
    }

    char *GetChatMessage() {
        return chatMessage;
    }

    void SetRoundTime(int time) {
        iCurrentTime = time;
    }

    int GetRoundTime(void) {
        return iCurrentTime;
    }

    int GetPrimaryWeaponID(void) {
        return iPrimaryWeaponID;
    }

    int GetSecondaryWeaponID(void) {
        return iSecondaryWeaponID;
    }

    void DrawCountdown(void);

    void ShowInteractiveHand(void) {
        if (level.bShowIHand) {
            level.bShowIHand = false;
            float depth[2];
            glGetFloatv(GL_DEPTH_RANGE, depth);
            glDepthRange(0, 0.1);
            interactiveHand.Draw();
            glDepthRange(depth[0], depth[1]);
        }
    }

    void DrawWeaponHud();

    void DrawBuyMenu(void);

    void SetMessages(char *msg) {
        if (msg) {
            //if(!strcmp(otherChatMessages, msg)) return;
            otherChatMessages = msg;
            int i = 0;
            char *ptr = strtok(msg, "\n");
            while (ptr) {
                messages[i].msg = ptr;
                if ((strstr(ptr, "joined the game!") || strstr(ptr, "left the game!")) && !strstr(ptr, ":")) {
                    messages[i].color = color4f(0, 0.65, 1, 1);
                    messages[i].bold = true;
                } else if (gui.iUserAccType == 1) {  //VIP not ADMIN
                    messages[i].color = color4f(1, 0.5, 0, 1);
                    messages[i].bold = true;
                } else if (gui.iUserAccType == 2) { // ADMIN
                    messages[i].color = color4f(0, 0.8, 0, 1);
                    messages[i].bold = true;
                } else {
                    messages[i].color = color4f(1, 1, 1, 1);
                    messages[i].bold = true;
                }
                ptr = strtok(NULL, "\n");
                i++;
            }
        }
    }

    void DrawMessages(void) {
        if (otherChatMessages) {
            for (int i = 0; i < 4; i++) {
                if (messages[i].bold) {
                    glPrintn(messages[i].color.r,
                             messages[i].color.g,
                             messages[i].color.b,
                             messages[i].color.a, hudBold, 45, (window.h / 2 - 100) - i * 20, "%s", messages[i].msg);
                } else {
                    glPrintn(messages[i].color.r,
                             messages[i].color.g,
                             messages[i].color.b,
                             messages[i].color.a, hudFont, 45, (window.h / 2 - 100) - i * 20, "%s", messages[i].msg);
                }
            }
        }
    }

private:
    Font inputBoxFont;
    InputBox teamSpeakBox;
    Image2 teamSpeakBG;
    Image2 scoreTableBG;
    Image2 interactiveHand;
    Font bigScoreFont;
    Font infoFont;
    Font hudBold;
    Font hudFont;
    bool bWrite2Chat;
    bool bTClicked;
    char *chatMessage;
    char *otherChatMessages;
    message messages[15];
    Font hudWeaponFont;

    BackgroundImage selectBG;
    Image primaryBuyBG;
    Image secondaryBuyBG;
    ButtonTextured3 OKButton;

    ButtonFonted2 *primaryWeaponButton;
    int iNumPrimaryWeaponButtons;
    GLuint *primaryWeaponSprite;

    ButtonFonted2 *secondaryWeaponButton;
    int iNumSecondaryWeaponButtons;
    GLuint *secondaryWeaponSprite;

    int iNumHits;
    bool bOKclicked;
    bool bWeaponClicked;
    int iCurrSelectedWeaponID;
    int iCurrSelectedWeapon;

    int iPrimaryWeaponID;
    int iSecondaryWeaponID;

    ProgressbarTextured cartridgePB;
    ProgressbarTextured damagePB;
    ProgressbarTextured cadencyPB;

    bool bShowBuyMenu;

    Image vignette[4];

};

#endif
