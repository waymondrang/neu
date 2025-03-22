#ifndef SKELETON_H
#define SKELETON_H

#include "../GameObject.h"
#include "Joint.h"
#include "../Animation.h"

#include <vector>
#include <string>

class Skeleton : public GameObject
{
public:
    Skeleton();
    virtual ~Skeleton();

    void update(float deltaTime);
    void draw(ShaderManager *ShaderManager);
    void drawWithShader(Shader *shaderManager);
    bool load(const char *filename);

    std::vector<Joint *> collectAllJoints();

    bool *getAnimatePointer() { return &enableAnimation; }
    Animation *getAnimation() { return animation; }
    Joint *getRootJoint() { return root; }

    void setAnimation(Animation *animation) { this->animation = animation; }

    void addFreedomDegree(float *freedomDegree) { freedomDegrees.push_back(freedomDegree); }

private:
    bool enableAnimation = true;

    Joint *root;
    Animation *animation = nullptr;

    std::vector<float *> freedomDegrees;
};

#endif