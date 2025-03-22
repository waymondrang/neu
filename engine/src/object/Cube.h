#ifndef CUBE_H
#define CUBE_H

#include "../GameObject.h"
#include "../Mesh.h"

class Cube : public GameObject
{
public:
    Cube(float scale = 1.0f);

    void draw(ShaderManager *shaderManager);
    void drawWithShader(Shader *shader);
    void update(float deltaTime);

    void setColor(const Vector3f &color) { this->color = color; }
    const Vector3f &getColor() const { return color; }

protected:
    Mesh mesh;

    Vector3f color;
};

#endif