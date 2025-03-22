#include "Joint.h"
#include "core.h"

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        Logger::error("OpenGL error: %s (%s:%d)\n", error.c_str(), file, line);
    }
    return errorCode;
}

#define glCheckError() glCheckError_(__FILE__, __LINE__)

Joint::Joint(GameObject *parent)
{
    this->parent = parent;

    // offset = Vector3f(0.0f, 0.0f, 0.0f); // USE POSITION
    boxmin = Vector3f(-1.0f, -1.0f, -1.0f);
    boxmax = Vector3f(1.0f, 1.0f, 1.0f);
    // pose = Vector3f(0.0f, 0.0f, 0.0f); // USE ROTATION
    rotxlimit = Vector2f(-6.28318530718f, 6.28318530718f);
    rotylimit = Vector2f(-6.28318530718f, 6.28318530718f);
    rotzlimit = Vector2f(-6.28318530718f, 6.28318530718f);
    // rotxlimit = Vector2f(-100000.0f, 100000.0f);
    // rotylimit = Vector2f(-100000.0f, 100000.0f);
    // rotzlimit = Vector2f(-100000.0f, 100000.0f);

    position = Vector3f(0.0f, 0.0f, 0.0f);
    rotation = Vector3f(0.0f, 0.0f, 0.0f);
    scale = Vector3f(1.0f, 1.0f, 1.0f);

    type = GameObjectType::JOINT;
}

Joint::~Joint()
{
    for (Joint *child : children)
    {
        delete child;
    }
}

void Joint::load(Tokenizer &tokenizer)
{
    // get joint name
    char token[256];
    tokenizer.GetToken(token);
    name = token;

    tokenizer.FindToken("{");
    while (true)
    {
        char token[256];
        tokenizer.GetToken(token);

        if (strcmp(token, "offset") == 0)
        {
            position.x = tokenizer.GetFloat();
            position.y = tokenizer.GetFloat();
            position.z = tokenizer.GetFloat();
        }
        else if (strcmp(token, "boxmin") == 0)
        {
            boxmin.x = tokenizer.GetFloat();
            boxmin.y = tokenizer.GetFloat();
            boxmin.z = tokenizer.GetFloat();
        }
        else if (strcmp(token, "boxmax") == 0)
        {
            boxmax.x = tokenizer.GetFloat();
            boxmax.y = tokenizer.GetFloat();
            boxmax.z = tokenizer.GetFloat();
        }
        else if (strcmp(token, "rotxlimit") == 0)
        {
            rotxlimit.x = tokenizer.GetFloat();
            rotxlimit.y = tokenizer.GetFloat();
        }
        else if (strcmp(token, "rotylimit") == 0)
        {
            rotylimit.x = tokenizer.GetFloat();
            rotylimit.y = tokenizer.GetFloat();
        }
        else if (strcmp(token, "rotzlimit") == 0)
        {
            rotzlimit.x = tokenizer.GetFloat();
            rotzlimit.y = tokenizer.GetFloat();
        }
        else if (strcmp(token, "pose") == 0)
        {
            rotation.x = tokenizer.GetFloat();
            rotation.y = tokenizer.GetFloat();
            rotation.z = tokenizer.GetFloat();
        }
        else if (strcmp(token, "balljoint") == 0)
        {
            Joint *child = new Joint(this);
            child->load(tokenizer);
            children.push_back(child);
        }
        else if (strcmp(token, "}") == 0)
        {
            break;
        }
        else
        {
            Logger::warn("Skipping unknown token in joint: %s\n", token);
            tokenizer.SkipLine();
        }
    }

    // check pose limits
    if (rotation.x < rotxlimit.x)
    {
        Logger::warn("Pose x beyond lower limit: %f\n", rotation.x);
        rotation.x = rotxlimit.x;
    }

    if (rotation.x > rotxlimit.y)
    {
        Logger::warn("Pose x beyond upper limit: %f\n", rotation.x);
        rotation.x = rotxlimit.y;
    }

    if (rotation.y < rotylimit.x)
    {
        Logger::warn("Pose y beyond lower limit: %f\n", rotation.y);
        rotation.y = rotylimit.x;
    }

    if (rotation.y > rotylimit.y)
    {
        Logger::warn("Pose y beyond upper limit: %f\n", rotation.y);
        rotation.y = rotylimit.y;
    }

    if (rotation.z < rotzlimit.x)
    {
        Logger::warn("Pose z beyond lower limit: %f\n", rotation.z);
        rotation.z = rotzlimit.x;
    }

    if (rotation.z > rotzlimit.y)
    {
        Logger::warn("Pose z beyond upper limit: %f\n", rotation.z);
        rotation.z = rotzlimit.y;
    }

    // print out pose vector
    Logger::debug("Pose: %f %f %f\n", rotation.x, rotation.y, rotation.z);

    // create vertices
    std::vector<Vector3f> vertices = {
        // left wall
        Vector3f(boxmin.x, boxmin.y, boxmin.z),
        Vector3f(boxmin.x, boxmin.y, boxmax.z),
        Vector3f(boxmin.x, boxmax.y, boxmin.z),
        Vector3f(boxmin.x, boxmax.y, boxmax.z),

        // right wall
        Vector3f(boxmax.x, boxmin.y, boxmin.z),
        Vector3f(boxmax.x, boxmin.y, boxmax.z),
        Vector3f(boxmax.x, boxmax.y, boxmin.z),
        Vector3f(boxmax.x, boxmax.y, boxmax.z),
    };

    std::vector<Vector2i> indices = {
        Vector2i(0, 1),
        Vector2i(1, 3),
        Vector2i(3, 2),
        Vector2i(2, 0),
        Vector2i(4, 5),
        Vector2i(5, 7),
        Vector2i(7, 6),
        Vector2i(6, 4),
        Vector2i(0, 4),
        Vector2i(1, 5),
        Vector2i(2, 6),
        Vector2i(3, 7),
    };

    // create vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // create vbo
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3f), vertices.data(), GL_STATIC_DRAW);

    // create ebo
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Vector2i), indices.data(), GL_STATIC_DRAW);

    // set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Joint::update(float deltaTime)
{
    localTransform = createTransformMatrix(position, rotation, scale);

    transform = parent->getTransform() * localTransform;

    for (Joint *child : children)
    {
        child->update(deltaTime);
    }
}

void Joint::draw(ShaderManager *shaderManager)
{
    // color set by skeleton
    Shader *shader = shaderManager->getShader("simple");

    shader->use();
    shader->setMatrix4x4f("model_matrix", transform);

    glBindVertexArray(vao);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0); // indices.size() * 2
    glBindVertexArray(0);

    for (Joint *child : children)
    {
        child->draw(shaderManager);
    }
}

void Joint::drawWithShader(Shader *shader)
{
    shader->use();
    shader->setMatrix4x4f("model_matrix", transform);

    glBindVertexArray(vao);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0); // indices.size() * 2
    glBindVertexArray(0);

    for (Joint *child : children)
    {
        child->drawWithShader(shader);
    }
}

void Joint::collectAllJoints(std::vector<Joint *> &joints)
{
    joints.push_back(this);

    for (Joint *child : children)
        child->collectAllJoints(joints);
}