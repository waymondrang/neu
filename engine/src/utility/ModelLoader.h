#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include "math/Vector.h"

#include <vector>
#include <string>

class ModelLoader
{
public:
    static void loadModel(const std::string path,
                          std::vector<Vector3f> &vertices,
                          std::vector<Vector3f> &normals,
                        //   std::vector<Vector3f> &colors,
                          std::vector<Vector3i> &faces);
};

#endif