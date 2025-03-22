#ifndef PLANE_H
#define PLANE_H

#include "../GameObject.h"
#include "../ShaderManager.h"
#include "../Mesh.h"

#include "math/Vector.h"

#include <string>

class Plane : public GameObject
{
public:
    Plane();

    void update(float deltaTime);
    void draw(ShaderManager *shaderManager);
    void drawWithShader(Shader *shader);

    void setColor(Vector3f color) { this->color = color; }

protected:
    Mesh mesh;
    Vector3f color;
};

#endif