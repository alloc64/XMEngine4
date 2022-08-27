/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _TIMER_H
#define _TIMER_H

class Timer {
public:
    Timer() {

    }

    ~Timer() {

    }

    unsigned int iFps;
    unsigned int iMaxFps;
    unsigned int iCountTick;
    unsigned int iMinCountTick;
    unsigned int iStartTick;
    float fFrameInterval;
    float fMilliseconds;
    float dt;

    double GetSystemTime();

    float GetMilliseconds() {
        return fMilliseconds;
    }

    void CalculateFrameRate(void);

    unsigned int GetFPS(void) {
        return iFps;
    }

    unsigned int GetMaxFPS(void) {
        return iMaxFps;
    }

    float GetDT(void) {
        return fFrameInterval;
    }

    void SetDT(float dt) {
        this->dt = dt;
    }


private:

};

#endif	

