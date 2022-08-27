/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _CARTRIDGES_H
#define _CARTRIDGES_H

#include "../../math/vector.h"
#include "../xmdmodel/xmdmodel.h"
#include "../weapons/weapons.h"

#define MAX_CARTRIDGES 100

extern xmWeapon weapon;

class OneCartridge {
public:
    int type;
    vec3 center;
    vec3 velocity;
    vec3 position;
    vec3 force;
    bool fired;

};

class CartridgeModel {
public:
    CartridgeModel() {
        strcpy(path, "");
    }

    int id;
    xmdModel model;
    char path[1024];
};


class Cartridge {
public:
    Cartridge() {
        iNumDrewnCarts = -1;
        iNumFallenCarts = 0;
        iNumLoadedCarts = 0;
    }

    void SetCenters(vec3 center) {
        for (int i = 0; i < 30; i++) {
            if (!cart[i].fired) continue;
            cart[i].center = center;
        }
    }

    inline bool CartridgeLoaded(char *path) {
        if (!cartModel || !path) return false;
        for (int i = 0; i < weapon.iNumWeapons; i++) {
            if (!strcmp(cartModel[i].path, path)) return true;
        }
        return false;
    }

    inline bool SetCartridges(int num) {
        if (!num) return false;
        cartModel = new CartridgeModel[num];
        return (cartModel);
    }

    void Fire();

    void Draw(int cartridgeType);

    void Update(int i);

    void DrawFallenCarts();

    int LoadCartridge(char *path);

    void CheckCollision(vec3 v0, vec3 v1, vec3 v2);

    OneCartridge cart[MAX_CARTRIDGES];
    CartridgeModel *cartModel;
    vec3 fallenCarts[MAX_CARTRIDGES];


private:
    int iNumDrewnCarts;
    int iNumFallenCarts;
    int iNumLoadedCarts;


};

#endif
