#ifndef PARTICLE_FORCE_GENERATOR_H
#define PARTICLE_FORCE_GENERATOR_H

#include "Particle.h"

class ParticleForceGenerator
{
public:
    virtual void updateForce(Particle *particle, float deltaTime) = 0;
};

class ParticleGravity : public ParticleForceGenerator
{
    // acceleration due to gravity
    Vector3f gravity;

public:
    ParticleGravity(const Vector3f &gravity);

    // applies the gravitational force to the given particle
    virtual void updateForce(Particle *particle, float deltaTime);
};

class ParticleSpringDamper : public ParticleForceGenerator
{
    // the particle at the other end of the spring
    Particle *other;

    // spring constant
    float springConstant;

    // rest length of the spring
    float restLength;

public:
    ParticleSpringDamper(Particle *other, float springConstant, float restLength);

    // applies the spring force to the given particle
    virtual void updateForce(Particle *particle, float deltaTime);
};

class ParticleAnchoredSpring : public ParticleForceGenerator
{
    // anchor point
    Vector3f anchor;

    // spring constant
    float springConstant;

    // rest length of the spring
    float restLength;

public:
    ParticleAnchoredSpring(const Vector3f anchor, float springConstant, float restLength);

    // applies the spring force to the given particle
    virtual void updateForce(Particle *particle, float deltaTime);
};

#endif