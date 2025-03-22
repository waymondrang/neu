#ifndef MESH_H
#define MESH_H

#include "math/Vector.h"
#include "Shader.h"

#include <vector>

enum SetupFlags
{
    SETUP_VERTICES = 1 << 0,
    SETUP_NORMALS = 1 << 1,
    SETUP_COLORS = 1 << 2,
    SETUP_FACES = 1 << 3,
    SETUP_VAO = 1 << 4,
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void draw(Shader *shader);
    void draw(Shader *shader, float *modelMatrix);
    void drawInstanced(Shader *shader, unsigned int count);
    void deleteMesh();

    void setupVertices(std::vector<Vector3f> &vertices);
    void setupNormals(std::vector<Vector3f> &normals);
    void setupColors(std::vector<Vector3f> &colors);
    void setupFaces(std::vector<Vector3i> &faces);

    void updateVertices(std::vector<Vector3f> &vertices);
    void updateNormals(std::vector<Vector3f> &normals);
    void updateColors(std::vector<Vector3f> &colors);
    void updateFaces(std::vector<Vector3i> &faces);

    unsigned int getVAO() { return VAO; }

private:
    unsigned int setup = 0;
    unsigned int faceCount;

    unsigned int VAO;
    unsigned int positionVBO;
    unsigned int normalVBO;
    unsigned int colorVBO;
    unsigned int EBO;
};

#endif