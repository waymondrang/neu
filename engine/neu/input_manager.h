#pragma once

#include <map>
#include <unordered_map>

#include "inputs.h"
#include "window_manager.h"

namespace Neu {

class InputManager {
 protected:
  WindowManager* windowManager;

  /**
   * Maps actions to mouse buttons.
   */
  std::unordered_map<std::string, int> actionMouseMap;

  /**
   * Maps actions to keys.
   */
  std::unordered_map<std::string, int> actionKeyMap;

 public:
  /**
   * Explicit InputManager constructor.
   *
   * @param windowManager The window manager.
   */
  InputManager(WindowManager* windowManager) : windowManager(windowManager) {}

  /**
   * Adds an action mapping.
   */
  void registerKeyAction(std::string action, Inputs::Keys key);

  /**
   * Adds a mouse action mapping.
   */
  void registerMouseAction(std::string action, Inputs::MouseButtons button);

  /**
   * Queries whether the key action was just pressed.
   */
  bool isKeyActionJustPressed(std::string action) const;

  /**
   * Queries whether the key action is pressed.
   */
  bool isKeyActionPressed(std::string action) const;

  /**
   * Queries whether the key action is repeated.
   */
  bool isKeyActionRepeated(std::string action) const;

  /**
   * Queries whether the mouse action was just pressed.
   */
  bool isMouseActionJustPressed(std::string action) const;

  /**
   * Queries whether the mouse action is pressed.
   */
  bool isMouseActionPressed(std::string action) const;

  /**
   * Queries whether the mouse action is repeated.
   */
  bool isMouseActionRepeated(std::string action) const;
};

};  // namespace Neu