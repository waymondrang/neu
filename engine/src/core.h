#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "math/Vector.h"

#include "utility/Logger.h"
#include "utility/Profiler.h"

#define MAX_LIGHTS 8

struct LightData {
    Vector4f light_position;
    Vector4f light_color;
};

struct LightBuffer {
    LightData lights[MAX_LIGHTS];
    int num_lights;
};