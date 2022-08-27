/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../main.h"

extern Texture texture;
extern Window window;
extern Font basicFont;
extern xmWeapon weapon;
extern HUD hud;

Networking net;
extern xmMenu menu;

Gui::Gui() {
    showMessage = false;
    bShowLoginMenu = true;
    bLoginUser = false;
    bLogoutUser = false;
    bDownloadedAvatar = false;
    bShowExitMSGBox = false;
    bLoadLevel = false;
    bLevelLoaded = false;
    bShowGUI = true;
    bChangedRes = false;
    bShowMenuWithServers = false;
    bConnect = false;
    bShowMyAcc = false;
    bRegisterMe = false;
    bAgreeBoxChecked = false;

    bUseShadows = true;
    bUpdateNick = false;
    globalButtonColor = color4f(0.9, 0.9, 0.9, 1.0);
}

Gui::~Gui() {

}

bool Gui::CreateServerItem(int i) {
    char tmp[255];
    if (!serverItemButton[i].Load(vec2(window.w / 2 - 263, window.h / 2 + 160 - (i * 25)), serverItem[i].ip,
                                  serverItemFont, color4f(0.8, 0.8, 0.8, 0.8), color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!serverMode[i].Load(serverItem[i].mode, color4f(1, 1, 1, 1),
                            vec2(window.w / 2 - 100, window.h / 2 + 185 - (i * 25)), serverItemFont))
        return false;
    sprintf(tmp, "%d", serverItem[i].numPlayers);
    if (!serverNumPlayers[i].Load(tmp, color4f(1, 1, 1, 1), vec2(window.w / 2 + 100, window.h / 2 + 185 - (i * 25)),
                                  serverItemFont))
        return false;
    sprintf(tmp, "%d", serverItem[i].ping);
    if (!serverPing[i].Load(tmp, color4f(1, 1, 1, 1), vec2(window.w / 2 + 205, window.h / 2 + 185 - (i * 25)),
                            serverItemFont))
        return false;
    if (!serverItemDeleteButton[i].Load("gui/mmExitButton.dds", "gui/mmExitButton_onclick.dds", vec2(16, 16),
                                        vec2(window.w / 2 + 273, window.h / 2 + 178 - (i * 25.5))))
        return false;
    return true;
}

bool Gui::RefreshServerItems() {
    char tmp[255];
    for (int i = 0; i < iNumServers; i++) {
        serverItem[i].Refresh();
        if (!serverMode[i].Load(serverItem[i].mode, color4f(1, 1, 1, 1),
                                vec2(window.w / 2 - 100, window.h / 2 + 185 - (i * 25)), serverItemFont))
            return false;
        sprintf(tmp, "%d", serverItem[i].numPlayers);
        if (!serverNumPlayers[i].Load(tmp, color4f(1, 1, 1, 1), vec2(window.w / 2 + 100, window.h / 2 + 185 - (i * 25)),
                                      serverItemFont))
            return false;
        sprintf(tmp, "%d", serverItem[i].ping);
        if (!serverPing[i].Load(tmp, color4f(1, 1, 1, 1), vec2(window.w / 2 + 205, window.h / 2 + 185 - (i * 25)),
                                serverItemFont))
            return false;
    }
    bRefreshServerList = false;
}

bool Gui::DeleteServerFromList(int id) {
    char *header = new char[strlen(deleteServerByIDHeader) * 2];
    sprintf(header, deleteServerByIDHeader, base64_encode(serverItem[id].ip, strlen(serverItem[id].ip)),
            gui.GetSESSIONID());
    printf("%s\n", header);
    char *ret = net.Send(header);
    printf("%s\n", ret);

    char tmp[255];
    int j = 0;
    for (int i = 0; i < iNumServers; i++) {
        if (i == id) continue;
        CreateServerItem(i);
    }

    iNumServers--;
    return true;
}

bool Gui::Load(void) {
    Font serverMenuFont;
    if (!basicFont.Load("gui/baseFont.ttf", 20)) return false;

    // login menu
    if (!inputFormsBG.Load("gui/loginTable.dds", vec2(0, window.h / 2 - 190), vec2(440, 220), color4f(1, 1, 1, 0.8),
                           true))
        return false;
    if (!inputBoxFont.Load("gui/baseFont.ttf", 12)) return false;
    if (!mailBox.Load("gui/inputBox.dds", "gui/inputBox_onclick.dds", "gui/inputBox_onclick.dds", "Email: ",
                      color4f(1, 1, 1, 1), color4f(0, 0, 0, 1), vec2(200, 25), vec2(0, window.h / 2 - 20), inputBoxFont,
                      28, 25, true, false));
    if (!passBox.Load("gui/inputBox.dds", "gui/inputBox_onclick.dds", "gui/inputBox_onclick.dds", "Password: ",
                      color4f(1, 1, 1, 1), color4f(0, 0, 0, 1), vec2(200, 25), vec2(0, window.h / 2 - 60), inputBoxFont,
                      28, 25, true, true));
    if (!exitButton.Load("gui/mmExitButton.dds", "gui/mmExitButton_onclick.dds", vec2(24, 24),
                         vec2(window.w - 21, window.h - 21)))
        return false;
    if (!autologinCheckBox.Load("gui/checkBox.dds", "gui/checkBox_onhover.dds", "gui/checkBox_checked.dds",
                                "Remember me? ", vec2(16, 16), vec2(window.w / 2 - 80, window.h / 2 - 100),
                                inputBoxFont))
        return false;
    if (!sendButton.Load("gui/sendButton.dds", "gui/sendButton_onclick.dds", vec2(100, 25),
                         vec2(window.w / 2, window.h / 2 - 125)))
        return false;
    if (!registerButton.Load("gui/registerButton.dds", "gui/registerButton_onclick.dds", vec2(100, 25),
                             vec2(window.w / 2 + 100, window.h / 2 - 125)))
        return false;

    // register me menu
    if (!registerMenuBG.Load("gui/loginTable.dds", vec2(0, window.h / 2 - 190), vec2(505, 288), color4f(0, 0, 0, 0.9f),
                             true))
        return false;
    if (!closeRegisterMenuButton.Load("gui/mmExitButton.dds", "gui/mmExitButton_onclick.dds", vec2(24, 24),
                                      vec2(window.w / 2 + 255 - 21, window.h / 2 + 95 - 21)))
        return false;
    if (!inputFormsBG.Load("gui/loginTable.dds", vec2(0, window.h / 2 - 190), vec2(440, 220), color4f(1, 1, 1, 0.8),
                           true))
        return false;
    if (!registerMeText.Load("Register me", color4f(1, 1, 1, 1), vec2(window.w / 2 - 220, window.h / 2 + 75),
                             inputBoxFont))
        return false;
    if (!registerMeLine.Load(vec2(window.w / 2 - 220, window.h / 2 + 40), vec2(0, 420), 1.0,
                             color4f(0.8, 0.8, 0.8, 1.0)));
    if (!inputNick.Load("gui/inputBox.dds", "gui/inputBox_onclick.dds", "gui/inputBox_onclick.dds", "Nick: ",
                        color4f(1, 1, 1, 1), color4f(0, 0, 0, 1), vec2(350, 25),
                        vec2(window.w / 2 - 175 + 30, window.h / 2 + 20), inputBoxFont, 28, 25, false, false));
    if (!inputName.Load("gui/inputBox.dds", "gui/inputBox_onclick.dds", "gui/inputBox_onclick.dds", "Name: ",
                        color4f(1, 1, 1, 1), color4f(0, 0, 0, 1), vec2(350, 25),
                        vec2(window.w / 2 - 175 + 30, window.h / 2 - 20), inputBoxFont, 28, 25, false, false));
    if (!inputPass.Load("gui/inputBox.dds", "gui/inputBox_onclick.dds", "gui/inputBox_onclick.dds", "Pass: ",
                        color4f(1, 1, 1, 1), color4f(0, 0, 0, 1), vec2(350, 25),
                        vec2(window.w / 2 - 175 + 30, window.h / 2 - 60), inputBoxFont, 28, 25, false, true));
    if (!inputMail.Load("gui/inputBox.dds", "gui/inputBox_onclick.dds", "gui/inputBox_onclick.dds", "Email: ",
                        color4f(1, 1, 1, 1), color4f(0, 0, 0, 1), vec2(350, 25),
                        vec2(window.w / 2 - 175 + 30, window.h / 2 - 100), inputBoxFont, 28, 25, false, false));
    if (!agreeCheckBox.Load("gui/checkBox.dds", "gui/checkBox_onhover.dds", "gui/checkBox_checked.dds",
                            "I agree with all conditions.", vec2(16, 16), vec2(window.w / 2 - 128, window.h / 2 - 145),
                            inputBoxFont))
        return false;
    if (!sendRegistrationButton.Load("gui/sendRegistationButton.dds", "gui/sendRegistationButton_onclick.dds",
                                     vec2(160, 25), vec2(window.w / 2 + 205, window.h / 2 - 140)))
        return false;


    // main menu
    if (!findServersButton.Load(vec2(20, window.h / 2), "Find Servers", basicFont, color4f(0.8, 0.8, 0.8, 0.8),
                                color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!myAccountButton.Load(vec2(20, window.h / 2 - 40), "My account", basicFont, color4f(0.8, 0.8, 0.8, 0.8),
                              color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!optionsButton.Load(vec2(20, window.h / 2 - 80), "Options", basicFont, color4f(0.8, 0.8, 0.8, 0.8),
                            color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!mainMenuExitButton.Load(vec2(20, window.h / 2 - 120), "Exit", basicFont, color4f(0.8, 0.8, 0.8, 0.8),
                                 color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!mainMenuBG.Load("gui/menuBackground.dds", vec2(0, 0), vec2(230, window.h), color4f(1, 1, 1, 0.8f),
                         false))
        return false;


    //options menu
    if (!optionsBG.Load("gui/menuTable.dds", vec2(0, window.h / 2 - 250), vec2(600, 500), color4f(0, 0, 0, 0.9f),
                        true))
        return false;
    char res[128];
    sprintf(res, "%dx%d\n", window.w, window.h);
    if (!resolutionButton.Load(vec2(window.w / 2 + 150, window.h / 2 + 200),
                               vec2(window.w / 2 - 270, window.h / 2 + 202), res, "Resolution: ", basicFont,
                               color4f(0.5, 0.5, 0.5, 1), color4f(1, 1, 1, 1), color4f(1, 1, 1, 1)))
        return false;
    if (!fullscreenButton.Load(vec2(window.w / 2 + 200, window.h / 2 + 160),
                               vec2(window.w / 2 - 270, window.h / 2 + 162), window.bFullscreen ? "true" : "false",
                               "Fullscreen: ", basicFont, color4f(0.5, 0.5, 0.5, 1), color4f(1, 1, 1, 1),
                               color4f(1, 1, 1, 1)))
        return false;
    if (!shadowsButton.Load(vec2(window.w / 2 + 200, window.h / 2 + 120), vec2(window.w / 2 - 270, window.h / 2 + 122),
                            "none", "Shadow detail: ", basicFont, color4f(0.5, 0.5, 0.5, 1), color4f(1, 1, 1, 1),
                            color4f(1, 1, 1, 1)))
        return false;
    if (!bumpmappingButton.Load(vec2(window.w / 2 + 200, window.h / 2 + 80),
                                vec2(window.w / 2 - 270, window.h / 2 + 82), bUseBumpMapping ? "true" : "false",
                                "Use bump mapping: ", basicFont, color4f(0.5, 0.5, 0.5, 1), color4f(1, 1, 1, 1),
                                color4f(1, 1, 1, 1)))
        return false;
    if (!vsyncButton.Load(vec2(window.w / 2 + 200, window.h / 2 + 40), vec2(window.w / 2 - 270, window.h / 2 + 42),
                          bUseVsync ? "true" : "false", "Use vertical sync: ", basicFont, color4f(0.5, 0.5, 0.5, 1),
                          color4f(1, 1, 1, 1), color4f(1, 1, 1, 1)))
        return false;
    if (!dofButton.Load(vec2(window.w / 2 + 200, window.h / 2), vec2(window.w / 2 - 270, window.h / 2 + 2),
                        bUseDOF ? "true" : "false", "Use depth of field efect: ", basicFont, color4f(0.5, 0.5, 0.5, 1),
                        color4f(1, 1, 1, 1), color4f(1, 1, 1, 1)))
        return false;
    if (!bloomButton.Load(vec2(window.w / 2 + 200, window.h / 2 - 40), vec2(window.w / 2 - 270, window.h / 2 - 42),
                          bUseAO ? "true" : "false", "Use bloom efect: ", basicFont, color4f(0.5, 0.5, 0.5, 1),
                          color4f(1, 1, 1, 1), color4f(1, 1, 1, 1)))
        return false;
    if (!saveSettingsBox.Create("gui/inputFormBG.dds", "Save this settings?\nThis causes a game restart!", "Yes", "No",
                                basicFont, vec2(380, 160), color4f(1, 1, 1, 0.85f), color4f(0.5, 0.5, 0.5, 1),
                                color4f(1, 1, 1, 1)))
        return false;

    //server menu
    if (!serversBG.Load("gui/menuTable.dds", vec2(0, window.h / 2 - 250), vec2(600, 500), color4f(1, 1, 1, 1.0f),
                        true))
        return false;
    if (!closeServersButton.Load("gui/mmExitButton.dds", "gui/mmExitButton_onclick.dds", vec2(24, 24),
                                 vec2(window.w / 2 + 300 - 21, window.h / 2 + 250 - 21)))
        return false;
    if (!topLine.Load(vec2(window.w / 2 - 260, window.h / 2 + 190), vec2(0, 490), 1.0, color4f(0.8, 0.8, 0.8, 1.0)));
    if (!bottomLine.Load(vec2(window.w / 2 - 260, window.h / 2 - 190), vec2(0, 510), 1.0, color4f(0.8, 0.8, 0.8, 1.0)));


    if (!serverMenuFont.Load("gui/baseFont.ttf", 18)) return false;
    if (!server.Load(vec2(window.w / 2 - 260, window.h / 2 + 200), "Server", serverMenuFont,
                     color4f(0.8, 0.8, 0.8, 0.8), color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!mod.Load(vec2(window.w / 2 - 100, window.h / 2 + 200), "Game mode", serverMenuFont,
                  color4f(0.8, 0.8, 0.8, 0.8), color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!players.Load(vec2(window.w / 2 + 70, window.h / 2 + 200), "Players", serverMenuFont,
                      color4f(0.8, 0.8, 0.8, 0.8), color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!ping.Load(vec2(window.w / 2 + 190, window.h / 2 + 200), "Ping", serverMenuFont, color4f(0.8, 0.8, 0.8, 0.8),
                   color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;

    if (!addServer.Load(vec2(window.w / 2 - 240, window.h / 2 - 210), "Add server", serverMenuFont,
                        color4f(0.8, 0.8, 0.8, 0.8), color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!refreshServers.Load(vec2(window.w / 2 - 100, window.h / 2 - 210), "Refresh servers", serverMenuFont,
                             color4f(0.8, 0.8, 0.8, 0.8), color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!quickConnect.Load(vec2(window.w / 2 + 80, window.h / 2 - 210), "Quick connect", serverMenuFont,
                           color4f(0.8, 0.8, 0.8, 0.8), color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;

    //quick connect menu
    if (!quickConnectBG.Load("gui/menuTableSmaller.dds", vec2(0, window.h / 2 - 75), vec2(500, 150),
                             color4f(1, 1, 1, 1.0f), true))
        return false;
    if (!quickConText.Load("Quick connect to server", color4f(1, 1, 1, 1), vec2(window.w / 2 - 220, window.h / 2 + 50),
                           inputBoxFont));
    if (!serverIpInputBox.Load("gui/inputBox.dds", "gui/inputBox_onclick.dds", "gui/inputBox_onclick.dds",
                               "Server IP: ", color4f(1, 1, 1, 1), color4f(0, 0, 0, 1), vec2(230, 25),
                               vec2(0, window.h / 2 + 10), inputBoxFont, 30, 25, true, false));
    if (!connectToServer.Load(vec2(window.w / 2 + 140, window.h / 2 - 12), "Connect", serverMenuFont,
                              color4f(0.8, 0.8, 0.8, 0.8), color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!closeQuickConnectButton.Load("gui/mmExitButton.dds", "gui/mmExitButton_onclick.dds", vec2(16, 16),
                                      vec2(window.w / 2 + 250 - 18, window.h / 2 + 75 - 18)))
        return false;

    //add server menu
    if (!addServerText.Load("Add server", color4f(1, 1, 1, 1), vec2(window.w / 2 - 220, window.h / 2 + 50),
                            inputBoxFont))
        return false;
    if (!addServerToList.Load(vec2(window.w / 2 + 125, window.h / 2 - 12), "Add server", serverMenuFont,
                              color4f(0.8, 0.8, 0.8, 0.8), color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!serverItemFont.Load("gui/baseFont.ttf", 14)) return false;

    //myAcc menu
    if (!myAccText.Load("My account", color4f(1, 1, 1, 1), vec2(window.w / 2 - 250, window.h / 2 + 230),
                        inputBoxFont))
        return false;
    if (!myAccBG.Load("gui/menuTable.dds", vec2(0, window.h / 2 - 250), vec2(600, 500), color4f(1, 1, 1, 1.0f),
                      true))
        return false;
    if (!myAccTopLine.Load(vec2(window.w / 2 - 260, window.h / 2 + 190), vec2(0, 510), 1.0,
                           color4f(0.8, 0.8, 0.8, 1.0)));
    if (!myAccBottomLine.Load(vec2(window.w / 2 - 260, window.h / 2 - 190), vec2(0, 510), 1.0,
                              color4f(0.8, 0.8, 0.8, 1.0)));

    if (!closeMyAccButton.Load("gui/mmExitButton.dds", "gui/mmExitButton_onclick.dds", vec2(24, 24),
                               vec2(window.w / 2 + 300 - 21, window.h / 2 + 250 - 21)))
        return false;

    //BUGGEDDDD !
    //if(!myAccUserAvatar.Load("gui/user/avatar.jpg", vec2(window.w/2-250, window.h/2+30), vec2(128, 128), color4f(1,1,1,1), false))
    //if(!myAccUserAvatar.Load("gui/user/notfound.jpg", vec2(window.w/2-250, window.h/2+30), vec2(128, 128), color4f(1,1,1,1), false)) return false;
    if (!myAccUserNick.Load("gui/inputBox.dds", "gui/inputBox_onclick.dds", "gui/inputBox_onclick.dds", "Nick: ",
                            color4f(1, 1, 1, 1), color4f(0, 0, 0, 1), vec2(280, 25),
                            vec2(window.w / 2 - 35, window.h / 2 + 150), inputBoxFont, 25, 35, false, false));
    if (!myAccUserAvatarPath.Load("gui/inputBox.dds", "gui/inputBox_onclick.dds", "gui/inputBox_onclick.dds",
                                  "Avatar: ", color4f(1, 1, 1, 1), color4f(0, 0, 0, 1), vec2(280, 25),
                                  vec2(window.w / 2 - 35, window.h / 2 + 100), inputBoxFont, 25, 35, false, false));
    if (!myAccUploadAvatar.Load(vec2(window.w / 2 + 150, window.h / 2 + 40), "Upload", serverMenuFont,
                                color4f(0.8, 0.8, 0.8, 0.8), color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!myAccBrowseAvatar.Load(vec2(window.w / 2 - 15, window.h / 2 + 40), "Browse", serverMenuFont,
                                color4f(0.8, 0.8, 0.8, 0.8), color4f(0.12, 0.36, 0.79, 1.0), false))
        return false;
    if (!saveMyAcc.Load(vec2(window.w / 2 - 250, window.h / 2 - 220), "Save", basicFont, color4f(0.5, 0.5, 0.5, 1),
                        color4f(1, 1, 1, 1), false))
        return false;

    //myAccMenu bottomStats
    if (!myAccStats.Load("My stats", color4f(1, 1, 1, 1), vec2(window.w / 2 - 250, window.h / 2 + 20),
                         inputBoxFont))
        return false;
    if (!myAccStatsLine.Load(vec2(window.w / 2 - 260, window.h / 2 - 10), vec2(0, 510), 1.0,
                             color4f(0.8, 0.8, 0.8, 1.0)));

    if (!myAccHeadshots.Load("Headshots: ", color4f(1, 1, 1, 1), vec2(window.w / 2 - 250, window.h / 2 - 30),
                             inputBoxFont))
        return false;
    if (!myAccAssists.Load("Assists: ", color4f(1, 1, 1, 1), vec2(window.w / 2 - 250, window.h / 2 - 50),
                           inputBoxFont))
        return false;
    if (!myAccDeaths.Load("Deaths: ", color4f(1, 1, 1, 1), vec2(window.w / 2 - 250, window.h / 2 - 70),
                          inputBoxFont))
        return false;
    if (!myAccKills.Load("Kills: ", color4f(1, 1, 1, 1), vec2(window.w / 2 - 250, window.h / 2 - 90),
                         inputBoxFont))
        return false;
    if (!myAccWins.Load("Wins: ", color4f(1, 1, 1, 1), vec2(window.w / 2 - 250, window.h / 2 - 110),
                        inputBoxFont))
        return false;
    if (!myAccLoses.Load("Loses: ", color4f(1, 1, 1, 1), vec2(window.w / 2 - 250, window.h / 2 - 130),
                         inputBoxFont))
        return false;
    if (!myAccVertLine.Load(vec2(window.w / 2, window.h / 2 - 175), vec2(0, 150), 1.0, color4f(0.8, 0.8, 0.8, 1.0)));
    if (!myAccPlayedTime.Load("Total played time: ", color4f(1, 1, 1, 1), vec2(window.w / 2 + 30, window.h / 2 - 30),
                              inputBoxFont))
        return false;
    if (!myAccAccuracy.Load("Overall accuracy : ", color4f(1, 1, 1, 1), vec2(window.w / 2 + 30, window.h / 2 - 50),
                            inputBoxFont))
        return false;
    if (!myAccRank.Load("Rank: ", color4f(1, 1, 1, 1), vec2(window.w / 2 + 30, window.h / 2 - 70), inputBoxFont))
        return false;


    //loading
    if (!mainProgressBar.Load(vec2(window.w / 2 - 230, window.h / 2 - 40), vec2(0, 450),
                              vec2(window.w / 2 - 230, window.h / 2 - 25), inputBoxFont, 0, 15, color4f(0.6, 0, 0, 1)))
        return false;
    if (!loadingBG.Load("gui/inputFormBG.dds", vec2(0, window.h / 2 - 160), vec2(500, 170), color4f(0, 0, 0, 0.9f),
                        true))
        return false;

    if (!exitMessageBox.Create("gui/loginTable.dds", "   Are you really sure?", "Yes", "No", basicFont, vec2(350, 160),
                               color4f(1, 1, 1, 0.85f), color4f(0.5, 0.5, 0.5, 1), color4f(1, 1, 1, 1)))
        return false;
    if (!logoutButton.Load(vec2(20, window.h / 2 - 210), "Logout", basicFont, color4f(0.8, 0.8, 0.8, 1),
                           color4f(0.9, 0.9, 0.9, 1), false))
        return false;

    if (!handleMessage.Create("gui/inputFormBG.dds",
                              "Unexpected error!", "OK",
                              inputBoxFont,
                              vec2(500, 170),
                              vec2(window.w / 2, window.h / 2 - 160),
                              vec2(-120, 50),
                              color4f(1, 1, 1, 1),
                              color4f(0.5, 0.5, 0.5, 1),
                              color4f(1, 1, 1, 1)))
        return false;

    if (!menu.Load("gui/default.xmm")) {
        printf("error: can't load XMMenu!\n");
        return false;
    }

    char *logins;
    if (!(logins = LoadLogins())) {
        printf("warinng: cannot use automatic logins!\n");
    } else {
        char *pass = strstr(logins, "|") + 1;
        char *name = strtok(logins, "|");

        if (name && pass) {
            //printf("%s %s\n", name, pass);
            autologinCheckBox.Checked(true);
            mailBox.SetText(name);
            passBox.SetText(pass);
        }
    }

    return true;
}


void Gui::MyAccount(void) {
    myAccBG.Draw();
    myAccText.Draw();
    myAccTopLine.Draw();
    myAccBottomLine.Draw();
    myAccUserAvatar.Draw();
    myAccUserNick.Draw();
    if (myAccUserNick.Changed()) bUpdateNick = true;

    myAccUserAvatarPath.Draw();

    if (myAccBrowseAvatar.clicked() && !bAvatarClicked) {
        bBrowseAvatar = true;
    }

    if (myAccUploadAvatar.clicked()) {
        bUploadAvatar = true;
        bShowUploadBox = true;
    }

    myAccStats.Draw();
    myAccStatsLine.Draw();
    myAccHeadshots.Draw();
    myAccAssists.Draw();
    myAccDeaths.Draw();
    myAccKills.Draw();
    myAccWins.Draw();
    myAccLoses.Draw();

    myAccVertLine.Draw();

    myAccPlayedTime.Draw();
    myAccAccuracy.Draw();
    myAccRank.Draw();

    //vsechno vytahnu z databazicQy
    glPrintn(1, 1, 1, 1, inputBoxFont, window.w / 2 - 150, window.h / 2 - 30, "%d\n", 0);
    glPrintn(1, 1, 1, 1, inputBoxFont, window.w / 2 - 150, window.h / 2 - 50, "%d\n", 0);
    glPrintn(1, 1, 1, 1, inputBoxFont, window.w / 2 - 150, window.h / 2 - 70, "%d\n", 0);
    glPrintn(1, 1, 1, 1, inputBoxFont, window.w / 2 - 150, window.h / 2 - 90, "%d\n", 0);
    glPrintn(1, 1, 1, 1, inputBoxFont, window.w / 2 - 150, window.h / 2 - 110, "%d\n", 0);
    glPrintn(1, 1, 1, 1, inputBoxFont, window.w / 2 - 150, window.h / 2 - 130, "%d\n", 0);
    glPrintn(1, 1, 1, 1, inputBoxFont, window.w / 2 + 180, window.h / 2 - 30, "%d\n", 0);
    glPrintn(1, 1, 1, 1, inputBoxFont, window.w / 2 + 180, window.h / 2 - 50, "%d\n", 0);
    glPrintn(1, 1, 1, 1, inputBoxFont, window.w / 2 + 180, window.h / 2 - 70, "%d\n", 0);


    if (closeMyAccButton.clicked()) bShowMyAcc = false;
    if (!myAccBrowseAvatar.clicked() && bAvatarClicked) bAvatarClicked = false;
}

void Gui::ServerMenu(void) {
    serversBG.Draw();

    if (server.clicked()) {

    }

    if (mod.clicked()) {

    }

    if (players.clicked()) {

    }

    if (ping.clicked()) {

    }

    topLine.Draw();

    if (closeServersButton.clicked()) {
        bShowMenuWithServers = false;
    }

    if (iNumServers <= 0) {
        glPrintn(1, 1, 1, 1, serverItemFont, window.w / 2 - 263, window.h / 2 + 170,
                 "You have no servers in list ...\n");
    } else {
        for (int i = 0; i < iNumServers; i++) {
            if (serverItemButton[i].clicked() && serverItem[i].responding) {
                //trlalalala...
                netw.SetServerIP(serverItem[i].ip);
                netw.SetPort(serverItem[i].port);
                bLevelLoaded ? bShowGUI = false : bLoadLevel = true;
            }

            serverMode[i].Draw();
            serverNumPlayers[i].Draw();
            serverPing[i].Draw();

            if (serverItemDeleteButton[i].clicked() && !bClickedDelete) {
                //smazu to muhhahahahaha...
                DeleteServerFromList(i);
                bClickedDelete = true;
            }

            if (!serverItemDeleteButton[i].clicked() && bClickedDelete) bClickedDelete = false;
        }
    }

    bottomLine.Draw();


    if (addServer.clicked()) {
        bShowAddServer = true;
    }

    if (refreshServers.clicked() && !bRefreshServerListClicked) {
        //freshnu servery mucQ
        bRefreshServerList = true;
        bRefreshServerListClicked = true;
    }

    if (!refreshServers.clicked()) bRefreshServerListClicked = false;

    if (quickConnect.clicked()) {
        bShowQuickConnect = true;
    }


    if (bShowQuickConnect) QuickConnect();
    if (bShowAddServer) AddServer();
}


void Gui::Draw(void) {
    menu.Draw();

    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, window.w, 0, window.h, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (bShowLoginMenu) {
        ShowLoginMenu();
    } else {
        ShowMainMenu();
    }

    if (bShowExitMSGBox) {
        if (exitMessageBox.Draw() == _YES) {
            bLogoutUser = true;
            bExitWindow = true;
            bShowExitMSGBox = false;
        }

        if (exitMessageBox.Draw() == _NO) {
            bShowExitMSGBox = false;
        }
    }

    if (bHandleMsgCalled) {
        if (handleMessage.Draw()) {
            bHandleMsgCalled = false;
            bShowGUI = true;
        }
    }

    if (showMessage && !bLoadLevel) {
        glPrint(1, 1, 1, 1, basicFont, 150, 100, "%s\n", cUserMessage);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);

}


void Gui::ShowMainMenu(void) {
    if (bLoadLevel) {
        loadingBG.Draw();
        glPrintn(1, 1, 1, 1, basicFont, window.w / 2 + 100, window.h / 2 - 90, "Loading ...");
        mainProgressBar.Draw();
    } else {

        mainMenuBG.Draw();
        if (bDownloadedAvatar) {
            if (userAvatarName.clicked()) {
                //skocim do user acc
                bShowMyAcc = true;
            }
        }
        userAvatar.Draw();


        if (logoutButton.clicked()) {
            bLogoutUser = true;
        }


        if (findServersButton.clicked()) {
            bShowMenuWithServers = true;
        }

        if (myAccountButton.clicked()) {
            bShowMyAcc = true;
        }

        if (optionsButton.clicked()) {
            bShowOptions = true;
        }

        if (mainMenuExitButton.clicked()) {
            bShowExitMSGBox = true;
        }

        if (bShowMenuWithServers) ServerMenu();
        if (bShowOptions) Options();
        if (bShowMyAcc) MyAccount();

        if (bLogoutUser) {
            inputFormsBG.Draw();
            glPrintn(1, 1, 1, 1, basicFont, window.w / 2 - 30, window.h / 2 - 100, "Disconnecting ...");

        }
    }
}

void Gui::QuickConnect() {
    quickConnectBG.Draw();
    serverIpInputBox.Draw();
    quickConText.Draw();
    if (closeQuickConnectButton.clicked()) {
        quickConText.SetDefaultText();
        bShowQuickConnect = false;
    }


    if (connectToServer.clicked() && !bConnect) {
        char *serverIP = serverIpInputBox.GetText();
        if (!serverIP) return;
        int port = 0;
        bool dDot = false;
        char cport[10];
        char ip[255];
        int j = 0, k = 0;
        for (int i = 0; i < strlen(serverIP); i++) {
            if (serverIP[i] == ':') dDot = true;

            if (dDot) {
                cport[j] = serverIP[i + 1];
                j++;
            } else {
                ip[k] = serverIP[i];
                k++;
            }
        }
        cport[j] = '\0';
        ip[k] = '\0';
        port = atoi(cport);
        //printf("%s:%d\n", ip, port);

        if (!dDot || (port < 1) || !ip)// || !IsIPv4(ip))
        {
            printf("error: invalid ip adress format!\n");
            quickConText.SetText("Quick connect to server - bad ip adress format!");
            bConnect = true;
            return;
        }

        netw.SetPort(port);
        netw.SetServerIP(ip);

        bLevelLoaded ? bShowGUI = false : bLoadLevel = true;
        bShowQuickConnect = false;
        quickConText.SetDefaultText();
        serverIpInputBox.SetText("");
    }

    if (!connectToServer.clicked() && bConnect) bConnect = false;
}


void Gui::AddServer(void) {
    quickConnectBG.Draw();
    serverIpInputBox.SetActive();
    serverIpInputBox.Draw();
    addServerText.Draw();
    if (closeQuickConnectButton.clicked()) {
        addServerText.SetDefaultText();
        bShowAddServer = false;
    }


    if (addServerToList.clicked() && !bAddServer) {
        char *serverIP = serverIpInputBox.GetText();
        if (!serverIP) return;
        int port = 0;
        bool dDot = false;
        char cport[10];
        char ip[255];
        int j = 0, k = 0;
        for (int i = 0; i < strlen(serverIP); i++) {
            if (serverIP[i] == ':') dDot = true;

            if (dDot) {
                cport[j] = serverIP[i + 1];
                j++;
            } else {
                ip[k] = serverIP[i];
                k++;
            }
        }
        cport[j] = '\0';
        ip[k] = '\0';
        port = atoi(cport);
        printf("%s:%d\n", ip, port);

        if (!dDot || (port < 1) || !ip)// || !IsIPv4(ip))
        {
            printf("error: invalid ip adress format!\n");
            addServerText.SetText("Add server - bad ip adress format!");
            bConnect = true;
            return;
        }

        for (int i = 0; i < iNumServers; i++)
            if (!strcmp(serverItem[i].ip, ip) && serverItem[iNumServers].port == port)return;

        serverItem[iNumServers].Set(ip, port);
        CreateServerItem(iNumServers);
        iNumServers++;
        bShowAddServer = false;
        addServerText.SetDefaultText();
        serverIpInputBox.SetText("");

        FILE *pFile = fopen("gui/servers.cfg", "a+");
        if (!pFile) return;
        fprintf(pFile, "%s %d\n", ip, port);
        fclose(pFile);
    }

    if (!addServer.clicked() && bAddServer) bAddServer = false;
}


void Gui::Options(void) {
    if (bShowOptions) {
        optionsBG.Draw();

        if (resolutionButton.clicked() && !bClicked[0]) {
            char res[256];
            int iNumResolutions = 0;
            resolutions = GetResolutions(iNumResolutions);
            iCurrResolution++;
            if (iCurrResolution >= iNumResolutions) iCurrResolution = 0;
            sprintf(res, "%dx%d\n", resolutions[iCurrResolution].w, resolutions[iCurrResolution].h);
            resolutionButton.SetButtonText(res);
            if (resolutions) delete[] resolutions;
            bClicked[0] = true;
            bChangedRes = true;
        }

        if (fullscreenButton.clicked() && !bClicked[6]) {
            bChangedFullscreen = true;
            bFullscreen = !bFullscreen;

            if (bFullscreen)
                fullscreenButton.SetButtonText("true");
            else
                fullscreenButton.SetButtonText("false");

            bClicked[6] = true;
        }

        if (shadowsButton.clicked() && !bClicked[1]) {
            bUseShadows++;
            if (bUseShadows > 3) bUseShadows = 0;
            switch (bUseShadows) {
                case 0:
                    shadowsButton.SetButtonText("none");
                    break;

                case 1:
                    shadowsButton.SetButtonText("low");
                    break;

                case 2:
                    shadowsButton.SetButtonText("medium");
                    break;

                case 3:
                    shadowsButton.SetButtonText("best");
                    break;
            }


            bClicked[1] = true;
        }

        if (bumpmappingButton.clicked() && !bClicked[2]) {
            bUseBumpMapping = !bUseBumpMapping;

            if (bUseBumpMapping)
                bumpmappingButton.SetButtonText("true");
            else
                bumpmappingButton.SetButtonText("false");

            bClicked[2] = true;
        }

        if (vsyncButton.clicked() && !bClicked[3]) {
            bUseVsync = !bUseVsync;

            if (bUseVsync)
                vsyncButton.SetButtonText("true");
            else
                vsyncButton.SetButtonText("false");

            bClicked[3] = true;
        }

        if (dofButton.clicked() && !bClicked[4]) {
            bUseDOF = !bUseDOF;

            if (bUseDOF)
                dofButton.SetButtonText("true");
            else
                dofButton.SetButtonText("false");

            bClicked[4] = true;
        }

        if (bloomButton.clicked() && !bClicked[5]) {
            bUseAO = !bUseAO;

            if (bUseAO)
                bloomButton.SetButtonText("true");
            else
                bloomButton.SetButtonText("false");

            bClicked[5] = true;
        }


        if (!resolutionButton.clicked()) bClicked[0] = false;
        if (!shadowsButton.clicked()) bClicked[1] = false;
        if (!bumpmappingButton.clicked()) bClicked[2] = false;
        if (!vsyncButton.clicked()) bClicked[3] = false;
        if (!dofButton.clicked()) bClicked[4] = false;
        if (!bloomButton.clicked()) bClicked[5] = false;
        if (!fullscreenButton.clicked()) bClicked[6] = false;


        if (saveOptionsButton.clicked()) {
            //ulozim nastaveni, ale az zitra :))))))
            bSaveOptions = true;
        }

        if (bSaveOptions) {
            if (saveSettingsBox.Draw() == _YES) {
                SaveOptions();
                bLogoutUser = true;
                bExitWindow = true;
            }

            if (saveSettingsBox.Draw() == _NO) {
                bSaveOptions = false;
            }
        }
    }
}

void Gui::RegisterForm(void) {
    registerMenuBG.Draw();
    registerMeText.Draw();
    registerMeLine.Draw();


    inputNick.Draw();
    inputName.Draw();
    inputPass.Draw();
    inputMail.Draw();
    if (agreeCheckBox.checked()) {
        bAgreeBoxChecked = !bAgreeBoxChecked;
    }

    if (closeRegisterMenuButton.clicked()) {
        bRegisterMe = false;
    }

    if (sendRegistrationButton.clicked()) {
        char *nick = inputNick.GetText();
        char *name = inputName.GetText();
        char *pass = inputPass.GetText();
        char *mail = inputMail.GetText();

        if (!nick || !*nick || !IsValidNick(nick)) {
            registerMeText.SetText("Register me - invalid nick only [A-z][0-9] is allowed!");
            return;
        }

        if (!name || !*name || !IsValidName(name)) {
            registerMeText.SetText("Register me - invalid name !");
            return;
        }

        if (!pass || strlen(pass) < 6) {
            registerMeText.SetText("Register me - password length is less than 6 chars !");
            return;
        }

        if (!mail || !isValidEmail(mail)) {
            registerMeText.SetText("Register me - invalid mail !");
            return;
        }

        if (!bAgreeBoxChecked) {
            registerMeText.SetText("Register me - you disagreed with the conditions !");
            return;
        }

        registerMeText.SetText("Register me");
        bSendRegistration = true;
    }
}

void Gui::RegisterUser() {
    char finalHeader[1024];
    char registerParams[256];
    char saltedParams[1024];

    char *salt = ")^f=EFDpsvG]ow}q{+GZaHO@IjYmGa_I(TgIm,]QZ5w]#2UIGSq+G4SV#+<jJI}>";
    char *nick = inputNick.GetText();
    char *name = inputName.GetText();
    char *pass = inputPass.GetText();
    char *mail = inputMail.GetText();

    sprintf(registerParams, "%s|%s|%s", nick, mail, salt);
    sprintf(finalHeader, registerHeaderUserExists, base64_encode(registerParams, strlen(registerParams)));

    char *ret = net.Send(finalHeader);
    printf("%s %d\n", ret, GetMessageType(ret));

    if (ret) {
        switch (GetMessageType(ret)) {
            case 12: {
                registerMeText.SetText("Register me - nick already exists !");
            }
                break;

            case 13: {
                sprintf(registerParams, "%s|%s|%s|%s|%s", nick, name, pass, mail, salt);
                sprintf(finalHeader, registerHeader, base64_encode(registerParams, strlen(registerParams)));
                printf("%s\n", finalHeader);

                char *ret = net.Send(finalHeader);
                printf("%s\n", ret);
                if (ret) {
                    //switch(GetMessageType(ret))
                    //{

                    //}
                }
            }
                break;
        }
    }
    bSendRegistration = false;
}

void Gui::ShowLoginMenu(void) {
    if (!bRegisterMe) {
        inputFormsBG.Draw();


        if (mailBox.IsActive() && GetAsyncKeyState(VK_TAB)) {
            passBox.SetActive();
            mailBox.UnsetActive();
        }

        mailBox.Draw();
        passBox.Draw();

        if (autologinCheckBox.checked()) {
            bSaveLogins = true;
        }

        //glPrintn(1,1,1,1, basicFont, 0, 50, "(c) FR4M3S0FT");


        if (sendButton.clicked() || (passBox.IsActive() && GetAsyncKeyState(VK_RETURN))) {
            bLoginUser = true;
        }

        if (registerButton.clicked()) {
            bRegisterMe = true;
        }
    }

    if (bRegisterMe) RegisterForm();

    //if(showLogins)
    //{
    //    glPrint(1,1,1,1, basicFont, 150, 200, "Ur Login: %s Ur pass: %s\n",mailBox.GetText,passBox.GetText());
    //}

    if (exitButton.clicked()) {
        bShowExitMSGBox = true;
    }

    if (bLoginUser) {
        inputFormsBG.Draw();
        glPrintn(1, 1, 1, 1, basicFont, window.w / 2, window.h / 2 - 100, "Connecting ...");

    }

}


bool Gui::SaveOptions(void) {
    FILE *pFile = fopen("init.cfg", "wb");
    if (!pFile) return false;

    fprintf(pFile, "//CFG file, by Jatro ;)\n");
    if (bChangedRes)
        fprintf(pFile, "screen size %dx%dx%d - %d;\n", resolutions[iCurrResolution].w, resolutions[iCurrResolution].h,
                resolutions[iCurrResolution - 1].b, bChangedFullscreen ? (int) bFullscreen : (int) window.bFullscreen);
    else
        fprintf(pFile, "screen size %dx%dx%d - %d;\n", window.w, window.h, window.b, (int) window.bFullscreen);
    fprintf(pFile, "use console %d;\n", window.bConsole);
    fprintf(pFile, "use shadows %d;\n", iCurrShadowType);
    fprintf(pFile, "use bumpmapping %d;\n", bUseBumpMapping);
    fprintf(pFile, "use vsync %d;\n", bUseVsync);
    fprintf(pFile, "use depth of field %d;\n", bUseDOF);
    fprintf(pFile, "use ao %d;\n", bUseAO);

    fclose(pFile);
}

void Gui::LoginUser() {
    char finalHeader[1024];
    char loginParams[256];

    if (!mailBox.GetText() || !passBox.GetText()) return;

    if (!SaveLogins()) {
        printf("error: can't save logins!\n");
    }

    sprintf(loginParams, "%s|%s", mailBox.GetText(), passBox.GetText());
    strcpy(loginParams, base64_encode(loginParams, strlen(loginParams)));
    sprintf(finalHeader, loginHeader, loginParams);

    char *msg = net.Send(finalHeader);

    if (msg) {
        msg = base64_decode(msg, strlen(msg));
    } else {
        SetDefaultMessage();
        bShowLoginMenu = true;
        bLoginUser = false;
        return;
    }

    int msgtype = GetMessageType(msg);
    SetMessageByType(msgtype);

    if (msgtype == 4) {
        if (!GetUserInfo(msg)) {
            SetDefaultMessage();
            bShowLoginMenu = true;
            bLoginUser = false;
        }
    } else {
        bShowLoginMenu = true;
        bLoginUser = false;
    }

    if (!bSaveLogins) {
        mailBox.SetText("");
        passBox.SetText("");
        autologinCheckBox.Checked(false);
    }
    return;
}

bool Gui::GetUserInfo(char *info) {
    if (!info) return false;

    char array[16][255];
    int i = 0;
    char *pch = strtok(info, "|");
    while (pch && i < 10) {
        strcpy(array[i++], pch);
        pch = strtok(NULL, "|");
    }

    int j = 0, k;
    char *ptr = strtok(base64_decode(array[1], strlen(array[1])), "|");
    while (ptr) {
        if (!strcmp(ptr, "\n") || !strcmp(ptr, "")) continue;
        char ip[32];
        for (k = 0; ptr[k] != ':'; k++) ip[k] = ptr[k];
        ip[k] = '\0';
        char *p = strstr(ptr, ":");
        if (!p) continue;
        int port = atoi(p + 1);

        serverItem[j].Set(ip, port);
        CreateServerItem(j);
        j++;
        ptr = strtok(NULL, "|");
    }

    iNumServers = j;

    strcpy(cUserID, array[2]);
    strcpy(cUserName, array[3]);
    strcpy(cUserMail, array[4]);
    strcpy(cUserNick, array[5]);
    iUserAccType = atoi(array[6] ? array[6] : "0");
    strcpy(cUserAvatarPath, array[7]);
    myAccUserNick.SetText(cUserNick);

    int avatarSize = atoi(array[9] ? array[9] : "0");
    char *avatarPath = "gui/user/avatar.jpg";

    if (cUserAvatarPath) {
        if (avatarSize > 0) {
            FILE *fp = fopen(avatarPath, "wb");
            if (fp) {
                fwrite(pch, avatarSize, 1, fp);
                fclose(fp);
            }
        }
        int center = strlen(cUserNick) * 16;
        //userAvatarName.Load(vec2(30+center/2, window.h-180), cUserNick, basicFont, color4f(0.5,0.5,0.5, 1.0), color4f(1,1,1,1), false);
        if (!userAvatar.Load(avatarPath, vec2(30, window.h - 150), vec2(128, 128), color4f(1, 1, 1, 1), false)) {
            if (!userAvatar.Load("gui/user/default.jpg", vec2(15, window.h - 80), vec2(128, 128), color4f(1, 1, 1, 1),
                                 false)) {
                printf("error: cannot load default avatar!\n");
            }
        }
    }

    bDownloadedAvatar = true;
    bLoginUser = false;
    bShowLoginMenu = false;

    return true;
}

void Gui::UploadAvatar(void) {
    if (!bUploadAvatar) return;

    bUploadAvatar = false;
    char *path = myAccUserAvatarPath.GetText();
    if (!path) return;

    int fileSize = GetFileSize(path);
    if (fileSize < 16) return; //jeste v renderu ceknu jestli neni fajl mensi nez < 16 a > nez 2MB treba

    FILE *fp = fopen(path, "rb");
    if (!fp) return;

    char *file = path;
    while (strstr(file, "\\")) {
        file = strstr(file, "\\");
        file += 1;
    }

    if (!file) return;

    unsigned char header[2];
    fread(&header, 2, sizeof(unsigned char), fp);

    if ((int) header[0] == 255 &&
        (int) header[1] == 216) //valid JPG, nahraje cokoliv jinyho, exekuce php zakazu na serveru
    {
        unsigned char *content = new unsigned char[fileSize + 1];
        char *head = new char[fileSize + strlen(uploadAvatarHeader) * 2];

        rewind(fp);
        fread(&content, sizeof(unsigned char), fileSize, fp);
        content[fileSize] = '\0';

        for (int i = 0; i < 10; i++) {
            printf("%d\n", (int) content[i]);
        }

        system("pause");

        strcpy(head, file);
        strcat(head, "|");
        strcat(head, base64_encode((char *) content, fileSize));

        char *encoded = base64_encode(head, strlen(head));

        sprintf(head, uploadAvatarHeader, gui.GetSESSIONID(), strlen(encoded), encoded);

        char *ret = net.Send(head);
        printf("%s\n", ret);
    }

    fclose(fp);
}

