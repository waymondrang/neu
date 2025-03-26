#pragma once

namespace Neu {

class WindowManager;
class InputManager;
class ShaderManager;  // todo: rename to shaderregistry
class ImGuiHelper;

class Scene;
class Renderer;

class Application {
 public:
  Application(int argc, char** argv);

  void setup();
  void start();
  void update(float deltaTime);
  void render();
  void terminate();

  static Application* createInstance(int argc, char** argv);
  static Application* getInstance();

  Scene* getWorld() const { return scene; }
  float getWallTime() const { return wallTime; }

  InputManager* getInputManager() const { return inputManager; }
  WindowManager* getWindowManager() const { return windowManager; }
  ShaderManager* getShaderManager() const { return shaderManager; }

  int getArgc() const { return argc; }
  char** getArgv() const { return argv; }

 private:
  int argc;
  char** argv;

  float wallTime = 0.0f;

  Scene* scene;
  Renderer* renderer;

  WindowManager* windowManager;
  InputManager* inputManager;
  ShaderManager* shaderManager;

  ImGuiHelper* imGuiHelper;

  static Application* instance;
};

}  // namespace Neu