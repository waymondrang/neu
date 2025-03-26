#include "application.h"

#include <cassert>
#include <string>

#include "core.h"
#include "imgui.h"
#include "imgui_helper.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "input_manager.h"
#include "math/Matrix.h"
#include "math/Vector.h"
#include "render/renderer.h"
#include "scene/scene.h"
#include "shader_manager.h"
#include "utility/profiler.h"
#include "window_manager.h"

using std::string;
using namespace Neu;

const int width = 1500;
const int height = 800;

Application* Application::instance;

Application::Application(int argc, char** argv) : argc(argc), argv(argv) {}

void Application::setup() {
  assert(this == instance);

  windowManager = new WindowManager(this);
  windowManager->initialize(width, height, "neu (in-development)");

  ///////////////////
  // INPUT MANAGER //
  ///////////////////

  inputManager = new InputManager(windowManager);

  /////////////////
  // KEY ACTIONS //
  /////////////////

  inputManager->registerKeyAction("CAMERA_FORWARD", Inputs::W);
  inputManager->registerKeyAction("CAMERA_BACKWARD", Inputs::S);
  inputManager->registerKeyAction("CAMERA_LEFT", Inputs::A);
  inputManager->registerKeyAction("CAMERA_RIGHT", Inputs::D);
  inputManager->registerKeyAction("CAMERA_UP", Inputs::SPACE);
  inputManager->registerKeyAction("CAMERA_DOWN", Inputs::LEFT_SHIFT);
  inputManager->registerKeyAction("CAMERA_SPEEDUP", Inputs::LEFT_CONTROL);
  inputManager->registerKeyAction("CLOSE_WINDOW", Inputs::ESCAPE);
  inputManager->registerKeyAction("TOGGLE_WIREFRAME", Inputs::E);
  inputManager->registerKeyAction("TOGGLE_CURSOR", Inputs::Q);

  inputManager->registerKeyAction("CHARACTER_FORWARD", Inputs::I);
  inputManager->registerKeyAction("CHARACTER_BACKWARD", Inputs::K);
  inputManager->registerKeyAction("CHARACTER_LEFT", Inputs::J);
  inputManager->registerKeyAction("CHARACTER_RIGHT", Inputs::L);

  ///////////////////
  // MOUSE ACTIONS //
  ///////////////////

  inputManager->registerMouseAction("TOGGLE_CURSOR", Inputs::MOUSE_MIDDLE);

  //////////////////
  // SET UP SCENE //
  //////////////////

  scene = new Scene();

  /////////////////////
  // SET UP RENDERER //
  /////////////////////

  renderer = new Renderer();

  /////////////
  // SHADERS //
  /////////////

#ifdef __APPLE__
  const string shaderBasePath = "../assets/shaders/";
#else
  const string shaderBasePath = "../assets/shaders/";
#endif

  const string simpleVertexShaderPath = shaderBasePath + "simple.vert";
  const string simpleFragmentShaderPath = shaderBasePath + "simple.frag";
  const string lightVertexShaderPath = shaderBasePath + "light.vert";
  const string lightFragmentShaderPath = shaderBasePath + "light.frag";
  const string depthVertexShaderPath = shaderBasePath + "depth.vert";
  const string depthFragmentShaderPath = shaderBasePath + "depth.frag";
  const string shadowVertexShaderPath = shaderBasePath + "shadow.vert";
  const string shadowFragmentShaderPath = shaderBasePath + "shadow.frag";
  const string particleVertexShaderPath = shaderBasePath + "particle.vert";
  const string particleFragmentShaderPath = shaderBasePath + "particle.frag";

  // FOR LINE DRAWING
  const string lineVertexShaderPath = shaderBasePath + "line.vert";
  const string lineFragmentShaderPath = shaderBasePath + "line.frag";

  shaderManager = new ShaderManager();
  shaderManager->registerShader("simple", simpleVertexShaderPath,
                                simpleFragmentShaderPath);
  shaderManager->registerShader("light", lightVertexShaderPath,
                                lightFragmentShaderPath);
  shaderManager->registerShader("depth", depthVertexShaderPath,
                                depthFragmentShaderPath);
  shaderManager->registerShader("shadow", shadowVertexShaderPath,
                                shadowFragmentShaderPath);
  shaderManager->registerShader("particle", particleVertexShaderPath,
                                particleFragmentShaderPath);
  shaderManager->registerShader("line", lineVertexShaderPath,
                                lineFragmentShaderPath);

  shaderManager->getShader("simple")->bindUniformBlock("camera", 0);
  shaderManager->getShader("light")->bindUniformBlock("camera", 0);
  shaderManager->getShader("shadow")->bindUniformBlock("camera", 0);
  shaderManager->getShader("particle")->bindUniformBlock("camera", 0);
  shaderManager->getShader("line")->bindUniformBlock("camera", 0);

  shaderManager->getShader("light")->bindUniformBlock("light", 1);
  // shaderManager->getShader("shadow")->bindUniformBlock("light", 1);

  shaderManager->createUniformBuffer("camera", 2 * sizeof(Matrix4x4f), 0);
  shaderManager->createUniformBuffer("light", sizeof(LightBuffer), 1);

  shaderManager->getShader("shadow")->setInteger("shadowMap", 0);

  // todo: dynamically compile shader with number of lights (recompile shader
  // when adding/removing light)

  ///////////////////////
  // IMGUIHELPER SETUP //
  ///////////////////////

  imGuiHelper = new ImGuiHelper();
  imGuiHelper->setup();
}

/**
 * Starts the game loop. Is only called once.
 */
void Application::start() {
  // ImGuiHelper::initialize((GLFWwindow *)windowManager->getNativeWindow());

  const float fps = 60.0f;
  const float invFps = 1.0f / fps;
  float lastFrameTime = 0.0f;

  while (!windowManager->windowShouldClose()) {
    // THROTTLE GAME LOOP
    float currentTime = static_cast<float>(glfwGetTime());
    // if (currentTime - lastFrameTime < invFps)
    //     continue;

    Neu::Profiler::getInstance().reset();

    NEU_PROFILE_START;

    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    windowManager->startFrame();
    imGuiHelper->startFrame();

    // debug:
    // deltaTime = 0.016f; // 60 fps

    // DEBUGGING UPDATE (PROBLEM: MUST CLEAR COLOR AND COLOR BUFFER BITS)
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    update(deltaTime);

    render();

    NEU_PROFILE_STOP;

    imGuiHelper->show();

    imGuiHelper->endFrame();
    windowManager->endFrame();
  }
}

/**
 * This function is called every frame to update the engine context.
 */
void Application::update(float deltaTime) {
  // PROFILE_START;

  wallTime += deltaTime;

  // either keep here or move to renderer (because it is fixed functionality)
  if (inputManager->isKeyActionJustPressed("TOGGLE_WIREFRAME"))
    renderer->toggleWireframe();

  if (inputManager->isMouseActionJustPressed("TOGGLE_CURSOR") ||
      inputManager->isKeyActionJustPressed("TOGGLE_CURSOR"))
    windowManager->toggleCursor();

  if (inputManager->isKeyActionJustPressed("CLOSE_WINDOW"))
    windowManager->closeWindow();

  scene->update(deltaTime);

  // UPDATE LIGHTS

  // LightBuffer lightBuffer;
  // lightBuffer.num_lights = 0;

  // for (Light* light : scene->getLights())
  //{
  //	if (lightBuffer.num_lights >= MAX_LIGHTS)
  //	{
  //		Logger::warn("Maximum number of lights reached\n");
  //		break;
  //	}

  //	lightBuffer.lights[lightBuffer.num_lights].light_position =
  //Vector4f(light->getPosition(), 1.0f);
  //	lightBuffer.lights[lightBuffer.num_lights].light_color =
  //Vector4f(light->getColor(), 1.0f); 	lightBuffer.num_lights++;
  //}

  // shaderManager->updateUniformBuffer("light", 0, sizeof(LightBuffer),
  // &lightBuffer);

  // PROFILE_STOP;
}

/**
 * Renders the scene.
 */
void Application::render() {
  renderer->renderScene(scene);
  // world->draw(&defaultShader, &depthShader, &lightShader);
}

/**
 * Terminates the engine and frees resources.
 */
void Application::terminate() {
  // shaderManager.terminate();
  // windowManager.terminate();

  ////////////////////
  // IMGUI SHUTDOWN //
  ////////////////////

  imGuiHelper->shutdown();

  //////////
  // GLFW //
  //////////

  glfwTerminate();
}

Application* Application::createInstance(int argc, char** argv) {
  assert(instance == nullptr);
  instance = new Application(
      argc, argv);  // todo: problem is instance isnt set when application
                    // constructor is called... need to separate application
                    // setup into its own function.
  assert(instance != nullptr);
  return instance;
}

Application* Application::getInstance() {
  assert(instance != nullptr);
  return instance;
}
