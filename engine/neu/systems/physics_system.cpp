#include "physics_system.h"

#include "application.h"
#include "components/rigid_body.h"
#include "components/transform.h"
#include "input_manager.h"
#include "scene/game_object.h"
#include "utility/logger.h"

// clang-format off
#include <Jolt/Jolt.h>

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>
// clang-format on

// STL includes
#include <cstdarg>
#include <iostream>
#include <thread>

#include "utility/profiler.h"

// All Jolt symbols are in the JPH namespace
using namespace JPH;

// If you want your code to compile using single or double precision write 0.0_r
// to get a Real value that compiles to double or float depending if
// JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

// We're also using STL classes in this example
using namespace std;

void Neu::PhysicsSystem::setup() {
  /////////////////////////
  // SET UP JOLT PHYSICS //
  /////////////////////////

  RegisterDefaultAllocator();

  // Create a factory, this class is responsible for creating instances of
  // classes based on their name or hash and is mainly used for deserialization
  // of saved data. It is not directly used in this example but still required.
  Factory::sInstance = new Factory();

  // Register all physics types with the factory and install their collision
  // handlers with the CollisionDispatch class. If you have your own custom
  // shape types you probably need to register their handlers with the
  // CollisionDispatch before calling this function. If you implement your own
  // default material (PhysicsMaterial::sDefault) make sure to initialize it
  // before this function or else this function will create one for you.
  RegisterTypes();

  // We need a temp allocator for temporary allocations during the physics
  // update. We're pre-allocating 10 MB to avoid having to do allocations during
  // the physics update. B.t.w. 10 MB is way too much for this example but it is
  // a typical value you can use. If you don't want to pre-allocate you can also
  // use TempAllocatorMalloc to fall back to malloc / free.
  temp_allocator = new TempAllocatorImpl(10 * 1024 * 1024);  // 10 MB

  // We need a job system that will execute physics jobs on multiple threads.
  // Typically you would implement the JobSystem interface yourself and let Jolt
  // Physics run on top of your own job scheduler. JobSystemThreadPool is an
  // example implementation.
  job_system = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers,
                                       thread::hardware_concurrency() - 1);

  // This is the max amount of rigid bodies that you can add to the physics
  // system. If you try to add more you'll get an error. Note: This value is low
  // because this is a simple test. For a real project use something in the
  // order of 65536.
  const uint cMaxBodies = 1024;

  // This determines how many mutexes to allocate to protect rigid bodies from
  // concurrent access. Set it to 0 for the default settings.
  const uint cNumBodyMutexes = 0;

  // This is the max amount of body pairs that can be queued at any time (the
  // broad phase will detect overlapping body pairs based on their bounding
  // boxes and will insert them into a queue for the narrowphase). If you make
  // this buffer too small the queue will fill up and the broad phase jobs will
  // start to do narrow phase work. This is slightly less efficient. Note: This
  // value is low because this is a simple test. For a real project use
  // something in the order of 65536.
  const uint cMaxBodyPairs = 1024;

  // This is the maximum size of the contact constraint buffer. If more contacts
  // (collisions between bodies) are detected than this number then these
  // contacts will be ignored and bodies will start interpenetrating / fall
  // through the world. Note: This value is low because this is a simple test.
  // For a real project use something in the order of 10240.
  const uint cMaxContactConstraints = 1024;

  // Now we can create the actual physics system.
  physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs,
                     cMaxContactConstraints, broad_phase_layer_interface,
                     object_vs_broadphase_layer_filter,
                     object_vs_object_layer_filter);

  // DEBUG: AUTOMATICALLY ADD FLOOR
  BodyInterface& body_interface = physicsSystem.GetBodyInterface();

  BoxShapeSettings floor_shape_settings(Vec3(100.0f, 1.0f, 100.0f));
  floor_shape_settings.SetEmbedded();

  ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
  ShapeRefC floor_shape =
      floor_shape_result
          .Get();  // We don't expect an error here, but you can check
                   // floor_shape_result for HasError() / GetError()

  BodyCreationSettings floor_settings(floor_shape, RVec3(0.0_r, -1.0_r, 0.0_r),
                                      Quat::sIdentity(), EMotionType::Static,
                                      Layers::NON_MOVING);

  Body* floor = body_interface.CreateBody(floor_settings);
  body_interface.AddBody(floor->GetID(), EActivation::DontActivate);

  /////////////////////
  // SET UP RENDERER //
  /////////////////////

  physicsDebugRenderer = new PhysicsRenderer();
}

void Neu::PhysicsSystem::addController(GameObject* gameObject) {
  // if (gameObject->hasComponent<RigidBodyComponent>()
  //	&& gameObject->hasComponent<TransformComponent>())
  //{
  //	auto& rigidBody = gameObject->getComponent<RigidBodyComponent>();
  //	auto& transform = gameObject->getComponent<TransformComponent>();
  //	auto& controller = gameObject->getComponent<ControllerComponent>(); //
  // TODO: DEAL WITH CONTROLLER COMPONENT?

  //	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

  //	CharacterSettings* settings = new CharacterSettings();
  //	settings->mShape = new BoxShape(Vec3(1.0f, 1.0f, 1.0f));
  //	settings->mLayer = Layers::MOVING;
  //	// EDIT CHARACTER SETTINGS HERE

  //	controller.character = new Character(
  //		settings,
  //		RVec3(transform.position.x, transform.position.y,
  // transform.position.z),
  // Quat::sEulerAngles(Vec3(transform.rotation.x, transform.rotation.y,
  // transform.rotation.z)), 		NULL, 		&physicsSystem
  //	);

  //	controller.character->AddToPhysicsSystem();

  //	rigidBody.setBodyID(controller.character->GetBodyID());
  //}
}

void Neu::PhysicsSystem::addBody(GameObject* gameObject) {
  if (gameObject->hasComponent<RigidBodyComponent>() &&
      gameObject->hasComponent<TransformComponent>()) {
    auto& rigidBodyComponent = gameObject->getComponent<RigidBodyComponent>();
    auto& transformComponent = gameObject->getComponent<TransformComponent>();

    BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

    ShapeSettings* shapeSettings = new BoxShapeSettings(Vec3(1.0f, 1.0f, 1.0f));

    BodyCreationSettings bodySettings(
        shapeSettings,  // maybe use scale?
        RVec3(transformComponent.position.x, transformComponent.position.y,
              transformComponent.position.z),
        Quat::sEulerAngles(Vec3(transformComponent.rotation.x,
                                transformComponent.rotation.y,
                                transformComponent.rotation.z)),
        EMotionType::Dynamic, Layers::MOVING);

    bodySettings.mFriction = 0.6;

    rigidBodyComponent.setBodyID(
        bodyInterface.CreateAndAddBody(bodySettings, EActivation::Activate));
  }
}

// CALL THIS AFTER ADDING BODIES OR AFTER ADDING NEW BODY
void Neu::PhysicsSystem::optimize() {
  // Optional step: Before starting the physics simulation you can optimize the
  // broad phase. This improves collision detection performance (it's pointless
  // here because we only have 2 bodies). You should definitely not call this
  // every frame or when e.g. streaming in a new level section as it is an
  // expensive operation. Instead insert all new objects in batches instead of 1
  // at a time to keep the broad phase efficient.
  physicsSystem.OptimizeBroadPhase();
}

void Neu::PhysicsSystem::update(std::vector<GameObject*>& gameObjects,
                                float deltaTime) {
  NEU_PROFILE_FUNCTION;

  BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

  physicsSystem.Update(deltaTime, collisionSteps, temp_allocator, job_system);

  for (GameObject* gameObject : gameObjects) {
    if (gameObject->hasComponent<RigidBodyComponent>() &&
        gameObject->hasComponent<TransformComponent>()) {
      RigidBodyComponent& rigidBodyComponent =
          gameObject->getComponent<RigidBodyComponent>();
      TransformComponent& transformComponent =
          gameObject->getComponent<TransformComponent>();

      if (bodyInterface.IsActive(rigidBodyComponent.getBodyID())) {
        RVec3 position = bodyInterface.GetCenterOfMassPosition(
            rigidBodyComponent.getBodyID());
        Vec3 linearVelocity =
            bodyInterface.GetLinearVelocity(rigidBodyComponent.getBodyID());
        Vec3 rotation =
            bodyInterface.GetRotation(rigidBodyComponent.getBodyID())
                .GetEulerAngles();

        transformComponent.position =
            Vector3f(position.GetX(), position.GetY(), position.GetZ());
        transformComponent.rotation =
            Vector3f(rotation.GetX(), rotation.GetY(), rotation.GetZ());

        rigidBodyComponent.setlinearVelocity(Vector3f(linearVelocity.GetX(),
                                                      linearVelocity.GetY(),
                                                      linearVelocity.GetZ()));
      }
    }
  }

  // DEBUG RENDERING

  NEU_PROFILE_START_CUSTOM(DebugRender);

  JPH::BodyManager::DrawSettings drawSettings;
  drawSettings.mDrawShape = false;
  drawSettings.mDrawBoundingBox = true;
  drawSettings.mDrawCenterOfMassTransform = true;
  physicsSystem.DrawBodies(drawSettings, physicsDebugRenderer);

  NEU_PROFILE_STOP_CUSTOM(DebugRender);
}