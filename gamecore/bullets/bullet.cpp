/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../../main.h"

#define _debug

extern Camera camera;
extern Timer timer;

void Bullet::Fire() {
    if (iNumFiredBullets >= 60) iNumFiredBullets = 0; else iNumFiredBullets++;
    bullet[iNumFiredBullets].fired = true;
    bullet[iNumFiredBullets].collided = false;

    bullet[iNumFiredBullets].start = camera.GetPosition();
    bullet[iNumFiredBullets].direction = -camera.GetCameraDirection();
    bullet[iNumFiredBullets].end = camera.GetPosition() - camera.GetCameraDirection() * 10000.0;
}

int Bullet::CheckBulletsForCollision(vec3 center, float radius) {
    int iNumHits = 0;
    for (int i = 0; i < iNumFiredBullets; i++) {
        if (bullet[i].fired && !bullet[i].collided) {
            float d;
            if (RaySphereIntersect(bullet[i].start, bullet[i].direction, center, radius, d)) {
                printf("%d intersected - %f %f %f\n", i, center.x, center.y, center.z);
                bullet[i].fired = false;
                bullet[i].collided = true;
                bullet[i].collPoints[0] = bullet[i].start;
                bullet[i].collPoints[1] = bullet[i].start + bullet[i].direction * d;
                iNumHits++;
            }
        }
    }

    return iNumHits;
}


void Bullet::Draw() {
    for (int i = 0; i < iNumFiredBullets; i++) {
#ifdef _debug
        glColor4f(0, 1, 0, 1);
        //if(bullet[i].fired)
        //{
        //glPushMatrix();
        //glTranslatef(-bullet[i].position.x, -bullet[i].position.y, -bullet[i].position.z);
        //glutSolidSphere(1.0f, 14, 14);
        //glPopMatrix();

        //glLineWidth(10.0);
        //glBegin(GL_LINES);
        //{
        //    glVertex3f(-bullet[i].start.x, -bullet[i].start.y, -bullet[i].start.z);
        //    glVertex3f(-bullet[i].end.x, -bullet[i].end.y, -bullet[i].end.z);
        //}
        //glEnd();
        //glLineWidth(1.0);
        //}

        if (bullet[i].collided) {
            glLineWidth(10.0);
            glBegin(GL_LINES);
            {
                glVertex3f(-bullet[i].collPoints[0].x, -bullet[i].collPoints[0].y, -bullet[i].collPoints[0].z);
                glVertex3f(-bullet[i].collPoints[1].x, -bullet[i].collPoints[1].y, -bullet[i].collPoints[1].z);
            }
            glEnd();
            glLineWidth(1.0);
        }
#endif
    }
}

/*



*/

