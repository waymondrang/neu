#include "shader.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "core.h"
#include "math/Matrix.h"
#include "math/Vector.h"

using std::ifstream;
using std::string;
using std::stringstream;

using namespace Neu;

/**
 * Empty constructor.
 */
Shader::Shader() {}

Shader::Shader(const string &vertexShaderPath,
               const string &fragmentShaderPath) {
  ID = glCreateProgram();
  Shader::attachShader(vertexShaderPath, GL_VERTEX_SHADER);
  Shader::attachShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
  link();

  // DEBUG

  GLint numBlocks;
  GLint nameLen;

  std::vector<string> nameList;

  glGetProgramiv(ID, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
  nameList.reserve(numBlocks);

  Logger::debug("Found %d %s in shader \"%s\"\n", numBlocks,
                numBlocks == 1 ? "block" : "blocks", name.c_str());

  for (int blockIx = 0; blockIx < numBlocks; blockIx++) {
    glGetActiveUniformBlockiv(ID, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH,
                              &nameLen);

    std::vector<GLchar> name;
    name.resize(nameLen);
    glGetActiveUniformBlockName(ID, blockIx, nameLen, NULL, &name[0]);

    nameList.push_back(std::string());
    nameList.back().assign(name.begin(),
                           name.end() - 1);  // Remove the null terminator.
  }

  for (unsigned int il = 0; il < nameList.size(); il++) {
    Logger::debug("Block name: %s\n", nameList[il].c_str());
  }

  Logger::info(
      "Created shader program (%d) with vertex shader \"%s\" and fragment "
      "shader \"%s\"\n",
      ID, vertexShaderPath.c_str(), fragmentShaderPath.c_str());
}

void Shader::attachShader(const string &shaderPath,
                          unsigned int shaderType) const {
  string shaderCode;
  ifstream shaderFile;

  shaderFile.exceptions(ifstream::failbit | ifstream::badbit);

  try {
    shaderFile.open(shaderPath);
    stringstream shaderStream;

    shaderStream << shaderFile.rdbuf();

    shaderFile.close();

    shaderCode = shaderStream.str();
  } catch (ifstream::failure e) {
    Logger::error("Failed to read shader file \"%s\"\n", shaderPath.c_str());
    return;
  }

  const char *vShaderCode = shaderCode.c_str();
  unsigned int shaderID;
  int success;
  char infoLog[512];

  shaderID = glCreateShader(shaderType);
  glShaderSource(shaderID, 1, &vShaderCode, NULL);
  glCompileShader(shaderID);

  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
    Logger::error("Shader compilation failed \"%s\"\n", infoLog);
    return;
  }

  glAttachShader(ID, shaderID);

  glDeleteShader(shaderID);
}

void Shader::link() const {
  int success;
  char infoLog[512];

  glLinkProgram(ID);
  glGetProgramiv(ID, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    Logger::error("Shader program linking failed\n%s", infoLog);
    exit(1);
  }
}

void Shader::use() const { glUseProgram(ID); }

void Shader::bindUniformBlock(const string &blockName,
                              unsigned int bindingPoint) {
  unsigned int blockIndex = glGetUniformBlockIndex(ID, blockName.c_str());

  Logger::info(
      "Binding uniform block \"%s\" to binding point %d (block index: %d) in "
      "shader \"%s\"\n",
      blockName.c_str(), bindingPoint, blockIndex, name.c_str());

  glUniformBlockBinding(ID, blockIndex, bindingPoint);
}

void Shader::setName(const string name) { this->name = name; }

void Shader::setInteger(const string &name, int value) {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVector3f(const string &name, const Vector3f &value) {
  glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y,
              value.z);
}

void Shader::setVector3f(const string &name, float x, float y, float z) {
  glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setMatrix4x4f(const string &name, const Matrix4x4f &value) {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     *value.data);
}

void Shader::deleteProgram() {
  Logger::info("Deleting shader program (%d)\n", ID);
  glDeleteProgram(ID);
}