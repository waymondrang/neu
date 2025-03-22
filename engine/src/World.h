#ifndef WORLD_H
#define WORLD_H

#include "Mesh.h"
#include "Light.h"
#include "camera/Camera.h"
#include "GameObject.h"
#include "Animation.h"

#include <vector>
#include <tuple>

class Application;

class World
{
public:
    Application *game;
    Camera *camera;

    World(Application *game);

    void setCamera(Camera *camera);
    // void draw(Shader *defaultShader,
    //           Shader *depthShader,
    //           Shader *lightShader);
    void draw();
    void drawWithShadow();
    void update(float deltaTime);
    void toggleWireframe();

    // getters
    std::vector<GameObject *> getGameObjects() const { return gameObjects; }
    std::vector<Light *> getLights() const { return lights; }
    Vector3f getBackgroundColor() const { return backgroundColor; }

private:
    bool wireframeMode = false;

    // FOR ANIMATIONS
    Animation *animation = nullptr;
    float globalAnimationDuration = 0.0f;

    std::vector<GameObject *> gameObjects;
    std::vector<Light *> lights;
    Vector3f backgroundColor;

    void render(ShaderManager *shaderManager);
    void renderWithShader(Shader *shader);
};

#endif