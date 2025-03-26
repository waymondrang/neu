#include "shader_manager.h"

#include "core.h"

using namespace Neu;

void ShaderManager::createUniformBuffer(const std::string &blockName,
                                        unsigned int size,
                                        unsigned int bindingPoint) {
  unsigned int blockID;
  glGenBuffers(1, &blockID);

  glBindBuffer(GL_UNIFORM_BUFFER, blockID);
  glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, blockID, 0, size);

  Logger::info(
      "Created uniform buffer \"%s\" with size %d and binding point %d\n",
      blockName.c_str(), size, bindingPoint);

  uniformBuffers[blockName] = blockID;
}

void ShaderManager::updateUniformBuffer(const std::string &blockName,
                                        unsigned int offset, unsigned int size,
                                        const void *data) {
  glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffers[blockName]);
  glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}