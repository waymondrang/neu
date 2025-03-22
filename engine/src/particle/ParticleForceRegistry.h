#ifndef PARTICLE_FORCE_REGISTRY_H
#define PARTICLE_FORCE_REGISTRY_H

#include "Particle.h"
#include "ParticleForceGenerator.h"

#include <vector>

struct ParticleForceRegistration
{
    Particle *particle;
    ParticleForceGenerator *fg;
};

typedef std::vector<ParticleForceRegistration> Registry;

class ParticleForceRegistry
{
public:
    Registry registrations;

    // registers the given force generator to apply to the given particle
    void add(Particle *particle, ParticleForceGenerator *fg);

    // removes the given registered pair from the registry
    void remove(Particle *particle, ParticleForceGenerator *fg);

    // removes all registered pairs from the registry
    void clear();

    // calls all force generators to update the forces of their corresponding particles
    void updateForces(float deltaTime);
};

#endif