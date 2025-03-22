#include "application.h"

#include "core.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "WindowManager.h"
#include "InputManager.h"
#include "ShaderManager.h"

#include "World.h"
#include "camera/FPSCamera.h"
#include "Shader.h"

#include "math/Matrix.h"
#include "math/Vector.h"

#include "render/framebuffer.h"

#include "imgui_helper.h"

#include <cassert>

using std::string;

const int width = 1500;
const int height = 800;

Application* Application::instance;

Application::Application(int argc, char** argv)
	: argc(argc), argv(argv)
{}

void Application::setup() {
	assert(this == instance);

	windowManager = new WindowManager(this);
	windowManager->initialize();
	windowManager->createWindow(width, height, "neu (in-development)");

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

	///////////////////
	// MOUSE ACTIONS //
	///////////////////

	inputManager->registerMouseAction("TOGGLE_CURSOR", Inputs::MOUSE_MIDDLE);

	///////////////////
	// SET UP CAMERA //
	///////////////////

	Vector3f cameraPosition = Vector3f(1.4f, 2.6f, -3.1f);
	Vector3f cameraTarget = Vector3f(0.0f, 0.0f, 0.0f);
	Vector3f cameraUp = Vector3f(0.0f, 1.0f, 0.0f);

	camera = new FPSCamera(cameraPosition, cameraTarget, cameraUp);
	camera->setPitch(0.1f);
	camera->setYaw(0.18f);
	camera->setResolution(width, height);

	world = new World(this);
	world->setCamera(camera); // why does world need a camera? world only describes the objects in the scene, camera and world should be linked to renderer

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

	shaderManager = new ShaderManager();
	shaderManager->registerShader("simple", simpleVertexShaderPath, simpleFragmentShaderPath);
	shaderManager->registerShader("light", lightVertexShaderPath, lightFragmentShaderPath);
	shaderManager->registerShader("depth", depthVertexShaderPath, depthFragmentShaderPath);
	shaderManager->registerShader("shadow", shadowVertexShaderPath, shadowFragmentShaderPath);
	shaderManager->registerShader("particle", particleVertexShaderPath, particleFragmentShaderPath);

	shaderManager->getShader("simple")->bindUniformBlock("camera", 0);
	shaderManager->getShader("light")->bindUniformBlock("camera", 0);
	shaderManager->getShader("shadow")->bindUniformBlock("camera", 0);
	shaderManager->getShader("particle")->bindUniformBlock("camera", 0);

	shaderManager->getShader("light")->bindUniformBlock("light", 1);
	// shaderManager->getShader("shadow")->bindUniformBlock("light", 1);

	shaderManager->createUniformBuffer("camera", 2 * sizeof(Matrix4x4f), 0);
	shaderManager->createUniformBuffer("light", sizeof(LightBuffer), 1);

	shaderManager->getShader("shadow")->setInteger("shadowMap", 0);

	// todo: dynamically compile shader with number of lights (recompile shader when adding/removing light)

	//////////////////////////////
	// SET UP SCENE FRAMEBUFFER //
	//////////////////////////////

	sceneBuffer = new FrameBuffer(windowManager->getWidth(), windowManager->getHeight());

	//windowManager->registerFramebufferSizeCallback([this](int width, int height) {
	//	Logger::debug("RESIZING SCENE BUFFER");
	//	sceneBuffer->RescaleFrameBuffer(width, height);
	//	});

	///////////////////////
	// IMGUIHELPER SETUP //
	///////////////////////

	imGuiHelper = new ImGuiHelper();
	imGuiHelper->setup();
}


/**
 * Starts the game loop. Is only called once.
 */
void Application::start()
{
	//ImGuiHelper::initialize((GLFWwindow *)windowManager->getNativeWindow());

	const float fps = 60.0f;
	const float invFps = 1.0f / fps;
	float lastFrameTime = 0.0f;

	while (!windowManager->windowShouldClose())
	{
		// THROTTLE GAME LOOP
		float currentTime = static_cast<float>(glfwGetTime());
		// if (currentTime - lastFrameTime < invFps)
		//     continue;

		Profiler::getInstance().reset();
		PROFILE_START;

		float deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		windowManager->startFrame();
		imGuiHelper->startFrame();
		imGuiHelper->showGamePropertyEditor();

		// debug:
		// deltaTime = 0.016f; // 60 fps

		update(deltaTime);

		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);

		//sceneBuffer->Bind();
		render();
		//sceneBuffer->Unbind();


		PROFILE_STOP;

		imGuiHelper->endFrame();
		windowManager->endFrame();

	}
}

/**
 * This function is called every frame to update the engine context.
 */
void Application::update(float deltaTime)
{
	PROFILE_START;

	wallTime += deltaTime;

	if (inputManager->isMouseActionJustPressed("TOGGLE_CURSOR") || inputManager->isKeyActionJustPressed("TOGGLE_CURSOR"))
		windowManager->toggleCursor();

	if (inputManager->isKeyActionJustPressed("CLOSE_WINDOW"))
		windowManager->closeWindow();

	camera->update(deltaTime);
	world->update(deltaTime);

	shaderManager->updateUniformBuffer("camera", 0, sizeof(Matrix4x4f), camera->getViewMatrix().ptr());
	shaderManager->updateUniformBuffer("camera", sizeof(Matrix4x4f), sizeof(Matrix4x4f), camera->getProjectionMatrix());

	// UPDATE LIGHTS

	LightBuffer lightBuffer;
	lightBuffer.num_lights = 0;

	for (Light* light : world->getLights())
	{
		if (lightBuffer.num_lights >= MAX_LIGHTS)
		{
			Logger::warn("Maximum number of lights reached\n");
			break;
		}

		lightBuffer.lights[lightBuffer.num_lights].light_position = Vector4f(light->getPosition(), 1.0f);
		lightBuffer.lights[lightBuffer.num_lights].light_color = Vector4f(light->getColor(), 1.0f);
		lightBuffer.num_lights++;
	}

	shaderManager->updateUniformBuffer("light", 0, sizeof(LightBuffer), &lightBuffer);

	PROFILE_STOP;
}

/**
 * Renders the scene.
 */
void Application::render()
{
	world->draw();
	// world->draw(&defaultShader, &depthShader, &lightShader);
}

/**
 * Terminates the engine and frees resources.
 */
void Application::terminate()
{
	//shaderManager.terminate();
	//windowManager.terminate();

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
	instance = new Application(argc, argv); // todo: problem is instance isnt set when application constructor is called... need to separate application setup into its own function.
	assert(instance != nullptr);
	return instance;
}

Application* Application::getInstance() {
	assert(instance != nullptr);
	return instance;
}

