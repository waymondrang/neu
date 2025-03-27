#include "components/component_manager.h"

using namespace Neu;

///////////////////////
// COMPONENT STORAGE //
///////////////////////

// DOES NOT CHECK IF COMPONENTS ARE ADDED TWICE
template <typename T>
void ComponentStorage<T>::addComponent(Entity entity, const T& component) {
  uint32_t index = components.size();
  components.push_back(component);
  entityToIndex[entity] = index;
  indexToEntity[index] = entity;
}

template <typename T>
void ComponentStorage<T>::removeComponent(Entity entity) {
  if (entityToIndex.find(entity) != entityToIndex.end()) {
    // INDEX TO BE REMOVED
    uint32_t index = entityToIndex[entity];

    // SWAP COMPONENTS
    uint32_t lastIndex = components.size() - 1;
    components[index] = components[lastIndex];

    // RE-MAP
    Entity lastEntity = indexToEntity[lastIndex];
    entityToIndex[lastEntity] = index;
    indexToEntity[index] = lastEntity;
    components.pop_back();
    entityToIndex.erase(entity);
    indexToEntity.erase(lastIndex);  // REMOVE DUPLICATE
  }
}

template <typename T>
T* ComponentStorage<T>::getComponent(Entity entity) {
  if (entityToIndex.find(entity) != entityToIndex.end()) {
    return &components[entityToIndex[entity]];
  }
  return nullptr;
}

///////////////////////
// COMPONENT MANAGER //
///////////////////////

template <typename T>
void ComponentManager::registerComponent() {
  std::type_index typeIndex(typeid(T));
  components[typeIndex] = std::make_unique<ComponentStorage<T>>();
}

template <typename T>
void ComponentManager::addComponent(Entity entity, const T& component) {
  auto* componentStorage = getComponentStorage<T>();
  if (componentStorage) componentStorage->addComponent(entity, component);
}

template <typename T>
void ComponentManager::removeComponent(Entity entity, const T& component) {
  auto* componentStorage = getComponentStorage<T>();
  if (componentStorage) componentStorage->removeComponent(entity);
}

template <typename T>
void ComponentManager::getComponent(Entity entity, const T& component) {
  auto* componentStorage = getComponentStorage<T>();
  if (componentStorage) return componentStorage->getComponent(entity);
  return nullptr;
}

template <typename T>
Neu::ComponentStorage<T>* ComponentManager::getComponentStorage() {
  std::type_index typeIndex(typeid(T));
  auto it = components.find(typeIndex);
  if (it != components.end()) {
    return std::static_cast<ComponentStorage<T>*>(it->second.get());
  }
  return nullptr;
}