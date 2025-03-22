#ifndef PARTICLE_CONTACT_H
#define PARTICLE_CONTACT_H

#include "Particle.h"
#include <math/Vector.h>

class ParticleContact
{
public:
    // particles involved in the contact
    // second particle is NULL if collision is with the environment
    Particle *particle[2];

    // normal restitution coefficient
    float restitution;

    // direction of contact in world space (from the perspective of particle[0])
    Vector3f contactNormal;

    // penetration at the contact (positive means overlap)
    float penetration;

    // resolve the contact for both velocity and interpenetration
    void resolve(float deltaTime);

    // calculate the separating velocity at this contact
    float calculateSeparatingVelocity();

private:
    // handle the impulse calculations for the collision
    void resolveVelocity(float deltaTime);

    // handle the interpenetration resolution for the collision
    void resolveInterpenetration(float deltaTime);
};

#endif