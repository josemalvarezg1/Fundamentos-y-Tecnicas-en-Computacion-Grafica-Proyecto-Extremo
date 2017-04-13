#include "Transfer.h"

extern volume v;
extern int width, height;
extern theVolume currentVolume;
extern double xGlobal, yGlobal;
extern glm::mat4 project_mat; //Matriz de Proyección
extern glm::mat4 view_mat_transfer; //Matriz de View
extern bool inTransfer;
extern bool afine;
int posicion_puntoControl = 0;

//Init del shader de transferencia (quad con textura)
void Transferencia::initShaders() {
	glslTransferProgram.loadShader("Shaders/transferencia.vert", CGLSLProgram::VERTEX);
	glslTransferProgram.loadShader("Shaders/transferencia.frag", CGLSLProgram::FRAGMENT);

	glslTransferProgram.create_link();

	glslTransferProgram.enable();

	glslTransferProgram.addAttribute("position");
	glslTransferProgram.addAttribute("texCoords");

	glslTransferProgram.addUniform("view_matrix");
	glslTransferProgram.addUniform("model_matrix");
	glslTransferProgram.addUniform("projection_matrix");
	glslTransferProgram.addUniform("quad_text");
	glslTransferProgram.addUniform("using_text");
	glslTransferProgram.addUniform("modo_interpolacion");
	glslTransferProgram.addUniform("color1");
	glslTransferProgram.addUniform("color2");
	glslTransferProgram.addUniform("color3");
	glslTransferProgram.addUniform("color4");

	glslTransferProgram.disable();
}

//Inicio valores del header
void Transferencia::initValues() {

	//Init values
	transfer_vbo = -1;
	transfer_vao = -1;
	texture1D = -1;
	texture2D = -1;
	posx = 0.74;
	posy = 0.74;
	point_actual = -1;
	point_anterior = -1;
	pos_puntos = { glm::vec2(0.541016, 0.578125), glm::vec2(0.945313, 0.971354) };
	puntos_matched = { 0, 255 };

	//Init de texturas
	texture2D = loadTexture("Texturas/transferFondo.png");
	texture_selector = loadTexture("Texturas/selector.png");

	//Primer color es negro
	rgb_values[0][0] = 0.0f; //R
	rgb_values[0][1] = 0.0f; //G
	rgb_values[0][2] = 0.0f; //B

							 //Ultimo color es blanco
	rgb_values[255][0] = 1.0f; //R
	rgb_values[255][1] = 1.0f; //G
	rgb_values[255][2] = 1.0f; //B

	for (int i = 0; i < 256; i++) {
		puntos_de_control[i] = -1;
		pos_slider.push_back(glm::vec2(0.5039, 0.455));
		pos_quads.push_back(glm::vec2(0.99, 0.23));
	}

	pos_quads[255].x = 0.488281;
	pos_quads[255].y = 0.276042;

	pos_quads[0].x = 0.486328;
	pos_quads[0].y = -0.229167;

	puntos_de_control[255] = 1.0;
	puntos_de_control[0] = 0.0;


	//Inicio colores y posicion del slider y el quad 
	colores_slider[0] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	colores_slider[1] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	colores_slider[2] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	colores_slider[3] = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
	colores_slider[4] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	colores_slider[5] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

	color_slider_actual = glm::vec3(1.0, 0.0, 0.0);
	color_quad_actual = glm::vec3(1.0, 0.0, 0.0);

}

//Creacion del quad
void Transferencia::createQuad() {
	GLfloat quadVertices[] = {
		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
	};

	// Creacion de VAO
	glGenVertexArrays(1, &transfer_vao);
	glGenBuffers(1, &transfer_vbo);
	glBindVertexArray(transfer_vao);
	glBindBuffer(GL_ARRAY_BUFFER, transfer_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

}


//Para el display del quad
void Transferencia::display_transfer() {

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	/////////////////////////////////////////////////////////////////////////////////////////////
	//Muestro el quad con la textura
	glslTransferProgram.enable();

	GLuint view_matr_loc = glslTransferProgram.getLocation("view_matrix");
	GLuint model_matr_loc = glslTransferProgram.getLocation("model_matrix");
	GLuint proj_matr_loc = glslTransferProgram.getLocation("projection_matrix");

	//Saco la matriz model para colocar el quad en la pantalla
	glm::mat4 model = glm::mat4();
	model = v.translate_en_matriz(posx, posy, 0.0);
	model = model * v.scale_en_matriz(0.26f);

	glUniformMatrix4fv(view_matr_loc, 1, GL_FALSE, glm::value_ptr(view_mat_transfer));

	glUniformMatrix4fv(proj_matr_loc, 1, GL_FALSE, glm::value_ptr(project_mat));

	glUniformMatrix4fv(model_matr_loc, 1, GL_FALSE, glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2D);
	glUniform1i(glslTransferProgram.getLocation("quad_text"), 0);
	glUniform1i(glslTransferProgram.getLocation("using_text"), 1);
	glUniform1i(glslTransferProgram.getLocation("modo_interpolacion"), 0);

	glBindVertexArray(transfer_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glslTransferProgram.disable();

	////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////
	//Muestro el quad de selector

	posx = 0.94; //Inicia en otra posicion

				 //Dibujo muchos "quads" pequenos pegados
	for (int i = 0; i < 6; i++) {
		glslTransferProgram.enable();

		view_matr_loc = glslTransferProgram.getLocation("view_matrix");
		model_matr_loc = glslTransferProgram.getLocation("model_matrix");
		proj_matr_loc = glslTransferProgram.getLocation("projection_matrix");

		//Saco la matriz model para colocar el quad en la pantalla
		model = glm::mat4();
		model = v.translate_en_matriz(posx, posy - 0.36, 0.0);
		model = model * v.scale_en_matriz(0.08f) * glm::mat4(glm::vec4(0.5, 0.0, 0.0, 0.0), glm::vec4(0.0, 1.0, 0.0, 0.0), glm::vec4(0.0, 0.0, 1.0, 0.0), glm::vec4(0.0, 0.0, 0.0, 1.0));

		glUniformMatrix4fv(view_matr_loc, 1, GL_FALSE, glm::value_ptr(view_mat_transfer));

		glUniformMatrix4fv(proj_matr_loc, 1, GL_FALSE, glm::value_ptr(project_mat));

		glUniformMatrix4fv(model_matr_loc, 1, GL_FALSE, glm::value_ptr(model));

		glUniform1i(glslTransferProgram.getLocation("using_text"), 0);

		glUniform4f(glslTransferProgram.getLocation("color1"), colores_slider[(i + 1) % 6].x, colores_slider[(i + 1) % 6].y, colores_slider[(i + 1) % 6].z, colores_slider[(i + 1) % 6].w);
		glUniform4f(glslTransferProgram.getLocation("color2"), colores_slider[(i + 1) % 6].x, colores_slider[(i + 1) % 6].y, colores_slider[(i + 1) % 6].z, colores_slider[(i + 1) % 6].w);
		glUniform4f(glslTransferProgram.getLocation("color3"), colores_slider[i].x, colores_slider[i].y, colores_slider[i].z, colores_slider[i].w);
		glUniform4f(glslTransferProgram.getLocation("color4"), colores_slider[i].x, colores_slider[i].y, colores_slider[i].z, colores_slider[i].w);
		glUniform1i(glslTransferProgram.getLocation("modo_interpolacion"), 0);

		glBindVertexArray(transfer_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		glslTransferProgram.disable();

		posx -= 0.08; //Separacion entre quads

	}


	posx = 0.74; //Vuelvo al valor por defecto

				 ////////////////////////////////////////////////////////////////////////////////////////////

				 /////////////////////////////////////////////////////////////////////////////////////////////
				 //Muestro el quad con colores
	glslTransferProgram.enable();

	view_matr_loc = glslTransferProgram.getLocation("view_matrix");
	model_matr_loc = glslTransferProgram.getLocation("model_matrix");
	proj_matr_loc = glslTransferProgram.getLocation("projection_matrix");

	//Saco la matriz model para colocar el quad en la pantalla
	model = glm::mat4();
	model = v.translate_en_matriz(posx, posy - 0.72, 0.0);
	model = model * v.scale_en_matriz(0.26f);

	glUniformMatrix4fv(view_matr_loc, 1, GL_FALSE, glm::value_ptr(view_mat_transfer));

	glUniformMatrix4fv(proj_matr_loc, 1, GL_FALSE, glm::value_ptr(project_mat));

	glUniformMatrix4fv(model_matr_loc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glslTransferProgram.getLocation("using_text"), 0);

	GLfloat alpha = 1.0;

	glUniform4f(glslTransferProgram.getLocation("color1"), 1.0, 1.0, 1.0, alpha);
	glUniform4f(glslTransferProgram.getLocation("color2"), 0.0, 0.0, 0.0, alpha);
	glUniform4f(glslTransferProgram.getLocation("color3"), color_slider_actual.x, color_slider_actual.y, color_slider_actual.z, alpha);
	glUniform4f(glslTransferProgram.getLocation("color4"), 0.0, 0.0, 0.0, alpha);
	glUniform1i(glslTransferProgram.getLocation("modo_interpolacion"), 0);

	glBindVertexArray(transfer_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glslTransferProgram.disable();

	////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////
	//Dibujo el circulo del selector
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
	glBlendEquation(GL_FUNC_ADD);

	glslTransferProgram.enable();

	view_matr_loc = glslTransferProgram.getLocation("view_matrix");
	model_matr_loc = glslTransferProgram.getLocation("model_matrix");
	proj_matr_loc = glslTransferProgram.getLocation("projection_matrix");

	//Saco la matriz model para colocar el quad en la pantalla
	model = glm::mat4();
	if (point_actual != -1) model = v.translate_en_matriz(pos_slider[puntos_matched[point_actual]].x, pos_slider[puntos_matched[point_actual]].y, 0.0);
	else model = v.translate_en_matriz(pos_slider[puntos_matched[0]].x, pos_slider[puntos_matched[0]].y, 0.0);
	model = model * v.scale_en_matriz(0.02f) * glm::mat4(glm::vec4(0.8, 0.0, 0.0, 0.0), glm::vec4(0.0, 1.0, 0.0, 0.0), glm::vec4(0.0, 0.0, 1.0, 0.0), glm::vec4(0.0, 0.0, 0.0, 1.0));

	glUniformMatrix4fv(view_matr_loc, 1, GL_FALSE, glm::value_ptr(view_mat_transfer));

	glUniformMatrix4fv(proj_matr_loc, 1, GL_FALSE, glm::value_ptr(project_mat));

	glUniformMatrix4fv(model_matr_loc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glslTransferProgram.getLocation("using_text"), 1);
	glBindTexture(GL_TEXTURE_2D, texture_selector);
	alpha = 1.0;

	glUniform1i(glslTransferProgram.getLocation("modo_interpolacion"), 1);
	glUniform4f(glslTransferProgram.getLocation("color1"), 1.0, 1.0, 1.0, alpha);

	glBindVertexArray(transfer_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glslTransferProgram.disable();

	////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////
	//Dibujo el circulo del quad de color
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
	glBlendEquation(GL_FUNC_ADD);

	glslTransferProgram.enable();

	view_matr_loc = glslTransferProgram.getLocation("view_matrix");
	model_matr_loc = glslTransferProgram.getLocation("model_matrix");
	proj_matr_loc = glslTransferProgram.getLocation("projection_matrix");

	//Saco la matriz model para colocar el quad en la pantalla
	model = glm::mat4();
	if (point_actual != -1) model = v.translate_en_matriz(pos_quads[puntos_matched[point_actual]].x, pos_quads[puntos_matched[point_actual]].y, 0.0);
	else model = v.translate_en_matriz(pos_quads[puntos_matched[0]].x, pos_quads[puntos_matched[0]].y, 0.0);
	model = model * v.scale_en_matriz(0.02f) * glm::mat4(glm::vec4(0.8, 0.0, 0.0, 0.0), glm::vec4(0.0, 1.0, 0.0, 0.0), glm::vec4(0.0, 0.0, 1.0, 0.0), glm::vec4(0.0, 0.0, 0.0, 1.0));

	glUniformMatrix4fv(view_matr_loc, 1, GL_FALSE, glm::value_ptr(view_mat_transfer));

	glUniformMatrix4fv(proj_matr_loc, 1, GL_FALSE, glm::value_ptr(project_mat));

	glUniformMatrix4fv(model_matr_loc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glslTransferProgram.getLocation("using_text"), 1);
	glBindTexture(GL_TEXTURE_2D, texture_selector);
	alpha = 1.0;

	glUniform1i(glslTransferProgram.getLocation("modo_interpolacion"), 1);
	glUniform4f(glslTransferProgram.getLocation("color1"), 1.0, 1.0, 1.0, alpha);

	glBindVertexArray(transfer_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glslTransferProgram.disable();

	////////////////////////////////////////////////////////////////////////////////////////////

	drawLinePoints();
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}


//Cambiar y obtener color del slider/quad
void Transferencia::obtenerColor() {

	float proporcion_x = (xGlobal * 100) / width; //Saco la proporcion del click X
	float proporcion_y = (yGlobal * 100) / height; //Saco la proporcion del click Y
	double xndc = (xGlobal) / (width / 2) - 1; //Casteo de screen a coordenadas normalizadas [-1 1]
	double yndc = (yGlobal) / (height / 2) - 1; //Casteo de screen a coordenadas normalizadas [-1 1]


	//El click esta en el slider
	if (proporcion_x > 75.0977 && proporcion_x < 98.82 && proporcion_y < 34.76 && proporcion_y > 27.2135) {
		//Saco el pixel de la pantalla

		if (point_actual != -1) {

			pos_slider[puntos_matched[point_actual]].x = xndc;
			pos_slider[puntos_matched[point_actual]].y = -yndc;

			unsigned char pixel[4];
			glReadPixels(xGlobal, height - yGlobal, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

			color_slider_actual.x = (float)pixel[0] / 255;
			color_slider_actual.y = (float)pixel[1] / 255;
			color_slider_actual.z = (float)pixel[2] / 255;

			display_transfer();	//Actualizo la funcion de transferencia

								//Actualizo el color del quad 
			double x_sentrado = (pos_quads[puntos_matched[point_actual]].x + 1) * (width / 2); //Casteo de coordenadas normalizadas a screen coords
			double y_sentrado = (pos_quads[puntos_matched[point_actual]].y + 1) * (height / 2); //Casteo de coordenadas normalizadas a screen coords

			glReadPixels(x_sentrado, y_sentrado, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

			rgb_values[puntos_matched[point_actual]][0] = (float)pixel[0] / 255;
			rgb_values[puntos_matched[point_actual]][1] = (float)pixel[1] / 255;
			rgb_values[puntos_matched[point_actual]][2] = (float)pixel[2] / 255;


			color_quad_actual.x = (float)pixel[0] / 255;
			color_quad_actual.y = (float)pixel[1] / 255;
			color_quad_actual.z = (float)pixel[2] / 255;

			refreshValues();

		}

	}

	//El click esta en el quad de color
	if (proporcion_x > 74.02 && proporcion_x < 99.9 && proporcion_y < 61.849  && proporcion_y > 36.0) {

		if (point_actual != -1) {
			double xndc = (xGlobal) / (width / 2) - 1; //Casteo de screen a coordenadas normalizadas [-1 1]
			double yndc = (yGlobal) / (height / 2) - 1; //Casteo de screen a coordenadas normalizadas [-1 1]

			pos_quads[puntos_matched[point_actual]].x = xndc;
			pos_quads[puntos_matched[point_actual]].y = -yndc;

			unsigned char pixel[4];
			glReadPixels(xGlobal, height - yGlobal, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

			color_quad_actual.x = (float)pixel[0] / 255;
			color_quad_actual.y = (float)pixel[1] / 255;
			color_quad_actual.z = (float)pixel[2] / 255;

			rgb_values[puntos_matched[point_actual]][0] = color_quad_actual.x;
			rgb_values[puntos_matched[point_actual]][1] = color_quad_actual.y;
			rgb_values[puntos_matched[point_actual]][2] = color_quad_actual.z;

			display_transfer();	//Actualizo la funcion de transferencia

			refreshValues();
		}

	}

}

//Dibuja los puntos y las lineas de la funcion
void Transferencia::drawLinePoints() {

	float anterior_x = 0.0;
	float anterior_y = 0.0;

	if (pos_puntos.size() > 0) {
		anterior_x = pos_puntos[0].x;
		anterior_y = pos_puntos[0].y;
	}


	for (int i = 0; i < pos_puntos.size(); i++) {
		//Dibujo todos los puntos
		glPointSize(8);
		if (i != point_actual) glColor3f(1.0, 0.0, 0.0);
		else glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_POINTS);
		glVertex2d(pos_puntos[i].x, pos_puntos[i].y);
		glEnd();

		glColor3f(0.5, 0.5, 0.5);
		//Ahora dibujo las lineas
		glBegin(GL_LINES);
		glVertex2d(anterior_x, anterior_y);
		glVertex2d(pos_puntos[i].x, pos_puntos[i].y);
		glEnd();

		anterior_x = pos_puntos[i].x;
		anterior_y = pos_puntos[i].y;
	}
	
}

//Updateo de valores para la transferencia (colores y alfa)
void Transferencia::refreshValues() {

	float x = 0.0f, y = puntos_de_control[0], x1, y1;
	int init = 0;
	for (int cont = 1; cont < 256; cont++) {

		if (puntos_de_control[cont] != -1) {
			x1 = cont;
			y1 = puntos_de_control[cont];

			float m = (y1 - y) / (x1 - x); //Pendiente de una recta
			float b = y1 - x1 * m; //Punto de origen
			float t = 0.0f; //Para interpolacion
			float paso = (1.0f / (float(cont) + 1.0f - x)); //El paso de la interpolacion

			for (int k = init; k <= cont; k++) {
				alfa_values[k] = m * k + b;

				if (k != cont) {
					rgb_values[k][1] = (1 - t)*rgb_values[int(x)][1] + t*rgb_values[cont][1];
					rgb_values[k][2] = (1 - t)*rgb_values[int(x)][2] + t*rgb_values[cont][2];
					rgb_values[k][0] = (1 - t)*rgb_values[int(x)][0] + t*rgb_values[cont][0];
					t += paso;

				}

			}
			x = x1; y = y1; init = cont;
		}

	}

	create1Dtext();
}

//Creamos la textura 1D (funcion de transferencia)
void Transferencia::create1Dtext() {

	GLfloat* info_transferencia = new GLfloat[256 * 4]; //Tendra toda la info de los colores y el alfa
	for (int i = 0; i < 256; i++) {
		info_transferencia[i * 4] = rgb_values[i][0];
		info_transferencia[i * 4 + 1] = rgb_values[i][1];
		info_transferencia[i * 4 + 2] = rgb_values[i][2];
		info_transferencia[i * 4 + 3] = alfa_values[i];
	}

	//Creamos la textura 1D 
	glGenTextures(1, &texture1D);
	glBindTexture(GL_TEXTURE_1D, texture1D);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA16F, 256, 0, GL_RGBA, GL_FLOAT, info_transferencia);

	delete[] info_transferencia;

}

//Cargar textura
unsigned int Transferencia::loadTexture(const char* filename) {
	ilInit();
	ILuint imageID;
	GLuint textureID;
	ILboolean success;
	ILenum error;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	success = ilLoadImage(filename);
	if (success)
	{
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
		if (!success) {
			error = ilGetError();
			cout << "Image conversion fails" << endl;
		}
		glGenTextures(1, &textureID);

		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			ilGetInteger(IL_IMAGE_FORMAT),
			ilGetInteger(IL_IMAGE_WIDTH),
			ilGetInteger(IL_IMAGE_HEIGHT),
			0,
			ilGetInteger(IL_IMAGE_FORMAT),
			GL_UNSIGNED_BYTE,
			ilGetData()
		);
	}

	ilDeleteImages(1, &imageID);
	return textureID;
}



//Ver si el click esta dentro de la imagen
void Transferencia::checkClick(bool moving) {

	float proporcion_x = (xGlobal * 100) / width;
	float proporcion_y = (yGlobal * 100) / height;

	//El click esta entre el 75% de la pantalla en X y 21% en Y del viewport (sirve con reshape por la proporcion)

	float step_X = 0.08084;
	float step_Y = 0.076125;

	float mapto256_X = 77.0;
	float mapto256_Y = 21.5;

	posicion_puntoControl = 0;


	if (proporcion_x > 76.85 && proporcion_x < 97.4609 && proporcion_y < 21.224 && proporcion_y > 1.43229) {
		inTransfer = true;

		while (mapto256_X <= 97.5) {
			if (mapto256_X >= proporcion_x) break;

			mapto256_X += step_X;
			posicion_puntoControl++;
		}

		float origin = height / 4.0f;
		float scale_origin = origin - (5 * height / 100);
		float alfa = (1 - (yGlobal - (5 * height / 100)) / scale_origin) - 0.19;


		double xndc = (xGlobal) / (width / 2) - 1; //Casteo de screen a coordenadas normalizadas [-1 1]
		double yndc = (yGlobal) / (height / 2) - 1; //Casteo de screen a coordenadas normalizadas [-1 1]


		int pos = checkPoints(xndc, -yndc, true);

		//Creo nuevo punto (primer click)
		if (pos == -1 && !moving) {
			puntos_de_control[posicion_puntoControl] = 1.0;
			glm::vec2 puntoAgregar = glm::vec2(xndc, -yndc);

			point_actual = middleOfPoint(xndc, -yndc);
			point_anterior = point_actual;
			if (point_actual != -1) {
				pos_puntos.insert(pos_puntos.begin() + point_actual, puntoAgregar);
				puntos_matched.insert(puntos_matched.begin() + point_actual, posicion_puntoControl);
			}
			//Asigno valores de color por defecto
			rgb_values[posicion_puntoControl][0] = 1.0;
			rgb_values[posicion_puntoControl][1] = 0.0;
			rgb_values[posicion_puntoControl][2] = 0.0;


			color_slider_actual.x = 1.0;
			color_slider_actual.y = 0.0;
			color_slider_actual.z = 0.0;

			color_quad_actual.x = 1.0;
			color_quad_actual.y = 0.0;
			color_quad_actual.z = 0.0;

			pos_quads[puntos_matched[point_actual]] = glm::vec2(0.99, 0.23);
			pos_slider[puntos_matched[point_actual]] = glm::vec2(0.5039, 0.455);

			puntos_de_control[posicion_puntoControl] = alfa;

		}
		//Si esta moviendo el punto
		else if (moving && (xndc + 0.025 < 0.945313 || pos == pos_puntos.size() - 1)) {
			pos = checkPoints(xndc, -yndc, false);
			if (pos == 0 || pos == pos_puntos.size() - 1 || point_actual == 0 || point_actual == pos_puntos.size() - 1) {

				if (pos != -1 && pos != point_anterior) point_actual = pos;


				double x_sentrado = (pos_slider[puntos_matched[point_actual]].x + 1) * (width / 2); //Casteo de coordenadas normalizadas a screen coords
				double y_sentrado = (pos_slider[puntos_matched[point_actual]].y + 1) * (height / 2); //Casteo de coordenadas normalizadas a screen coords

				unsigned char pixel[4];
				glReadPixels(x_sentrado, y_sentrado, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

				color_slider_actual.x = (float)pixel[0] / 255;
				color_slider_actual.y = (float)pixel[1] / 255;
				color_slider_actual.z = (float)pixel[2] / 255;

				display_transfer();

				puntos_de_control[puntos_matched[point_actual]] = alfa;

				glm::vec2 puntoAgregar = glm::vec2(pos_puntos[point_actual].x, -yndc);
				pos_puntos[point_actual] = puntoAgregar;

			}
			else {
				//Guardo la variable anterior
				glm::vec2 auxiliar_pos_quad = pos_quads[puntos_matched[point_actual]];
				glm::vec2 auxiliar_pos_slider = pos_slider[puntos_matched[point_actual]];

				rgb_values[puntos_matched[point_actual]][0] = -1.0f; //R
				rgb_values[puntos_matched[point_actual]][1] = -1.0f; //G
				rgb_values[puntos_matched[point_actual]][2] = -1.0f; //B	

				puntos_de_control[puntos_matched[point_actual]] = -1;

				puntos_matched[point_actual] = posicion_puntoControl;


				pos_quads[puntos_matched[point_actual]] = auxiliar_pos_quad;
				pos_slider[puntos_matched[point_actual]] = auxiliar_pos_slider;

				//Actualizo colores de slider y quad
				double x_sentrado = (pos_slider[puntos_matched[point_actual]].x + 1) * (width / 2); //Casteo de coordenadas normalizadas a screen coords
				double y_sentrado = (pos_slider[puntos_matched[point_actual]].y + 1) * (height / 2); //Casteo de coordenadas normalizadas a screen coords

				unsigned char pixel[4];
				glReadPixels(x_sentrado, y_sentrado, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

				color_slider_actual.x = (float)pixel[0] / 255;
				color_slider_actual.y = (float)pixel[1] / 255;
				color_slider_actual.z = (float)pixel[2] / 255;

				display_transfer();

				x_sentrado = (pos_quads[puntos_matched[point_actual]].x + 1) * (width / 2); //Casteo de coordenadas normalizadas a screen coords
				y_sentrado = (pos_quads[puntos_matched[point_actual]].y + 1) * (height / 2); //Casteo de coordenadas normalizadas a screen coords

				glReadPixels(x_sentrado, y_sentrado, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

				rgb_values[puntos_matched[point_actual]][0] = (float)pixel[0] / 255; //R
				rgb_values[puntos_matched[point_actual]][1] = (float)pixel[1] / 255; //G
				rgb_values[puntos_matched[point_actual]][2] = (float)pixel[2] / 255; //B	

				color_quad_actual.x = (float)pixel[0] / 255;
				color_quad_actual.y = (float)pixel[1] / 255;
				color_quad_actual.z = (float)pixel[2] / 255;

				puntos_de_control[puntos_matched[point_actual]] = alfa;

				//Calculo de OFFSETS para que el punto no se pase de otro
				if ((pos_puntos[point_actual].x - 0.03 < pos_puntos[point_actual - 1].x) && \
					(point_actual - 1 != 0)) {
					//Acerco un punto desde la derecha hacia la izquierda 
					point_actual--;
					posicion_puntoControl -= 10;

				}
				else if ((pos_puntos[point_actual].x + 0.03 > pos_puntos[point_actual + 1].x) && \
					(point_actual + 1 != pos_puntos.size() - 1)) {
					//Acerco un punto desde la izquierda hacia la derecha 
					point_actual++;
					posicion_puntoControl += 10;
				}


				//Separo un poco el punto
				if (point_anterior < point_actual && xndc < 0.91) {
					glm::vec2 puntoAgregar = glm::vec2(xndc + 0.03, -yndc);
					pos_puntos[point_actual] = puntoAgregar;

				}
				//Separo un poco el punto
				else if (point_anterior > point_actual && xndc > 0.59) {
					glm::vec2 puntoAgregar = glm::vec2(xndc - 0.03, -yndc);
					pos_puntos[point_actual] = puntoAgregar;
				}
				//Es el mismo punto, no lo separo
				else {
					glm::vec2 puntoAgregar = glm::vec2(xndc, -yndc);
					pos_puntos[point_actual] = puntoAgregar;
				}

			}
		}
		else if ((xndc < 0.949219 || pos == pos_puntos.size() - 1) && !moving) {

			if (pos_puntos[pos + 1].x - 0.03 > xndc && pos_puntos[pos - 1].x + 0.03 < xndc) {
				unsigned char pixel[4];
				double x_sentrado = (pos_slider[puntos_matched[pos]].x + 1) * (width / 2); //Casteo de coordenadas normalizadas a screen coords
				double y_sentrado = (pos_slider[puntos_matched[pos]].y + 1) * (height / 2); //Casteo de coordenadas normalizadas a screen coords

				glReadPixels(x_sentrado, y_sentrado, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

				color_slider_actual.x = (float)pixel[0] / 255;
				color_slider_actual.y = (float)pixel[1] / 255;
				color_slider_actual.z = (float)pixel[2] / 255;

				color_quad_actual.x = rgb_values[puntos_matched[pos]][0];
				color_quad_actual.y = rgb_values[puntos_matched[pos]][1];
				color_quad_actual.z = rgb_values[puntos_matched[pos]][2];


				puntos_de_control[puntos_matched[pos]] = alfa;
				glm::vec2 puntoAgregar;
				if (pos != 0 && pos != pos_puntos.size() - 1) puntoAgregar = glm::vec2(xndc, -yndc);
				else puntoAgregar = glm::vec2(pos_puntos[pos].x, -yndc);
				pos_puntos[pos] = puntoAgregar;
				point_actual = pos;
				point_anterior = point_actual;
			}
		}

		//Como hubo cambios, updateo los valores
		refreshValues();
	}
}

//Ver si el click nuevo esta muy cerca de otro punto
int Transferencia::checkPoints(float x, float y, bool test) {

	if (x > 0.92) return pos_puntos.size() - 1;
	if (x < 0.545) return 0;

	for (int i = 0; i < pos_puntos.size(); i++) {
		if ((x + 0.03 > pos_puntos[i].x  && x - 0.03 < pos_puntos[i].x) && (i != point_actual || test)) return i;
	}

	return -1;
}


//Ver donde esta el click en comparacion a los otros puntos
int Transferencia::middleOfPoint(float x, float y) {

	for (int i = 0; i < pos_puntos.size(); i++) {
		if (pos_puntos[i].x > x) return i;
	}

	return -1;
}

//Borrar puntos
void Transferencia::deletePoint() {
	//Se elimina el punto de control seleccionado

	double xndc = (xGlobal) / (width / 2) - 1; //Casteo de screen a coordenadas normalizadas [-1 1]
	double yndc = (yGlobal) / (height / 2) - 1; //Casteo de screen a coordenadas normalizadas [-1 1]


	for (int i = 0; i < pos_puntos.size(); i++) {
		if ((abs(pos_puntos[i].x - xndc) < 0.0000001) && (abs(pos_puntos[i].y - yndc * -1.0) < 0.0000001)) {

			if (i != 0 && i != pos_puntos.size() - 1) {
				pos_puntos.erase(pos_puntos.begin() + i);
				puntos_de_control[puntos_matched[i]] = -1;
				rgb_values[puntos_matched[i]][0] = 0.0f; //R
				rgb_values[puntos_matched[i]][1] = 0.0f; //G
				rgb_values[puntos_matched[i]][2] = 0.0f; //B	
				puntos_matched.erase(puntos_matched.begin() + i);

				point_actual--;

				//Actualizo slider y quad de pantalla
				double x_sentrado = (pos_slider[puntos_matched[point_actual]].x + 1) * (width / 2); //Casteo de coordenadas normalizadas a screen coords
				double y_sentrado = (pos_slider[puntos_matched[point_actual]].y + 1) * (height / 2); //Casteo de coordenadas normalizadas a screen coords

				unsigned char pixel[4];
				glReadPixels(x_sentrado, y_sentrado, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

				color_slider_actual.x = (float)pixel[0] / 255;
				color_slider_actual.y = (float)pixel[1] / 255;
				color_slider_actual.z = (float)pixel[2] / 255;

				display_transfer();

				x_sentrado = (pos_quads[puntos_matched[point_actual]].x + 1) * (width / 2); //Casteo de coordenadas normalizadas a screen coords
				y_sentrado = (pos_quads[puntos_matched[point_actual]].y + 1) * (height / 2); //Casteo de coordenadas normalizadas a screen coords

				glReadPixels(x_sentrado, y_sentrado, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

				rgb_values[puntos_matched[point_actual]][0] = (float)pixel[0] / 255; //R
				rgb_values[puntos_matched[point_actual]][1] = (float)pixel[1] / 255; //G
				rgb_values[puntos_matched[point_actual]][2] = (float)pixel[2] / 255; //B	

				color_quad_actual.x = (float)pixel[0] / 255;
				color_quad_actual.y = (float)pixel[1] / 255;
				color_quad_actual.z = (float)pixel[2] / 255;


				refreshValues();

				break;
			}
		}
	}

}