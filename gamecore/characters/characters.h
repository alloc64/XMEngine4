/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _CHARACTERS_H
#define _CHARACTERS_H

#include "../../main.h"

extern Network netw;
extern xmAnimation character[4];

#define charHeight 50.0

class InterpolatedCharacterEntity {
#define interpolationDelta 1.25f
private:

    vec3 angle;
    vec3 angleVel;

    vec3 pos;
    vec3 vel;

public:
    void update(float dt) {
        pos = pos + vel * dt;
        angle = angle + angleVel * dt;
    }

    void setNewTarget(vec3 newTarget, vec3 newAngle, float timeBetweenPackets) {
        vel = (newTarget - pos) * (interpolationDelta / timeBetweenPackets);
        angleVel = (newAngle - angle) * (interpolationDelta / timeBetweenPackets);
        //if(angleVel.y < 0) angleVel.y += 360;

        //printf("%f %f\n", angleVel.x, angleVel.y);
    }

    vec3 getPosition() {
        return pos;
    }

    vec3 getAngle() {
        return angle;
    }
};

class Characters {
public:
    Characters() {
        bHit = false;
        iCausedDamage = 0;
        iHitPlayerID = 0;
        bAnimateDeath = false;
        bWasDeathAnimated = false;
        bDeathAnimated = false;
        inPos = vec3(0);
        iCurrentWatchedPlayer = 0;
        iCurrentWatchedPlayerRealID = 0;
        camType = FIRST_PERSON;

        oldDt = 0;
        t = 0;
        currTime = 0;
        cnt = 0;
        dt = 0.0;
        bSetSpawn = false;
    }

    bool Load(void);

    void Draw(bool animate);

    mat4 bbOrientation;
    mat4 charOrientation;
    vec3 inPos;
    bool bHit;
    int iHitPlayerID;
    int iCausedDamage;
    int iCurrentWatchedPlayer;
    int iCurrentWatchedPlayerRealID;

    bool bAnimateDeath;
    bool bDeathAnimated;
    bool bWasDeathAnimated;
    bool bSetSpawn;

    bool bClicked[3];
    int camType;

    float oldDt;
    float t;
    float currTime;
    int cnt;
    float dt;

    inline int GetCurrentWatchedPlayerID(void) {
        return iCurrentWatchedPlayerRealID;
    }

    inline bool IsCurrentWatchedPlayerDead(void) {
        return netw.playerPositions[GetCurrentWatchedPlayerID()].dead;
    }

    inline void SetAlive(void) {
        bAnimateDeath = true;
        bDeathAnimated = true;
    }

    inline mat4 GetWeaponPosition(int charType) {
        return character[charType].GetWeaponPosition();
    }

    inline mat4 GetSpinePosition(int charType) {
        return character[charType].GetSpine2Position();
    }

    //special pro interpolace, funguuje to :)
    void Interpolate(int pid, vec3 &pos, vec3 &angle);

    InterpolatedCharacterEntity interpolatedCharEntity[MAX_PLAYERS];

    void DrawChar(int i, int charType, vec3 pos, vec3 angle, bool animate);

    void DrawWeapon(int wid, int charType, vec3 pos, vec3 angle);

private:

};

#endif
