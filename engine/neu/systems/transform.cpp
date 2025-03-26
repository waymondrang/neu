#include "transform.h"

#include "components/transform.h"
#include "scene/game_object.h"
#include "utility/profiler.h"

using namespace Neu;

void TransformSystem::update(std::vector<GameObject *> &gameObjects) {
  NEU_PROFILE_FUNCTION;

  for (auto *gameObject : gameObjects) {
    if (gameObject->hasComponent<TransformComponent>()) {
      TransformComponent &transform =
          gameObject->getComponent<TransformComponent>();

      transform.transform = createTransformMatrix(
          transform.position, transform.rotation, transform.scale);
    }
  }
}