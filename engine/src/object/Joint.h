#ifndef JOINT_H
#define JOINT_H

#include "math/Vector.h"
#include "math/Matrix.h"
#include "../utility/Tokenizer.h"
#include "../GameObject.h"

#include <vector>

class Joint : public GameObject
{
public:
    Joint(GameObject *parent);
    ~Joint();

    void load(Tokenizer &tokenizer);
    void update(float deltaTime);
    void draw(ShaderManager *shaderManager);
    void drawWithShader(Shader *shader);
    void collectAllJoints(std::vector<Joint *> &joints);

    // getters
    Vector3f getOffset() const { return offset; }
    Vector3f getBoxMin() const { return boxmin; }
    Vector3f getBoxMax() const { return boxmax; }
    Vector2f getRotXLimit() const { return rotxlimit; }
    Vector2f getRotYLimit() const { return rotylimit; }
    Vector2f getRotZLimit() const { return rotzlimit; }
    Vector3f getPose() const { return pose; }
    Vector3f *getPose() { return &pose; }
    std::string getName() const { return name; }
    Matrix4x4f getLocalTransform() const { return localTransform; }
    std::vector<Joint *> &getChildren() { return children; }

private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    Matrix4x4f localTransform;

    GameObject *parent;
    std::vector<Joint *> children;

    Vector3f offset;
    Vector3f boxmin;
    Vector3f boxmax;
    Vector2f rotxlimit;
    Vector2f rotylimit;
    Vector2f rotzlimit;
    Vector3f pose;
};

#endif