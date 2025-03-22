#ifndef SHADER_H
#define SHADER_H

#include "math/Vector.h"
#include "math/Matrix.h"

#include <string>

class Shader
{
public:
    Shader();
    Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    void link();
    void use();

    void bindUniformBlock(const std::string &blockName, unsigned int bindingPoint);

    void deleteProgram();

    /////////////////////////
    // GETTERS AND SETTERS //
    /////////////////////////

    std::string getName() const { return name; }

    void setName(const std::string name);
    void setInteger(const std::string &name, int value);
    void setVector3f(const std::string &name, const Vector3f &value);
    void setVector3f(const std::string &name, float x, float y, float z);
    void setMatrix4x4f(const std::string &name, const Matrix4x4f &value);

private:
    unsigned int ID;

    unsigned int vertexShaderID;
    unsigned int fragmentShaderID;

    std::string name;

    void attachShader(const std::string &shaderPath, unsigned int type);
};

#endif