#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 3) in vec3 a_instance_pos;
layout(location = 4) in float a_instance_radii;
layout(location = 5) in vec4 a_instance_color;

// todo: create simple fragment shader for particle

layout(std140) uniform camera {
    mat4 view_matrix;
    mat4 projection_matrix;
};

uniform mat4 model_matrix;
uniform mat4 light_space_matrix;

// uniform vec3 color;

out vert_out {
    vec3 frag_pos;
    vec4 frag_color;
    vec3 normal;
    vec4 frag_pos_light_space;
} vs_out;

void main() {
    // create 4x4 transform matrix from a_instance_pos

    ///////////////////////
    // INSTANCE POSITION //
    ///////////////////////

    mat4 instance_transform = mat4(a_instance_radii);
    instance_transform[3] = vec4(a_instance_pos, 1.0);

    gl_Position = projection_matrix * view_matrix * model_matrix * instance_transform * vec4(a_pos, 1.0);
    vs_out.frag_pos = vec3(instance_transform * vec4(a_pos, 1.0));
    vs_out.normal = transpose(inverse(mat3(instance_transform))) * a_normal;

    // gl_Position = projection_matrix * view_matrix * a_instance_pos * vec4(a_pos, 1.0);
    // vs_out.frag_pos = vec3(a_instance_pos * vec4(a_pos, 1.0));   
    // vs_out.normal = transpose(inverse(mat3(a_instance_pos))) * a_normal;

    vs_out.frag_color = a_instance_color;
    
    // debug:
    // vs_out.frag_color = a_instance_pos;
    vs_out.frag_pos_light_space = light_space_matrix * vec4(vs_out.frag_pos, 1.0);
}
