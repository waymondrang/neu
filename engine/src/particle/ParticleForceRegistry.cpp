#include "ParticleForceRegistry.h"

void ParticleForceRegistry::add(Particle *particle, ParticleForceGenerator *fg)
{
    ParticleForceRegistration registration;
    registration.particle = particle;
    registration.fg = fg;
    registrations.push_back(registration);
}

void ParticleForceRegistry::remove(Particle *particle, ParticleForceGenerator *fg)
{
    for (Registry::iterator i = registrations.begin(); i != registrations.end(); i++)
    {
        if (i->particle == particle && i->fg == fg)
        {
            registrations.erase(i);
            return;
        }
    }
}

void ParticleForceRegistry::clear()
{
    registrations.clear();
}

void ParticleForceRegistry::updateForces(float deltaTime)
{
    for (Registry::iterator i = registrations.begin(); i != registrations.end(); i++)
    {
        i->fg->updateForce(i->particle, deltaTime);
    }
}