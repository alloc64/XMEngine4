/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _BULLET_H
#define _BULLET_H

class OneBullet {
public:
    OneBullet() {
        fired = false;
        collided = false;
    }

    bool fired;
    bool collided;
    vec3 start;
    vec3 direction;
    vec3 end;
    vec3 collPoints[2];
};

class Bullet {
public:
    Bullet() {
        iNumFiredBullets = -1;
    }

    void Fire();

    void Draw();

    void Update(int i);

    int CheckBulletsForCollision(vec3 center, float radius);

    bool CheckBulletForCollision(vec3 pos, vec3 dir, vec3 v0, vec3 v1, vec3 v2, int index);

    /*void GetDirection(void)
    {
        float wz;
        double cx, cy, cz;
        double proj[16];
        double mv[16];
        int vp[4];

        glGetDoublev(GL_MODELVIEW_MATRIX, mv);
        glGetDoublev(GL_PROJECTION_MATRIX, proj);
        glGetIntegerv(GL_VIEWPORT, vp);
        float wx = (float) window.w/2;
        float wy = (float) window.h/2;
        glReadPixels( wx, wy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz );
        gluUnProject( wx, wy, wz, mv, proj, vp, &cx, &cy, &cz );
        vec3 coords = vec3( cx, cy, cz );
        /*vec3 normal = Normalize(coords - camera.GetPosition());

        printf("%f %f %f\n", normal.x, normal.y, normal.z);

        glLineWidth(100);
        glPointSize(100);
        glColor4f(0,1,0,1);
        glPushMatrix();
        {
            glTranslatef(coords.x, coords.y, coords.z);
            {
                glBegin(GL_QUADS);
                {
                    glNormal3f( 0.0f,-1.0f, 0.0f);
                    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f + normal.x, -1.0f + normal.y, -1.0f + normal.z);
                    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f + normal.x, -1.0f + normal.y, -1.0f + normal.z);
                    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f + normal.x, -1.0f + normal.y, 1.0f + normal.z);
                    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f + normal.x, -1.0f + normal.y, 1.0f + normal.z);
                }
                glEnd();
            }
        }
        glPopMatrix();

    }*/

    OneBullet bullet[60];
    int iNumFiredBullets;
private:

};

#endif
