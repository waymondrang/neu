#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;

layout(std140) uniform camera {
    mat4 view_matrix;
    mat4 projection_matrix;
};

uniform mat4 model_matrix;
uniform vec3 color;

out vert_out {
    vec3 frag_pos;
    vec3 frag_color;
    vec3 normal;
} vs_out;

void main() {
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(a_pos, 1.0);

    vs_out.normal = transpose(inverse(mat3(model_matrix))) * a_normal, 1.0;
    vs_out.frag_pos = (model_matrix * vec4(a_pos, 1.0)).xyz;
    vs_out.frag_color = color;
}