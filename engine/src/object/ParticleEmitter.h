#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

#include "../core.h"

#include "../GameObject.h"
#include "../particle/LinkedListParticle.h"

#include <list>

class ShaderManager;
class Shader;

class ParticleEmitter : public GameObject
{
public:
    ParticleEmitter();

    void reset();

    /////////////////////////
    // GAME OBJECT METHODS //
    /////////////////////////

    void update(float deltaTime);
    void draw(ShaderManager *shaderManager);
    void drawWithShader(Shader *shader);

    /////////////////////
    // POINTER GETTERS //
    /////////////////////

    int *getParticleCreationRatePointer() { return &particleCreationRate; }
    Vector3f *getInitialPositionPointer() { return &initialPosition; }
    Vector3f *getInitialPositionVariancePointer() { return &initialPositionVariance; }
    Vector3f *getInitialVelocityPointer() { return &initialVelocity; }
    Vector3f *getInitialVelocityVariancePointer() { return &initialVelocityVariance; }
    float *getInitialLifeSpanPointer() { return &initialLifeSpan; }
    float *getInitialLifeSpanVariancePointer() { return &initialLifeSpanVariance; }
    Vector3f *getGravityPointer() { return &gravity; }
    float *getAirDensityPointer() { return &airDensity; }
    float *getDragCoefficientPointer() { return &dragCoefficient; }
    float *getParticleRadiusPointer() { return &particleRadius; }
    float *getCollisionElasticityPointer() { return &collisionElasticity; }
    float *getCollisionFrictionPointer() { return &collisionFriction; }

    Vector4f *getParticleColorPointer() { return &particleColor; }
    float *getParticleColorRedVariancePointer() { return &particleColorRedVariance; }
    float *getParticleColorGreenVariancePointer() { return &particleColorGreenVariance; }
    float *getParticleColorBlueVariancePointer() { return &particleColorBlueVariance; }
    float *getParticleColorAlphaVariancePointer() { return &particleColorAlphaVariance; }
    float *getParticleColorUniformVariancePointer() { return &particleColorUniformVariance; }

    float *getParticleRadiusVariancePointer() { return &particleRadiusVariance; }
    float *getParticleRadiusLowerBoundPointer() { return &particleRadiusLowerBound; }

    /////////////////////////
    // GETTERS AND SETTERS //
    /////////////////////////

    std::list<LinkedListParticle> *getParticlesPointer() { return &particles; }
    void setParticles(std::list<LinkedListParticle> particles) { this->particles = particles; }

    int getParticleCreationRate() const { return particleCreationRate; }
    void setParticleCreationRate(int particleCreationRate) { this->particleCreationRate = particleCreationRate; }

    Vector3f getInitialPosition() const { return initialPosition; }
    void setInitialPosition(const Vector3f &initialPosition) { this->initialPosition = initialPosition; }

    Vector3f getInitialPositionVariance() const { return initialPositionVariance; }
    void setInitialPositionVariance(const Vector3f &initialPositionVariance) { this->initialPositionVariance = initialPositionVariance; }

    Vector3f getInitialVelocity() const { return initialVelocity; }
    void setInitialVelocity(const Vector3f &initialVelocity) { this->initialVelocity = initialVelocity; }

    Vector3f getInitialVelocityVariance() const { return initialVelocityVariance; }
    void setInitialVelocityVariance(const Vector3f &initialVelocityVariance) { this->initialVelocityVariance = initialVelocityVariance; }

    float getInitialLifeSpan() const { return initialLifeSpan; }
    void setInitialLifeSpan(float initialLifeSpan) { this->initialLifeSpan = initialLifeSpan; }

    float getInitialLifeSpanVariance() const { return initialLifeSpanVariance; }
    void setInitialLifeSpanVariance(float initialLifeSpanVariance) { this->initialLifeSpanVariance = initialLifeSpanVariance; }

    Vector4f getParticleColor() const { return particleColor; }
    void setParticleColor(const Vector4f &particleColor) { this->particleColor = particleColor; }

    float getParticleColorRedVariance() const { return particleColorRedVariance; }
    void setParticleColorRedVariance(float particleColorRedVariance) { this->particleColorRedVariance = particleColorRedVariance; }

    float getParticleColorGreenVariance() const { return particleColorGreenVariance; }
    void setParticleColorGreenVariance(float particleColorGreenVariance) { this->particleColorGreenVariance = particleColorGreenVariance; }

    float getParticleColorBlueVariance() const { return particleColorBlueVariance; }
    void setParticleColorBlueVariance(float particleColorBlueVariance) { this->particleColorBlueVariance = particleColorBlueVariance; }

    float getParticleColorAlphaVariance() const { return particleColorAlphaVariance; }
    void setParticleColorAlphaVariance(float particleColorAlphaVariance) { this->particleColorAlphaVariance = particleColorAlphaVariance; }

    float getParticleColorUniformVariance() const { return particleColorUniformVariance; }
    void setParticleColorUniformVariance(float particleColorUniformVariance) { this->particleColorUniformVariance = particleColorUniformVariance; }

    float getParticleRadius() const { return particleRadius; }
    void setParticleRadius(float particleRadius) { this->particleRadius = particleRadius; }

    float getParticleRadiusVariance() const { return particleRadiusVariance; }
    void setParticleRadiusVariance(float particleRadiusVariance) { this->particleRadiusVariance = particleRadiusVariance; }

    float getParticleRadiusLowerBound() const { return particleRadiusLowerBound; }
    void setParticleRadiusLowerBound(float particleRadiusLowerBound) { this->particleRadiusLowerBound = particleRadiusLowerBound; }

    float getCollisionElasticity() const { return collisionElasticity; }
    void setCollisionElasticity(float collisionElasticity) { this->collisionElasticity = collisionElasticity; }

    float getCollisionFriction() const { return collisionFriction; }
    void setCollisionFriction(float collisionFriction) { this->collisionFriction = collisionFriction; }

    Vector3f getGravity() const { return gravity; }
    void setGravity(const Vector3f &gravity) { this->gravity = gravity; }

    float getAirDensity() const { return airDensity; }
    void setAirDensity(float airDensity) { this->airDensity = airDensity; }

    float getDragCoefficient() const { return dragCoefficient; }
    void setDragCoefficient(float dragCoefficient) { this->dragCoefficient = dragCoefficient; }

    int getParticleCount() const { return particles.size(); }

private:
    std::list<LinkedListParticle> particles;

    LinkedListParticle *head;

    unsigned int instanceTranslationVBO;
    unsigned int instanceRadiiVBO;
    unsigned int instanceColorVBO;

    std::vector<Vector3f> translations;
    std::vector<float> radii;
    std::vector<Vector4f> colors;

    Mesh particleMesh;

    int particleCreationRate = 10;   // particle / s
    float timeSinceLastSpawn = 0.0f; // ms

    float particleMass = 0.1f; // kg

    Vector3f initialPosition = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f initialPositionVariance = Vector3f(0.0f, 0.0f, 0.0f);

    Vector3f initialVelocity = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f initialVelocityVariance = Vector3f(0.0f, 0.0f, 0.0f);

    float initialLifeSpan = 1; // s
    float initialLifeSpanVariance = 0;

    Vector4f particleColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    float particleColorRedVariance = 0.0;
    float particleColorGreenVariance = 0.0;
    float particleColorBlueVariance = 0.0;
    float particleColorAlphaVariance = 0.0;
    float particleColorUniformVariance = 0.0;

    Vector3f gravity = Vector3f(0.0f, -9.8f, 0.0f);

    float airDensity = 1.225f;     // kg/m^3 (air at sea level)
    float dragCoefficient = 0.47f; // sphere

    float particleRadius = 0.1f;
    float particleRadiusVariance = 0.0f;
    float particleRadiusLowerBound = 0.0f;

    float collisionElasticity = 0.0f;
    float collisionFriction = 0.0f;
};

#endif