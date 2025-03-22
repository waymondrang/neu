#include "ParticleContactResolver.h"

ParticleContactResolver::ParticleContactResolver(unsigned int iterations) : iterations(iterations) {}

void ParticleContactResolver::setIterations(unsigned int iterations)
{
    this->iterations = iterations;
}

void ParticleContactResolver::resolveContacts(ParticleContact *contactArray, unsigned int numContacts, float duration)
{
    iterationsUsed = 0;

    while (iterationsUsed < iterations)
    {
        // find the contact with the largest closing velocity
#ifdef __APPLE__
        float max = __FLT_MAX__;
#else
        float max = FLT_MAX;
#endif

        unsigned int maxIndex = numContacts;

        for (int i = 0; i < numContacts; i++)
        {
            float separationVelocity = contactArray[i].calculateSeparatingVelocity();
            if (separationVelocity < max && (separationVelocity < 0 || contactArray[i].penetration > 0))
            {
                max = separationVelocity;
                maxIndex = i;
            }
        }

        if (maxIndex == numContacts)
            break;

        contactArray[maxIndex].resolve(duration);

        iterationsUsed++;
    }
}