#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

uniform mat4 view_matrix;
uniform mat4 projection_matrix; 
uniform mat4 model_matrix; 
uniform vec4 color1;
uniform vec4 color2;
uniform vec4 color3;
uniform vec4 color4;
uniform int modo_interpolacion;

out vec2 TexCoords;
out vec4 quadColor;

void main() {

	if (modo_interpolacion != 1) {

		if (gl_VertexID == 0) quadColor = color1;
		if (gl_VertexID == 1) quadColor = color2;
		if (gl_VertexID == 2) quadColor = color3;
		if (gl_VertexID == 3) quadColor = color4; 

	} else {

		quadColor = color1;
		
	}

	gl_Position = model_matrix * vec4(position, 1.0f);
	TexCoords = texCoords;
	
}  