#version 330 core
out vec4 FragColor;

in vert_out {
    vec3 frag_pos;
    vec4 frag_color;
    vec3 normal;
    vec4 frag_pos_light_space;
} fs_in;

void main() {
    FragColor = fs_in.frag_color;
}