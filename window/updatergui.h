/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _UPDATER_GUI
#define _UPDATER_GUI

/*
#define WINVER 0x0501
#define _WIN32_IE 0x301
#define PBS_MARQUEE 0x08
#define PBM_SETMARQUEE (WM_USER+10)
#define _WIN32_WINNT 0x0501
*/

#include <windows.h>
#include <commctrl.h>
#include "resource.h"


class UpdaterGUI {
public:
    HWND hWnd;
    char progBarTitle[2048];

    void SetProgressBarPercents(int percents) {
        LRESULT pos = SendDlgItemMessage(hWnd, IDC_PROGRESSBAR, PBM_GETPOS, 0, 0);
        if (pos < 100) {
            SendDlgItemMessage(hWnd, IDC_PROGRESSBAR, PBM_SETPOS, percents, 0);
        } else {
            SendDlgItemMessage(hWnd, IDC_PROGRESSBAR, PBM_SETPOS, 100, 0);
        }
    }

    void SetProgressBarTitle(char *title) {
        if (!title) return;
        strcpy(progBarTitle, title);
        InvalidateRect(hWnd, NULL, TRUE);
    }
};

#endif
