#include "Particle.h"

#include "../core.h"

void Particle::integrate(float deltaTime)
{
    if (inverseMass <= 0.0f)
        return;

    // // debug print all variables
    // Logger::debug("acceleration: %f %f %f\n", acceleration.x, acceleration.y, acceleration.z);
    // Logger::debug("force accumulator: %f %f %f\n", forceAccumulator.x, forceAccumulator.y, forceAccumulator.z);

    // work out acceleration from the force
    Vector3f resultingAcceleration = acceleration;

    // force = ma; force * (mass ^ -1) = a
    resultingAcceleration += forceAccumulator * inverseMass;

    // Update velocity first
    velocity += resultingAcceleration * deltaTime;

    // Apply damping
    velocity *= powf(damping, deltaTime);

    // Update position using the updated velocity
    position += velocity * deltaTime;

    clearAccumulator();
}

void Particle::setMass(float mass)
{
    if (mass <= 0.0f)
    {
        inverseMass = 0.0f;
    }
    else
    {
        inverseMass = 1.0f / mass;
    }
}

float Particle::getMass()
{
    if (inverseMass == 0.0f)
    {
#ifdef __APPLE__
        return __FLT_MAX__;
#else
        return FLT_MAX;
#endif
    }
    else
    {
        return 1.0f / inverseMass;
    }
}

void Particle::setInverseMass(float inverseMass)
{
    this->inverseMass = inverseMass;
}

float Particle::getInverseMass()
{
    return inverseMass;
}

bool Particle::hasFiniteMass()
{
    return inverseMass >= 0.0f;
}

void Particle::setDamping(float damping)
{
    this->damping = damping;
}

float Particle::getDamping()
{
    return damping;
}

void Particle::setPosition(Vector3f position)
{
    this->position = position;
}

Vector3f Particle::getPosition()
{
    return position;
}

void Particle::setVelocity(Vector3f velocity)
{
    this->velocity = velocity;
}

Vector3f Particle::getVelocity()
{
    return velocity;
}

void Particle::setAcceleration(Vector3f acceleration)
{
    this->acceleration = acceleration;
}

Vector3f Particle::getAcceleration()
{
    return acceleration;
}

void Particle::clearAccumulator()
{
    forceAccumulator = Vector3f(0.0f, 0.0f, 0.0f);
}

void Particle::addForce(Vector3f force)
{
    forceAccumulator += force;
}

void Particle::addImpulse(Vector3f impulse)
{
    velocity += impulse * inverseMass;
}