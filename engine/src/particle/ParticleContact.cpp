#include "ParticleContact.h"

void ParticleContact::resolve(float deltaTime)
{
    resolveVelocity(deltaTime);
    resolveInterpenetration(deltaTime);
}

float ParticleContact::calculateSeparatingVelocity()
{
    Vector3f relativeVelocity = particle[0]->getVelocity();
    if (particle[1])
    {
        relativeVelocity -= particle[1]->getVelocity();
    }
    return dot(relativeVelocity, contactNormal);
}

void ParticleContact::resolveVelocity(float deltaTime)
{
    // find velocity in direction of contact
    float separatingVelocity = calculateSeparatingVelocity();

    // check if it needs to be resolved
    if (separatingVelocity > 0)
    {
        // contact is either separating or stationary
        return;
    }

    // calculate new separating velocity
    float newSepVelocity = -separatingVelocity * restitution;

    // check the velocity build-up due to acceleration only
    Vector3f accCausedVelocity = particle[0]->getAcceleration();
    if (particle[1])
    {
        accCausedVelocity -= particle[1]->getAcceleration();
    }
    float accCausedSepVelocity = dot(accCausedVelocity, contactNormal) * deltaTime;

    // if we've got a closing velocity due to acceleration build-up, remove it from the new separating velocity
    if (accCausedSepVelocity < 0)
    {
        newSepVelocity += restitution * accCausedSepVelocity;

        // make sure we haven't removed more than there was to remove
        if (newSepVelocity < 0)
        {
            newSepVelocity = 0;
        }
    }

    float deltaVelocity = newSepVelocity - separatingVelocity;

    // apply change in velocity to each object in proportion to their inverse mass
    float totalInverseMass = particle[0]->getInverseMass();
    if (particle[1])
    {
        totalInverseMass += particle[1]->getInverseMass();
    }

    // if all particles have infinite mass, do nothing
    if (totalInverseMass <= 0)
    {
        return;
    }

    // calculate impulse to apply
    float impulse = deltaVelocity / totalInverseMass;

    // find amount of impulse per unit of inverse mass
    Vector3f impulsePerIMass = contactNormal * impulse;

    // apply impulses
    particle[0]->setVelocity(particle[0]->getVelocity() + impulsePerIMass * particle[0]->getInverseMass());

    if (particle[1])
    {
        particle[1]->setVelocity(particle[1]->getVelocity() + impulsePerIMass * -particle[1]->getInverseMass());
    }
}

void ParticleContact::resolveInterpenetration(float deltaTime)
{
    if (penetration <= 0)
    {
        return;
    }

    float totalInverseMass = particle[0]->getInverseMass();
    if (particle[1])
    {
        totalInverseMass += particle[1]->getInverseMass();
    }

    if (totalInverseMass <= 0)
    {
        return;
    }

    Vector3f movePerIMass = contactNormal * (penetration / totalInverseMass);

    Vector3f particleMovement1;
    Vector3f particleMovement2;

    particleMovement1 = movePerIMass * particle[0]->getInverseMass();
    if (particle[1])
    {
        particleMovement2 = movePerIMass * -particle[1]->getInverseMass();
    }

    particle[0]->setPosition(particle[0]->getPosition() + particleMovement1);
    if (particle[1])
    {
        particle[1]->setPosition(particle[1]->getPosition() + particleMovement2);
    }
}