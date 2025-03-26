#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "math/Vector.h"
#include "utility/logger.h"
#include "utility/profiler.h"

// THESE CERTAINLY DO NOT BELONG HERE, REFACTOR AND TURN THIS FILE INTO PCH

#define MAX_LIGHTS 8

struct LightData {
  Vector4f light_position;
  Vector4f light_color;
};

struct LightBuffer {
  LightData lights[MAX_LIGHTS];
  int num_lights;
};