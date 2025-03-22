#include "Mesh.h"
#include "core.h"
#include "math/Matrix.h"
#include "utility/ModelLoader.h"
#include "Shader.h"

#include "utility/Profiler.h"

Mesh::Mesh()
{
    glGenVertexArrays(1, &VAO);
}

Mesh::~Mesh()
{
    deleteMesh();
}

void Mesh::draw(Shader *shader)
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)(faceCount * 3), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::drawInstanced(Shader *shader, unsigned int count)
{
    PROFILE_START;

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)(faceCount * 3), GL_UNSIGNED_INT, 0, count);
    glBindVertexArray(0);

    PROFILE_STOP;
}

void Mesh::draw(Shader *shader, float *modelMatrix)
{
    shader->setMatrix4x4f("model_matrix", modelMatrix);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)(faceCount * 3), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::deleteMesh()
{
    glDeleteVertexArrays(1, &VAO);

    if (setup & SETUP_VERTICES)
        glDeleteBuffers(1, &positionVBO);
    if (setup & SETUP_NORMALS)
        glDeleteBuffers(1, &normalVBO);
    if (setup & SETUP_COLORS)
        glDeleteBuffers(1, &colorVBO);
    if (setup & SETUP_FACES)
        glDeleteBuffers(1, &EBO);
}

void Mesh::setupVertices(std::vector<Vector3f> &vertices)
{
    if (setup & SETUP_VERTICES)
    {
        Logger::error("Tried to re-setup vertex array buffer\n");
        return;
    }

    // generate vertex array object
    glGenBuffers(1, &positionVBO);

    // bind vertex buffer object
    glBindVertexArray(VAO);

    // bind vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3f), vertices.data(), GL_STATIC_DRAW);

    // set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void *)0);
    glEnableVertexAttribArray(0);

    // unbind vertex array object
    glBindVertexArray(0);

    setup |= SETUP_VERTICES;
}

void Mesh::setupNormals(std::vector<Vector3f> &normals)
{
    if (setup & SETUP_NORMALS)
    {
        Logger::error("Tried to re-setup normal array buffer\n");
        return;
    }

    // generate vertex array object
    glGenBuffers(1, &normalVBO);

    // bind vertex buffer object
    glBindVertexArray(VAO);

    // bind vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vector3f), normals.data(), GL_STATIC_DRAW);

    // set vertex attribute pointers
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void *)0);
    glEnableVertexAttribArray(1);

    // unbind vertex array object
    glBindVertexArray(0);

    setup |= SETUP_NORMALS;
}

void Mesh::setupColors(std::vector<Vector3f> &colors)
{
    if (setup & SETUP_COLORS)
    {
        Logger::error("Tried to re-setup color array buffer\n");
        return;
    }

    // generate vertex array object
    glGenBuffers(1, &colorVBO);

    // bind vertex buffer object
    glBindVertexArray(VAO);

    // bind vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Vector3f), colors.data(), GL_STATIC_DRAW);

    // set vertex attribute pointers
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void *)0);
    glEnableVertexAttribArray(2);

    // unbind vertex array object
    glBindVertexArray(0);

    setup |= SETUP_COLORS;
}

void Mesh::setupFaces(std::vector<Vector3i> &faces)
{
    if (setup & SETUP_FACES)
    {
        Logger::error("Tried to re-setup face array buffer\n");
        return;
    }

    faceCount = faces.size();

    // generate vertex array object
    glGenBuffers(1, &EBO);

    // bind vertex buffer object
    glBindVertexArray(VAO);

    // bind vertex buffer object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceCount * sizeof(Vector3i), faces.data(), GL_STATIC_DRAW);

    // unbind vertex array object
    glBindVertexArray(0);

    setup |= SETUP_FACES;
}

void Mesh::updateVertices(std::vector<Vector3f> &vertices)
{
    if (!(setup & SETUP_VERTICES))
    {
        Logger::error("Tried to update vertices without setting up vertices\n");
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vector3f), vertices.data());
}

void Mesh::updateNormals(std::vector<Vector3f> &normals)
{
    if (!(setup & SETUP_NORMALS))
    {
        Logger::error("Tried to update normals without setting up normals\n");
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, normals.size() * sizeof(Vector3f), normals.data());
}

void Mesh::updateColors(std::vector<Vector3f> &colors)
{
    if (!(setup & SETUP_COLORS))
    {
        Logger::error("Tried to update colors without setting up colors\n");
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size() * sizeof(Vector3f), colors.data());
}

void Mesh::updateFaces(std::vector<Vector3i> &faces)
{
    if (!(setup & SETUP_FACES))
    {
        Logger::error("Tried to update faces without setting up faces\n");
        return;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, faces.size() * sizeof(Vector3i), faces.data());
}
