#pragma once

// clang-format off
#include <Jolt/Jolt.h>

#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/PhysicsSystem.h>
// clang-format on

#include "render/physics_renderer.h"

// All Jolt symbols are in the JPH namespace
using namespace JPH;

// Layer that objects can be in, determines which other objects it can collide
// with Typically you at least want to have 1 layer for moving bodies and 1
// layer for static bodies, but you can have more layers if you want. E.g. you
// could have a layer for high detail collision (which is not used by the
// physics simulation but only if you do collision testing).
namespace Layers {
static constexpr ObjectLayer NON_MOVING = 0;
static constexpr ObjectLayer MOVING = 1;
static constexpr ObjectLayer NUM_LAYERS = 2;
};  // namespace Layers

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter {
 public:
  virtual bool ShouldCollide(ObjectLayer inObject1,
                             ObjectLayer inObject2) const override {
    switch (inObject1) {
      case Layers::NON_MOVING:
        return inObject2 ==
               Layers::MOVING;  // Non moving only collides with moving
      case Layers::MOVING:
        return true;  // Moving collides with everything
      default:
        JPH_ASSERT(false);
        return false;
    }
  }
};

// Each broadphase layer results in a separate bounding volume tree in the broad
// phase. You at least want to have a layer for non-moving and moving objects to
// avoid having to update a tree full of static objects every frame. You can
// have a 1-on-1 mapping between object layers and broadphase layers (like in
// this case) but if you have many object layers you'll be creating many broad
// phase trees, which is not efficient. If you want to fine tune your broadphase
// layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on
// the TTY.
namespace BroadPhaseLayers {
static constexpr BroadPhaseLayer NON_MOVING(0);
static constexpr BroadPhaseLayer MOVING(1);
static constexpr uint NUM_LAYERS(2);
};  // namespace BroadPhaseLayers

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface {
 public:
  BPLayerInterfaceImpl() {
    // Create a mapping table from object to broad phase layer
    mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
    mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
  }

  virtual uint GetNumBroadPhaseLayers() const override {
    return BroadPhaseLayers::NUM_LAYERS;
  }

  virtual BroadPhaseLayer GetBroadPhaseLayer(
      ObjectLayer inLayer) const override {
    JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
    return mObjectToBroadPhase[inLayer];
  }

 private:
  BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter {
 public:
  virtual bool ShouldCollide(ObjectLayer inLayer1,
                             BroadPhaseLayer inLayer2) const override {
    switch (inLayer1) {
      case Layers::NON_MOVING:
        return inLayer2 == BroadPhaseLayers::MOVING;
      case Layers::MOVING:
        return true;
      default:
        JPH_ASSERT(false);
        return false;
    }
  }
};

namespace Neu {

class GameObject;

class PhysicsSystem {
 public:
  void setup();

  void addController(GameObject* gameObject);
  void addBody(GameObject* gameObject);

  void optimize();
  void update(std::vector<GameObject*>& gameObjects, float deltaTime);

 private:
  // Create mapping table from object layer to broadphase layer
  // Note: As this is an interface, PhysicsSystem will take a reference to this
  // so this instance needs to stay alive! Also have a look at
  // BroadPhaseLayerInterfaceTable or BroadPhaseLayerInterfaceMask for a simpler
  // interface.
  BPLayerInterfaceImpl broad_phase_layer_interface;

  // Create class that filters object vs broadphase layers
  // Note: As this is an interface, PhysicsSystem will take a reference to this
  // so this instance needs to stay alive! Also have a look at
  // ObjectVsBroadPhaseLayerFilterTable or ObjectVsBroadPhaseLayerFilterMask for
  // a simpler interface.
  ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;

  // Create class that filters object vs object layers
  // Note: As this is an interface, PhysicsSystem will take a reference to this
  // so this instance needs to stay alive! Also have a look at
  // ObjectLayerPairFilterTable or ObjectLayerPairFilterMask for a simpler
  // interface.
  ObjectLayerPairFilterImpl object_vs_object_layer_filter;

  TempAllocatorImpl* temp_allocator;
  JobSystemThreadPool* job_system;

  JPH::PhysicsSystem physicsSystem;
  BodyID sphere_id;

  const int collisionSteps = 1;

  // DEBUG RENDERER
  PhysicsRenderer* physicsDebugRenderer;
};
}  // namespace Neu