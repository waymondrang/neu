#pragma once

#include "camera.h"

namespace Neu {

class FPSCamera : public Camera {
 public:
  FPSCamera(const Vector3f& position, const Vector3f& target,
            const Vector3f& up);

  /**
   * Updates the camera's position and orientation based
   * on the input from the user.
   */
  void update(float deltaTime);
  FPSCamera();

 protected:
  bool firstMouse = true;
  float lastX = 0.0f;
  float lastY = 0.0f;

  void handleMouse();
  void handleKeyboard(float deltaTime);
};

}  // namespace Neu