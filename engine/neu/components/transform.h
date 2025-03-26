#pragma once

#include <vector>

#include "math/Matrix.h"
#include "math/Vector.h"

class GameObject;

namespace Neu {

class TransformComponent {
 public:
  GameObject* parent;
  std::vector<GameObject*> children;

  Vector3f position{0.0f, 0.0f, 0.0f};
  Vector3f rotation{0.0f, 0.0f, 0.0f};
  Vector3f scale{1.0f, 1.0f, 1.0f};

  Matrix4x4f transform;
};

}  // namespace Neu