#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include "math/Matrix.h"
#include "math/Vector.h"

static unsigned int uid = 0;

namespace Neu {

class GameObject {
 public:
  GameObject() : id(uid++) {}
  ~GameObject() {
    // TODO: FREE ALL COMPONENTS
  }

  std::string getName() const { return name; }
  void setName(std::string name) { this->name = name; }

  unsigned int getID() const { return id; }

  template <typename T>
  T& addComponent() {
    T* c = new T();
    components.insert({(std::string) typeid(T).name(), (void*)c});
    return *c;
  }

  template <typename T>
  T& getComponent() {
    return *(T*)components[(std::string) typeid(T).name()];
  }

  template <typename T>
  bool hasComponent() {
    return components.contains(typeid(T).name());
  }

  // DEBUG
  Vector3f color;

 protected:
  unsigned int id;
  std::string name;

  std::unordered_map<std::string, void*> components;
};

}  // namespace Neu