/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../main.h"

double Timer::GetSystemTime(void) {
    long long freq, time;
    SetThreadAffinityMask(GetCurrentThread(), 1);
    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&freq));
    double factor = 1000 / double((freq) ? freq : 1);
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&time));

    return time * factor;
}

void Timer::CalculateFrameRate(void) {
    static int iFramesPerSecond = 0;
    static float fLastTime = 0.0f;
    static float fFrameTime = 0.0f;
    double iCurrentTime = GetSystemTime();
    float fSecond = iCurrentTime * 0.001f;

    fFrameInterval = fSecond - fFrameTime;
    fFrameTime = fSecond;

    ++iFramesPerSecond;

    if (fSecond - fLastTime > 1.0f) {
        fLastTime = fSecond;
        iFps = iFramesPerSecond;
        iFramesPerSecond = 0;
        if (iFps > iMaxFps) iMaxFps = iFps;
    }

}
