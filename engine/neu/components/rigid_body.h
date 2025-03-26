#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

#include "math/Vector.h"

using namespace JPH;

namespace Neu {

// STICK TO BOXES FOR NOW
class RigidBodyComponent {
 public:
  // EVENTUALLY, MOVING DATA INTO COMPONENT/SYSTEM MANAGER THESE WILL BE
  // REDUNDANT
  inline Vector3f getLinearVelocity() { return linearVelocity; }
  inline void setlinearVelocity(Vector3f linearVelocity) {
    this->linearVelocity = linearVelocity;
  }

  inline BodyID getBodyID() const { return ID; }
  inline void setBodyID(BodyID ID) { this->ID = ID; }

  Vector3f linearVelocity;

 protected:
  BodyID ID;
};

}  // namespace Neu
