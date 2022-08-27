/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _WEAPONS_H
#define _WEAPONS_H

#define MAIN_WEAPON 0
#define SECONDARY_WEAPON 1
#define OTHER_WEAPON 2

extern Font smallFont;
extern xmAnimation *fpsWeaponModel;
extern xmdModel *weaponModel;

enum weaponTypes {
    SNIPER = 0,
    ASSAULT_RIFLE = 1,
    SMG = 2,
    PISTOL = 3,
    GRENADE = 4,
    KNIFE = 5,
    SHOTGUN = 6,
    VERTEXENTITY = 7,
    BONEDENTITY = 8
};

struct Decal {
    vec3 normal;
    vec3 position;
    int texID;
};

struct Damage {
    Damage() {
        head = 0;
        body = 0;
        feet = 0;

        secBody = 0;
        secFeet = 0;
    }

    int head;
    int body;
    int feet;

    int secBody;
    int secFeet;
};


class xmWeapon {
public:
    xmWeapon() {
        iNumDecals = 0;
        id = 0;
        strcpy(name, "");
        strcpy(realName, "");
        weight = 0.0f;
        magazine = 0;
        supply = 0;
        strcpy(animatedModelPath, "");
        strcpy(staticModelPath, "");
        strcpy(cartridgeModelPath, "");
        type = 0;
        throwable = false;
        efficiency = 0;
        effectiveRange = 0;
        bDropWeapon = false;
        iCurrWeapon = 0;
        bKnifeAnim = false;
        bShoot = false;
        bWeaponChanged = true;
        bZoom = false;
        bZoomClicked = false;
        bZoomAnimated = false;
        bWasZoomed = false;
        bIsZoomed = false;
        bFlashLightAnim = false;
        bFlashLightAnimated = false;
        bHideFlashlight = false;
        bEntityLoaded = false;
        wid = 1;
        animStart = 0;
        animEnd = 0;
        animLoopable = 0;
        cartridgeType = -1;
    }

    int id;
    char name[1024];
    char realName[1024];
    float weight;
    int magazine;
    int supply;
    char animatedModelPath[1024];
    char staticModelPath[1024];
    char spriteModelPath[1024];
    char cartridgeModelPath[1024];
    int type;
    int menuType;
    bool throwable;
    Damage damage;
    int efficiency;
    int effectiveRange;
    int animStart;
    int animEnd;
    int animLoopable;
    short int iNumWeapons;
    short int iNumVEntities;
    short int iNumBEntities;

    int iCurrAmmoCount;
    int cartridgeType;

    vec3 weapPos;
    vec3 weapVel;
    vec3 weapForce;
    vec3 cartridgePos;
    float weapRadius;
    bool bDropWeapon;
    bool bWeaponChanged;
    int iCurrWeapon;
    bool bKnifeAnim;
    bool bFlashLightAnim;
    bool bFlashLightAnimated;
    bool bShoot;
    bool bZoom;
    bool bZoomClicked;
    bool bZoomAnimated;
    bool bWasZoomed;
    bool bIsZoomed;
    bool bReloadWeapon;
    bool bAnimateReload;
    bool bHideFlashlight;
    bool bEntityLoaded;
    int wid;

    bool Load(char *path);

    inline void ResetAnimationState() {
        fpsWeaponModel[wid].bPlay = false;
        return;
    }


    inline void ResetAnimationState(int id) {
        if (!fpsWeaponModel || !fpsWeaponModel[wid].anim || id < 0 || wid < 0) return;

        fpsWeaponModel[wid].bPlay = false;
        fpsWeaponModel[wid].fAnimationTime = (float) fpsWeaponModel[wid].anim[id].animStartFrame;
        return;
    }

    inline void SetWeaponMessage(char *message) {
        if (!message) return;
        glPushMatrix();
        //glPrintn(1,1,1,1, smallFont, window.w/2+100, window.h/2, message);
        glPrintn(1, 1, 1, 1, smallFont, 300, 400, message);
        glPopMatrix();
    }

    inline bool ReloadWeapon() {
        return bReloadWeapon;
    }

    inline int GetCurrentAmmo(void) {
        return iCurrAmmoCount;
    }

    inline vec3 GetBarrelPosition(void) {
        return fpsWeaponModel[wid].GetBarrelPosition();
    }

    inline bool IsFlashlightActive(void) {
        return bFlashLightAnimated;
    }

    inline void DecreaseMagazine(void) {
        if (iCurrAmmoCount <= 0) {
            iCurrAmmoCount = 0;
            bReloadWeapon = true;
        } else {
            iCurrAmmoCount--;
        }
    }

    inline bool isShooting(void) {
        return bShoot;
    }

    inline bool isFlashLightActive(void) {
        return bFlashLightAnim;
    }

    inline void AddDecal(vec3 center, vec3 normal, int id) {
        if (iNumDecals >= 99) iNumDecals = 0; else iNumDecals++;
        decal[iNumDecals].position = center;
        decal[iNumDecals].normal = normal;
        decal[iNumDecals].texID = id;
    }

    inline int GetCurrentWeaponAnimID(void) {
        return fpsWeaponModel[wid].GetCurrentAnimationID();
    }

    inline int GetCurrentWeaponID(void) {
        return wid;
    }

    inline int GetCurrentWeapon() {
        return iCurrWeapon;
    }

    inline int GetCurrentWeaponFrame(void) {
        return (int) fpsWeaponModel[wid].fAnimationTime;
    }

    inline vec3 GetWeaponCartrigePosition(int wid) {
        return weaponModel[wid].weaponBulletPosition;
    }

    inline vec3 GetWeaponSpritePosition(int wid) {
        return weaponModel[wid].weaponSpritePosition;
    }

    inline vec3 GetWeaponHandlePosition(int wid) {
        return weaponModel[wid].weaponHandlePosition;
    }

    inline bool IsEntityLoaded(int wid) {
        return weapon[wid].bEntityLoaded;
    }

    bool DropWeapon(int wid);

    void DrawWeapon(int wid, vec3 pos);

    void DrawWeapon(int wid);

    void DrawFPSWeapon();

    void DrawFPSWeaponAt(int wID, int startRange, int endRange, int rate);

    void DrawFPSWeaponAt(int wID, int animID);

    void DrawDecals(void);

    void DrawSpark(void);

    xmWeapon *weapon;
    Decal decal[100];
    int iNumDecals;
    GLuint decalID[5];
private:

};

inline int classifyWeapon(char *name) {
    extern xmWeapon weapon;

    for (int i = 0; i < weapon.iNumWeapons; i++) {
        if (strstr(name, weapon.weapon[i].name)) return weapon.weapon[i].id;
    }

    return -1;
}

inline int getWeaponID(char *name) {
    extern xmWeapon weapon;

    for (int i = 0; i < weapon.iNumWeapons; i++) {
        if (strstr(name, weapon.weapon[i].name)) return weapon.weapon[i].wid;
    }

    return -1;
}

#endif
