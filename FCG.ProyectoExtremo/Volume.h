#pragma once
#include "Main.h"

//Estructura de modelos
class volume {
	public:
		GLuint vbo;
		GLuint textvbo = -1;
		GLuint vao;
		GLuint textvao = -1;
		GLuint vindex;
		GLuint texture2D = -1;
		GLuint texture3D = -1; //Textura actual
		GLuint FBO = -1;

		float x_dist, y_dist, z_dist;
		GLuint *x_texture, *y_texture, *z_texture;

		float rotacion[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		double scaleT = 0.5;
		float ejeX = 0.0;
		float ejeY = 0.0;
		float ejeZ = 0.0;
		int widthVolume;
		int heightVolume;
		int depthVolume;
		glm::mat4 scale_en_matriz(float scale_tx);
		glm::mat4 translate_en_matriz(float translate_tx, float translate_ty, float translate_tz);
		glm::mat4 rotacion_en_matriz(float rotacion_tx, float rotacion_ty, float rotacion_tz, float rotacion_ta);
		void createFBO();
		void createCube();
		void createQuadTDFrenteyAtras(float dist);
		void createQuadTDDeryIzq(float dist);
		void createQuadTDArryAba(float dist);
		void deleteVBOandVAO();
		void read_raw(const char* filename, int width, int height, int depth, int nbytes);
		void sample_2D_rawstyle(int nbytes, GLubyte *volume_size_8, GLushort *volume_size_16);
		void create2Dtext();
		
};