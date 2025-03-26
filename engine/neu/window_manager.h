#pragma once

#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "core.h"
#include "inputs.h"
#include "math/Vector.h"

struct GLFWwindow;

namespace Neu {

class Application;

class WindowManager {
 public:
  /**
   * Initializes the window manager. Creates a window with the default width and
   * height.
   */
  WindowManager(Application* context);

  void initialize(int width, int height, std::string title);

  /**
   * Returns the time between the current frame and the last frame.
   * This method should be called at the beginning of each game loop
   * iteration.
   */
  float getDeltaTime();

  /**
   * Performs pre-frame operations. It should
   * be called at the beginning of each game loop iteration.
   */
  void startFrame();

  /**
   * Performs post-frame operations. It should
   * be called at the end of each game loop iteration.
   */
  void endFrame();

  /**
   * Abstraction of glfwWindowShouldClose.
   */
  void closeWindow();

  /**
   * Returns true if the window should close, false otherwise.
   */
  int windowShouldClose() const;

  /**
   * Registers a framebuffer size callback.
   * The callback should have the following signature:
   * void callback(int width, int height)
   */
  void registerFramebufferSizeCallback(std::function<void(int, int)> callback);

  /**
   * Notifies all registered framebuffer size callbacks.
   */
  void notifyFramebufferSizeCallbacks(int width, int height);

  /**
   * Toggles the cursor between normal and disabled states.
   *
   * @return True if the cursor is disabled, false otherwise.
   */
  bool toggleCursor();

  /**
   * Returns true if the key is pressed this frame.
   */
  bool isKeyPressed(int key) const;

  /**
   * Returns true if the key is repeated this frame. In other words, the key was
   * pressed in the previous frame and is still being pressed.
   */
  bool isKeyRepeated(int key) const;

  /**
   * Returns true if the mouse button is pressed this frame.
   */
  bool isMouseButtonPressed(int button) const;

  /**
   * Returns true if the mouse button is repeated this frame. In other words,
   * the mouse button was pressed in the previous frame and is still being
   * pressed.
   */
  bool isMouseButtonRepeated(int button) const;

  /**
   * Queries and returns the mouse position.
   */
  Vector2f getMousePosition() const;

  /**
   * Returns the native window handle.
   */
  GLFWwindow* getNativeWindow() const { return window; }

  /**
   * Returns the x position of the mouse.
   */
  float getMouseX() const { return mouseX; }

  /**
   * Returns the y position of the mouse.
   */
  float getMouseY() const { return mouseY; }

  /**
   * Returns the width of the window.
   */
  int getWidth() const { return width; }

  /**
   * Returns the height of the window.
   */
  int getHeight() const { return height; }

  /**
   * Returns the resolution of the window.
   */
  Vector2i getResolution() const { return Vector2i(width, height); }

  /**
   * Returns whether the cursor is disabled.
   */
  bool isCursorDisabled() const { return cursorDisabled; }

 private:
  GLFWwindow* window;
  Application* context;

  /**
   * lastFrame is calculated in WindowManager because it relies on the
   * glfwGetTime function.
   */
  float lastFrame = 0.0f;

  /**
   * The width (in pixels) of the window.
   */
  int width;

  /**
   * The height (in pixels) of the window.
   */
  int height;

  /**
   * The x position of the mouse.
   */
  float mouseX;

  /**
   * The y position of the mouse.
   */
  float mouseY;

  /**
   * True if the cursor is disabled, false otherwise.
   */
  bool cursorDisabled = false;

  std::vector<std::function<void(int, int)>> framebufferSizeCallbacks;
  std::vector<std::function<void(int, int)>> keyCallbacks;

  static void framebufferSizeCallback(GLFWwindow* window, int width,
                                      int height);

  /**
   * Called every frame after glfwPollEvents to update the input states.
   */
  void updateInputStates();

  // Optimization idea: use a bitset instead of a map
  std::map<int, bool> keyPressed;
  std::map<int, bool> keyRepeated;
  std::map<int, bool> mouseButtonPressed;
  std::map<int, bool> mouseButtonRepeated;

  /**
   * Defines the explicit mapping between GLFW key codes and the engine's key
   * codes.
   */
  const std::unordered_map<int, int> keyboardMap = {
      {GLFW_KEY_A, Inputs::A},
      {GLFW_KEY_B, Inputs::B},
      {GLFW_KEY_C, Inputs::C},
      {GLFW_KEY_D, Inputs::D},
      {GLFW_KEY_E, Inputs::E},
      {GLFW_KEY_F, Inputs::F},
      {GLFW_KEY_G, Inputs::G},
      {GLFW_KEY_H, Inputs::H},
      {GLFW_KEY_I, Inputs::I},
      {GLFW_KEY_J, Inputs::J},
      {GLFW_KEY_K, Inputs::K},
      {GLFW_KEY_L, Inputs::L},
      {GLFW_KEY_M, Inputs::M},
      {GLFW_KEY_N, Inputs::N},
      {GLFW_KEY_O, Inputs::O},
      {GLFW_KEY_P, Inputs::P},
      {GLFW_KEY_Q, Inputs::Q},
      {GLFW_KEY_R, Inputs::R},
      {GLFW_KEY_S, Inputs::S},
      {GLFW_KEY_T, Inputs::T},
      {GLFW_KEY_U, Inputs::U},
      {GLFW_KEY_V, Inputs::V},
      {GLFW_KEY_W, Inputs::W},
      {GLFW_KEY_X, Inputs::X},
      {GLFW_KEY_Y, Inputs::Y},
      {GLFW_KEY_Z, Inputs::Z},
      {GLFW_KEY_0, Inputs::ZERO},
      {GLFW_KEY_1, Inputs::ONE},
      {GLFW_KEY_2, Inputs::TWO},
      {GLFW_KEY_3, Inputs::THREE},
      {GLFW_KEY_4, Inputs::FOUR},
      {GLFW_KEY_5, Inputs::FIVE},
      {GLFW_KEY_6, Inputs::SIX},
      {GLFW_KEY_7, Inputs::SEVEN},
      {GLFW_KEY_8, Inputs::EIGHT},
      {GLFW_KEY_9, Inputs::NINE},
      {GLFW_KEY_SPACE, Inputs::SPACE},
      {GLFW_KEY_LEFT_SHIFT, Inputs::LEFT_SHIFT},
      {GLFW_KEY_LEFT_CONTROL, Inputs::LEFT_CONTROL},
      {GLFW_KEY_ESCAPE, Inputs::ESCAPE}};

  const std::unordered_map<int, int> mouseButtonMap = {
      {GLFW_MOUSE_BUTTON_LEFT, Inputs::MOUSE_LEFT},
      {GLFW_MOUSE_BUTTON_MIDDLE, Inputs::MOUSE_MIDDLE},
      {GLFW_MOUSE_BUTTON_RIGHT, Inputs::MOUSE_RIGHT}};
};

};  // namespace Neu