#ifndef FPS_CAMERA_H
#define FPS_CAMERA_H

#include "Camera.h"

class Application;

class FPSCamera : public Camera
{
protected:
    void handleCursorInput();
    void handleKeyboardInput(float deltaTime);

    bool firstMouse = true;
    float lastX;
    float lastY;

public:
    FPSCamera(const Vector3f &position, const Vector3f &target, const Vector3f &up);

    /**
     * Updates the camera's position and orientation based
     * on the input from the user.
     */
    void update(float deltaTime) override;
};

#endif
