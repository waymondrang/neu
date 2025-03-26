#include "input_manager.h"

using namespace Neu;

void InputManager::registerKeyAction(std::string action, Inputs::Keys key) {
  actionKeyMap[action] = key;
}

void InputManager::registerMouseAction(std::string action,
                                       Inputs::MouseButtons button) {
  actionMouseMap[action] = button;
}

bool InputManager::isKeyActionJustPressed(std::string action) const {
  auto it = actionKeyMap.find(action);
  if (it == actionKeyMap.end()) return false;

  return windowManager->isKeyPressed(it->second) &&
         !windowManager->isKeyRepeated(it->second);
}

bool InputManager::isKeyActionPressed(std::string action) const {
  auto it = actionKeyMap.find(action);
  if (it == actionKeyMap.end()) return false;

  return windowManager->isKeyPressed(it->second);
}

bool InputManager::isKeyActionRepeated(std::string action) const {
  auto it = actionKeyMap.find(action);
  if (it == actionKeyMap.end()) return false;

  return windowManager->isKeyRepeated(it->second);
}

bool InputManager::isMouseActionJustPressed(std::string action) const {
  auto it = actionMouseMap.find(action);
  if (it == actionMouseMap.end()) return false;

  return windowManager->isMouseButtonPressed(it->second) &&
         !windowManager->isMouseButtonRepeated(it->second);
}

bool InputManager::isMouseActionPressed(std::string action) const {
  auto it = actionMouseMap.find(action);
  if (it == actionMouseMap.end()) return false;

  return windowManager->isMouseButtonPressed(it->second);
}

bool InputManager::isMouseActionRepeated(std::string action) const {
  auto it = actionMouseMap.find(action);
  if (it == actionMouseMap.end()) return false;

  return windowManager->isMouseButtonRepeated(it->second);
}