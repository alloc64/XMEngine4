/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../main.h"
#include <commctrl.h>

//#define _info

extern Window window;
extern Timer timer;
extern Texture texture;
extern Camera camera;
extern Gui gui;
extern xmWeapon weapon;
extern xmMenu menu;

//shaders
extern Shader menuLighting;
extern xmLevelShaders xmlLighting;
UpdaterGUI updaterGui;
extern Updater updater;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HANDLE hThread[4];
char *cClassName = "FR4M3SOFT_WND_CLASS";

Window::Window() {
    bWaitForLoader = true;
    bExitRender = false;
    bExitResourceLoader = false;
    bExit = false;
    bRendered = false;

    strcpy(cTitle, "FR4M3 MT WINDOW2");
    fov = 50.0f;
}

Window::~Window() {

}

bool Window::Create(char *title, int w, int h, int b, bool fullscreen, bool console) {
    DWORD ExStyle;
    DWORD Style;
    WNDCLASS wc;

    RECT Rect;
    Rect.left = 0;
    Rect.top = 0;
    Rect.right = w;
    Rect.bottom = h;

    wc.style = CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC) WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreatePatternBrush((HBITMAP) LoadImage(NULL,
                                                              "gui/Loading.bmp",
                                                              IMAGE_BITMAP,
                                                              0, 0,
                                                              LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = cClassName;

    if (!RegisterClass(&wc)) {
        printf("error: cannot register window class!\n");
        return false;
    }

    if (bFullscreen) {
        DEVMODE dmScreenSettings;
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = w;
        dmScreenSettings.dmPelsHeight = h;
        dmScreenSettings.dmBitsPerPel = b;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
            if (MessageBox(NULL,
                           "This video mode is unsupported by your graphic card!\n If u click YES i'll use window mode!\n else program will exit!",
                           "error:", MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {
                bFullscreen = false;
            } else {
                return false;
            }
        }
        ExStyle = WS_EX_APPWINDOW;
        Style = WS_POPUP;
        ShowCursor(true);
    } else {

        ExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        Style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

        ShowCursor(true);//false);
    }

    AdjustWindowRectEx(&Rect, Style, false, ExStyle);
    if (!(hWnd = CreateWindowEx(ExStyle,
                                cClassName,
                                cTitle,
                                Style,
                                0,
                                0,
                                Rect.right - Rect.left,
                                Rect.bottom - Rect.top,
                                NULL,
                                NULL,
                                hInst,
                                NULL))) {
        printf("error: cannot create WAPI window!\n");
        return false;
    }

    hDC = GetDC(hWnd);

    if (!SetupPixelFormat()) {
        printf("error: cannot set window pixel format!\n");
        return false;
    }

    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);

    return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    char c = 0;
    switch (uMsg) {
        case WM_CLOSE:
            CloseWindow();
            return 0;
            break;

        case WM_CHAR:
            c = (char) wParam;
            if ((c >= 48 && c <= 57) || (c >= 64 && c <= 90) || (c >= 97 && c <= 122) || c == 8 || c == 32 || c == 46) {
                window.character = c;
            }
            break;

        case WM_SIZE:
            window.w = LOWORD(lParam);
            window.h = HIWORD(lParam);
            window.bResize = true;
            return 0;
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool Window::SetupPixelFormat() {
    int PixelFormat;
    PIXELFORMATDESCRIPTOR pfd =
            {
                    sizeof(PIXELFORMATDESCRIPTOR),
                    1,
                    PFD_DRAW_TO_WINDOW |
                    PFD_SUPPORT_OPENGL |
                    PFD_DOUBLEBUFFER,
                    PFD_TYPE_RGBA,
                    32,
                    0, 0, 0, 0, 0, 0,
                    0,
                    0,
                    0,
                    0, 0, 0, 0,
                    16,
                    0,
                    0,
                    PFD_MAIN_PLANE,
                    0,
                    0, 0, 0
            };

    if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) {
        printf("error: cannot choose window pixel format!\n");
        return false;
    }

    if (!SetPixelFormat(hDC, PixelFormat, &pfd)) {
        printf("error: cannot set pixel format!\n");
        return false;
    }
    return true;
}

bool LoadConfiguration(void) {
    FILE *pFile = fopen("init.cfg", "r");
    if (!pFile) return false;

    int w, h, b, f;
    int shadows;
    bool con;
    bool bump;
    bool vsync;
    bool dof;
    bool ao;
    char buff[256];
    while (fgets(buff, sizeof(buff), pFile)) {
        if (sscanf(buff, "screen size %dx%dx%d - %d;\n", &w, &h, &b, &f) == 4) {
            window.w = w;
            window.h = h;
            window.b = b;
            window.bFullscreen = (bool) f;
#ifdef _info
            printf("%dx%dx%d - %d\n", window.w, window.h, window.b, window.bFullscreen);
#endif
        }

        if (sscanf(buff, "use console %d;\n", &con) == 1) {
            window.bConsole = con;
#ifdef _info
            printf("%d\n", window.bConsole);
#endif
        }

        if (sscanf(buff, "use shadows %d;\n", &shadows) == 1) {
            gui.iCurrShadowType = shadows;
            if (shadows > 0) gui.bUseShadows = true; else gui.bUseShadows = false;
#ifdef _info
            printf("%d\n", gui.iCurrShadowType);
#endif
        }

        if (sscanf(buff, "use bumpmapping %d;\n", &bump) == 1) {
            gui.bUseBumpMapping = bump;
#ifdef _info
            printf("%d\n", gui.bUseBumpMapping);
#endif
        }

        if (sscanf(buff, "use vsync %d;\n", &vsync) == 1) {
            gui.bUseVsync = vsync;
#ifdef _info
            printf("%d\n", gui.bUseVsync);
#endif
        }

        if (sscanf(buff, "use depth of field %d;\n", &dof) == 1) {
            gui.bUseDOF = dof;
#ifdef _info
            printf("%d\n", gui.bUseDOF);
#endif
        }

        if (sscanf(buff, "use ao %d;\n", &ao) == 1) {
            gui.bUseAO = ao;
#ifdef _info
            printf("%d\n", gui.bUseAO);
#endif
        }

    }
    fclose(pFile);
    return true;
}


void PrepareLoader(void *data) {
    {
        if (!wglMakeCurrent(window.hDC, window.hRC[1])) {
            printf("GetLastError = %d\n", GetLastError());
            printf("error: cannot get context for loader thrEad! - trying it again\n");
        }

        if (!LoadExtensions()) {
            printf("error: cannot load shader contexts!\n");
            if (MessageBox(NULL, "Problems with shaders!", NULL, NULL)) {
                CloseWindow();
                return;
            }
        }

        if (!LoadVBOFunctions()) {
            printf("error: cannot create vbo contexts!\n");
            if (MessageBox(NULL, "Problems with vbo!", NULL, NULL)) {
                CloseWindow();
                return;
            }
        }

        if (!LoadFBOFunctions()) {
            printf("error: cannot create fbo contexts!\n");
            if (MessageBox(NULL, "Problems with fbo!", NULL, NULL)) {
                CloseWindow();
                return;
            }
        }

        if (!texture.LoadExtensions()) {
            printf("error: cannot find texture extensions!\n");
            CloseWindow();
        }

        while (!window.bExitResourceLoader) {
            if (!window.ResourceLoader()) {
                CloseWindow();
                return;
            }
            Sleep(10);
        }
        wglMakeCurrent(NULL, NULL);
    }
    _endthread();
}


void PlayAVI(char *filename) {
    bool bPause = false;
    HWND hVideoHandle = NULL;

    if (hVideoHandle == NULL) {
        hVideoHandle = MCIWndCreate(window.hWnd, window.hInst,
                                    WS_CHILD | WS_VISIBLE | MCIWNDF_NOMENU | MCIWNDF_NOPLAYBAR, filename);
        if (hVideoHandle == NULL) return;
        MCIWndPlay(hVideoHandle);
    } else {
        if (bPause) {
            MCIWndPlay(hVideoHandle);
            bPause = false;
        } else {
            MCIWndClose(hVideoHandle);
            bPause = true;
        }
    }
}


void PrepareRender() {
    {
        WaitForLoader();
        _getContext:
        if (!wglMakeCurrent(window.hDC, window.hRC[0])) {
            printf("GetLastError = %d\n", GetLastError());
            printf("error: cannot get context for render thrEad! - trying it again\n");
            goto _getContext;
        }

        if (!window.InitGL()) {
            printf("error: cannot load GL resources!\n");
            if (MessageBox(NULL, "Cannot start window", NULL, NULL)) {
                CloseWindow();
                return;
            }
        }

        if (!LoadVBOFunctions()) {
            printf("error: cannot create vbo contexts!\n");
            if (MessageBox(NULL, "Problems with vbo!", NULL, NULL)) {
                CloseWindow();
                return;
            }
        }

        if (!LoadFBOFunctions()) {
            printf("error: cannot create fbo contexts!\n");
            if (MessageBox(NULL, "Problems with fbo!", NULL, NULL)) {
                CloseWindow();
                return;
            }
        }

        if (!LoadExtensions()) {
            printf("error: cannot load shader contexts!\n");
            if (MessageBox(NULL, "Problems with shaders!", NULL, NULL)) {
                CloseWindow();
                return;
            }
        }


        if (!menu.CreateLightingShader()) {
            printf("error: cannot create ::xmMenu shader!\n");
            if (MessageBox(NULL, "Problems with shaders!", NULL, NULL)) {
                CloseWindow();
                return;
            }
        }


        int mvf;
        glGetIntegerv(GL_MAX_VARYING_FLOATS, &mvf);
        printf("GL_MAX_VARYING_FLOATS - %d\n", mvf);

        ResizeView(window.w, window.h);
    }
}

void PrepareNetwork(void *data) {
    {
        WaitForLoader();

        _getContext:
        if (!wglMakeCurrent(window.hDC, window.hRC[2])) {
            printf("GetLastError = %d\n", GetLastError());
            printf("error: cannot get context for render thrEad! - trying it again\n");
            goto _getContext;
        }

        while (!window.bExitNetwork) {
            WaitForLoader();
            if (!window.Networking()) {
                CloseWindow();
            }
            Sleep(10);
        }

        wglMakeCurrent(NULL, NULL);
    }
    _endthread();

}

void PrepareChatRoom(void *data) {
    {
        WaitForLoader();
        while (!window.bExitRender) {
            if (!window.ChatRoom()) {
                CloseWindow();
            }
            Sleep(1500);
        }
    }
    _endthread();
}

void UpdaterThread(void *data) {
    {
        char directoryList[4096];

        if (!updater.Connect(/*MASTER_SERVER_IP*/"83.208.74.122", MASTER_SERVER_PORT)) {
            printf("error: cannot connect to update server!\n");
            //goto _updateError;
        }

        if (!updater.GetDirectoryList("./", directoryList)) {
            printf("error: cannot get directory list!\n");
            //goto _updateError;
        }

        updaterGui.SetProgressBarPercents(40);
        updaterGui.SetProgressBarTitle("ufoprsaaaaaaaaaa");
        printf("ufoprso %s\n", directoryList);

        //_updateError:
        //    printf("error :(");
        //    PostQuitMessage(0);
    }
    _endthread();
}

INT_PTR CALLBACK UpdaterDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    updaterGui.hWnd = hWnd;
    switch (uMsg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps;

            HDC hdc = BeginPaint(hWnd, &ps);
            {
                LOGFONT logFont;
                GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(logFont), &logFont);
                logFont.lfItalic = TRUE;

                logFont.lfHeight = -14;
                logFont.lfItalic = FALSE;
                lstrcpy(logFont.lfFaceName, "Verdana");
                HFONT hFont = CreateFontIndirect(&logFont);
                SelectObject(hdc, hFont);
                SetTextColor(hdc, 0x0);
                SetBkMode(hdc, TRANSPARENT);
                TextOut(hdc, 15, 50, updaterGui.progBarTitle, strlen(updaterGui.progBarTitle));
                DeleteObject(hFont);
            }
            EndPaint(hWnd, &ps);
        }
            break;

        case WM_INITDIALOG: {
            SendDlgItemMessage(hWnd, IDC_PROGRESSBAR, PBM_SETRANGE32, 0, 100);
            SendDlgItemMessage(hWnd, IDC_PROGRESSBAR, PBM_SETPOS, 0, 0);
            window.upThread = (HANDLE) _beginthread(UpdaterThread, 0, NULL);
        }
            break;

    }

    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    window.hInst = hInstance;

    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC = ICC_WIN95_CLASSES;

    if (!InitCommonControlsEx(&icc)) return -1;

    if (!OpenConsole()) {
        printf("error: cannot open console!\n");
        return 2;
    }
/*
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_HLAVNI), NULL, UpdaterDialogProc);
    CloseHandle(window.upThread);

    return -1;
*/
    if (!LoadConfiguration()) {
        printf("error: cannot parse init file!\n");
        return 1;
    }

    if (!window.Create(window.cTitle,
                       window.w,
                       window.h,
                       window.b,
                       window.bFullscreen,
                       window.bConsole)) {
        printf("error: window creation failed!\n");
        return 3;
    }

    //PlayAVI("gui/intro.avi");


    window.hRC[0] = wglCreateContext(window.hDC);
    window.hRC[1] = wglCreateContext(window.hDC);
    window.hRC[2] = wglCreateContext(window.hDC);

    if (!window.hRC[0] || !window.hRC[1] || !window.hRC[2]) {
        printf("error: cannot create opengl context!\n");
        return 5;
    }

    if (!wglShareLists(window.hRC[0], window.hRC[1])
        || !wglShareLists(window.hRC[0], window.hRC[2])) {
        printf("error: wglShareLists failed!\n");
        return 4;
    }
    srand(time(NULL));

    hThread[0] = (HANDLE) _beginthread(PrepareLoader, 0, NULL);
    hThread[1] = (HANDLE) _beginthread(PrepareNetwork, 0, NULL);
    hThread[2] = (HANDLE) _beginthread(PrepareChatRoom, 0, NULL);

    WaitForSingleObject(hThread[0], 100);
    WaitForSingleObject(hThread[1], 100);
    WaitForSingleObject(hThread[2], 100);

    MSG msg;
    PrepareRender();

    while (!window.bExitRender) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_MOUSEWHEEL) {
                weapon.ResetAnimationState(0);
                weapon.bWeaponChanged = true;
                if (weapon.bFlashLightAnim) weapon.bFlashLightAnim = false;
                short int zDelta = (short int) HIWORD(msg.wParam);
                if (zDelta > 0) {
                    printf("nahoru\n");
                    weapon.iCurrWeapon++;
                }
                if (zDelta < 0) {
                    printf("dolu\n");
                    weapon.iCurrWeapon--;
                }
            }
        } else {
            if (!window.bWaitForLoader) {
                timer.CalculateFrameRate();
                ResizeView(window.w, window.h);
                window.MainRender();

                wglSwapLayerBuffers(window.hDC, WGL_SWAP_MAIN_PLANE);
            }
        }
    }

    wglMakeCurrent(NULL, NULL);
    window.bExit = true;

    if (!wglDeleteContext(window.hRC[0])) printf("error: cannot delete hRC context 1!\n");
    if (!wglDeleteContext(window.hRC[1])) printf("error: cannot delete hRC context 2!\n");
    if (!wglDeleteContext(window.hRC[2])) printf("error: cannot delete hRC context 3!\n");

    if (window.bFullscreen) {

        if (!ChangeDisplaySettings(NULL, CDS_TEST)) {
            ChangeDisplaySettings(NULL, CDS_RESET);
            ChangeDisplaySettings(&window.devmode, CDS_RESET);
        } else {
            ChangeDisplaySettings(NULL, CDS_RESET);
        }
        ShowCursor(true);
    }

    ChangeDisplaySettings(NULL, 0);
    ReleaseDC(window.hWnd, window.hDC);
    DestroyWindow(window.hWnd);
    UnregisterClass(cClassName, window.hInst);
    return (int) msg.wParam;
}


