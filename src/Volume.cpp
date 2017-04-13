#include "Volume.h"


extern int width, height; //Viewport

// Crear cubo unitario
void volume::createCube() {

	
	GLfloat vertex[] = {
		//Coordenadas de objeto		Coordenadas de inicio del rayo
		-0.5f,-0.5f,0.5f,		0.0f,0.0f,0.0f,	
		0.5f,-0.5f,0.5f,		1.0f,0.0f,0.0f,	
		0.5f,0.5f,0.5f,			1.0f,1.0f,0.0f,	
		-0.5f,0.5f,0.5f,		0.0f,1.0f,0.0f,
		-0.5f,-0.5f,-0.5f,		0.0f,0.0f,1.0f,	
		0.5f,-0.5f,-0.5f,		1.0f,0.0f,1.0f,	
		0.5f,0.5f,-0.5f,		1.0f,1.0f,1.0f,	
		-0.5f,0.5f,-0.5f,		0.0f,1.0f,1.0f,	
	};

	GLuint index[] = {
		//Indices del cubo
		5 ,1 ,0 ,
		5 ,0 ,4 ,
		7 ,3 ,2 ,
		7 ,2 ,6 ,
		7 ,6 ,5 ,
		7 ,5 ,4 ,
		2 ,3 ,0 ,
		2 ,0 ,1 ,
		4 ,0 ,3 ,
		4 ,3 ,7 ,
		6 ,2 ,1 ,
		6 ,1 ,5
	};

	//Creamos el VBO y el VAO del cubo
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vindex);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vindex);

	glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(GL_FLOAT), vertex, GL_STATIC_DRAW);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GL_UNSIGNED_INT), index, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vindex);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, BUFFER_OFFSET(0)); //Coordenadas de objeto
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, BUFFER_OFFSET(sizeof(GL_FLOAT) * 3)); //Coordenadas de inicio de rayo

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//Creamos la textura 2D (que ira en el FBO)
void volume::create2Dtext() {
	if (texture2D == -1) glGenTextures(1, &texture2D);
	glBindTexture(GL_TEXTURE_2D, texture2D);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void volume::sample_2D_rawstyle(int nbytes, GLubyte *volume_size_8, GLushort *volume_size_16) {

	z_texture = (GLuint *)realloc(z_texture, sizeof(GLuint)*depthVolume);
	y_texture = (GLuint *)realloc(y_texture, sizeof(GLuint)*heightVolume);
	x_texture = (GLuint *)realloc(x_texture, sizeof(GLuint)*widthVolume);

	GLubyte *new_volume_size_8 = (BYTE *)malloc(sizeof(BYTE)*widthVolume*heightVolume);
	GLushort *new_volume_size_16 = (GLushort *)malloc(sizeof(GLushort)*widthVolume*heightVolume);

	glGenTextures(depthVolume, z_texture);

	int it = 0;
	for (unsigned int i = 0; i<depthVolume; i++) {
		int l = 0;
		glBindTexture(GL_TEXTURE_2D, z_texture[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		for (unsigned int j = 0; j<heightVolume; j++) {
			for (unsigned int k = 0; k<widthVolume; k++) {
				if (nbytes == 1) new_volume_size_8[l++] = volume_size_8[it++];
				else new_volume_size_16[l++] = volume_size_16[it++];
			}
		}
		if (nbytes == 1) glTexImage2D(GL_TEXTURE_2D, 0, 1, widthVolume, heightVolume, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, new_volume_size_8);
		else glTexImage2D(GL_TEXTURE_2D, 0, 1, widthVolume, heightVolume, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, new_volume_size_16);
	}

	glGenTextures(widthVolume, x_texture);

	new_volume_size_8 = (BYTE *)realloc(new_volume_size_8, sizeof(BYTE)*depthVolume*heightVolume);
	new_volume_size_16 = (GLushort *)realloc(new_volume_size_16, sizeof(GLushort)*depthVolume*heightVolume);

	for (unsigned int i = 0; i<widthVolume; i++) {
		int l = 0;
		glBindTexture(GL_TEXTURE_2D, x_texture[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		for (unsigned int j = 0; j<depthVolume; j++) {
			for (unsigned int k = 0; k<heightVolume; k++) {
				if (nbytes == 1) new_volume_size_8[l++] = volume_size_8[i*widthVolume + j*widthVolume*heightVolume + k];
				else new_volume_size_16[l++] = volume_size_16[i*widthVolume + j*widthVolume*heightVolume + k];
			}
		}
		if (nbytes == 1) glTexImage2D(GL_TEXTURE_2D, 0, 1, heightVolume, depthVolume, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, new_volume_size_8);
		else glTexImage2D(GL_TEXTURE_2D, 0, 1, heightVolume, depthVolume, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, new_volume_size_16);
	}

	glGenTextures(heightVolume, y_texture);

	new_volume_size_8 = (BYTE *)realloc(new_volume_size_8, sizeof(BYTE)*depthVolume*widthVolume);
	new_volume_size_16 = (GLushort *)realloc(new_volume_size_16, sizeof(GLushort)*depthVolume*widthVolume);

	for (unsigned int i = 0; i<heightVolume; i++) {
		int l = 0;
		glBindTexture(GL_TEXTURE_2D, y_texture[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		for (unsigned int j = 0; j<depthVolume; j++) {
			for (unsigned int k = 0; k<widthVolume; k++) {
				if (nbytes == 1) new_volume_size_8[l++] = volume_size_8[i + j*widthVolume*heightVolume + k*widthVolume];
				else new_volume_size_16[l++] = volume_size_16[i + j*widthVolume*heightVolume + k*widthVolume];
			}
		}
		if (nbytes == 1) glTexImage2D(GL_TEXTURE_2D, 0, 1, widthVolume, depthVolume, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, new_volume_size_8);
		else glTexImage2D(GL_TEXTURE_2D, 0, 1, widthVolume, depthVolume, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, new_volume_size_16);
	}

	z_dist = 2.0f / (float)depthVolume;
	y_dist = 2.0f / (float)heightVolume;
	x_dist = 2.0f / (float)widthVolume;

}

// Leer el archivo .raw de 8 y 16 bits y crear la textura 3D
void volume::read_raw(const char* filename, int width, int height, int depth, int nbytes) {

	//Inicializacion de tamaño
	widthVolume = width;
	heightVolume = height;
	depthVolume = depth;

	const int size = width*height*depth*nbytes;
	FILE *raw_file;
	fopen_s(&raw_file, filename, "rb");

	GLubyte *volume_size_8 = new GLubyte[size];
	GLushort* volume_size_16 = new GLushort[size];

	if (nbytes == 1) {
		fread(volume_size_8, sizeof(GLubyte), size, raw_file);
	}
	else {
		fread(volume_size_16, sizeof(GLubyte), size, raw_file);
	}

	fclose(raw_file);

	//Creamos la textura 3D	
	if (texture3D == -1) glGenTextures(1, &texture3D);
	glBindTexture(GL_TEXTURE_3D, texture3D);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//Chequeamos que tipo de modelo es
	if (nbytes == 1) glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, width, height, depth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, volume_size_8);
	else glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, width, height, depth, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, volume_size_16);

	sample_2D_rawstyle(nbytes, volume_size_8, volume_size_16);
	
	delete[] volume_size_8;
	delete[] volume_size_16;

}


void volume::createQuadTDFrenteyAtras(float dist) {
	GLfloat quadVertices[] = {
		-1.0f,  1.0f, dist,  1.0f, 0.0f,
		-1.0f, -1.0f, dist,  0.0f, 0.0f,
		1.0f,  1.0f, dist,  1.0f, 1.0f,
		1.0f, -1.0f, dist,  0.0f, 1.0f, 
	};

	// Creacion de VAO
	if (textvao == -1) {
		glGenVertexArrays(1, &textvao);
		glBindVertexArray(textvao);
	}
	if(textvbo == -1) glGenBuffers(1, &textvbo);
	
	glBindBuffer(GL_ARRAY_BUFFER, textvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

}


void volume::createQuadTDDeryIzq(float dist) {

	GLfloat quadVertices[] = {
		dist , -1.0f,  1.0f,  0.0f, 0.0f,
		dist , -1.0f, -1.0f,  0.0f, 1.0f,
		dist, 1.0f,  1.0f,  1.0f, 0.0f,
		dist, 1.0f, -1.0f,  1.0f, 1.0f,
	};

	// Creacion de VAO
	if (textvao == -1) {
		glGenVertexArrays(1, &textvao);
		glBindVertexArray(textvao);
	}
	if (textvbo == -1) glGenBuffers(1, &textvbo);

	glBindBuffer(GL_ARRAY_BUFFER, textvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

}


void volume::createQuadTDArryAba(float dist) {

	GLfloat quadVertices[] = {
		-1.0f, dist,  1.0f,  0.0f, 0.0f,
		-1.0f, dist, -1.0f,  0.0f, 1.0f,
		1.0f, dist,  1.0f,   1.0f, 0.0f,
		1.0f, dist, -1.0f,  1.0f, 1.0f,
	};

	// Creacion de VAO
	if (textvao == -1) {
		glGenVertexArrays(1, &textvao);
		
	}
	if (textvbo == -1) glGenBuffers(1, &textvbo);

	glBindVertexArray(textvao);
	glBindBuffer(GL_ARRAY_BUFFER, textvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

}


void volume::deleteVBOandVAO() {
	glDeleteBuffers(1, &textvbo);
	glDeleteVertexArrays(1, &textvao);
}


//Para crear la matriz de escalamiento
glm::mat4 volume::scale_en_matriz(float scale_tx) {
	glm::mat4 scaleMatrix = glm::mat4(glm::vec4(scale_tx, 0.0, 0.0, 0.0), glm::vec4(0.0, scale_tx, 0.0, 0.0), glm::vec4(0.0, 0.0, scale_tx, 0.0), glm::vec4(0.0, 0.0, 0.0, 1)); //Creo matriz de escalamiento
	return scaleMatrix;
}

//Para crear la matriz de translate
glm::mat4 volume::translate_en_matriz(float translate_tx, float translate_ty, float translate_tz) {
	glm::mat4 translateMatrix = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0), glm::vec4(0.0, 1.0, 0.0, 0.0), glm::vec4(0.0, 0.0, 1.0, 0.0), glm::vec4(translate_tx, translate_ty, translate_tz, 1)); //Creo matriz de translate
	return translateMatrix;
}

//Para crear la matriz de rotación
glm::mat4 volume::rotacion_en_matriz(float rotacion_tx, float rotacion_ty, float rotacion_tz, float rotacion_ta) {
	glm::mat4 rotateMatrix;
	//Creo matriz de rotación usando los quat
	rotateMatrix[0][0] = 1.0 - 2.0 * (rotacion_ty * rotacion_ty + rotacion_tz * rotacion_tz);
	rotateMatrix[0][1] = 2.0 * (rotacion_tx * rotacion_ty + rotacion_tz * rotacion_ta);
	rotateMatrix[0][2] = 2.0 * (rotacion_tx * rotacion_tz - rotacion_ty * rotacion_ta);
	rotateMatrix[0][3] = 0.0;
	rotateMatrix[1][0] = 2.0 * (rotacion_tx * rotacion_ty - rotacion_tz * rotacion_ta);
	rotateMatrix[1][1] = 1.0 - 2.0 * (rotacion_tx * rotacion_tx + rotacion_tz * rotacion_tz);
	rotateMatrix[1][2] = 2.0 * (rotacion_ty * rotacion_tz + rotacion_tx * rotacion_ta);
	rotateMatrix[1][3] = 0.0;
	rotateMatrix[2][0] = 2.0 * (rotacion_tx * rotacion_tz + rotacion_ty * rotacion_ta);
	rotateMatrix[2][1] = 2.0 * (rotacion_ty * rotacion_tz - rotacion_tx * rotacion_ta);
	rotateMatrix[2][2] = 1.0 - 2.0 * (rotacion_tx * rotacion_tx + rotacion_ty * rotacion_ty);
	rotateMatrix[2][3] = 0.0;
	rotateMatrix[3][0] = 0.0;
	rotateMatrix[3][1] = 0.0;
	rotateMatrix[3][2] = 0.0;
	rotateMatrix[3][3] = 1.0;
	return rotateMatrix;
}

//Crear el FBO para guardar la textura 2D
void volume::createFBO() {
	if (FBO == -1) glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

