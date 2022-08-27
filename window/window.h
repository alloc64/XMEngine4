/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _WINDOW_H
#define _WINDOW_H

#include "../main.h"
#include <iostream>
#include <gl/gl.h>

extern Timer timer;
extern Network netw;

class Window {
public:
    Window();

    ~Window();

    int w, h, b;
    float fov;
    bool bFullscreen;
    bool bConsole;
    bool bResize;
    bool bExit;

    bool bExitResourceLoader;
    bool bExitRender;
    bool bExitNetwork;
    bool bWaitForLoader;

    char cTitle[1024];
    char character;

    HINSTANCE hInst;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC[4];
    DEVMODE devmode;
    HBITMAP LSBMP; //loading screen bmp
    HANDLE upThread;

    bool InitGL(void);

    bool Create(char *title, int w, int h, int b, bool fullscreen, bool console);

    bool SetupPixelFormat();

    bool OpenConsole();

    bool ResourceLoader(void);

    bool Networking(void);

    void MainRender(void);

    bool ChatRoom(void);

    void SetFOV(float fov) {
        this->fov = fov;
    }

    char GetChar() {
        return character;
    }

    void SetChar(char ch) {
        character = ch;
    }

    bool bRendered;

private:


};

extern Window window;


inline bool OpenConsole() {
    HANDLE hStdHandle = INVALID_HANDLE_VALUE;
    if (hStdHandle == INVALID_HANDLE_VALUE)// && window.bConsole)
    {
        int hConHandle;
        FILE *fp;
        CONSOLE_SCREEN_BUFFER_INFO coninfo;
        AllocConsole();

        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
        coninfo.dwSize.Y = 3000;
        SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
        SetConsoleTitle("Console");

        hStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        hConHandle = _open_osfhandle((long) hStdHandle, _O_TEXT);
        fp = _fdopen(hConHandle, "w");
        *stdout = *fp;
        setvbuf(stdout, NULL, _IONBF, 0);

        hStdHandle = GetStdHandle(STD_INPUT_HANDLE);
        hConHandle = _open_osfhandle((long) hStdHandle, _O_TEXT);
        fp = _fdopen(hConHandle, "r");
        *stdin = *fp;
        setvbuf(stdin, NULL, _IONBF, 0);

        hStdHandle = GetStdHandle(STD_ERROR_HANDLE);
        hConHandle = _open_osfhandle((long) hStdHandle, _O_TEXT);
        fp = _fdopen(hConHandle, "w");
        *stderr = *fp;
        setvbuf(stderr, NULL, _IONBF, 0);
        ios_base::sync_with_stdio(true);
    }

    return true;
}

inline void ResizeView(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(window.fov/*/weapons.fCameraZoom*/, 1.0 * (GLfloat) width / (GLfloat) height, 1.0f, 100000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    window.bResize = false;
}

inline void WaitForLoader(void) {
    //#define _SHOW_DEPTH_ERRORS
    while (window.bWaitForLoader) //zpusobi vytuhnuti dokud se vlakna nesrovnaj
    {
#ifdef _SHOW_DEPTH_ERRORS
        printf("warning: waiting for resource loader thread!\n");
#endif
        Sleep(10);
    }
}

inline bool FixedTimeStep(int value) {
    static double lastTime = 0.0f;
    double elapsedTime = 0.0;
    double currentTime = timer.GetSystemTime() * 0.001;
    elapsedTime = currentTime - lastTime;

    if (elapsedTime > (1.0 / value)) {
        lastTime = currentTime;
        return true;
    }

    return false;
}

inline void CloseWindow() {

    //if(MessageBox(NULL, "ERROR", NULL, NULL))
    {
        netw.DisconnectPlayer();
        window.bWaitForLoader = true;
        window.bExitRender = true;
        window.bExitNetwork = true;
        window.bExitResourceLoader = true;
        window.bExit = true;

        PostQuitMessage(0);
        exit(0);
    }
}

inline void drawbox(vec3 min, vec3 max) {
    glBegin(GL_LINE_LOOP);
    glVertex3f(max.x, max.y, min.z); // 0
    glVertex3f(min.x, max.y, min.z); // 1
    glVertex3f(min.x, min.y, min.z); // 2
    glVertex3f(max.x, min.y, min.z); // 3
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(max.x, min.y, max.z); // 4
    glVertex3f(max.x, max.y, max.z); // 5
    glVertex3f(min.x, max.y, max.z); // 6
    glVertex3f(min.x, min.y, max.z); // 7
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(max.x, max.y, min.z); // 0
    glVertex3f(max.x, max.y, max.z); // 5
    glVertex3f(min.x, max.y, max.z); // 6
    glVertex3f(min.x, max.y, min.z); // 1
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(max.x, min.y, max.z); // 4
    glVertex3f(min.x, min.y, max.z); // 7
    glVertex3f(min.x, min.y, min.z); // 2
    glVertex3f(max.x, min.y, min.z); // 3
    glEnd();
}

#endif

