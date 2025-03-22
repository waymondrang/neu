#include "Light.h"

#include "core.h"

#include <vector>

Light::Light()
{
    Logger::info("Creating light\n");

    update_view_matrix();
    update_projection_matrix();
    update_light_space_matrix();

    ////////////////
    // light cube //
    ////////////////

    float light_scale = 0.1f;

    std::vector<Vector3f> vertices;
    std::vector<Vector3i> faces;

    vertices.push_back(Vector3f(-1.0f, -1.0f, -1.0f) * light_scale);
    vertices.push_back(Vector3f(1.0f, -1.0f, -1.0f) * light_scale);
    vertices.push_back(Vector3f(1.0f, 1.0f, -1.0f) * light_scale);
    vertices.push_back(Vector3f(-1.0f, 1.0f, -1.0f) * light_scale);
    vertices.push_back(Vector3f(-1.0f, -1.0f, 1.0f) * light_scale);
    vertices.push_back(Vector3f(1.0f, -1.0f, 1.0f) * light_scale);
    vertices.push_back(Vector3f(1.0f, 1.0f, 1.0f) * light_scale);
    vertices.push_back(Vector3f(-1.0f, 1.0f, 1.0f) * light_scale);

    faces.push_back(Vector3i(0, 1, 2));
    faces.push_back(Vector3i(2, 3, 0));
    faces.push_back(Vector3i(4, 5, 6));
    faces.push_back(Vector3i(6, 7, 4));
    faces.push_back(Vector3i(0, 4, 7));
    faces.push_back(Vector3i(7, 3, 0));
    faces.push_back(Vector3i(1, 5, 6));
    faces.push_back(Vector3i(6, 2, 1));
    faces.push_back(Vector3i(0, 1, 5));
    faces.push_back(Vector3i(5, 4, 0));
    faces.push_back(Vector3i(3, 2, 6));
    faces.push_back(Vector3i(6, 7, 3));

    mesh.setupVertices(vertices);
    mesh.setupFaces(faces);

    name = "light";
    type = GameObjectType::LIGHT;
    color = Vector3f(1.0f, 1.0f, 1.0f);
}

void Light::enableShadowMapping()
{
    shadowMappingEnabled = true;

    Logger::debug("Enabling shadow mapping\n");

    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::draw(ShaderManager *shaderManager)
{
    Shader *shader = shaderManager->getShader("simple");

    shader->use();
    shader->setVector3f("color", color);
    shader->setMatrix4x4f("model_matrix", transform);

    mesh.draw(shader);
}

void Light::drawWithShader(Shader *shader)
{
    shader->use();
    shader->setVector3f("color", color);
    shader->setMatrix4x4f("model_matrix", transform);

    mesh.draw(shader);
}

void Light::update_view_matrix()
{
    Vector3f direction = normalize(position);
    Vector3f up = Vector3f(0.0f, 1.0f, 0.0f); // +y axis
    Vector3f right = normalize(cross(up, direction));

    up = normalize(cross(direction, right)); // re-orthogonalize up

    lightViewMatrix(0, 0) = right.x;
    lightViewMatrix(1, 0) = up.x;
    lightViewMatrix(2, 0) = direction.x;
    lightViewMatrix(3, 0) = 0.0f;

    lightViewMatrix(0, 1) = right.y;
    lightViewMatrix(1, 1) = up.y;
    lightViewMatrix(2, 1) = direction.y;
    lightViewMatrix(3, 1) = 0.0f;

    lightViewMatrix(0, 2) = right.z;
    lightViewMatrix(1, 2) = up.z;
    lightViewMatrix(2, 2) = direction.z;
    lightViewMatrix(3, 2) = 0.0f;

    lightViewMatrix(0, 3) = -dot(right, position);
    lightViewMatrix(1, 3) = -dot(up, position);
    lightViewMatrix(2, 3) = -dot(direction, position);
    lightViewMatrix(3, 3) = 1.0f;
}

// orthographic projection
void Light::update_projection_matrix()
{
    float left = -10.0f;
    float right = 10.0f;
    float bottom = -10.0f;
    float top = 10.0f;

    lightProjectionMatrix = Matrix4x4f::identity();
    lightProjectionMatrix(0, 0) = 2.0f / (right - left);
    lightProjectionMatrix(1, 1) = 2.0f / (top - bottom);
    lightProjectionMatrix(2, 2) = -2.0f / (farPlane - nearPlane);
    lightProjectionMatrix(0, 3) = -(right + left) / (right - left);
    lightProjectionMatrix(1, 3) = -(top + bottom) / (top - bottom);
    lightProjectionMatrix(2, 3) = -(farPlane + nearPlane) / (farPlane - nearPlane);
}

void Light::update_light_space_matrix()
{
    lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;
}

void Light::update(float deltaTime)
{
    transform = inverse(look_at(position, target, Vector3f(0.0f, 1.0f, 0.0f)));

    // if (!animate)
    //     return;

    // float rotationSpeed = 1.0f;
    // angle += rotationSpeed * deltaTime;

    // float radius = sqrt(position.x * position.x + position.z * position.z);
    // float new_x = radius * cos(angle);
    // float new_z = radius * sin(angle);

    // position.x = new_x;
    // position.z = new_z;

    update_view_matrix();
    update_projection_matrix();
    update_light_space_matrix();
}

Matrix4x4f Light::translate(Matrix4x4f matrix, Vector3f translation)
{
    matrix(0, 3) = translation.x;
    matrix(1, 3) = translation.y;
    matrix(2, 3) = translation.z;

    return matrix;
}

Matrix4x4f Light::look_at(Vector3f position, Vector3f target, Vector3f up)
{
    Matrix4x4f matrix = Matrix4x4f::identity();

    Vector3f direction = normalize(target - position);
    Vector3f right = normalize(cross(up, direction));
    up = normalize(cross(direction, right));

    matrix(0, 0) = right.x;
    matrix(1, 0) = up.x;
    matrix(2, 0) = direction.x;
    matrix(3, 0) = 0.0f;

    matrix(0, 1) = right.y;
    matrix(1, 1) = up.y;
    matrix(2, 1) = direction.y;
    matrix(3, 1) = 0.0f;

    matrix(0, 2) = right.z;
    matrix(1, 2) = up.z;
    matrix(2, 2) = direction.z;
    matrix(3, 2) = 0.0f;

    matrix(0, 3) = -dot(right, position);
    matrix(1, 3) = -dot(up, position);
    matrix(2, 3) = -dot(direction, position);
    matrix(3, 3) = 1.0f;

    return matrix;
}