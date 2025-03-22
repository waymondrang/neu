#include "LinkedListParticle.h"

#include "../core.h"

#include "../ShaderManager.h"
#include "../Shader.h"
#include <math.h>

LinkedListParticle::LinkedListParticle()
{
    nextParticle = nullptr;
    previousParticle = nullptr;

    accumulatedTime = 0;
}

/**
 * Returns true if the particle is expired.
 */
bool LinkedListParticle::update(float deltaTime)
{
    accumulatedTime += deltaTime;

    if (accumulatedTime >= expirationTime)
        return true;

    integrate(deltaTime);

    transform = Matrix4x4f::identity();
    transform(0, 3) = position.x;
    transform(1, 3) = position.y;
    transform(2, 3) = position.z;

    const float groundPlaneY = 0.0f; // hard code ground plane properties

    if ((position.y - radius) <= groundPlaneY)
    {
        const Vector3f planeNormal = Vector3f(0.0f, 1.0f, 0.0f); // hard code plane properties (lol)
        const Vector3f planeVelocity = Vector3f(0.0f, 0.0f, 0.0f);

        float closingVelocity = dot(velocity - planeVelocity, planeNormal);

        Vector3f j = -(1.0f + collisionElasticity) * closingVelocity * planeNormal;
        j /= inverseMass;

        // apply impulse
        addImpulse(j);

        // reset position
        const float bias = 0.001f;
        position.y = groundPlaneY + radius + bias;

        /////////////////////////////
        // FRICTION FROM COLLISION //
        /////////////////////////////

        if (length(velocity) > 0.0f)
        {
            Vector3f friction = -normalize(velocity) * collisionFriction;
            addForce(friction);
        }
    }
    else
    {
        //////////////////////////////////////////////////////////////////////////////
        // IF NOT TOUCHING GROUND PLANE, PARTICLE IS IN AIR; APPLY AERODYNAMIC DRAG //
        //////////////////////////////////////////////////////////////////////////////

        // Vector3f drag = -normalize(velocity) * 0.5f * airDensity * dragCoefficient * M_PI * radius * radius * length(velocity) * length(velocity);
        Vector3f drag = 0.5f * (*airDensityReference) * length_squared(velocity) * (*dragCoefficientReference) * 3.14159265f * radius * radius * -normalize(velocity); // approx pi
        addForce(drag);
    }

    return false;
}

void LinkedListParticle::update(float deltaTime, int &particleCount)
{
    // debug: fixed deltatime
    // deltaTime = 0.01;

    /////////////////////////////
    // HANDLE EXPIRED PARTICLE //
    /////////////////////////////

    accumulatedTime += deltaTime;

    if (accumulatedTime >= expirationTime)
    {
        if (nextParticle)
        {
            // note: stupid me put these in the wrong order
            nextParticle->setPreviousParticle(previousParticle);
            previousParticle->setNextParticle(nextParticle);

            // note: stupid me named the same function
            nextParticle->update(deltaTime, particleCount);
        }
        else
        {
            previousParticle->setNextParticle(nullptr);
        }

        particleCount--;

        free(this);

        return;
    }

    ///////////////////////////
    // INTEGRATE AND ITERATE //
    ///////////////////////////

    integrate(deltaTime);

    transform = Matrix4x4f::identity();
    transform(0, 3) = position.x;
    transform(1, 3) = position.y;
    transform(2, 3) = position.z;

    if (nextParticle)
        nextParticle->update(deltaTime, particleCount);
}

void LinkedListParticle::drawMeshWithShader(Mesh *mesh, Shader *shader)
{
    //////////////////
    // PERFORM DRAW //
    //////////////////

    shader->setMatrix4x4f("model_matrix", transform);
    // shader->setVector3f("color", color);

    mesh->draw(shader);

    if (nextParticle)
        nextParticle->drawMeshWithShader(mesh, shader);
}