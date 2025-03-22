#include "Skeleton.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "../core.h"
#include "../utility/Tokenizer.h"

Skeleton::Skeleton()
{
    position = Vector3f(0.0f, 0.0f, 0.0f);
    rotation = Vector3f(0.0f, 0.0f, 0.0f);
    scale = Vector3f(1.0f, 1.0f, 1.0f);

    type = GameObjectType::SKELETON;
}

Skeleton::~Skeleton()
{
}

void Skeleton::update(float deltaTime)
{

    transform = createTransformMatrix(position, rotation, scale);

    if (animation && enableAnimation)
    {
        std::vector<float> channels = animation->getFreedomDegrees();
        for (unsigned int i = 0; i < channels.size(); i++)
            *freedomDegrees[i] = channels[i];
    }

    root->update(deltaTime);
}

void Skeleton::draw(ShaderManager *ShaderManager)
{
    Shader *shader = ShaderManager->getShader("simple");

    shader->use();
    shader->setVector3f("color", Vector3f(1.0f, 0.0f, 0.0f));

    root->draw(ShaderManager);
}

void Skeleton::drawWithShader(Shader *shader)
{
    shader->use();
    shader->setVector3f("color", Vector3f(1.0f, 0.0f, 0.0f));

    root->drawWithShader(shader);
}

bool Skeleton::load(const char *filename)
{
    Logger::debug("Loading skeleton from file: %s\n", filename);

    Tokenizer tokenizer;
    if (!tokenizer.Open(filename))
    {
        Logger::error("Failed to open file: %s\n", filename);
        return false;
    }

    if (!tokenizer.FindToken("balljoint"))
    {
        Logger::error("Failed to find token: balljoint\n");
        return false;
    }

    // parse tree
    root = new Joint(this);
    root->load(tokenizer);

    // finish
    tokenizer.Close();

    Logger::info("Loaded skeleton from file: %s\n", filename);

    Logger::debug("Setting up freedom degrees vector\n"); // regardless of whether animation is set

    std::vector<Joint *> joints;
    root->collectAllJoints(joints);

    freedomDegrees.push_back(&position.x);
    freedomDegrees.push_back(&position.y);
    freedomDegrees.push_back(&position.z);

    for (unsigned int i = 0; i < joints.size(); i++)
    {
        Vector3f *rotation = joints[i]->getRotationPointer();
        freedomDegrees.push_back(&rotation->x);
        freedomDegrees.push_back(&rotation->y);
        freedomDegrees.push_back(&rotation->z);
    }

    Logger::debug("Freedom degrees vector size: %d\n", (int)freedomDegrees.size());

    return true;
}

std::vector<Joint *> Skeleton::collectAllJoints()
{
    std::vector<Joint *> joints;
    root->collectAllJoints(joints);
    return joints;
}