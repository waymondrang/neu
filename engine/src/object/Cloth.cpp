#include "Cloth.h"

#include "../core.h"

void Cloth::updateVertices()
{
    for (int i = 0; i < particles.size(); i++)
    {
        vertices[i] = particles[i]->getPosition();
    }
}

void Cloth::updateNormals()
{
    ///////////////////
    // RESET NORMALS //
    ///////////////////

    for (int i = 0; i < normals.size(); i++)
    {
        normals[i] = Vector3f(0.0f, 0.0f, 0.0f);
    }

    for (int i = 0; i < height * particleDensity - 1; i++)
    {
        for (int j = 0; j < width * particleDensity - 1; j++)
        {
            // Logger::debug("Processing particle %d (%d, %d)\n", i * width * particleDensity + j, i, j);

            ///////////////////////
            // CALCULATE NORMALS //
            ///////////////////////

            Vector3f v1 = vertices[i * width * particleDensity + j];
            Vector3f v2 = vertices[(i + 1) * width * particleDensity + j];
            Vector3f v3 = vertices[(i + 1) * width * particleDensity + (j + 1)];

            Vector3f normal = cross(v2 - v1, v3 - v1);
            normal = normalize(normal);

            normals[i * width * particleDensity + j] += normal;
            normals[(i + 1) * width * particleDensity + j] += normal;
            normals[(i + 1) * width * particleDensity + (j + 1)] += normal;

            v1 = vertices[i * width * particleDensity + j];
            v2 = vertices[(i + 1) * width * particleDensity + (j + 1)];
            v3 = vertices[i * width * particleDensity + (j + 1)];

            normal = cross(v2 - v1, v3 - v1);
            normal = normalize(normal);

            normals[i * width * particleDensity + j] += normal;
            normals[(i + 1) * width * particleDensity + (j + 1)] += normal;
            normals[i * width * particleDensity + (j + 1)] += normal;
        }
    }

    ///////////////////////
    // NORMALIZE NORMALS //
    ///////////////////////

    for (int i = 0; i < normals.size(); i++)
    {
        normals[i] = normalize(normals[i]);
    }
}

// note: only call once!
void Cloth::calculateIndices(std::vector<Vector3i> &faces)
{
    for (int i = 0; i < height * particleDensity - 1; i++)
    {
        for (int j = 0; j < width * particleDensity - 1; j++)
        {
            Logger::debug("Processing particle %d (%d, %d)\n", i * width * particleDensity + j, i, j);

            ///////////////////////
            // CALCULATE INDICES //
            ///////////////////////

            Vector3i triangle1(i * width * particleDensity + j, (i + 1) * width * particleDensity + j, (i + 1) * width * particleDensity + (j + 1));
            Vector3i triangle2(i * width * particleDensity + j, (i + 1) * width * particleDensity + (j + 1), i * width * particleDensity + (j + 1));

            faces.push_back(triangle1);
            faces.push_back(triangle2);
        }
    }
}

void Cloth::updateMeshVectors()
{
    /////////////////////
    // UPDATE VERTICES //
    /////////////////////

    updateVertices();

    ////////////////////
    // UPDATE NORMALS //
    ////////////////////

    updateNormals();
}

Cloth::Cloth(int width, int height, int particleDensity)
    : width(width), height(height), particleDensity(particleDensity)
{
    Logger::info("Creating cloth with width: %d, height: %d, particle density: %d\n", width, height, particleDensity);

    //////////////////////
    // SET UP PARTICLES //
    //////////////////////

    for (int i = 0; i < height * particleDensity; i++)
    {
        for (int j = 0; j < width * particleDensity; j++)
        {
            Particle *particle = new Particle();
            particle->setPosition(Vector3f((float)width - (float)j / (float)particleDensity, (float)height - (float)i / (float)particleDensity, 0.0f));

            // anchor top corners
            if (i == 0)
                particle->setMass(0.0f);
            else
                particle->setMass(0.1f);

            particle->setDamping(0.90f);
            particle->setAcceleration(Vector3f(0.0f, -9.8f, 0.0f));
            particles.push_back(particle);
        }
    }

    ///////////////////
    // SET UP FORCES //
    ///////////////////

    for (int i = 0; i < height * particleDensity - 1; i++)
    {
        for (int j = 0; j < width * particleDensity - 1; j++)
        {
            Particle *p1 = particles[i * width * particleDensity + j];
            Particle *p2 = particles[(i + 1) * width * particleDensity + j];
            Particle *p3 = particles[(i + 1) * width * particleDensity + (j + 1)];
            Particle *p4 = particles[i * width * particleDensity + (j + 1)];

            // outer springs
            forces.push_back(new SpringDamperForce(p1, p2, &springConstant, &dampingConstant));
            forces.push_back(new SpringDamperForce(p1, p4, &springConstant, &dampingConstant));
            forces.push_back(new SpringDamperForce(p2, p3, &springConstant, &dampingConstant));
            forces.push_back(new SpringDamperForce(p3, p4, &springConstant, &dampingConstant));

            // inner springs
            forces.push_back(new SpringDamperForce(p1, p3, &springConstant, &dampingConstant));
            forces.push_back(new SpringDamperForce(p2, p4, &springConstant, &dampingConstant));
        }
    }

    /////////////////
    // SET UP MESH //
    /////////////////

    // std::vector<Vector3f> vertices;
    // std::vector<Vector3f> normals;

    vertices.resize(width * height * particleDensity * particleDensity);
    normals.resize(width * height * particleDensity * particleDensity);

    updateMeshVectors();

    //////////////////////
    // GENERATE INDICES //
    //////////////////////

    calculateIndices(faces);

    Logger::debug("Generated mesh vectors\n");

    //////////////////////////
    // CALL SETUP FUNCTIONS //
    //////////////////////////

    mesh.setupVertices(vertices);
    mesh.setupNormals(normals);
    mesh.setupFaces(faces);

    //////////////////////////
    // INITIALIZE TRANSFORM //
    //////////////////////////

    position = Vector3f(0.0f, 0.0f, 0.0f);
    rotation = Vector3f(0.0f, 0.0f, 0.0f);
    scale = Vector3f(1.0f, 1.0f, 1.0f);

    transform = createTransformMatrix(position, rotation, scale);

    ////////////////////
    // SET ATTRIBUTES //
    ////////////////////

    type = GameObjectType::CLOTH;
    // color = Vector3f(0.0f, 0.4118f, 0.5804f); // sea blue
    color = Vector3f(1.0f, 0.0f, 0.0f); // white
}

void Cloth::draw(ShaderManager *shaderManager)
{
    ///////////////
    // DRAW MESH //
    ///////////////

    Shader *shader = shaderManager->getShader("light");

    shader->use();
    shader->setVector3f("color", color);
    shader->setMatrix4x4f("model_matrix", transform);

    mesh.draw(shader);
}

void Cloth::drawWithShader(Shader *shader)
{
    ///////////////
    // DRAW MESH //
    ///////////////

    shader->use();
    shader->setVector3f("color", color);
    shader->setMatrix4x4f("model_matrix", transform);

    mesh.draw(shader);
}

void Cloth::update(float deltaTime)
{
    if (deltaTime <= 0.0f)
        return;

    // debug: use modified time
    deltaTime = fixedDeltaTime;

    /////////////////////////
    // UPDATE MESH VECTORS //
    /////////////////////////

    updateMeshVectors();

    mesh.updateVertices(vertices);
    mesh.updateNormals(normals);

    //////////////////////
    // UPDATE TRANSFORM //
    //////////////////////

    transform = createTransformMatrix(position, rotation, scale);

    ////////////////////
    // UPDATE PHYSICS //
    ////////////////////

    ///////////////////////
    // AERODYNAMIC FORCE //
    ///////////////////////

    for (int i = 0; i < faces.size(); i++)
    {
        Vector3f v1 = vertices[faces[i].x];
        Vector3f v2 = vertices[faces[i].y];
        Vector3f v3 = vertices[faces[i].z];

        Vector3f normal = cross(v2 - v1, v3 - v1);
        normal = normalize(normal);

        Vector3f v_surface = (v1 + v2 + v3) / 3.0f;
        Vector3f v = v_surface - windVelocity;

        const float fluidDensity = 1.225f;
        const float dragCoefficient = 1.28f;

        float area_0 = 0.5f * length(cross(v2 - v1, v3 - v1));
        float area = area_0 * dot(normalize(v), normal);

        for (int j = 0; j < 3; j++)
        {
            Vector3f f_aero = -0.5f * fluidDensity * length(v) * length(v) * area * dragCoefficient * normal;
            particles[faces[i][j]]->addForce(f_aero);
        }
    }

    for (int i = 0; i < forces.size(); i++)
    {
        forces[i]->applyForce();
    }

    for (int i = 0; i < particles.size(); i++)
    {
        particles[i]->integrate(deltaTime);
    }

    //////////////////////////
    // CHECK FOR COLLISIONS //
    //////////////////////////

    for (int i = 0; i < particles.size(); i++)
    {
        Particle *particle = particles[i];

        if (particle->getPosition().y < collisionPlaneHeight)
        {
            const Vector3f planeNormal = Vector3f(0.0f, 1.0f, 0.0f);
            const Vector3f planeVelocity = Vector3f(0.0f, 0.0f, 0.0f);
            const float collisionRestitution = 0.2f;

            float closingVelocity = dot(particle->getVelocity() - planeVelocity, planeNormal);

            Vector3f j = -(1.0f + collisionRestitution) * closingVelocity * planeNormal;
            j /= particle->getInverseMass();

            // apply impulse
            particle->addImpulse(j);

            const float bias = 0.001f;

            // adjust position
            Vector3f position = particle->getPosition();
            position.y = collisionPlaneHeight + bias;
            particle->setPosition(position);
        }
    }
}

void Cloth::reset()
{
    for (int i = 0; i < height * particleDensity; i++)
    {
        for (int j = 0; j < width * particleDensity; j++)
        {
            Particle *particle = particles[i * width * particleDensity + j];
            particle->setPosition(Vector3f((float)width - (float)j / (float)particleDensity, (float)height - (float)i / (float)particleDensity, 0.0f));
            particle->setVelocity(Vector3f(0.0f, 0.0f, 0.0f));
            particle->clearAccumulator();
        }
    }
}