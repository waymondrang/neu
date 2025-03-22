
#ifndef PARTICLE_H
#define PARTICLE_H

#include "math/Vector.h"

// https://github.com/idmillington/cyclone-physics/blob/master/include/cyclone/particle.h
class Particle
{
protected:
    // inverse of the mass of the particle
    float inverseMass;

    // damping is required to remove energy from system
    float damping;

    // linear position of particle in world space
    Vector3f position;

    // linear velocity of particle in world space
    Vector3f velocity;

    // accumulated force to be applied at the next integration step
    Vector3f forceAccumulator = Vector3f(0.0f, 0.0f, 0.0f);

    // linear acceleration of particle in world space
    Vector3f acceleration;

public:
    // integrates particle forward in time
    void integrate(float deltaTime);

    // sets mass of particle
    void setMass(float mass);

    // gets mass of particle
    float getMass();

    // sets inverse mass of particle
    void setInverseMass(float inverseMass);

    // gets inverse mass of particle
    float getInverseMass();

    bool hasFiniteMass();

    // sets damping of particle
    void setDamping(float damping);

    // gets damping of particle
    float getDamping();

    // sets position of particle
    void setPosition(Vector3f position);

    // gets position of particle
    Vector3f getPosition();

    // sets velocity of particle
    void setVelocity(Vector3f velocity);

    // gets velocity of particle
    Vector3f getVelocity();

    // sets constant acceleration of particle
    void setAcceleration(Vector3f acceleration);

    // gets constant acceleration of particle
    Vector3f getAcceleration();

    // clears force accumulator
    void clearAccumulator();

    // adds force to force accumulator
    void addForce(Vector3f force);

    // adds impulse to particle
    void addImpulse(Vector3f impulse);
};

#endif