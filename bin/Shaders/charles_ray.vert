#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 ver_coord; 

uniform mat4 view_matrix;
uniform mat4 projection_matrix; 
uniform mat4 model_matrix; 
uniform vec3 view; 

out vec3 pos_in;
out vec3 FragPos;
out vec3 viewPos;

void main() {
	
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0); //La posición viene por la mult de matrices
    pos_in = ver_coord;
    FragPos = vec3(model_matrix * vec4(position, 1.0));
    viewPos = view;

}