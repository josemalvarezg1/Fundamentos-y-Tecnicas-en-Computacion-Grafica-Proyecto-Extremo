#version 330

in vec3 color_out;

layout(location = 0) out vec4 color;

void main() {	

	color = vec4(color_out, 1.0);
	
}