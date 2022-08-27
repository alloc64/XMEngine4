/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "main.h"

Window window;
Timer timer;
Texture texture;
Font basicFont;
Gui gui;
xmMenu menu;
xmLevel level;
Frustum frustum;
Camera camera;
HUD hud;
Characters characters;

Network netw;
Updater updater;
Font smallFont;
extern PostProcess post;

bool bLoadGui = true;
bool bLoadLevel = true;

bool Window::ResourceLoader(void) {
    if (bLoadGui) {
        gui.Load();//if(!gui.Load()) //return false;
        bLoadGui = false;
    }

    if (gui.bLoadLevel && bLoadLevel) {
        _reconnect:
        SetConnectionLag(netw.GetSocket(), 30000);
        gui.mainProgressBar.SetPercents(1);
        gui.mainProgressBar.SetMessage("Trying connect to server ...");
        if (netw.ConnectToServerByUserList()) {
            gui.mainProgressBar.SetPercents(3);
            gui.mainProgressBar.SetMessage("Connecting to server ...");
            if (!netw.GetCurrentMap()) {
                printf("error: invalid map name, server side error, exitting!\n");
                return false;
            }

            printf("ok: getting hashes for %s\n", netw.GetCurrentMap());

            char *netwHash = netw.GetFileHash(netw.GetCurrentMap());
            if (!netwHash) {
                printf("error: map \"%s\" not found on server!\n", netw.GetCurrentMap());
                bLoadLevel = true;
                gui.bLoadLevel = false;
                gui.bLevelLoaded = false;
                gui.MessageBox("Map \"%s\" not found on server!\nError on server side, try it later!");
                netw.DisconnectPlayer();
            }

            if (!level.Load(netw.GetCurrentMap())) {
                printf("error: cannot load map \"%s\"!\n", netw.GetCurrentMap());
                bLoadLevel = true;
                gui.bLoadLevel = false;
                gui.bLevelLoaded = false;
                gui.MessageBox("cannot load map!");
                netw.DisconnectPlayer();
                //return false;
            }

            if (!hud.Load()) {
                printf("error: cannot load HUD!\n");
                return false;
            }

            SetConnectionLag(netw.GetSocket(), 100);

            gui.bLoadLevel = false;
            gui.bLevelLoaded = true;
            gui.bShowGUI = false;
            bLoadLevel = false;
        } else {
            _error:
            bLoadLevel = true;
            gui.bLoadLevel = false;
            gui.bLevelLoaded = false;
            gui.MessageBox("Cannot estabilish conection to server!\n");
            netw.DisconnectPlayer();
        }
    }

    window.bWaitForLoader = false;

    return true;
}

bool Window::Networking(void) {
    //if(window.bRendered)
    {
        if (gui.bShowGUI) {
            if (gui.bLoginUser) {
                gui.LoginUser();
            }

            if (gui.bRefreshServerList) {
                gui.RefreshServerItems();
            }

            gui.UpdatePlayerNick();
            gui.UploadAvatar();

            if (gui.bBrowseAvatar) {
                if (!gui.GetAvatarPath()) {
                    //UFOPORNOOOOO
                    //
                }
                gui.bBrowseAvatar = false;
            }

            if (gui.bSendRegistration) {
                gui.RegisterUser();
            }

            if (gui.bLogoutUser) {
                gui.LogoutUser();
                if (gui.bExitWindow) CloseWindow();
            }
        } else {
            if (gui.bLevelLoaded)// && camera.IsCameraMoving())
            {
                if (netw.GetNumberConnectionFails() > CONECTION_FAILS) {
                    printf("error: connection 2 server lost!\n");

                    gui.MessageBox("Connection to server lost!\n");
                    gui.bShowGUI = true;
                    return true; // abych si nezrusil vlakno
                }
                if (!netw.AdjustPositions()) {
                    printf("error: ::SendPositions()\n");
                    //bla bla bla
                }

                if (!netw.GetPlayerScores()) {
                    printf("error: ::GetPlayerScores()!\n");
                }

                if (!netw.SendPlayerHit()) {
                    //naka chybicka milacku ;D
                    printf("error: ::SendPlayerHit");
                }

                if (hud.WannaSendMessage()) {
                    if (!netw.SendChatMessage(gui.GetSESSIONID(), gui.cUserNick, hud.GetChatMessage())) {
                        printf("error: ::SendChatMessage()");
                    }
                    hud.bSendChatMessage = false;
                }
            }
        }
    }

    return true;
}

bool Window::ChatRoom(void) {
    char *msg = netw.GetChatMessages(gui.GetSESSIONID(), gui.cUserNick);
    if (msg) hud.SetMessages(msg);

    return true;
}


bool Window::InitGL(void) {
    glClearColor(0.35f, 0.53f, 0.7f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    smallFont.Load("gui/baseFont.ttf", 12);

    return true;
}


void Window::MainRender(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (!gui.bShowGUI && GetAsyncKeyState(VK_ESCAPE)) {
        gui.bShowGUI = true;
    }

    if (gui.bShowGUI) {
        gui.Draw();
    } else {
        camera.CamUpdate();
        frustum.Update();

        post.RenderLevel();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, window.w, 0, window.h, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        hud.Draw();
        glPrintn(1, 1, 1, 1, smallFont, window.w / 2 - 50, 100, "fps %d\nping %.1f\nloss: %d\nvis lights: %d\n",
                 timer.GetFPS(), netw.GetPing(), netw.GetPacketLoss(), level.iNumVisibleAreaLights);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glEnable(GL_DEPTH_TEST);
    }

    return;
}
