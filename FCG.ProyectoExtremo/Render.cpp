#include "Render.h"

extern vector<volume> volumes; //Todos los modelos irán en este vector
extern volume v;
extern Transferencia t;
extern TwBar *menuTW, *modelTW;
extern Camera camera;
extern int width, height, selectedModel;
extern bool selecting, toggleLight;
extern double  xGlobal, yGlobal;
extern float color_luz_ambiental[4], color_luz_difuso[4];
extern float scaleT, ejeX, ejeY, ejeZ, ejeXL, ejeYL, ejeZL;
extern glm::mat4 project_mat; //Matriz de Proyección
extern glm::mat4 view_mat; //Matriz de View
extern CGLSLProgram glslFBOProgram; //Programa de shaders que crea el FBO
extern CGLSLProgram glslRayProgram; //Programa de shaders de ray casting
extern CGLSLProgram glsl2DProgram; //Programa de shaders de texturas 2D
extern glm::vec3 eye;
extern theVolume currentVolume;

extern float rotacion[];

void display_2D() {

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 1, -1);

	glsl2DProgram.enable();

	GLuint view_matr_loc = glsl2DProgram.getLocation("view_matrix");
	GLuint model_matr_loc = glsl2DProgram.getLocation("model_matrix");
	GLuint proj_matr_loc = glsl2DProgram.getLocation("projection_matrix");

	view_mat = camera.GetViewMatrix();
	glUniformMatrix4fv(view_matr_loc, 1, GL_FALSE, glm::value_ptr(view_mat));

	project_mat = glm::perspective(camera.Zoom, (float)width / (float)height, 0.1f, 1000.0f);
	glUniformMatrix4fv(proj_matr_loc, 1, GL_FALSE, glm::value_ptr(project_mat));

	glm::mat4 model_mat = glm::mat4();
	model_mat = v.translate_en_matriz(v.ejeX, v.ejeY, v.ejeZ);
	model_mat = model_mat * v.rotacion_en_matriz(v.rotacion[0], v.rotacion[1], v.rotacion[2], v.rotacion[3]) * v.rotacion_en_matriz(0.0, 0.0, -0.71, 0.71);
	model_mat = model_mat* v.scale_en_matriz(v.scaleT * 0.50) * glm::mat4(glm::vec4(-1.0, 0.0, 0.0, 0.0), glm::vec4(0.0, 1.0, 0.0, 0.0), glm::vec4(0.0, 0.0, 1.0, 0.0), glm::vec4(0.0, 0.0, 0.0, 1));

	glUniformMatrix4fv(model_matr_loc, 1, GL_FALSE, glm::value_ptr(model_mat));

	float distance = 0.0f;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLdouble wx, wy, wz;
	GLdouble abswx, abswy, abswz;

	glm::vec4 viewport = glm::vec4(0.0, 0.0f, width, height);
	glm::vec3 wincoord = glm::vec3(width/2.0f, height/2.0f, 0.0f);	
	glm::mat4 model_view_mat = view_mat * model_mat;

	glm::vec3 ray = glm::unProject(wincoord, model_view_mat, project_mat, viewport);

	wx = ray[0];
	wy = ray[1];
	wz = ray[2];

	abswx = abs(wx), abswy = abs(wy), abswz = abs(wz);
	int i;
	if (abswz >= abswx && abswz >= abswy) {
		if (wz >= 0) {
			v.deleteVBOandVAO();
			distance = -1.0f;
			for (i = v.depthVolume - 1; i >= 0; i--) {
				
				v.createQuadTDFrenteyAtras(distance);

				// UNIFORM DE TEXTURAS
				GLint text1Dloc = glsl2DProgram.getLocation("texture1D");
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_1D, t.texture1D);
				glUniform1i(text1Dloc, 0);

				GLint text2Dloc = glsl2DProgram.getLocation("quad_text");
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, v.z_texture[i]);
				glUniform1i(text2Dloc, 1);


				glBindVertexArray(v.textvao);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);

				distance += v.z_dist;

				v.deleteVBOandVAO();
			}
		}
		else {
			v.deleteVBOandVAO();
			distance = 1.0f;
			glFrontFace(GL_CW);
			for (i = 0; i<v.depthVolume; i++) {

				v.createQuadTDFrenteyAtras(distance);

				// UNIFORM DE TEXTURAS
				GLint text1Dloc = glsl2DProgram.getLocation("texture1D");
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_1D, t.texture1D);
				glUniform1i(text1Dloc, 0);

				GLint text2Dloc = glsl2DProgram.getLocation("quad_text");
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, v.z_texture[i]);
				glUniform1i(text2Dloc, 1);


				glBindVertexArray(v.textvao);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);

				distance -= v.z_dist;

				v.deleteVBOandVAO();
				
			}
		}
	}
	else if (abswx >= abswz && abswx >= abswy) {
		if (wx >= 0) {
			v.deleteVBOandVAO();
			distance = -1.0f;
			for (i = 0; i<v.widthVolume; i++) {
				v.createQuadTDDeryIzq(distance);

				// UNIFORM DE TEXTURAS
				GLint text1Dloc = glsl2DProgram.getLocation("texture1D");
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_1D, t.texture1D);
				glUniform1i(text1Dloc, 0);

				GLint text2Dloc = glsl2DProgram.getLocation("quad_text");
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, v.x_texture[i]);
				glUniform1i(text2Dloc, 1);


				glBindVertexArray(v.textvao);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);

				distance += v.x_dist;

				v.deleteVBOandVAO();
				
			}
		}
		else {
			v.deleteVBOandVAO();
			distance = 1.0f;
			glFrontFace(GL_CW);
			for (i = v.widthVolume - 1; i >= 0; i--) {
				v.createQuadTDDeryIzq(distance);

				// UNIFORM DE TEXTURAS
				GLint text1Dloc = glsl2DProgram.getLocation("texture1D");
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_1D, t.texture1D);
				glUniform1i(text1Dloc, 0);

				GLint text2Dloc = glsl2DProgram.getLocation("quad_text");
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, v.x_texture[i]);
				glUniform1i(text2Dloc, 1);


				glBindVertexArray(v.textvao);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);

				distance -= v.x_dist;

				v.deleteVBOandVAO();
				
			}
		}
	}
	else {
		if (wy >= 0) {
			v.deleteVBOandVAO();
			distance = -1.0f;
			for (i = 0; i<v.heightVolume; i++) {
				v.createQuadTDArryAba(distance);

				// UNIFORM DE TEXTURAS
				GLint text1Dloc = glsl2DProgram.getLocation("texture1D");
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_1D, t.texture1D);
				glUniform1i(text1Dloc, 0);

				GLint text2Dloc = glsl2DProgram.getLocation("quad_text");
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, v.y_texture[i]);
				glUniform1i(text2Dloc, 1);


				glBindVertexArray(v.textvao);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);
				distance += v.y_dist;

				v.deleteVBOandVAO();
				
			}
		}
		else {
			v.deleteVBOandVAO();
			distance = 1.0f;
			glFrontFace(GL_CW);
			for (i = v.heightVolume - 1; i >= 0; i--) {

				v.createQuadTDArryAba(distance);

				// UNIFORM DE TEXTURAS
				GLint text1Dloc = glsl2DProgram.getLocation("texture1D");
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_1D, t.texture1D);
				glUniform1i(text1Dloc, 0);

				GLint text2Dloc = glsl2DProgram.getLocation("quad_text");
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, v.y_texture[i]);
				glUniform1i(text2Dloc, 1);


				glBindVertexArray(v.textvao);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);

				distance -= v.y_dist;

				v.deleteVBOandVAO();
				
			}
		}
	}
	glFrontFace(GL_CCW);
	glsl2DProgram.disable();

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);

	////////////////////////////////////////////////////////////////////////////////////////////////

	t.display_transfer(); // Pintar la funcion de transferencia

	////////////////////////////////////////////////////////////////////////////////////////////////

}

//Función que dibuja los modelos
void display_Ray() {
	////////////////////////////////////////////////////////////////////////////////////////////////
	///Pintamos al FBO///
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 1, -1);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, v.FBO);
	glViewport(0, 0, width, height);
	glslFBOProgram.enable();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLuint view_matr_loc = glslFBOProgram.getLocation("view_matrix");
		GLuint model_matr_loc = glslFBOProgram.getLocation("model_matrix");
		GLuint proj_matr_loc = glslFBOProgram.getLocation("projection_matrix");
		
		view_mat = camera.GetViewMatrix();
		glUniformMatrix4fv(view_matr_loc, 1, GL_FALSE, glm::value_ptr(view_mat));

		project_mat = glm::perspective(camera.Zoom, (float)width / (float)height, 0.1f, 1000.0f);
		glUniformMatrix4fv(proj_matr_loc, 1, GL_FALSE, glm::value_ptr(project_mat));

		glm::mat4 model_mat = glm::mat4();
		model_mat = v.translate_en_matriz(v.ejeX, v.ejeY, v.ejeZ);
		model_mat = model_mat * v.rotacion_en_matriz(v.rotacion[0], v.rotacion[1], v.rotacion[2], v.rotacion[3]);
		model_mat = model_mat* v.scale_en_matriz(v.scaleT);

		glUniformMatrix4fv(model_matr_loc, 1, GL_FALSE, glm::value_ptr(model_mat));

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		
		glBindVertexArray(v.vao);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDisable(GL_CULL_FACE);

	glslFBOProgram.disable();

	////////////////////////////////////////////////////////////////////////////////////////////////
	///Pintamos el modelo///
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glslRayProgram.enable();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view_matr_loc = glslRayProgram.getLocation("view_matrix");
		model_matr_loc = glslRayProgram.getLocation("model_matrix");
		proj_matr_loc = glslRayProgram.getLocation("projection_matrix");
		GLuint width_loc = glslRayProgram.getLocation("width");
		GLuint height_loc = glslRayProgram.getLocation("height");
		GLuint width_volume_loc = glslRayProgram.getLocation("widthVolume");
		GLuint height_volume_loc = glslRayProgram.getLocation("heightVolume");
		GLuint depth_volume_loc = glslRayProgram.getLocation("depthVolume");
		GLuint view_loc = glslRayProgram.getLocation("view");
		GLuint light_pos_loc = glslRayProgram.getLocation("lightPos");
		GLuint lighting_loc = glslRayProgram.getLocation("lighting");

		glUniform1i(width_loc, width);
		glUniform1i(height_loc, height);
		glUniform1i(width_volume_loc, v.widthVolume);
		glUniform1i(height_volume_loc, v.heightVolume);
		glUniform1i(depth_volume_loc, v.widthVolume);
		glUniform3f(view_loc, camera.Position[0], camera.Position[1], camera.Position[2]);
		glUniform3f(light_pos_loc, ejeXL, ejeYL, ejeZL);
		glUniform1i(lighting_loc, toggleLight);

		view_mat = camera.GetViewMatrix();
		glUniformMatrix4fv(view_matr_loc, 1, GL_FALSE, glm::value_ptr(view_mat));

		project_mat = glm::perspective(camera.Zoom, (float)width / (float)height, 0.1f, 1000.0f);
		glUniformMatrix4fv(proj_matr_loc, 1, GL_FALSE, glm::value_ptr(project_mat));

		model_mat = glm::mat4();
		model_mat = v.translate_en_matriz(v.ejeX, v.ejeY, v.ejeZ);
		model_mat = model_mat * v.rotacion_en_matriz(v.rotacion[0], v.rotacion[1], v.rotacion[2], v.rotacion[3]);
		model_mat = model_mat* v.scale_en_matriz(v.scaleT);

		glUniformMatrix4fv(model_matr_loc, 1, GL_FALSE, glm::value_ptr(model_mat));
		
		// UNIFORM DE TEXTURAS
		GLint text2Dloc = glslRayProgram.getLocation("renderedTexture");
		glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, v.texture2D);
		glUniform1i(text2Dloc, 0);

		GLint text3Dloc = glslRayProgram.getLocation("rawTexture");
		glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_3D, v.texture3D);
		glUniform1i(text3Dloc, 1);

		GLint text1Dloc = glslRayProgram.getLocation("transgeneroTexture");
		glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_1D, t.texture1D);
		glUniform1i(text1Dloc, 2);
		//FIN UNIFORM DE TEXTURAS

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		
		glBindVertexArray(v.vao);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDisable(GL_CULL_FACE);

	glslRayProgram.disable();

	glDisable(GL_STENCIL_TEST);

	////////////////////////////////////////////////////////////////////////////////////////////////

	t.display_transfer(); // Pintar la funcion de transferencia

	////////////////////////////////////////////////////////////////////////////////////////////////
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 