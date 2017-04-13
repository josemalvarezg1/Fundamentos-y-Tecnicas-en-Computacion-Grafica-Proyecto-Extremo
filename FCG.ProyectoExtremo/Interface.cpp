#include "Interface.h"

extern vector<volume> volumes; //Todos los modelos irán en este vector
extern volume v;
extern Transferencia t;
extern TwBar *menuTW, *modelTW;
extern Camera camera;
extern int width, height, selectedModel;
extern bool selecting, toggleLight;
extern double xGlobal, yGlobal;
extern bool keys[1024]; //Todas las teclas				
extern float color_luz_ambiental[4], color_luz_difuso[4];
extern float scaleT, ejeX, ejeY, ejeZ, ejeXL, ejeYL, ejeZL;
extern GLfloat lastX, lastY;
extern glm::mat4 project_mat; //Matriz de Proyección
extern glm::mat4 view_mat; //Matriz de View
extern CGLSLProgram glslProgram; //Programa de shaders
extern CGLSLProgram glslDepthProgram; //Programa de shaders del depth map
extern float rotacion[];
extern theVolume currentVolume;
extern theDisplay currentDisplay;
extern bool inTransfer;
extern int posicion_puntoControl;
bool pressingRight = false, pressingLeft = false, afine = false;
glm::quat firstQuat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
GLint index = -1;

void Interface::TwEventMouseButtonGLFW3(GLFWwindow* window, int button, int action, int mods) {
	if (TwEventMouseButtonGLFW(button, action)) return;

	glReadPixels(xGlobal, height - yGlobal, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

	if (action == 1 && button == 1) { //Click derecho (Rotar)
		pressingRight = true;
	}
	if (action == 1 && button == 0) { //Click izquierdo (Trasladar)	
									  
		//Para dibujar los puntos
		if(!afine) t.checkClick(false);
		pressingLeft = true;
		t.obtenerColor(); //Chequeo click en colores
	}
	if (action == 0) { //suelto el click
		
		inTransfer = false;
		pressingRight = false;
		pressingLeft = false;
		//t.point_actual = -1;
		index = -1;
		afine = false;
	}

}

void Interface::cursorPos(GLFWwindow* window, double x, double y) {
	xGlobal = x;
	yGlobal = y;
	if (TwEventMousePosGLFW(x, y)) {
		lastX = x;
		lastY = y;
		return;
	}
	if (pressingRight && !inTransfer &&  index == 1) {
		afine = true;
		float angle = sqrt(pow((xGlobal - lastX),2) + pow((yGlobal - lastY),2)) / 100.0f;		
		glm::quat newQuat = glm::angleAxis(angle, glm::normalize(glm::vec3((yGlobal - lastY), (xGlobal - lastX), 0.0f)));
		firstQuat = glm::cross(newQuat, firstQuat);
		v.rotacion[0] = firstQuat[0];
		v.rotacion[1] = firstQuat[1];
		v.rotacion[2] = firstQuat[2];
		v.rotacion[3] = firstQuat[3];
	}
	if (pressingLeft) {	
		//Para dibujar los puntos
		if(!afine) {
			t.obtenerColor(); //Chequeo click en colores
			if (inTransfer) t.checkClick(true);
		}

		if (!inTransfer &&  index == 1) {
			afine = true;

			if (!keys['Z']) {
				//Traslacion en X
				if (xGlobal > lastX) v.ejeX += (xGlobal - lastX) / 500.0f;
				else if (xGlobal < lastX) v.ejeX -= (lastX - xGlobal) / 500.0f;

				//Traslacion en Y
				if (yGlobal > lastY) v.ejeY -= (yGlobal - lastY) / 500.0f;
				else if (yGlobal < lastY) v.ejeY += (lastY - yGlobal) / 500.0f;
			}
			else {
				//Traslacion en Z
				if (yGlobal > lastY) v.ejeZ += (yGlobal - lastY) / 500.0f;
				else if (yGlobal < lastY) v.ejeZ -= (lastY - yGlobal) / 500.0f;
			}
		}
		
	}
	lastX = x;
	lastY = y;
}

void Interface::TwEventMousePosGLFW3(GLFWwindow* window, double x, double y) {
	TwMouseMotion(int(x), int(y));
}

void Interface::cursorScroll(GLFWwindow* window, double xoffset, double yoffset) {
	if (TwEventMouseWheelGLFW(yoffset)) return;
	//Para escalar
	if (yoffset == 1) {
		scaleT += 0.05;
	}
	if (yoffset == -1) {
		if (scaleT >= 0.05)	scaleT -= 0.05;
	}
	v.scaleT = scaleT; // Actualizamos el valor del escalamiento del volumen
}

void Interface::TwEventKeyGLFW3(GLFWwindow* window, int key, int scancode, int action, int mods) {
	TwEventKeyGLFW(key, action);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)	glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) keys[key] = true;
		else if (action == GLFW_RELEASE) keys[key] = false;

		if  (key == 'D') {

			if (action == GLFW_PRESS) {
				t.deletePoint();
			}
		}
	}
}

void Interface::TwEventCharGLFW3(GLFWwindow* window, int codepoint) {
	TwEventCharGLFW(codepoint, GLFW_PRESS);
}

void Interface::TwWindowSizeGLFW3(GLFWwindow* window, int width, int height) {
	TwWindowSize(width, height);
}

//Función de salir
void TW_CALL exit(void *clientData) {
	exit(1);
}

//Activar o desactivar la luz
void TW_CALL setLight(const void *value, void *clientData) {
	toggleLight = *(const int *)value;
}

void TW_CALL getLight(void *value, void *clientData) {
	(void)clientData;
	*(int *)value = toggleLight;
}

void Interface::initAntTweakBar() {
	menuTW = TwNewBar("Menú");
	TwDefine("Menú size='270 250' position='20 20' color='128 0 0' label='Menú'");
	{
		TwEnumVal model[4] = { { Engine, "Engine (8 bits)" },{ Head, "Head (8 bits)" },{ Torso, "Torso (8 bits)" },{ Walnut, "Walnut (16 bits)" } };
		TwType selectModel = TwDefineEnum("selectedModel", model, 4);
		TwAddVarRW(menuTW, "Modelo", selectModel, &currentVolume, "");
	}

	{
		TwEnumVal displayMode[2] = { { RayC, "Ray Casting" },{ T2D, "Texturas 2D" } };
		TwType selectDisplay = TwDefineEnum("selectDisplay", displayMode, 2);
		TwAddVarRW(menuTW, "Despliegue", selectDisplay, &currentDisplay, "");
	}

	modelTW = TwNewBar("Figura");
	TwWindowSize(200, 400);
	TwDefine("Figura visible=false size='270 520' position='20 20' color='128 0 0' label='Volumen'");

	TwAddVarCB(menuTW, "toggleLight", TW_TYPE_BOOL32, setLight, getLight, NULL, " label='Prender/Apagar' group='Luz'");
	TwAddVarRW(menuTW, "ejeXL", TW_TYPE_FLOAT, &ejeXL, "step=0.01 label='x' group='Trasladar luz' group='Luz'");
	TwAddVarRW(menuTW, "ejeYL", TW_TYPE_FLOAT, &ejeYL, "step=0.01 label='y' group='Trasladar luz' group='Luz'");
	TwAddVarRW(menuTW, "ejeZL", TW_TYPE_FLOAT, &ejeZL, "step=0.01 label='z' group='Trasladar luz' group='Luz'");

	TwAddButton(menuTW, "exit", exit, NULL, " label='Salir' key=Esc");

	TwAddButton(modelTW, "exitF", exit, NULL, " label='Salir' key=Esc");
}