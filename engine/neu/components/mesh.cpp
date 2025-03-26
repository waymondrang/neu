#include "mesh.h"

#include "core.h"

using namespace Neu;

MeshComponent::MeshComponent() {
  glGenVertexArrays(1, &VAO);
  setup.set(SETUP_VAO);
}

MeshComponent::~MeshComponent() {
  glDeleteVertexArrays(1, &VAO);

  if (setup.test(SETUP_VERTICES)) glDeleteBuffers(1, &positionVBO);
  if (setup.test(SETUP_NORMALS)) glDeleteBuffers(1, &normalVBO);
  if (setup.test(SETUP_COLORS)) glDeleteBuffers(1, &colorVBO);
  if (setup.test(SETUP_FACES)) glDeleteBuffers(1, &EBO);
}

void MeshComponent::setupVertices(std::vector<Vector3f>& vertices) {
  if (setup.test(SETUP_VERTICES)) return;

  // generate vertex array object
  glGenBuffers(1, &positionVBO);

  // bind vertex buffer object
  glBindVertexArray(VAO);

  // bind vertex buffer object
  glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3f),
               vertices.data(), GL_STATIC_DRAW);

  // set vertex attribute pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
  glEnableVertexAttribArray(0);

  // unbind vertex array object
  glBindVertexArray(0);

  setup.set(SETUP_VERTICES);
}

void MeshComponent::setupNormals(std::vector<Vector3f>& normals) {
  if (setup.test(SETUP_NORMALS)) return;

  // generate vertex array object
  glGenBuffers(1, &normalVBO);

  // bind vertex buffer object
  glBindVertexArray(VAO);

  // bind vertex buffer object
  glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vector3f),
               normals.data(), GL_STATIC_DRAW);

  // set vertex attribute pointers
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
  glEnableVertexAttribArray(1);

  // unbind vertex array object
  glBindVertexArray(0);

  setup.set(SETUP_NORMALS);
}

void MeshComponent::setupColors(std::vector<Vector3f>& colors) {
  if (setup.test(SETUP_COLORS)) return;

  // generate vertex array object
  glGenBuffers(1, &colorVBO);

  // bind vertex buffer object
  glBindVertexArray(VAO);

  // bind vertex buffer object
  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Vector3f), colors.data(),
               GL_STATIC_DRAW);

  // set vertex attribute pointers
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
  glEnableVertexAttribArray(2);

  // unbind vertex array object
  glBindVertexArray(0);

  setup.set(SETUP_COLORS);
}

void MeshComponent::setupFaces(std::vector<Vector3i>& faces) {
  if (setup.test(SETUP_FACES)) return;

  faceCount = faces.size();

  // generate vertex array object
  glGenBuffers(1, &EBO);

  // bind vertex buffer object
  glBindVertexArray(VAO);

  // bind vertex buffer object
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceCount * sizeof(Vector3i),
               faces.data(), GL_STATIC_DRAW);

  // unbind vertex array object
  glBindVertexArray(0);

  setup.set(SETUP_FACES);
}

void MeshComponent::updateVertices(std::vector<Vector3f>& vertices) {
  if (setup.test(SETUP_VERTICES)) return;

  glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vector3f),
                  vertices.data());
}

void MeshComponent::updateNormals(std::vector<Vector3f>& normals) {
  if (setup.test(SETUP_NORMALS)) return;

  glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, normals.size() * sizeof(Vector3f),
                  normals.data());
}

void MeshComponent::updateColors(std::vector<Vector3f>& colors) {
  if (setup.test(SETUP_COLORS)) return;

  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size() * sizeof(Vector3f),
                  colors.data());
}

void MeshComponent::updateFaces(std::vector<Vector3i>& faces) {
  if (setup.test(SETUP_FACES)) return;

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, faces.size() * sizeof(Vector3i),
                  faces.data());
}