/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../../main.h"

extern Timer timer;
extern Camera camera;
extern Frustum frustum;

int Cartridge::LoadCartridge(char *path) {
    if (!CartridgeLoaded(path) && path) {
        if (cartModel[iNumLoadedCarts].model.Load(path)) {
            cartModel[iNumLoadedCarts].id = iNumLoadedCarts;
            int id = cartModel[iNumLoadedCarts].id;
            iNumLoadedCarts++;
            printf("ok: loaded cartridge %s\n", path);
            return id;
        }
    }
    printf("error: cannot load %s\n", path);
    return -1;
}

void Cartridge::Fire() {
    if (iNumDrewnCarts >= 30) iNumDrewnCarts = 0; else iNumDrewnCarts++;
    cart[iNumDrewnCarts].fired = true;
    cart[iNumDrewnCarts].velocity = vec3(15.0, 15.0, 0);
    cart[iNumDrewnCarts].force = vec3(0.0);
    cart[iNumDrewnCarts].position = vec3(0.0);
}

void Cartridge::Update(int i) {
    if (cart[i].fired) {
        float mass = 2.0f;
        cart[i].force -= vec3(0, -9.81 * mass, 0);
        cart[i].velocity -= cart[i].force * mass * timer.GetDT();
        cart[i].position -= cart[i].velocity * timer.GetDT();
    }
}

void Cartridge::Draw(int cartridgeType) {
    if (cartridgeType < 0) return;

    glPushMatrix();
    glLoadIdentity();
    glRotatef(180, 0, 0, 1);

    for (int i = 0; i < 30; i++) {
        if (!cart[i].fired) continue;
        Update(i);
        glPushMatrix();
        {
            glTranslatef(cart[i].position.x + cart[i].center.x, cart[i].position.y + cart[i].center.y,
                         cart[i].position.z + cart[i].center.z);
            cartModel[cartridgeType].model.Draw();
        }
        glPopMatrix();
    }
    glPopMatrix();
}

/*
void Cartridge::CheckCollision(vec3 v0, vec3 v1, vec3 v2)
{
    for(int i=0; i < 30; i++)
    {
        if(cart[i].fired)
        {
            vec3 pos = cart[i].position+camera.GetPosition()+cart[i].center;
            glPushMatrix();
            {
                glTranslatef(pos.x, pos.y, pos.z);
                cartModel[0].model.Draw();
            }
            glPopMatrix();

            vec3 cp;
            if(PointTriangleDistanceSquared(pos, v0, v1, v2, &cp) <= 1.0)
            {
                if(iNumFallenCarts >= 30) iNumFallenCarts = 0; else iNumFallenCarts++;
                cart[i].fired = false;
                fallenCarts[iNumFallenCarts] = pos;
                printf("%f %f %f\n", fallenCarts[iNumFallenCarts].x, fallenCarts[iNumFallenCarts].y, fallenCarts[iNumFallenCarts].z);
            }
        }
    }
}

void Cartridge::DrawFallenCarts()
{
    for(int i=0; i < 100; i++)
    {
        if(!frustum.SphereInFrustum(fallenCarts[i], 1.0)) continue;
        glPushMatrix();
        {
            glTranslatef(-fallenCarts[i].x, -fallenCarts[i].y, -fallenCarts[i].z);
            cartModel[0].model.Draw();
        }
        glPopMatrix();
    }
}
*/
