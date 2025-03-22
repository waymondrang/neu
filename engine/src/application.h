#pragma once

//////////////////////////
// FORWARD DECLARATIONS //
//////////////////////////

class WindowManager;
class InputManager;
class ShaderManager;
class ImGuiHelper;

class World;
class Camera;
class FrameBuffer;

class Application
{
public:
	Application(int argc, char** argv);
	
	void setup();
	void start();
	void update(float deltaTime);
	void render();
	void terminate();

	static Application* createInstance(int argc, char** argv);
	static Application* getInstance();

	////////////////////////////////
	// Inline Getters and Setters //
	////////////////////////////////

	World* getWorld() const { return world; }
	Camera* getCamera() const { return camera; }
	float getWallTime() const { return wallTime; }

	InputManager* getInputManager() const { return inputManager; }
	WindowManager* getWindowManager() const { return windowManager; }
	ShaderManager* getShaderManager() const { return shaderManager; }

	int getArgc() const { return argc; }
	char** getArgv() const { return argv; }

	void window_size_callback(int width, int height);

private:
	int argc;
	char** argv;

	float wallTime = 0.0f;

	World* world;
	Camera* camera;

	WindowManager* windowManager;
	InputManager* inputManager;
	ShaderManager* shaderManager;

	ImGuiHelper* imGuiHelper;

	FrameBuffer* sceneBuffer;

	static Application* instance;
};