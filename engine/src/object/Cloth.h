#ifndef CLOTH_H
#define CLOTH_H

#include <vector>

#include "../GameObject.h"
#include "../Mesh.h"
#include "../particle/Particle.h"

class Force
{
public:
    virtual void applyForce() = 0;
};

class SpringDamperForce : public Force
{
public:
    SpringDamperForce(Particle *p1, Particle *p2, float *springConstant, float *dampingConstant)
        : p1(p1), p2(p2), springConstant(springConstant), dampingConstant(dampingConstant)
    {
        initialDistance = length(p2->getPosition() - p1->getPosition());
    }

    void applyForce()
    {
        Vector3f e_star = p2->getPosition() - p1->getPosition();
        float l = length(e_star);
        Vector3f e = normalize(e_star);

        // float f = -springConstant * (initialDistance - l) - dampingConstant * dot(p1->getVelocity() - p2->getVelocity(), e);
        float f = -(*springConstant) * (initialDistance - l) - (*dampingConstant) * dot(p1->getVelocity() - p2->getVelocity(), e);

        p1->addForce(f * e);
        p2->addForce(-f * e);
    }

protected:
    Particle *p1;
    Particle *p2;

    float initialDistance;
    float *springConstant;
    float *dampingConstant;
};

class Cloth : public GameObject
{
public:
    Cloth(int width, int height, int particleDensity);

    void draw(ShaderManager *shaderManager);
    void drawWithShader(Shader *shader);
    void update(float deltaTime);

    void setWidth(int width) { this->width = width; }
    int getWidth() const { return width; }

    void setHeight(int height) { this->height = height; }
    int getHeight() const { return height; }

    void setParticleDensity(int particleDensity) { this->particleDensity = particleDensity; }
    int getParticleDensity() const { return particleDensity; }

    void setColor(Vector3f color) { this->color = color; }
    Vector3f getColor() const { return color; }

    // hack
    void setCollisionPlaneHeight(float collisionPlaneHeight) { this->collisionPlaneHeight = collisionPlaneHeight; }

    float *getSpringConstantPointer() { return &springConstant; }
    float *getDampingConstantPointer() { return &dampingConstant; }
    float *getFixedDeltaTimePointer() { return &fixedDeltaTime; }
    float *getWindVelocityPointer() { return &windVelocity; }

    void reset();

protected:
    int width;
    int height;
    int particleDensity;

    // global cloth attributes
    float springConstant = 100.0f;
    float dampingConstant = 0.6f;
    float fixedDeltaTime = 0.01f;

    float windVelocity = 4.0f;

    std::vector<Particle *> particles;
    std::vector<Force *> forces;

    float collisionPlaneHeight = 0.0f;

    void updateVertices();
    void updateNormals();
    void updateMeshVectors();

    void calculateIndices(std::vector<Vector3i> &indices);

    Mesh mesh;

    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Vector3i> faces;

    Vector3f color;
};

#endif