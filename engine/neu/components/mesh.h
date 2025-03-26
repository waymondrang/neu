#pragma once

#include <bitset>
#include <vector>

#include "math/Vector.h"

namespace Neu {

class MeshComponent {
 public:
  MeshComponent();
  ~MeshComponent();

  // THESE FUNCTIONS CAN BE MADE STATIC, I.E.
  // static void setupVertices(MeshComponent mesh, std::vector<Vector3f>&
  // vertices);
  void setupVertices(std::vector<Vector3f>& vertices);
  void setupNormals(std::vector<Vector3f>& normals);
  void setupColors(std::vector<Vector3f>& colors);
  void setupFaces(std::vector<Vector3i>& faces);

  void updateVertices(std::vector<Vector3f>& vertices);
  void updateNormals(std::vector<Vector3f>& normals);
  void updateColors(std::vector<Vector3f>& colors);
  void updateFaces(std::vector<Vector3i>& faces);

  inline unsigned int getVAO() { return VAO; };
  inline unsigned int getFaceCount() { return faceCount; }

 protected:
  enum SetupFlags {
    SETUP_VERTICES,
    SETUP_NORMALS,
    SETUP_COLORS,
    SETUP_FACES,
    SETUP_VAO,
    SETUP_COUNT  // KEEP AS LAST ITEM
  };

  std::bitset<SETUP_COUNT> setup;

  unsigned int faceCount;
  unsigned int VAO;

  unsigned int positionVBO;
  unsigned int normalVBO;
  unsigned int colorVBO;

  unsigned int EBO;
};

}  // namespace Neu