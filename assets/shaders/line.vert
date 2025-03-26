#version 330 core

layout(location = 0) in vec3 a_pos;

layout(std140) uniform camera {
    mat4 view_matrix;
    mat4 projection_matrix;
};

uniform vec3 color;

out vec4 vert_color;

void main() {
    gl_Position = projection_matrix * view_matrix * vec4(a_pos, 1.0);
    vert_color = vec4(color, 1.0);
}