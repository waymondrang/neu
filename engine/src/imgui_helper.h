#pragma once

class Application;
class GameObject;

class GameObjectPropertyEditor {
public:
	void draw();

private:
	GameObject* focusedGameObject = nullptr;
	void drawGameObject(GameObject* gameObject);
};

class ImGuiHelper {
public:
	void setup();

	/**
	 * Starts a new frame for ImGui.
	 */
	void startFrame();

	/**
	 * Ends the current frame for ImGui. Should be called
	 * after all ImGui widgets have been drawn.
	 */
	void endFrame();

	void shutdown();

	void showGamePropertyEditor();
private:
	GameObjectPropertyEditor gameObjectPropertyEditor;
};