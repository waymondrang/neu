#version 330 core

layout (location = 0) in vec3 a_pos;

uniform mat4 light_space_matrix;
uniform mat4 model_matrix;

void main()
{
	gl_Position = light_space_matrix * model_matrix * vec4(a_pos, 1.0);
}