/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _CAMERA_H
#define _CAMERA_H

#include "../../network/client.h"

#include "../../window/window.h"
#include "../../math/vector.h"
#include "../../gamecore/xmlevel/xmlevel.h"

#define sprintSpeed 120.0
#define walkSpeed 75.0
#define crouchSpeed 60.0

extern Timer timer;
extern Network netw;

enum {
    FIRST_PERSON = 0,
    THIRD_PERSON = 1,
    FREE_LOOK = 2
};

class Camera {
public:
    Camera();

    ~Camera();

    float camSpeed;
    float camRadius;
    float camMouseSens;
    vec3 camAngle;
    vec3 camPosition;
    vec3 camSize;
    vec3 camDirection;
    float accum, t, dt;

    int camType;
    float thirdPersonCamRadius;
    vec3 camThirdPersonPosition;

    inline void
    Create(vec3 pos, vec3 size, vec3 angle, float sensitivity, float radius, float thirdPersonRadius, int actualType) {
        SetAngle(angle);
        SetPosition(pos);
        SetCameraSize(size);
        SetCameraRadius(radius);
        SetCameraSensitivity(sensitivity);
        thirdPersonCamRadius = thirdPersonRadius;
        SetType(actualType);
        SetCameraFreezed();
    }

    inline void SetType(int type) {
        camType = type;
    }

    inline int GetType(void) {
        return camType;
    }

    inline vec3 GetAngle() {
        return camAngle;
    }

    inline vec3 GetPosition() {
        return camPosition;
    }

    void RotateAroundPoint(vec3 vCenter, float angle, float x, float y, float z);

    inline float GetRadius() {
        return camRadius;
    }

    inline vec3 GetCameraDirection() {
        return camDirection;
    }

    inline vec3 GetCameraSize(void) {
        return camSize;
    }

    inline void SetPosition(vec3 pos) {
        camPosition = pos;
    }

    inline void SetAngle(vec3 angle) {
        camAngle = angle;
    }

    inline void SetCameraSpeed(float speed) {
//             float frameTime = timer.GetDT();
//             if (frameTime > 0.25) frameTime = 0.25;
//             accum += frameTime;
//             while(accum >= dt)
//             {
        camSpeed = speed * timer.GetDT();//dt;
//                  accum -= dt;
//                  t += dt;
//             }

    }

    inline void SetCameraRadius(float radius) {
        camRadius = radius;
    }

    inline void SetCameraSize(vec3 size) {
        camSize = size;
    }

    inline bool IsCameraWalking() {
        return bWalking;
    }

    inline bool IsCameraMoving() {
        return (bMovingUP || bMovingDOWN || bMovingLEFT || bMovingRIGHT) ? true : false;
    }

    inline bool IsCameraStrafingLeft() {
        return bMovingLEFT;
    }

    inline bool IsCameraStrafingRight() {
        return bMovingRIGHT;
    }

    inline bool IsJumping() {
        return bJump;
    }

    inline bool IsCrouched() {
        return bCrouched;
    }

    inline void SetCameraSensitivity(float value) {
        camMouseSens = value;
    }

    inline float GetCameraSensitivity() {
        return camMouseSens;
    }

    inline void SetDefaulCameraSensitivity() {
        camMouseSens = 5.0f;
    }

    inline void SetCameraMovable(void) {
        bCanMove = true;
    }

    inline void SetCameraFreezed(void) {
        bCanMove = false;
    }

    inline bool CanCameraMove(void) {
        return bCanMove;
    }

    void MoveCam(float speed);

    void CamLook();

    void CamForward();

    void CamBackward();

    void CamStrafeLeft();

    void CamStrafeRight();

    void CamUpdateMovement();

    void CamUpdate();

    float sprintTime;
    float walkTime;
    float campJumpRotation;

    bool bCanSprint;
    bool bCanMove;
    bool bCrouched;
    bool bJump;
    bool bMovingUP;
    bool bMovingDOWN;
    bool bMovingLEFT;
    bool bMovingRIGHT;
    bool bCrouch;
    bool bColliding;
    bool bWalking;
    bool bLeap;
    bool c;

    vec3 camVelocity;
    vec3 camForce;
};

#endif
