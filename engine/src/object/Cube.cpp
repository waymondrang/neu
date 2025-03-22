#include "Cube.h"

#include "../utility/Logger.h"

#include "math/Vector.h"

#include <vector>

Cube::Cube(float scale)
{
    Logger::info("Creating cube with scale: %f\n", scale);

    std::vector<Vector3f> vertices = {
        Vector3f(-1.0f, 1.0f, -1.0f) * scale,
        Vector3f(1.0f, 1.0f, 1.0f) * scale,
        Vector3f(1.0f, 1.0f, -1.0f) * scale,
        Vector3f(-1.0f, -1.0f, 1.0f) * scale,
        Vector3f(1.0f, -1.0f, 1.0f) * scale,
        Vector3f(-1.0f, 1.0f, 1.0f) * scale,
        Vector3f(-1.0f, -1.0f, -1.0f) * scale,
        Vector3f(-1.0f, -1.0f, 1.0f) * scale,
        Vector3f(1.0f, -1.0f, -1.0f) * scale,
        Vector3f(-1.0f, -1.0f, 1.0f) * scale,
        Vector3f(-1.0f, -1.0f, -1.0f) * scale,
        Vector3f(-1.0f, 1.0f, -1.0f) * scale,
        Vector3f(-1.0f, 1.0f, 1.0f) * scale,
        Vector3f(-1.0f, 1.0f, 1.0f) * scale,
    };

    std::vector<Vector3f> normals = {
        Vector3f(-0.57732844, 0.57736117, -0.57736117),
        Vector3f(0.57732844, 0.57736117, 0.57736117),
        Vector3f(0.5773721, 0.57733935, -0.57733935),
        Vector3f(-0.57732844, -0.57736117, 0.57736117),
        Vector3f(0.5773721, -0.57733935, 0.57733935),
        Vector3f(-0.5773721, 0.57733935, 0.57733935),
        Vector3f(-0.5773721, -0.57733935, -0.57733935),
        Vector3f(-0.57732844, -0.57736117, 0.57736117),
        Vector3f(0.57732844, -0.57736117, -0.57736117),
        Vector3f(-0.57732844, -0.57736117, 0.57736117),
        Vector3f(-0.5773721, -0.57733935, -0.57733935),
        Vector3f(-0.57732844, 0.57736117, -0.57736117),
        Vector3f(-0.5773721, 0.57733935, 0.57733935),
        Vector3f(-0.5773721, 0.57733935, 0.57733935),
    };

    std::vector<Vector3i> faces = {
        Vector3i(0, 1, 2),
        Vector3i(1, 3, 4),
        Vector3i(5, 6, 7),
        Vector3i(8, 9, 10),
        Vector3i(2, 4, 8),
        Vector3i(11, 8, 6),
        Vector3i(0, 12, 1),
        Vector3i(1, 13, 3),
        Vector3i(5, 11, 6),
        Vector3i(8, 4, 9),
        Vector3i(2, 1, 4),
        Vector3i(11, 2, 8),
    };

    mesh.setupVertices(vertices);
    mesh.setupNormals(normals);
    mesh.setupFaces(faces);

    this->position = Vector3f(0.0f, 0.0f, 0.0f);
    this->rotation = Vector3f(0.0f, 0.0f, 0.0f);
    this->scale = Vector3f(1.0f, 1.0f, 1.0f);

    color = Vector3f(1.0f, 1.0f, 1.0f);

    name = "cube";
    type = GameObjectType::CUBE;
}

void Cube::update(float deltaTime)
{
    transform = createTransformMatrix(position, rotation, scale);
}

void Cube::draw(ShaderManager *shaderManager)
{
    Shader *shader = shaderManager->getShader("light");

    shader->use();
    shader->setVector3f("color", color);
    shader->setMatrix4x4f("model_matrix", transform);

    mesh.draw(shader);
}

void Cube::drawWithShader(Shader *shader)
{
    shader->use();
    shader->setVector3f("color", color);
    shader->setMatrix4x4f("model_matrix", transform);

    mesh.draw(shader);
}