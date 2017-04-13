#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 ver_coord;

uniform mat4 view_matrix;
uniform mat4 projection_matrix; 
uniform mat4 model_matrix; 

out vec2 color_out;

void main() {

	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0); //La posición viene por la mult de matrices
	color_out = ver_coord;

}