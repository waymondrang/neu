/*
 * NOTE: THE ACTIONS AND DATA WITHIN THIS FILE WILL BE MOVED INTO A "TEMPLATED"
 * ENTITY
 */

#include "fps_camera.h"

#include "application.h"
#include "core.h"
#include "input_manager.h"
#include "window_manager.h"

#define MAX_PITCH 1.5533430343f

using namespace Neu;

FPSCamera::FPSCamera() {}

FPSCamera::FPSCamera(const Vector3f& position, const Vector3f& target,
                     const Vector3f& up)
    : Camera(position, target, up) {
  s = 1.0f;
}

void FPSCamera::handleMouse() {
  NEU_PROFILE_FUNCTION;

  Application* application = Application::getInstance();

  if (!application->getWindowManager()->isCursorDisabled()) {
    firstMouse = true;
    return;
  }

  float xpos = application->getWindowManager()->getMouseX();
  float ypos = application->getWindowManager()->getMouseY();

  // todo: camera shouldnt need to to track whether the mouse is the first
  // mouse, decouple into window manager
  if (firstMouse)  // application->getWindowManager()->isFirstMouse()
  {
    lastX = (float)xpos;
    lastY = (float)ypos;

    firstMouse = false;
  }

  float xOffset = (float)(xpos - lastX);
  float yOffset = (float)(ypos - lastY);

  lastX = (float)xpos;
  lastY = (float)ypos;

  const float sensitivity = 0.1f;

  xOffset *= sensitivity;
  yOffset *= sensitivity;

  /**
   * Instead of calling the radians function, the conversion
   * can be done inline. 0.0174532925199f is pi / 180.
   */

  xOffset = xOffset * 0.0174532925199f;
  yOffset = yOffset * 0.0174532925199f;

  yaw += xOffset;
  pitch += yOffset;

  yaw = fmod(yaw, 6.28318530718f);

  if (pitch > MAX_PITCH) pitch = MAX_PITCH;
  if (pitch < -MAX_PITCH) pitch = -MAX_PITCH;
}

// honestly could delegate this into another class
void FPSCamera::handleKeyboard(float deltaTime) {
  NEU_PROFILE_FUNCTION;

  Application* application = Application::getInstance();

  if (!application->getWindowManager()->isCursorDisabled()) return;

  const float cameraSpeed = 5.0f * deltaTime;
  float cameraSpeedup = 1.0f;

  if (application->getInputManager()->isKeyActionPressed("CAMERA_SPEEDUP"))
    cameraSpeedup = 10.0f;

  if (application->getInputManager()->isKeyActionPressed("CAMERA_FORWARD"))
    position -= direction * cameraSpeed * cameraSpeedup;

  if (application->getInputManager()->isKeyActionPressed("CAMERA_BACKWARD"))
    position += direction * cameraSpeed * cameraSpeedup;

  if (application->getInputManager()->isKeyActionPressed("CAMERA_RIGHT"))
    position += right * cameraSpeed * cameraSpeedup;

  if (application->getInputManager()->isKeyActionPressed("CAMERA_LEFT"))
    position -= right * cameraSpeed * cameraSpeedup;

  if (application->getInputManager()->isKeyActionPressed("CAMERA_UP"))
    position += globalUp * cameraSpeed * cameraSpeedup;

  if (application->getInputManager()->isKeyActionPressed("CAMERA_DOWN"))
    position -= globalUp * cameraSpeed * cameraSpeedup;
}

void FPSCamera::update(float deltaTime) {
  NEU_PROFILE_FUNCTION;

  handleMouse();
  handleKeyboard(deltaTime);

  updateViewMatrix();
}