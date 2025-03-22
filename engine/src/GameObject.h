#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "ShaderManager.h"

#include "math/Vector.h"
#include "math/Matrix.h"

#include <string>

enum class GameObjectType
{
    DEFAULT,
    SKELETON,
    JOINT,
    SKIN,
    PLANE,
    CUBE,
    CLOTH,
    LIGHT,
    PARTICLE_EMITTER,
    PARTICLE,
};

static unsigned int uid = 0;

class GameObject
{
public:
    GameObject() {}
    virtual ~GameObject() {}

    virtual void update(float deltaTime) = 0;
    virtual void draw(ShaderManager *shaderManager) = 0;
    virtual void drawWithShader(Shader *shader) = 0;

    GameObjectType getType() const { return type; }

    Vector3f *getPositionPointer() { return &position; }
    Vector3f *getRotationPointer() { return &rotation; }
    Vector3f *getScalePointer() { return &scale; }

    Vector3f getPosition() const { return position; }
    Vector3f getRotation() const { return rotation; }
    Vector3f getScale() const { return scale; }

    Matrix4x4f &getTransform() { return transform; }
    std::string getName() const { return name; }
    bool isVisible() const { return visible; }

    bool *getVisiblePointer() { return &visible; }

    void setPosition(Vector3f position) { this->position = position; }
    void setRotation(Vector3f rotation) { this->rotation = rotation; }
    void setScale(Vector3f scale) { this->scale = scale; }
    void setName(std::string name) { this->name = name; }
    void setVisible(bool visible) { this->visible = visible; }

    unsigned int getID() const { return id; }

protected:
    unsigned int id = uid++;

    std::string name;
    bool visible = true;

    Vector3f position = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f rotation = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f scale = Vector3f(1.0f, 1.0f, 1.0f);

    Matrix4x4f transform = Matrix4x4f::identity();

    GameObjectType type = GameObjectType::DEFAULT;
};

#endif