#ifndef PARTICLE_CONTACT_RESOLVER_H
#define PARTICLE_CONTACT_RESOLVER_H

#include "ParticleContact.h"

class ParticleContactResolver
{
private:
    // number of iterations allowed
    unsigned int iterations;

    // keep track of number of iterations used
    unsigned int iterationsUsed;

public:
    ParticleContactResolver(unsigned int iterations);

    // sets the number of iterations that can be used
    void setIterations(unsigned int iterations);

    // resolves a set of particle contacts for both penetration and velocity
    void resolveContacts(ParticleContact *contactArray, unsigned int numContacts, float deltaTime);
};

#endif