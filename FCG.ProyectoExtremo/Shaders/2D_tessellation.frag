#version 330

uniform sampler1D texture1D;
uniform sampler2D quad_text;

in vec2 color_out;
out vec4 color;

void main() {	

	float value = texture2D(quad_text, color_out).r;
 	color = texture1D(texture1D, value);
 	
}