#ifndef LINKED_LIST_PARTICLE_H
#define LINKED_LIST_PARTICLE_H

#include "Particle.h"
#include "../GameObject.h"
#include "../Mesh.h"
#include "math/Vector.h"

class LinkedListParticle : public Particle
{
public:
    LinkedListParticle();

    void update(float deltaTime, int &particleCount);

    ///////////////////////////
    // GAME OBJECT FUNCTIONS //
    ///////////////////////////

    bool update(float deltaTime);
    void drawMeshWithShader(Mesh *mesh, Shader *shader);

    /////////////////////////
    // GETTERS AND SETTERS //
    /////////////////////////

    LinkedListParticle *getNextParticle() { return nextParticle; }
    LinkedListParticle *getPreviousParticle() { return previousParticle; }
    float getExpirationTime() { return expirationTime; }

    void setNextParticle(LinkedListParticle *nextParticle) { this->nextParticle = nextParticle; }
    void setPreviousParticle(LinkedListParticle *previousParticle) { this->previousParticle = previousParticle; }
    void setExpirationTime(float expirationTime) { this->expirationTime = expirationTime; }

    void setColor(Vector4f &color) { this->color = color; }
    Vector4f getColor() { return color; }

    void setRadius(float radius) { this->radius = radius; }
    float getRadius() { return radius; }

    void setCollisionElasticity(float collisionElasticity) { this->collisionElasticity = collisionElasticity; }
    float getCollisionElasticity() { return collisionElasticity; }

    void setCollisionFriction(float collisionFriction) { this->collisionFriction = collisionFriction; }
    float getCollisionFriction() { return collisionFriction; }

    void setAirDensityReference(float *airDensityReference) { this->airDensityReference = airDensityReference; }
    void setDragCoefficientReference(float *dragCoeffientReference) { this->dragCoefficientReference = dragCoeffientReference; }

private:
    LinkedListParticle *previousParticle;
    LinkedListParticle *nextParticle;

    Matrix4x4f transform;

    Vector4f color;

    float accumulatedTime; // hack, without needing to track global time (tracking global time is probably something to implement in the future)
    float expirationTime;
    float radius;

    float collisionElasticity;
    float collisionFriction;

    float *airDensityReference;
    float *dragCoefficientReference;
};

#endif