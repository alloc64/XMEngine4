/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "../../main.h"

extern Timer timer;
extern HUD hud;

Camera::Camera() {
    camSpeed = 0.0f;
    camRadius = 0.0f;

    camAngle.x = 180.0;
    camAngle.y = 270.0;
    camAngle.z = 0.0;

    bColliding = true;
    bJump = false;
    bCrouch = false;
    bCanSprint = true;

    camPosition = vec3(0, 0, 0);
    camMouseSens = 5.0f;

    sprintTime = 0.0f;
    walkTime = 0.0f;
    campJumpRotation = 0.0;

    accum = 0.0, t = 0.0, dt = 1.0 / 60.0;
    bCrouched = false;
}

Camera::~Camera() {

}

void Camera::MoveCam(float speed) {
    camPosition.x += (float) (sin(-camAngle.y * DEG) * speed);
    if (!bJump/* && GetType() == FREE_LOOK*/) camPosition.y -= (float) (sin(camAngle.x * DEG) * speed);
    camPosition.z += (float) (cos(camAngle.y * DEG) * speed);
}

void Camera::CamForward() {
    MoveCam(-camSpeed);
}

void Camera::CamBackward() {
    MoveCam(camSpeed);
}

void Camera::CamStrafeLeft() {
    camPosition.x -= (float) ((camSpeed / 2) * sin((camAngle.y - 90) * DEG));
    camPosition.z += (float) ((camSpeed / 2) * cos((camAngle.y - 90) * DEG));
}

void Camera::CamStrafeRight() {
    camPosition.x += (float) ((camSpeed / 2) * sin((camAngle.y - 90) * DEG));
    camPosition.z -= (float) ((camSpeed / 2) * cos((camAngle.y - 90) * DEG));
}

void Camera::CamLook() {
    if (CanCameraMove() || camType == THIRD_PERSON || camType == FREE_LOOK) {
        if (!hud.IsBuyMenuShown()) {
            POINT mousePos;
            vec2 middle = vec2(window.w / 2.0f, window.h / 2.0f);
            GetCursorPos(&mousePos);

            if ((mousePos.x > window.w) && (mousePos.y > window.h)) {
                SetCursorPos((int) middle.x, (int) middle.y);
                return;
            }

            SetCursorPos((int) middle.x, (int) middle.y);

            camAngle.x -= (float) ((middle.y - mousePos.y)) / camMouseSens;
            camAngle.y += (float) ((middle.x - mousePos.x)) / camMouseSens;

            if (camAngle.x <= 91) camAngle.x = 91;
            if (camAngle.x >= 269) camAngle.x = 269;
            if (camAngle.y >= 359) camAngle.y -= 359;
            if (camAngle.y < 0) camAngle.y += 359;

            glPushMatrix();
            {
                float mat[16];
                glRotatef(camAngle.y, 0, 1, 0);
                glRotatef(camAngle.x, 1, 0, 0);
                glGetFloatv(GL_MODELVIEW_MATRIX, mat);
                camDirection.x = mat[8];
                camDirection.y = mat[9];
                camDirection.z = -mat[10];
            }
            glPopMatrix();
        }
    }

    switch (camType) {
        case FIRST_PERSON:
        case FREE_LOOK: {
            glRotatef(camAngle.x, 1, 0, 0);
            glRotatef(camAngle.y, 0, 1, 0);
            glRotatef(camAngle.z, 0, 0, 1);
            glTranslatef(camPosition.x, camPosition.y, camPosition.z);
        }
            break;

        case THIRD_PERSON: {
            float mdl[16];
            {
                glTranslatef(0, 0, -thirdPersonCamRadius);
                glRotatef(camAngle.x, 1, 0, 0);
                glRotatef(camAngle.y, 0, 1, 0);
                glTranslatef(camPosition.x, camPosition.y, camPosition.z);

                glGetFloatv(GL_MODELVIEW_MATRIX, mdl);
                camThirdPersonPosition.x = (mdl[0] * mdl[12] + mdl[1] * mdl[13] + mdl[2] * mdl[14]);
                camThirdPersonPosition.y = (mdl[4] * mdl[12] + mdl[5] * mdl[13] + mdl[6] * mdl[14]);
                camThirdPersonPosition.z = (mdl[8] * mdl[12] + mdl[9] * mdl[13] + mdl[10] * mdl[14]);
                camPosition = camThirdPersonPosition;
            }
            glLoadIdentity();
            mat4 rot;
            glPushMatrix();
            {
                glRotatef(camAngle.x, 1, 0, 0);
                glRotatef(camAngle.y, 0, 1, 0);
                glGetFloatv(GL_MODELVIEW_MATRIX, rot.mat);
                glLoadIdentity();
                vec3 pos = rot * camPosition;
                glTranslatef(-pos.x, -pos.y, -pos.z);
                glutWireSphere(12, 12, 12);
            }
            glPopMatrix();

            glRotatef(camAngle.x, 1, 0, 0);
            glRotatef(camAngle.y, 0, 1, 0);
            glTranslatef(camPosition.x, camPosition.y, camPosition.z);
        }
            break;
    }
}

bool b = false;

void Camera::CamUpdateMovement() {
    if (!CanCameraMove() || hud.IsBuyMenuShown()) return;

    if (GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80 && !hud.IsChatActive()) {
        bMovingUP = true;
        CamForward();
    } else {
        bMovingUP = false;
    }

    if (GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80 && !hud.IsChatActive()) {
        bMovingDOWN = true;
        CamBackward();
    } else {
        bMovingDOWN = false;
    }

    if (GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80 && !hud.IsChatActive()) {
        bMovingLEFT = true;
        CamStrafeLeft();
    } else {
        bMovingLEFT = false;
    }

    if (GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80 && !hud.IsChatActive()) {
        bMovingRIGHT = true;
        CamStrafeRight();
    } else {
        bMovingRIGHT = false;
    }

    if (GetKeyState(VK_SHIFT) & 0x80 && bCanSprint && IsCameraMoving()) {
        bWalking = false;
        SetCameraSpeed(120.0f);

        if (sprintTime >= 15.0f) {
            bCanSprint = false;
            sprintTime = 0.0f;
        }

        sprintTime += 1.0f * timer.fFrameInterval;

    } else {
        bWalking = true;
        SetCameraSpeed(75.0f);

        if (walkTime >= 10.0f) {
            bCanSprint = true;
            walkTime = 0.0f;
        }
        walkTime += 1.0f * timer.fFrameInterval;

    }

    if (GetType() == FREE_LOOK) return;

    if (GetAsyncKeyState(VK_CONTROL)) {
        SetCameraSize(vec3(15, 25, 15));
        SetCameraSpeed(60.0f);
        bCrouched = true;
    } else {
        if (bCrouched) //bWasOnGround)
        {
            camPosition.y += 15;
            SetCameraSize(vec3(15, 48, 15));
            bCrouched = false;
        }
    }

    if (GetAsyncKeyState(VK_SPACE) && !bJump && !c && !hud.IsChatActive()) {
        bJump = true;
        bLeap = true;
        c = true;
        camVelocity = vec3(0, 40.0f, 0);
        camForce = vec3(0, 0, 0);
        camPosition.y += 4.0f;
    } else {
        if (bJump) {
            float dt = timer.GetDT();
            float mass = 140.0f;
            if (camForce.y < 20) camForce += vec3(0, -9.81 / mass, 0);
            camVelocity += camForce * mass * dt;
            camPosition += camVelocity * dt;
        } else {
            if (bLeap) {
                //camAngle.x += 10.0 * timer.GetDT();
                //camAngle.y += 10.0 * timer.GetDT();
            }
        }
    }
    if (!GetAsyncKeyState(VK_SPACE)) c = false;

    return;

    if (!bColliding && !bJump) {
        camPosition.y -= 50 * timer.GetDT();
        /*float dt = timer.GetDT();
        float mass = 140.0f;
        if(camForce.y < 20) camForce += vec3(0, -9.81 / mass, 0);
        camVelocity += camForce * mass * dt;
        camPosition += camVelocity * dt; */
    } else {
        //camVelocity = vec3(0, 40.0f, 0);
        //camForce = vec3(0,0,0);
    }


}

void Camera::CamUpdate() {
    CamUpdateMovement();
    CamLook();
    //bColliding = false;

}
