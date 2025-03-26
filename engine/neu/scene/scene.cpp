#include "scene.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <json/json.hpp>
#include <random>

#include "application.h"
#include "components/mesh.h"
#include "components/rigid_body.h"
#include "components/transform.h"
#include "game_object.h"
#include "shader_manager.h"
#include "utility/profiler.h"
#include "window_manager.h"

using json = nlohmann::json;
using namespace Neu;

Scene::Scene() {
  backgroundColor = Vector3f(0.1f, 0.1f, 0.1f);

  ////////////////////
  // SET UP SYSTEMS //
  ////////////////////

  physicsSystem.setup();

  ////////////////////
  // DEBUG ENTITIES //
  ////////////////////

  std::vector<Vector3f> vertices = {
      Vector3f(-1.0f, 1.0f, -1.0f),  Vector3f(1.0f, 1.0f, 1.0f),
      Vector3f(1.0f, 1.0f, -1.0f),   Vector3f(-1.0f, -1.0f, 1.0f),
      Vector3f(1.0f, -1.0f, 1.0f),   Vector3f(-1.0f, 1.0f, 1.0f),
      Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(-1.0f, -1.0f, 1.0f),
      Vector3f(1.0f, -1.0f, -1.0f),  Vector3f(-1.0f, -1.0f, 1.0f),
      Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(-1.0f, 1.0f, -1.0f),
      Vector3f(-1.0f, 1.0f, 1.0f),   Vector3f(-1.0f, 1.0f, 1.0f),
  };

  std::vector<Vector3f> normals = {
      Vector3f(-0.57732844, 0.57736117, -0.57736117),
      Vector3f(0.57732844, 0.57736117, 0.57736117),
      Vector3f(0.5773721, 0.57733935, -0.57733935),
      Vector3f(-0.57732844, -0.57736117, 0.57736117),
      Vector3f(0.5773721, -0.57733935, 0.57733935),
      Vector3f(-0.5773721, 0.57733935, 0.57733935),
      Vector3f(-0.5773721, -0.57733935, -0.57733935),
      Vector3f(-0.57732844, -0.57736117, 0.57736117),
      Vector3f(0.57732844, -0.57736117, -0.57736117),
      Vector3f(-0.57732844, -0.57736117, 0.57736117),
      Vector3f(-0.5773721, -0.57733935, -0.57733935),
      Vector3f(-0.57732844, 0.57736117, -0.57736117),
      Vector3f(-0.5773721, 0.57733935, 0.57733935),
      Vector3f(-0.5773721, 0.57733935, 0.57733935),
  };

  std::vector<Vector3i> faces = {
      Vector3i(0, 1, 2),  Vector3i(1, 3, 4),  Vector3i(5, 6, 7),
      Vector3i(8, 9, 10), Vector3i(2, 4, 8),  Vector3i(11, 8, 6),
      Vector3i(0, 12, 1), Vector3i(1, 13, 3), Vector3i(5, 11, 6),
      Vector3i(8, 4, 9),  Vector3i(2, 1, 4),  Vector3i(11, 2, 8),
  };

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> uniformPositiveDistribution(0.0f, 1.0f);

  for (int i = 0; i < 100; i++) {
    GameObject* cube = new GameObject();

    Vector4f randomParticleColor = Vector4f(
        uniformPositiveDistribution(gen), uniformPositiveDistribution(gen),
        uniformPositiveDistribution(gen), uniformPositiveDistribution(gen));

    cube->color = randomParticleColor;

    // cube->setColor(Vector3f(0.5294f, 0.8078f, 0.9216f));
    auto& transform = cube->addComponent<TransformComponent>();
    transform.position = Vector3f(0.0f, 4.0f * i, 0.0f);
    transform.rotation = Vector3f(uniformPositiveDistribution(gen),
                                  uniformPositiveDistribution(gen),
                                  uniformPositiveDistribution(gen)) *
                         2.0f * JPH_PI;

    cube->addComponent<RigidBodyComponent>();

    auto& mesh = cube->addComponent<MeshComponent>();
    mesh.setupVertices(vertices);
    mesh.setupNormals(normals);
    mesh.setupFaces(faces);

    gameObjects.push_back(cube);
    physicsSystem.addBody(cube);
  }

  //////////////////////////////
  // ADD CHARACTER CONTROLLER //
  //////////////////////////////

  GameObject* player = new GameObject();

  player->color = Vector3f(1.0f, 0.0f, 0.0f);

  auto& transform = player->addComponent<TransformComponent>();
  transform.position = Vector3f(0.0f, 4.0f, 0.0f);

  // CONTROLLER PLAYER USING CUSTOM CONTROLLER CLASS

  player->addComponent<RigidBodyComponent>();

  auto& mesh = player->addComponent<MeshComponent>();
  mesh.setupVertices(vertices);
  mesh.setupNormals(normals);
  mesh.setupFaces(faces);

  gameObjects.push_back(player);
  physicsSystem.addController(player);

  // RUN AFTER ADDING BODIES
  physicsSystem.optimize();

  ////////////
  // CAMERA //
  ////////////

  camera.setPosition(Vector3f(0.0f, 5.0f, -10.0f));

  // REGISTER CAMERA FRAMEBUFFER ONCHANGE
  Application::getInstance()
      ->getWindowManager()
      ->registerFramebufferSizeCallback([this](int width, int height) {
        camera.setResolution(Vector2i(width, height));
      });
}

void Scene::update(float deltaTime) {
  NEU_PROFILE_FUNCTION;

  // move to input manager/window manager

  ///////////////////
  // UPDATE CAMERA //
  ///////////////////

  camera.update(deltaTime);

  Application::getInstance()->getShaderManager()->updateUniformBuffer(
      "camera", 0, sizeof(Matrix4x4f), camera.getViewMatrix().ptr());
  Application::getInstance()->getShaderManager()->updateUniformBuffer(
      "camera", sizeof(Matrix4x4f), sizeof(Matrix4x4f),
      camera.getProjectionMatrix());

  ////////////////////
  // UPDATE SYSTEMS //
  ////////////////////

  // for (auto* gameObject : gameObjects)
  //{
  transformSystem.update(gameObjects);
  // controllerSystem.update(gameObjects); // TODO: CONTROLLER SYSTEM IS NOT
  // FUNCTIONAL
  physicsSystem.update(gameObjects, deltaTime);
  //}

  ///////////////////
  // UPDATE LIGHTS //
  ///////////////////

  // for (Light* light : lights)
  //	light->update(deltaTime);

  // PROFILE_STOP;
}