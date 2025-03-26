#pragma once

#include <vector>

namespace Neu {
class GameObject;

class TransformSystem {
 public:
  void update(std::vector<GameObject*>& gameObjects);
};

}  // namespace Neu