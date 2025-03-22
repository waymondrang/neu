#include "World.h"

#include "core.h"

#include <fstream>
#include <json/json.hpp>

#include "InputManager.h"
#include "WindowManager.h"

#include "object/Skeleton.h"
#include "object/Skin.h"
#include "object/Cloth.h"

#include "object/Plane.h"
#include "object/Cube.h"
#include "object/ParticleEmitter.h"

#include "utility/Profiler.h"

#include "application.h"


using json = nlohmann::json;

int findFirstArgumentFileType(int argc, char **argv, const char *fileType)
{
    for (int i = 1; i < argc; i++)
    {
        if (strstr(argv[i], fileType))
        {
            return i;
        }
    }

    return -1;
}

World::World(Application *game)
{
    this->game = game;

    //////////////////
    // IMPORT SCENE //
    //////////////////

    int sceneFileIndex = findFirstArgumentFileType(game->getArgc(), game->getArgv(), ".json");

    if (sceneFileIndex != -1)
    {
        std::ifstream f(game->getArgv()[sceneFileIndex]);
        json data = json::parse(f);

        ///////////////////
        // PARSE OBJECTS //
        ///////////////////

        json::reference jsonGameObjects = data["objects"];

        for (json::reference object : jsonGameObjects)
        {
            if (object.contains("type"))
            {
                if (object["type"] == "plane")
                {
                    Logger::debug("Creating plane game object\n");
                    Plane *plane = new Plane();

                    if (object.contains("name"))
                        plane->setName(object["name"]);

                    if (object.contains("position"))
                        plane->setPosition(Vector3f(object["position"][0], object["position"][1], object["position"][2]));

                    if (object.contains("rotation"))
                        plane->setRotation(Vector3f(object["rotation"][0], object["rotation"][1], object["rotation"][2]));

                    if (object.contains("scale"))
                        plane->setScale(Vector3f(object["scale"][0], object["scale"][1], object["scale"][2]));

                    if (object.contains("color"))
                        plane->setColor(Vector3f(object["color"][0], object["color"][1], object["color"][2]));

                    gameObjects.push_back(plane);
                }
                else if (object["type"] == "cube")
                {
                    Logger::debug("Creating cube game object\n");
                    Cube *cube = new Cube();

                    if (object.contains("name"))
                        cube->setName(object["name"]);

                    if (object.contains("position"))
                        cube->setPosition(Vector3f(object["position"][0], object["position"][1], object["position"][2]));

                    if (object.contains("rotation"))
                        cube->setRotation(Vector3f(object["rotation"][0], object["rotation"][1], object["rotation"][2]));

                    if (object.contains("scale"))
                        cube->setScale(Vector3f(object["scale"][0], object["scale"][1], object["scale"][2]));

                    if (object.contains("color"))
                        cube->setColor(Vector3f(object["color"][0], object["color"][1], object["color"][2]));

                    gameObjects.push_back(cube);
                }
                else if (object["type"] == "particleEmitter")
                {
                    Logger::debug("Creating particle emitter game object\n");
                    ParticleEmitter *particleEmitter = new ParticleEmitter();

                    if (object.contains("name"))
                        particleEmitter->setName(object["name"]);

                    if (object.contains("particleCreationRate"))
                        particleEmitter->setParticleCreationRate(object["particleCreationRate"]);

                    if (object.contains("initialPosition"))
                        particleEmitter->setInitialPosition(Vector3f(object["initialPosition"][0], object["initialPosition"][1], object["initialPosition"][2]));

                    if (object.contains("initialPositionVariance"))
                        particleEmitter->setInitialPositionVariance(Vector3f(object["initialPositionVariance"][0], object["initialPositionVariance"][1], object["initialPositionVariance"][2]));

                    if (object.contains("initialVelocity"))
                        particleEmitter->setInitialVelocity(Vector3f(object["initialVelocity"][0], object["initialVelocity"][1], object["initialVelocity"][2]));

                    if (object.contains("initialVelocityVariance"))
                        particleEmitter->setInitialVelocityVariance(Vector3f(object["initialVelocityVariance"][0], object["initialVelocityVariance"][1], object["initialVelocityVariance"][2]));

                    if (object.contains("initialLifeSpan"))
                        particleEmitter->setInitialLifeSpan(object["initialLifeSpan"]);

                    if (object.contains("initialLifeSpanVariance"))
                        particleEmitter->setInitialLifeSpanVariance(object["initialLifeSpanVariance"]);

                    if (object.contains("particleColor"))
                        particleEmitter->setParticleColor(Vector4f(object["particleColor"][0], object["particleColor"][1], object["particleColor"][2], object["particleColor"][3]));

                    if (object.contains("particleColorRedVariance"))
                        particleEmitter->setParticleColorRedVariance(object["particleColorRedVariance"]);

                    if (object.contains("particleColorGreenVariance"))
                        particleEmitter->setParticleColorGreenVariance(object["particleColorGreenVariance"]);

                    if (object.contains("particleColorBlueVariance"))
                        particleEmitter->setParticleColorBlueVariance(object["particleColorBlueVariance"]);

                    if (object.contains("particleColorAlphaVariance"))
                        particleEmitter->setParticleColorAlphaVariance(object["particleColorAlphaVariance"]);

                    if (object.contains("particleColorUniformVariance"))
                        particleEmitter->setParticleColorUniformVariance(object["particleColorUniformVariance"]);

                    if (object.contains("particleRadius"))
                        particleEmitter->setParticleRadius(object["particleRadius"]);

                    if (object.contains("particleRadiusVariance"))
                        particleEmitter->setParticleRadiusVariance(object["particleRadiusVariance"]);

                    if (object.contains("particleRadiusLowerBound"))
                        particleEmitter->setParticleRadiusLowerBound(object["particleRadiusLowerBound"]);

                    if (object.contains("gravity"))
                        particleEmitter->setGravity(Vector3f(object["gravity"][0], object["gravity"][1], object["gravity"][2]));

                    if (object.contains("airDensity"))
                        particleEmitter->setAirDensity(object["airDensity"]);

                    if (object.contains("dragCoefficient"))
                        particleEmitter->setDragCoefficient(object["dragCoefficient"]);

                    if (object.contains("collisionElasticity"))
                        particleEmitter->setCollisionElasticity(object["collisionElasticity"]);

                    if (object.contains("collisionFriction"))
                        particleEmitter->setCollisionFriction(object["collisionFriction"]);

                    gameObjects.push_back(particleEmitter);
                }
                else
                {
                    Logger::warn("Unknown game object type");
                }
            }
            else
            {
                Logger::warn("Game object does not contain type field");
            }
        }

        //////////////////
        // PARSE LIGHTS //
        //////////////////

        json::reference jsonLightObjects = data["lights"];

        for (json::reference object : jsonLightObjects)
        {
            Logger::debug("Creating plane game object\n");
            Light *light = new Light();

            if (object["type"].contains("name"))
                light->setName(object["type"]["name"]);

            if (object.contains("position"))
                light->setPosition(Vector3f(object["position"][0], object["position"][1], object["position"][2]));

            if (object.contains("target"))
                light->setTarget(Vector3f(object["target"][0], object["target"][1], object["target"][2]));
            else if (object.contains("rotation"))
                light->setRotation(Vector3f(object["rotation"][0], object["rotation"][1], object["rotation"][2]));

            if (object.contains("scale"))
                light->setScale(Vector3f(object["scale"][0], object["scale"][1], object["scale"][2]));

            if (object.contains("color"))
                light->setColor(Vector3f(object["color"][0], object["color"][1], object["color"][2]));

            if (object.contains("castsShadows") && object["castsShadows"])
                light->enableShadowMapping();

            lights.push_back(light);
        }
    }
    else
    {
        Logger::error("No scene file found\n");
        // exit(1);
    }

    //////////////////////
    // CLOTH SIMULATION //
    //////////////////////

    // Cloth *cloth = new Cloth(4, 4, 10);
    // cloth->setName("cloth");
    // cloth->setPosition(Vector3f(-2.0f, 0.0f, 0.0f)); // note: do not change y position
    // cloth->setColor(Vector3f(0.3f, 0.5f, 1.0f));
    // cloth->setCollisionPlaneHeight(0);
    // gameObjects.push_back(cloth);

    //////////////////////
    // PARTICLE EMITTER //
    //////////////////////

    // ParticleEmitter *particleEmitter = new ParticleEmitter();
    // particleEmitter->setName("particleEmitter.a");

    // gameObjects.push_back(particleEmitter);

    // particleEmitter = new ParticleEmitter();
    // particleEmitter->setName("particleEmitter.b");

    // gameObjects.push_back(particleEmitter);

    // debug: create light
    Light *light = new Light();
    light->setName("light");
    light->setPosition(Vector3f(10.0f, 10.0f, 10.0f));
    light->setTarget(Vector3f(0.0f, 0.0f, 0.0f));
    light->setColor(Vector3f(1.0f, 1.0f, 1.0f));
    light->enableShadowMapping();

    lights.push_back(light);

    //////////////////////
    // BACKGROUND COLOR //
    //////////////////////

    backgroundColor = Vector3f(0.1f, 0.1f, 0.1f);
}

void World::setCamera(Camera *camera)
{
    this->camera = camera;
}

void World::update(float deltaTime)
{
    PROFILE_START;

    if (game->getInputManager()->isKeyActionJustPressed("TOGGLE_WIREFRAME"))
        toggleWireframe();

    // update game objects
    for (GameObject *gameObject : gameObjects)
        gameObject->update(deltaTime);

    ///////////////////
    // UPDATE LIGHTS //
    ///////////////////

    for (Light *light : lights)
        light->update(deltaTime);

    //////////////////////
    // UPDATE ANIMATION //
    //////////////////////

    if (animation)
        animation->evaluate(fmod(game->getWallTime(), globalAnimationDuration));

    PROFILE_STOP;
}

void World::drawWithShadow()
{
    PROFILE_START;

    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_FRONT);

    ShaderManager *shaderManager = game->getShaderManager();
    Shader *depthShader = shaderManager->getShader("depth");
    Shader *shadowShader = shaderManager->getShader("shadow");

    //////////////////////
    // UPDATE DEPTH MAP //
    //////////////////////

    Light *light = lights[0];

    depthShader->use();
    depthShader->setMatrix4x4f("light_space_matrix", light->getLightSpaceMatrix());

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, light->getDepthMapFBO());
    glClear(GL_DEPTH_BUFFER_BIT);

    // render using the depth shader
    renderWithShader(depthShader);

    ////////////////////////
    // NORMAL RENDER PASS //
    ////////////////////////

    // post-rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // reset viewport (window size)
    glViewport(0, 0, camera->getResolution().x, camera->getResolution().y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_BACK);

    // render using depth map
    shadowShader->use();
    shadowShader->setVector3f("view_pos", camera->getPosition());
    shadowShader->setVector3f("light_pos", light->getPosition());
    shadowShader->setMatrix4x4f("light_space_matrix", light->getLightSpaceMatrix());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, light->getDepthMap());

    renderWithShader(shadowShader);

    ////////////////
    // LIGHT CUBE //
    ////////////////

    light->draw(shaderManager);

    glEnable(GL_DEPTH_TEST);

    PROFILE_STOP;
}

// draw without lighting uniforms
void World::draw()
{
    PROFILE_START;

    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderManager *shaderManager = game->getShaderManager();

    for (Light *light : lights)
    {
        light->draw(shaderManager);
    }

    render(shaderManager);

    PROFILE_STOP;
}

void World::render(ShaderManager *shaderManager)
{
    PROFILE_START;

    for (GameObject *gameObject : gameObjects)
        if (gameObject->isVisible())
            gameObject->draw(shaderManager);

    PROFILE_STOP;
}

void World::renderWithShader(Shader *shader)
{
    for (GameObject *gameObject : gameObjects)
        if (gameObject->isVisible())
            gameObject->drawWithShader(shader);
}

void World::toggleWireframe()
{
    if (wireframeMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    wireframeMode = !wireframeMode;
}