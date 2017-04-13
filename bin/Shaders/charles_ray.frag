#version 330

uniform sampler2D renderedTexture;
uniform sampler3D rawTexture;
uniform sampler1D transgeneroTexture;
uniform int width;
uniform int height;

uniform int widthVolume;
uniform int heightVolume;
uniform int depthVolume;

uniform vec3 lightPos;
uniform bool lighting;

in vec3 pos_in;
in vec3 FragPos;
in vec3 viewPos;

layout (location = 0) out vec4 FragColor;

vec3 getGradient(vec3 rayPos) { 	

    vec3 gradient;
    float x1 = texture3D(rawTexture, rayPos - vec3(0.01,0.0,0.0)).a;
    float x2 = texture3D(rawTexture, rayPos + vec3(0.01,0.0,0.0)).a;
    float y1 = texture3D(rawTexture, rayPos - vec3(0.0,0.01,0.0)).a;
    float y2 = texture3D(rawTexture, rayPos + vec3(0.0,0.01,0.0)).a;
    float z1 = texture3D(rawTexture, rayPos - vec3(0.0,0.0,0.01)).a;
    float z2 = texture3D(rawTexture, rayPos + vec3(0.0,0.0,0.01)).a;

    gradient.x = x2-x1;
    gradient.y = y2-y1;
    gradient.z = z2-z1;	

    float len = length (gradient);
    vec3 normal = vec3 (0.0, 1.0, 0.0); 
	if (len > 0.0) normal = gradient / len;

    return normal;

}

void main() {

	float step = 1.0 / sqrt(widthVolume * widthVolume + heightVolume * heightVolume + depthVolume * depthVolume);
	vec2 screen = vec2(width, height); 
	vec3 exitPoint = texture2D(renderedTexture, gl_FragCoord.st/screen).xyz;
    
    if (pos_in != exitPoint) {

	  	//Get direction of the ray
		vec3 direction = exitPoint.xyz - pos_in.xyz;
		float D = length (direction);
		direction = normalize(direction);

		vec4 color = vec4(0.0);
		color.a = 1.0f;

		vec3 trans = pos_in;
		vec3 stepRay = direction * step;

		for (float t = 0.0; t <= D; t += step) {
			
			float intensity = texture3D(rawTexture, trans).x;
			vec4 colorSample = texture1D(transgeneroTexture, intensity).rgba;
			vec3 samplePoint = trans + direction * stepRay * (float(t) + 0.5);
			
	    	if (lighting) {

		    	//Calculo la iluminacion

				vec3 gradient = getGradient(trans);
	            gradient = gradient*vec3(2,2,2) - vec3(1,1,1);
	 
	    	    vec3 vecLight = normalize(normalize(lightPos) - trans);	  
		    	float diffuseTerm = abs(dot(vecLight, gradient));

		    	//Blinn-Phong
		    	vec3 lightDir = normalize(lightPos - FragPos);
				vec3 viewDir = normalize(viewPos - FragPos);
				vec3 halfwayDir = normalize(lightDir + viewDir);
				float spec = pow(max(dot(vecLight, halfwayDir), 0.0), 64.0);

	    		colorSample.rgb *= diffuseTerm;	
	    		colorSample.rgb += colorSample.rgb * spec;	

	    	}

			colorSample.w = 1.0 - exp(-0.5 * colorSample.w);
			color.xyz += colorSample.w * colorSample.xyz * color.w;
			color.w *= (1.0 - colorSample.w);
			
			if (1.0 - color.w > 0.95) break;

			trans += stepRay;
			
		}

		color.w = 1.0 - color.w;
		FragColor = color;

	} else {

		FragColor = vec4(1.0 , 1.0, 1.0, 0.0);

	}

}  