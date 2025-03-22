#ifndef SKIN_H
#define SKIN_H

#include "../GameObject.h"
#include "Skeleton.h"
#include "../Mesh.h"

#include "math/Vector.h"
#include "math/Matrix.h"

#include <vector>
#include <string>

typedef struct VertexWeightStruct
{
    unsigned int jointIndex;
    float weight;
} VertexWeight;

// typedef struct SkinVertexStruct
// {
//     Vector3f position;
//     Vector3f normal;
//     std::vector<VertexWeight> weights;
// } SkinVertex;

class Skin : public GameObject
{
public:
    Skin();
    virtual ~Skin();

    void update(float deltaTime);
    void draw(ShaderManager *ShaderManager);
    void drawWithShader(Shader *shader);

    bool load(const char *filename);

    void setSkeleton(Skeleton *skeleton);

protected:
    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<std::vector<VertexWeight>> weights;
    std::vector<Vector3i> triangles;
    std::vector<Matrix4x4f> bindings;

    Skeleton *skeleton = nullptr;
    Mesh mesh;
    Vector3f color;
};

#endif