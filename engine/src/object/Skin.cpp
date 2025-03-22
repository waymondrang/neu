#include "Skin.h"

#include <cassert>

#include "../core.h"
#include "../utility/Tokenizer.h"

Skin::Skin()
{
    position = Vector3f(0.0f, 0.0f, 0.0f);
    rotation = Vector3f(0.0f, 0.0f, 0.0f);
    scale = Vector3f(1.0f, 1.0f, 1.0f);

    color = Vector3f(1.0f, 1.0f, 1.0f);

    transform = Matrix4x4f::identity();

    type = GameObjectType::SKIN;
}

Skin::~Skin()
{
}

void Skin::update(float deltaTime)
{
    transform = createTransformMatrix(position, rotation, scale);

    if (skeleton)
    {
        std::vector<Vector3f> updatedVertices = vertices;
        std::vector<Vector3f> updatedNormals = normals;

        // TODO: compute joint pointer array in Skeleton class instead of here
        std::vector<Joint *> joints = skeleton->collectAllJoints();

        updatedVertices.resize(vertices.size());
        updatedNormals.resize(normals.size());

        for (unsigned int i = 0; i < vertices.size(); i++)
        {
            updatedVertices[i] = Vector3f(0.0f, 0.0f, 0.0f);
            updatedNormals[i] = Vector3f(0.0f, 0.0f, 0.0f);

            for (unsigned int j = 0; j < weights[i].size(); j++)
            {
                updatedVertices[i] += Vector3f(weights[i][j].weight * (joints[weights[i][j].jointIndex]->getTransform() * bindings[weights[i][j].jointIndex] * Vector4f(vertices[i], 1.0f)));
                updatedNormals[i] += Vector3f(weights[i][j].weight * (transpose(inverse(joints[weights[i][j].jointIndex]->getTransform())) * Vector4f(normals[i], 0.0f)));
            }

            // normalize normals
            updatedNormals[i] = normalize(updatedNormals[i]);
        }

        mesh.updateVertices(updatedVertices);
        mesh.updateNormals(updatedNormals);
    }
}

void Skin::setSkeleton(Skeleton *skeleton)
{
    this->skeleton = skeleton;
}

void Skin::draw(ShaderManager *ShaderManager)
{
    Shader *shader = ShaderManager->getShader("light");

    shader->use();
    shader->setVector3f("color", color);
    shader->setMatrix4x4f("model_matrix", transform);

    mesh.draw(shader);
}

void Skin::drawWithShader(Shader *shader)
{
    shader->use();
    shader->setVector3f("color", color);
    shader->setMatrix4x4f("model_matrix", transform);

    mesh.draw(shader);
}

bool Skin::load(const char *filename)
{
    Logger::info("Loading skin from file: %s\n", filename);

    Tokenizer tokenizer;

    if (!tokenizer.Open(filename))
    {
        Logger::error("Failed to open file: %s\n", filename);
        return false;
    }

    ///////////////
    // POSITIONS //
    ///////////////

    if (!tokenizer.FindToken("positions"))
    {
        Logger::error("Failed to find token \"positions\" in skin file: %s\n", filename);
        return false;
    }

    unsigned int numVertices = (unsigned int)tokenizer.GetInt();
    tokenizer.SkipLine();

    vertices.resize(numVertices);

    for (unsigned int i = 0; i < numVertices; i++)
    {
        vertices[i].x = tokenizer.GetFloat();
        vertices[i].y = tokenizer.GetFloat();
        vertices[i].z = tokenizer.GetFloat();

        // DEBUG: CHECK FOR DUPLICATE VERTICES
        for (unsigned int j = 0; j < i; j++)
        {
            if (vertices[i] == vertices[j])
            {
                Logger::warn("Duplicate vertex found at index %d (%f %f %f)\n", i, vertices[i].x, vertices[i].y, vertices[i].z);
            }
        }
    }

    /////////////
    // NORMALS //
    /////////////

    if (!tokenizer.FindToken("normals"))
    {
        Logger::error("Failed to find token \"normals\" in skin file: %s\n", filename);
        return false;
    }

    tokenizer.SkipLine();

    normals.resize(numVertices);

    for (unsigned int i = 0; i < numVertices; i++)
    {
        normals[i].x = tokenizer.GetFloat();
        normals[i].y = tokenizer.GetFloat();
        normals[i].z = tokenizer.GetFloat();
    }

    //////////////////
    // SKIN WEIGHTS //
    //////////////////

    if (!tokenizer.FindToken("skinweights"))
    {
        Logger::error("Failed to find token \"skinweights\" in skin file: %s\n", filename);
        return false;
    }

    tokenizer.SkipLine();

    weights.resize(numVertices);

    for (unsigned int i = 0; i < numVertices; i++)
    {
        unsigned int numAttachments = (unsigned int)tokenizer.GetInt();
        for (unsigned int j = 0; j < numAttachments; j++)
        {
            VertexWeight vertexWeight;
            vertexWeight.jointIndex = (unsigned int)tokenizer.GetInt();
            vertexWeight.weight = tokenizer.GetFloat();
            weights[i].push_back(vertexWeight);
        }
    }

    ///////////////
    // TRIANGLES //
    ///////////////

    if (!tokenizer.FindToken("triangles"))
    {
        Logger::error("Failed to find token \"triangles\" in skin file: %s\n", filename);
        return false;
    }

    unsigned int numTriangles = (unsigned int)tokenizer.GetInt();
    tokenizer.SkipLine();

    for (unsigned int i = 0; i < numTriangles; i++)
    {
        Vector3i triangle = Vector3i(tokenizer.GetInt(), tokenizer.GetInt(), tokenizer.GetInt());
        triangles.push_back(triangle);

        // DEBUG: CHECK FOR DUPLICATE TRIANGLES
        for (unsigned int j = 0; j < i; j++)
        {
            if (triangles[i] == triangles[j])
            {
                Logger::warn("Duplicate triangle found at index %d (%d %d %d)\n", i, triangles[i].x, triangles[i].y, triangles[i].z);
            }
        }
    }

    //////////////
    // BINDINGS //
    //////////////

    if (!tokenizer.FindToken("bindings"))
    {
        Logger::error("Failed to find token \"bindings\" in skin file: %s\n", filename);
        return false;
    }

    unsigned int numJoints = (unsigned int)tokenizer.GetInt();
    tokenizer.SkipLine();

    bindings.resize(numJoints);
    for (unsigned int i = 0; i < numJoints; i++)
    {
        tokenizer.FindToken("matrix");
        tokenizer.FindToken("{");

        bindings[i] = Matrix4x4f::identity();

        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                bindings[i](k, j) = tokenizer.GetFloat();
            }
        }

        bindings[i] = inverse(bindings[i]);

        tokenizer.FindToken("}");
    }

    // debug: print bindings
    for (unsigned int i = 0; i < numJoints; i++)
    {
        Logger::debug("Binding %d:\n", i);
        Logger::debug("%f %f %f %f\n", bindings[i](0, 0), bindings[i](1, 0), bindings[i](2, 0), bindings[i](3, 0));
        Logger::debug("%f %f %f %f\n", bindings[i](0, 1), bindings[i](1, 1), bindings[i](2, 1), bindings[i](3, 1));
        Logger::debug("%f %f %f %f\n", bindings[i](0, 2), bindings[i](1, 2), bindings[i](2, 2), bindings[i](3, 2));
        Logger::debug("%f %f %f %f\n", bindings[i](0, 3), bindings[i](1, 3), bindings[i](2, 3), bindings[i](3, 3));
    }

    tokenizer.Close();

    ////////////////
    // SETUP MESH //
    ////////////////

    mesh.setupVertices(vertices);
    mesh.setupNormals(vertices);
    mesh.setupFaces(triangles);

    Logger::info("Loaded skin from file: %s\n", filename);

    return true;
}