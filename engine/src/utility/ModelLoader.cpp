#include "ModelLoader.h"

#include <tinyply/tinyply.h>

#include <fstream>
#include <vector>
#include <string>
#include <memory>

#include "../core.h"

// clang-format off
struct float_3 { float x, y, z; };
struct uint_3 { unsigned int x, y, z; };
struct uint8_3 { uint8_t x, y, z; };
// clang-format on

void ModelLoader::loadModel(const std::string path,
                            std::vector<Vector3f> &vertexData,
                            std::vector<Vector3f> &normalData,
                            // std::vector<Vector3f> &colorData,
                            std::vector<Vector3i> &faceData)
{
    Logger::info("Loading model: %s\n", path.c_str());

    // https://github.com/ddiakopoulos/tinyply/blob/master/source/example-utils.hpp
    std::ifstream fileStream(path, std::ios::binary);
    tinyply::PlyFile plyFile;

    plyFile.parse_header(fileStream);

    std::shared_ptr<tinyply::PlyData> vertices, normals, colors, faces;

    try
    {
        vertices = plyFile.request_properties_from_element("vertex", {"x", "y", "z"});
    }
    catch (const std::exception &e)
    {
        std::cerr << "tinyply exception: " << e.what() << std::endl;
    }

    try
    {
        normals = plyFile.request_properties_from_element("vertex", {"nx", "ny", "nz"});
    }
    catch (const std::exception &e)
    {
        std::cerr << "tinyply exception: " << e.what() << std::endl;
    }

    try
    {
        colors = plyFile.request_properties_from_element("vertex", {"red", "green", "blue"});
    }
    catch (const std::exception &e)
    {
        std::cerr << "tinyply exception: " << e.what() << std::endl;
    }

    try
    {
        faces = plyFile.request_properties_from_element("face", {"vertex_indices"});
    }
    catch (const std::exception &e)
    {
        std::cerr << "tinyply exception: " << e.what() << std::endl;
    }

    plyFile.read(fileStream);

    vertexData.resize(vertices->count);
    normalData.resize(normals->count);

    // if (colorData)
    //     colorData.resize(colors->count);

    faceData.resize(faces->count);

    //////////////
    // vertices //
    //////////////

    if (vertices->t == tinyply::Type::FLOAT32)
    {
        const float_3 *vertexDataPtr = (float_3 *)vertices->buffer.get();
        for (size_t i = 0; i < vertices->count; i++)
        {
            // vertexData[i] = Vector3f(vertexDataPtr[i].x, vertexDataPtr[i].y, vertexDataPtr[i].z);
            vertexData.at(i) = Vector3f(vertexDataPtr[i].x, vertexDataPtr[i].y, vertexDataPtr[i].z);
        }
    }

    /////////////
    // normals //
    /////////////

    if (normals->t == tinyply::Type::FLOAT32)
    {
        const float_3 *normalDataPtr = (float_3 *)normals->buffer.get();
        for (size_t i = 0; i < normals->count; i++)
        {
            // normalData[i] = Vector3f(normalDataPtr[i].x, normalDataPtr[i].y, normalDataPtr[i].z);
            normalData.at(i) = Vector3f(normalDataPtr[i].x, normalDataPtr[i].y, normalDataPtr[i].z);
        }
    }

    ///////////////////
    // vertex colors //
    ///////////////////

    // if (colorData)
    // {
    //     if (colors->t == tinyply::Type::FLOAT32)
    //     {
    //         const float_3 *colorDataPtr = (float_3 *)colors->buffer.get();
    //         for (size_t i = 0; i < colors->count; i++)
    //         {
    //             // colorData[i] = Vector3f(colorDataPtr[i].x, colorDataPtr[i].y, colorDataPtr[i].z);
    //             colorData.at(i) = Vector3f(colorDataPtr[i].x, colorDataPtr[i].y, colorDataPtr[i].z);
    //         }
    //     }
    //     else if (colors->t == tinyply::Type::UINT8)
    //     {
    //         const uint8_3 *colorDataPtr = (uint8_3 *)colors->buffer.get();
    //         for (size_t i = 0; i < colors->count; i++)
    //         {
    //             // colorData[i] = Vector3f(colorDataPtr[i].x / 255.0f, colorDataPtr[i].y / 255.0f, colorDataPtr[i].z / 255.0f);
    //             colorData.at(i) = Vector3f(colorDataPtr[i].x / 255.0f, colorDataPtr[i].y / 255.0f, colorDataPtr[i].z / 255.0f);
    //         }
    //     }
    // }

    ///////////
    // faces //
    ///////////

    if (faces->t == tinyply::Type::UINT32)
    {
        const uint_3 *faceDataPtr = (uint_3 *)faces->buffer.get();
        for (size_t i = 0; i < faces->count; i++)
        {
            // faceData[i] = Vector3i(faceDataPtr[i].x, faceDataPtr[i].y, faceDataPtr[i].z);
            faceData.at(i) = Vector3i(faceDataPtr[i].x, faceDataPtr[i].y, faceDataPtr[i].z);
        }
    }
}