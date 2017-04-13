#version 330

in vec2 TexCoords;
in vec4 quadColor;

out vec4 color;

uniform sampler2D quad_text;
uniform int using_text;


void main() {             

	if (using_text == 1) {

	    float textValue = texture2D(quad_text, TexCoords).r;
	   	color = vec4(vec3(textValue, textValue, textValue), 1.0);

   	} else {

   		color = quadColor;

   	}

}  