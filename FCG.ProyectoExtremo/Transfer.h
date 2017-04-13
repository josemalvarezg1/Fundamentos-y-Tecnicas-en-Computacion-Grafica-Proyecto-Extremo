#pragma once
#include "Main.h"
#include "Volume.h"

class Transferencia {
	public:
		GLuint transfer_vbo;
		GLuint transfer_vao;
		GLuint texture1D;
		GLuint texture2D;
		GLuint texture_selector;
		short int point_actual;
		short int point_anterior;
		float posx;
		float posy;
		float puntos_de_control[256];
		vector<glm::vec2> pos_puntos;
		vector<int> puntos_matched;
		float alfa_values[256];
		float rgb_values[256][3];
		glm::vec3 color_slider_actual;
		vector<glm::vec2> pos_slider;
		glm::vec3 color_quad_actual;
		vector<glm::vec2> pos_quads;
		glm::vec4 colores_slider[6];
		CGLSLProgram glslTransferProgram;
		//Metodos
		void initShaders();
		void createQuad();
		void display_transfer();
		void create1Dtext();
		unsigned int loadTexture(const char* filename);
		void checkClick(bool moving);
		void drawLinePoints();
		void initValues();
		void refreshValues();
		void deletePoint();
		void obtenerColor();
		int checkPoints(float x, float y, bool test);
		int middleOfPoint(float x, float y);
};