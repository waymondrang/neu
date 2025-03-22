#include "Plane.h"

#include "utility/Logger.h"

#include "math/Matrix.h"

#include <vector>

Plane::Plane()
{
    // Logger::info("Creating plane with scale: %f\n", scale);

    std::vector<Vector3f> vertices = {
        Vector3f(-1.0f, 0.0f, -1.0f),
        Vector3f(1.0f, 0.0f, -1.0f),
        Vector3f(1.0f, 0.0f, 1.0f),
        Vector3f(-1.0f, 0.0f, 1.0f),
    };

    std::vector<Vector3f> normals = {
        Vector3f(0.0f, 1.0f, 0.0f),
        Vector3f(0.0f, 1.0f, 0.0f),
        Vector3f(0.0f, 1.0f, 0.0f),
        Vector3f(0.0f, 1.0f, 0.0f),
    };

    std::vector<Vector3i> faces = {
        Vector3i(0, 1, 2),
        Vector3i(0, 2, 3),
    };

    mesh.setupVertices(vertices);
    mesh.setupNormals(normals);
    mesh.setupFaces(faces);

    this->position = Vector3f(0.0f, 0.0f, 0.0f);
    this->rotation = Vector3f(0.0f, 0.0f, 0.0f);
    this->scale = Vector3f(1.0f, 1.0f, 1.0f);

    color = Vector3f(1.0f, 1.0f, 1.0f);

    name = "plane";
    type = GameObjectType::PLANE;
}

void Plane::update(float deltaTime)
{
    transform = createTransformMatrix(position, rotation, scale);
}

void Plane::draw(ShaderManager *shaderManager)
{
    Shader *shader = shaderManager->getShader("light");

    shader->use();
    shader->setVector3f("color", color);
    shader->setMatrix4x4f("model_matrix", transform);

    mesh.draw(shader);
}

void Plane::drawWithShader(Shader *shader)
{
    shader->use();
    shader->setVector3f("color", color);
    shader->setMatrix4x4f("model_matrix", transform);

    mesh.draw(shader);
}