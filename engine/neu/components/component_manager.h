#pragma once

#include <typeindex>
#include <unordered_map>
#include <vector>

#include "scene/game_object.h"  // WHY IS IT STILL IN SCENE

using Entity = uint32_t;

namespace Neu {
struct IComponentStorage {
  virtual ~IComponentStorage() = default;
};

template <typename T>
class ComponentStorage : public IComponentStorage {
 public:
  void addComponent(Entity entity, const T& component);
  void removeComponent(Entity entity);
  T* getComponent(Entity entity);

 private:
  std::vector<T> components;
  std::unordered_map<Entity, uint32_t> entityToIndex;
  std::unordered_map<uint32_t, Entity> indexToEntity;
};

class ComponentManager {
 public:
  template <typename T>
  void registerComponent();

  template <typename T>
  void addComponent(Entity entity, const T& component);

  template <typename T>
  void removeComponent(Entity entity, const T& component);

  template <typename T>
  void getComponent(Entity entity, const T& component);

 private:
  template <typename T>
  ComponentStorage<T>* getComponentStorage();

  std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>>
      components;
};
}  // namespace Neu