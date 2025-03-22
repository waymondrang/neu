#ifndef LIGHT_H
#define LIGHT_H

#include "math/Vector.h"
#include "math/Matrix.h"

#include "Mesh.h"
#include "GameObject.h"
#include "ShaderManager.h"

#include <vector>

#define SHADOW_WIDTH 2048
#define SHADOW_HEIGHT 2048

class Light : public GameObject
{
public:
    Light();

    void update(float deltaTime);
    void draw(ShaderManager *shaderManager);
    void drawWithShader(Shader *shader);


    void enableShadowMapping();
    void update_view_matrix();

    void setColor(Vector3f color) { this->color = color; }
    void setTarget(Vector3f target) { this->target = target; }

    Vector3f getColor() const { return color; }
    Vector3f getTarget() const { return target; }

    Vector3f *getColorRef() { return &color; }
    Vector3f *getPositionRef() { return &position; }
    Vector3f *getTargetRef() { return &target; }

    int getDepthMapFBO() { return depthMapFBO; }
    int getDepthMap() { return depthMap; }
    Matrix4x4f getLightSpaceMatrix() const { return lightSpaceMatrix; }

private:
    Vector3f target;
    Vector3f color;

    Matrix4x4f lightViewMatrix;
    Matrix4x4f lightProjectionMatrix;
    Matrix4x4f lightSpaceMatrix;

    // TODO: DECOUPLE LIGHT FROM LIGHT CUBE
    Mesh mesh;

    ////////////////////
    // SHADOW MAPPING //
    ////////////////////

    bool shadowMappingEnabled = false;

    unsigned int depthMap;
    unsigned int depthMapFBO;

    float nearPlane = 1.0f;
    float farPlane = 50.0f;

    float angle = 0.0f;
    bool animate = false;

    void update_projection_matrix();
    void update_light_space_matrix();

    static Matrix4x4f translate(Matrix4x4f matrix, Vector3f translation);
    static Matrix4x4f look_at(Vector3f position, Vector3f target, Vector3f up);
};

#endif