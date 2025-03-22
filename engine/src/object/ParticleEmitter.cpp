#include "ParticleEmitter.h"

#include "core.h"

#include "../ShaderManager.h"
#include "../Shader.h"

#include "../particle/LinkedListParticle.h"

#include "../utility/Profiler.h"
#include "../utility/ModelLoader.h"

#include <random>
#include <cassert>
#include <algorithm>

#define MAX_PARTICLES 10000

ParticleEmitter::ParticleEmitter()
{
    head = new LinkedListParticle();
    head->setNextParticle(nullptr);
    head->setPreviousParticle(head);

    ///////////////////////////////
    // CREATE MESH FOR PARTICLES //
    ///////////////////////////////

    float vertexScale = 0.1;

    // std::vector<Vector3f> vertices = {
    //     Vector3f(-1.0f, 1.0f, -1.0f) * vertexScale,
    //     Vector3f(1.0f, 1.0f, 1.0f) * vertexScale,
    //     Vector3f(1.0f, 1.0f, -1.0f) * vertexScale,
    //     Vector3f(-1.0f, -1.0f, 1.0f) * vertexScale,
    //     Vector3f(1.0f, -1.0f, 1.0f) * vertexScale,
    //     Vector3f(-1.0f, 1.0f, 1.0f) * vertexScale,
    //     Vector3f(-1.0f, -1.0f, -1.0f) * vertexScale,
    //     Vector3f(-1.0f, -1.0f, 1.0f) * vertexScale,
    //     Vector3f(1.0f, -1.0f, -1.0f) * vertexScale,
    //     Vector3f(-1.0f, -1.0f, 1.0f) * vertexScale,
    //     Vector3f(-1.0f, -1.0f, -1.0f) * vertexScale,
    //     Vector3f(-1.0f, 1.0f, -1.0f) * vertexScale,
    //     Vector3f(-1.0f, 1.0f, 1.0f) * vertexScale,
    //     Vector3f(-1.0f, 1.0f, 1.0f) * vertexScale,
    // };

    // std::vector<Vector3f> normals = {
    //     Vector3f(-0.57732844, 0.57736117, -0.57736117),
    //     Vector3f(0.57732844, 0.57736117, 0.57736117),
    //     Vector3f(0.5773721, 0.57733935, -0.57733935),
    //     Vector3f(-0.57732844, -0.57736117, 0.57736117),
    //     Vector3f(0.5773721, -0.57733935, 0.57733935),
    //     Vector3f(-0.5773721, 0.57733935, 0.57733935),
    //     Vector3f(-0.5773721, -0.57733935, -0.57733935),
    //     Vector3f(-0.57732844, -0.57736117, 0.57736117),
    //     Vector3f(0.57732844, -0.57736117, -0.57736117),
    //     Vector3f(-0.57732844, -0.57736117, 0.57736117),
    //     Vector3f(-0.5773721, -0.57733935, -0.57733935),
    //     Vector3f(-0.57732844, 0.57736117, -0.57736117),
    //     Vector3f(-0.5773721, 0.57733935, 0.57733935),
    //     Vector3f(-0.5773721, 0.57733935, 0.57733935),
    // };

    // std::vector<Vector3i> faces = {
    //     Vector3i(0, 1, 2),
    //     Vector3i(1, 3, 4),
    //     Vector3i(5, 6, 7),
    //     Vector3i(8, 9, 10),
    //     Vector3i(2, 4, 8),
    //     Vector3i(11, 8, 6),
    //     Vector3i(0, 12, 1),
    //     Vector3i(1, 13, 3),
    //     Vector3i(5, 11, 6),
    //     Vector3i(8, 4, 9),
    //     Vector3i(2, 1, 4),
    //     Vector3i(11, 2, 8),
    // };

    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Vector3i> faces;

    ModelLoader::loadModel("../assets/scenes/sphere.ply", vertices, normals, faces);

    particleMesh.setupVertices(vertices);
    particleMesh.setupNormals(normals);
    particleMesh.setupFaces(faces);

    /////////////////////////
    // SET UP INSTANCE VBO //
    /////////////////////////

    // todo: use glbufferdata to change size of buffer on change of particle creation rate

    glGenBuffers(1, &instanceTranslationVBO);
    glGenBuffers(1, &instanceRadiiVBO);
    glGenBuffers(1, &instanceColorVBO);

    glBindVertexArray(particleMesh.getVAO());

    glBindBuffer(GL_ARRAY_BUFFER, instanceTranslationVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f) * MAX_PARTICLES, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void *)0);
    glVertexAttribDivisor(3, 1);

    glBindBuffer(GL_ARRAY_BUFFER, instanceRadiiVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_PARTICLES, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(float), (void *)0);
    glVertexAttribDivisor(4, 1);

    glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4f) * MAX_PARTICLES, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vector4f), (void *)0);
    glVertexAttribDivisor(5, 1);

    // todo: add buffer for color
    // todo: add variance for size

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    ////////////////////////////
    // GAME OBJECT PROPERTIES //
    ////////////////////////////

    // todo: same for translation and scales array as above

    translations.reserve(MAX_PARTICLES);
    radii.reserve(MAX_PARTICLES);
    colors.reserve(MAX_PARTICLES);

    name = "particleEmitter";
    type = GameObjectType::PARTICLE_EMITTER;
}

void ParticleEmitter::update(float deltaTime)
{
    PROFILE_START;

    ///////////////////////////
    // UPDATE SELF TRANSFORM //
    ///////////////////////////

    transform = createTransformMatrix(position, rotation, scale);

    /////////////////////////
    // SPAWN NEW PARTICLES //
    /////////////////////////

    int particlesToSpawn = floorf(timeSinceLastSpawn * particleCreationRate);

    if (particlesToSpawn > 0)
    {
        // https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> negativePositionDistribution(-1.0f, 1.0f);

        for (int i = 0; i < particlesToSpawn; i++)
        {
            if (particles.size() >= MAX_PARTICLES)
                break;

            LinkedListParticle newParticle = LinkedListParticle();

            newParticle.setAcceleration(gravity);
            newParticle.setMass(particleMass);
            newParticle.setDamping(0.9f);

            newParticle.setCollisionElasticity(collisionElasticity);
            newParticle.setCollisionFriction(collisionFriction);
            newParticle.setAirDensityReference(&airDensity);
            newParticle.setDragCoefficientReference(&dragCoefficient);

            ////////////////////////
            // VARIANT PROPERTIES //
            ////////////////////////

            Vector4f randomParticleColor = particleColor + Vector4f(negativePositionDistribution(gen) * particleColorRedVariance,
                                                                    negativePositionDistribution(gen) * particleColorGreenVariance,
                                                                    negativePositionDistribution(gen) * particleColorBlueVariance,
                                                                    negativePositionDistribution(gen) * particleColorAlphaVariance);
            float uniformColorVariance = negativePositionDistribution(gen) * particleColorUniformVariance;

            randomParticleColor.x = std::clamp(randomParticleColor.x + uniformColorVariance, 0.0f, 1.0f);
            randomParticleColor.y = std::clamp(randomParticleColor.y + uniformColorVariance, 0.0f, 1.0f);
            randomParticleColor.z = std::clamp(randomParticleColor.z + uniformColorVariance, 0.0f, 1.0f);
            randomParticleColor.w = std::clamp(randomParticleColor.w + uniformColorVariance, 0.0f, 1.0f);

            newParticle.setColor(randomParticleColor);

            newParticle.setRadius(std::max(particleRadiusLowerBound, particleRadius + negativePositionDistribution(gen) * particleRadiusVariance));
            newParticle.setVelocity(initialVelocity + Vector3f(negativePositionDistribution(gen) * initialVelocityVariance.x, negativePositionDistribution(gen) * initialVelocityVariance.y, negativePositionDistribution(gen) * initialVelocityVariance.z));
            newParticle.setPosition(initialPosition + Vector3f(negativePositionDistribution(gen) * initialPositionVariance.x, negativePositionDistribution(gen) * initialPositionVariance.y, negativePositionDistribution(gen) * initialPositionVariance.z));
            newParticle.setExpirationTime(initialLifeSpan + negativePositionDistribution(gen) * initialLifeSpanVariance);

            particles.push_back(newParticle);
        }

        timeSinceLastSpawn = 0;
    }
    else
    {
        if (particleCreationRate > 0)
            timeSinceLastSpawn += deltaTime;
        else
            timeSinceLastSpawn = 0;
    }

    //////////////////////////
    // UPDATE ALL PARTICLES //
    //////////////////////////

    translations.clear();
    radii.clear();
    colors.clear();

    for (std::list<LinkedListParticle>::iterator it = particles.begin(); it != particles.end();)
    {
        bool isExpired = (*it).update(deltaTime);

        if (isExpired)
        {
            particles.erase(it++);
            continue;
        }

        translations.push_back((*it).getPosition());
        radii.push_back((*it).getRadius());
        colors.push_back((*it).getColor());

        it++;
    }

    assert(translations.size() <= MAX_PARTICLES);

    /////////////////////////
    // UPDATE INSTANCE VBO //
    /////////////////////////

    glBindBuffer(GL_ARRAY_BUFFER, instanceTranslationVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector3f) * translations.size(), translations.data());

    glBindBuffer(GL_ARRAY_BUFFER, instanceRadiiVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * radii.size(), radii.data());

    glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector4f) * colors.size(), colors.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    PROFILE_STOP;
}

void ParticleEmitter::draw(ShaderManager *shaderManager)
{
    PROFILE_START;

    Shader *shader = shaderManager->getShader("particle");
    shader->use();
    shader->setMatrix4x4f("model_matrix", transform);

    particleMesh.drawInstanced(shader, unsigned int(particles.size()));

    PROFILE_STOP;
}

void ParticleEmitter::drawWithShader(Shader *shader)
{
    Logger::debug("Not implemented\n");

    // shader->use();

    // std::vector<Matrix4x4f> translations;

    // if (head->getNextParticle())
    //     head->getNextParticle()->collectTranslation(translations);
    // // head->getNextParticle()->drawMeshWithShader(&particleMesh, shader);

    // // Logger::debug("collected %d translations\n", translations.size());

    // // could collect size from translations

    // assert(translations.size() <= MAX_PARTICLES);

    // glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Matrix4x4f) * translations.size(), translations.data());
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // particleMesh.drawInstanced(shader, translations.size());
}

void ParticleEmitter::reset()
{
    particles.clear();

    timeSinceLastSpawn = 0;
}