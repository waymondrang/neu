#pragma once

#include <vector>

#include "fps_camera.h"
#include "systems/physics_system.h"
#include "systems/transform.h"

namespace Neu {

class GameObject;
class Application;

class Scene {
 public:
  Scene();

  void update(float deltaTime);

  inline std::vector<GameObject*> getGameObjects() const { return gameObjects; }
  // inline std::vector<Light*> getLights() const { return lights; }
  inline Vector3f getBackgroundColor() const { return backgroundColor; }
  inline FPSCamera& getCamera() { return camera; }

 private:
  Application* application;

  /////////////
  // SYSTEMS //
  /////////////

  Neu::PhysicsSystem physicsSystem;
  TransformSystem transformSystem;

  FPSCamera camera;
  std::vector<GameObject*> gameObjects;
  // std::vector<Light*> lights;

  Vector3f backgroundColor;  // maybe move this to renderer
};

}  // namespace Neu