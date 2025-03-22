#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "Shader.h"

#include <string>
#include <unordered_map>

/**
 * RESPONSIBLE FOR MANAGING SHADERS AND UNIFORMS
 */

class ShaderManager
{
public:
    ShaderManager() {}

    ~ShaderManager()
    {
        for (auto &shader : shaders)
        {
            delete shader.second;
        }
    }

    void registerShader(const std::string &name, const std::string &vertexShaderPath, const std::string &fragmentShaderPath)
    {
        shaders[name] = new Shader(vertexShaderPath, fragmentShaderPath);
        shaders[name]->setName(name);
    }

    Shader *getShader(std::string name)
    {
        return shaders[name];
    }

    void createUniformBuffer(const std::string &blockName, unsigned int size, unsigned int bindingPoint);
    void updateUniformBuffer(const std::string &blockName, unsigned int offset, unsigned int size, const void *data);

protected:
    std::unordered_map<std::string, Shader*> shaders;
    std::unordered_map<std::string, unsigned int> uniformBuffers;
};

#endif