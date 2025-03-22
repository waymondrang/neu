#include "ParticleForceGenerator.h"

#include "../core.h"

ParticleGravity::ParticleGravity(const Vector3f &gravity)
{
    this->gravity = gravity;
}

void ParticleGravity::updateForce(Particle *particle, float deltaTime)
{
    // check that the particle has a finite mass
    if (!particle->hasFiniteMass())
        return;

    // apply the force
    particle->addForce(gravity * particle->getMass());
}

ParticleSpringDamper::ParticleSpringDamper(Particle *other, float springConstant, float restLength)
{
    this->other = other;
    this->springConstant = springConstant;
    this->restLength = restLength;
}

void ParticleSpringDamper::updateForce(Particle *particle, float deltaTime)
{
    //////////////////
    // SPRING FORCE //
    //////////////////

    // calculate the vector of the spring (x_a - x_b where x_a is this particle)
    Vector3f force;
    force = particle->getPosition();
    force -= other->getPosition(); // other is stored in the spring force generator

    // calculate the magnitude of the force
    float magnitude = length(force);
    magnitude = fabsf(magnitude - restLength); // why abs?
    magnitude *= springConstant;

    // apply the force
    force = normalize(force);
    force *= -magnitude;
    particle->addForce(force);

    ///////////////////
    // DAMPING FORCE //
    ///////////////////

    // calculate relative velocity
    Vector3f relativeVelocity = particle->getVelocity();
    relativeVelocity -= other->getVelocity();

    // calculate damping force
    float damping = 0.1f;
    float dampingMagnitude = dot(relativeVelocity, force) * damping;

    // apply the damping force
    force = normalize(relativeVelocity);
    force *= -dampingMagnitude;
    particle->addForce(force);

    Logger::debug("Applying force (%f, %f, %f) to particle\n", force.x, force.y, force.z);
}

ParticleAnchoredSpring::ParticleAnchoredSpring(const Vector3f anchor, float springConstant, float restLength)
{
    this->anchor = anchor;
    this->springConstant = springConstant;
    this->restLength = restLength;
}

void ParticleAnchoredSpring::updateForce(Particle *particle, float deltaTime)
{
    // calculate the vector of the spring (x_a - x_b where x_a is this particle)
    Vector3f force;
    force = particle->getPosition();
    force -= anchor;

    // calculate the magnitude of the force
    float magnitude = length(force);
    magnitude = fabsf(magnitude - restLength); // why abs?
    magnitude *= springConstant;

    // apply the force
    force = normalize(force);
    force *= -magnitude;
    particle->addForce(force);
}