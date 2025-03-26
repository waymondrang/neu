#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Neu {

class GameObject;

class GameObjectEditor {
 public:
  void draw();

 private:
  GameObject* focusedGameObject = nullptr;
  void drawGameObject(GameObject* gameObject);
};

class ImGuiHelper {
 public:
  ImGuiHelper();

  void setup();

  /**
   * Starts a new frame for ImGui.
   */
  void startFrame();

  void show();

  /**
   * Ends the current frame for ImGui. Should be called
   * after all ImGui widgets have been drawn.
   */
  void endFrame();

  void shutdown();

  void showMenuBar();

  void showEntityEditorWindow();
  void showProfilerWindow();

  void showFramerateWidget();

 private:
  GameObjectEditor entityEditor;

  bool showProfiler = true;
  bool showEntityEditor = false;
};

}  // namespace Neu