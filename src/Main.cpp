#include "Main.h"
#include "Volume.h"
#include "Interface.h"
#include "Render.h"
#include "Transfer.h"


using namespace std;

int width = 1024, height = 768, selectedModel = 0;
GLuint frame = 0;
bool selecting = false, toggleLight = false;
double xGlobal, yGlobal;
bool keys[1024]; //Todas las teclas				
Camera camera(glm::vec3(0.0f, 0.0, 2.0f));
GLfloat lastX = 400, lastY = 300, deltaTime = 0.0f, lastFrame = 0.0f, currenttime = 0.0f, timebase = 0.0f;
volume v;
Transferencia t;
Interface i;
TwBar *menuTW;
TwBar *modelTW;
GLFWwindow* window;

//			Variables globales a usar en AntTweakBar
float color_luz_ambiental[] = { 0.0f, 0.0f, 0.0f, 0.0 };
float color_luz_difuso[] = { 1.0f, 1.0f, 1.0f, 0.0 };
float scaleT = 0.5 , ejeX = 0.0, ejeY = 0.0, ejeZ = 0.0, ejeXL = 1.0, ejeYL = 2.0, ejeZL = 1.0;


float rotacion[] = { 0.0f, 0.0f, 0.0f, 1.0f };

CGLSLProgram glslFBOProgram; //Programa de shaders que crea el FBO
CGLSLProgram glslRayProgram; //Programa de shaders del ray casting
CGLSLProgram glsl2DProgram; //Programa de shaders de texturas 2D

glm::mat4 project_mat; //Matriz de Proyeccion
glm::mat4 view_mat; //Matriz de View
glm::mat4 view_mat_transfer; //Matriz de View para la transferencia
glm::vec3 eye(0.0f, 0.0f, 2.0f); // Ojo

vector<volume> volumes; //Todos los modelos iran en este vector

//El modelo por defecto es Engine
theVolume currentVolume = Engine;
theVolume lastVolume = Engine;

//El metodo de despliegue por defecto es Ray Casting
theDisplay currentDisplay = RayC;

//Si el click esta en la funcion de transferencia
bool inTransfer = false;

//Funcion de reshape
void reshape(GLFWwindow* window, int w, int h) {
	
	w = max(w, 1);
	h = max(h, 1);
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	TwWindowSize(width, height);
	project_mat = glm::perspective(45.0f, (float)width / (float)height, 1.0f, 1000.0f);

	//Por cada reshape re hacemos la textura y la cargamos al FBO correspondiente
	v.create2Dtext();
	v.createFBO();
}

//Funcion que mueve la camara con WS
void moverme() {
	if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);
}

//Impresión de pantalla y lectura de shaders
bool initGlew() {

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		return false;
	else {
		std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
		std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

		//Leemos y cargamos los shaders correspondientes
		glslRayProgram.loadShader("Shaders/charles_ray.vert", CGLSLProgram::VERTEX);
		glslRayProgram.loadShader("Shaders/charles_ray.frag", CGLSLProgram::FRAGMENT);

		glslRayProgram.create_link();

		glslRayProgram.enable();

			glslRayProgram.addAttribute("position");
			glslRayProgram.addAttribute("ver_coord");

			glslRayProgram.addUniform("view_matrix");
			glslRayProgram.addUniform("projection_matrix");
			glslRayProgram.addUniform("model_matrix");
			glslRayProgram.addUniform("view");
			glslRayProgram.addUniform("renderedTexture");
			glslRayProgram.addUniform("rawTexture");
			glslRayProgram.addUniform("transgeneroTexture");
			glslRayProgram.addUniform("width");
			glslRayProgram.addUniform("height");
			glslRayProgram.addUniform("widthVolume");
			glslRayProgram.addUniform("heightVolume");
			glslRayProgram.addUniform("depthVolume");
			glslRayProgram.addUniform("lightPos");
			glslRayProgram.addUniform("lighting");

		glslRayProgram.disable();

		glslFBOProgram.loadShader("Shaders/FBO.vert", CGLSLProgram::VERTEX);
		glslFBOProgram.loadShader("Shaders/FBO.frag", CGLSLProgram::FRAGMENT);

		glslFBOProgram.create_link();

		glslFBOProgram.enable();

			glslFBOProgram.addAttribute("position");
			glslFBOProgram.addAttribute("ver_coord");

			glslFBOProgram.addUniform("view_matrix");
			glslFBOProgram.addUniform("projection_matrix");
			glslFBOProgram.addUniform("model_matrix");

		glslFBOProgram.disable();

		glsl2DProgram.loadShader("Shaders/2D_tessellation.vert", CGLSLProgram::VERTEX);
		glsl2DProgram.loadShader("Shaders/2D_tessellation.frag", CGLSLProgram::FRAGMENT);

		glsl2DProgram.create_link();

		glsl2DProgram.enable();

			glsl2DProgram.addAttribute("position");
			glsl2DProgram.addAttribute("ver_coord");

			glsl2DProgram.addUniform("view_matrix");
			glsl2DProgram.addUniform("projection_matrix");
			glsl2DProgram.addUniform("model_matrix");
			glsl2DProgram.addUniform("texture1D");
			glsl2DProgram.addUniform("quad_text");
			

		glsl2DProgram.disable();


		t.initShaders();
		return true;
	}

}

//Se actualiza el texture3D segun el modelo seleccionado en la interfaz
void checkSelectedModel() {

	if (currentVolume != lastVolume) {
				
		switch (currentVolume) {

			case 0:
				v.read_raw("Modelos/raw/engine.raw", 256, 256, 256, 1);
				lastVolume = Engine;
				break;

			case 1:
				v.read_raw("Modelos/raw/head.raw", 256, 256, 256, 1);
				lastVolume = Head;
				break;

			case 2:
				v.read_raw("Modelos/raw/torso512.raw", 256, 256, 114, 1);
				lastVolume = Torso;
				break;

			case 3:
				v.read_raw("Modelos/raw/walnut_400x296x352_16.raw", 400, 296, 352, 2);
				lastVolume = Walnut;
				break;

		}
	}

}

int main(int argc, char* argv[]) {

	if (!glfwInit()) {
		std::cerr << "Error inicializando glfw...\n";
		return 0;
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(1024, 768, "F.C.G. Proyecto Extremo - Alejandro Barone, Jose Manuel Alvarez", nullptr, nullptr);
	if (!window) {
		std::cerr << "Error creando ventana...\n";
		glfwTerminate();
		return 0;
	}

	glfwSetFramebufferSizeCallback(window, reshape);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	TwInit(TW_OPENGL, NULL);
	initGlew();

	i.initAntTweakBar();
	glfwSetWindowSizeCallback(window, i.TwWindowSizeGLFW3);

	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)i.TwEventMouseButtonGLFW3);
	glfwSetCursorPosCallback(window, i.cursorPos);
	glfwSetScrollCallback(window, i.cursorScroll);
	glfwSetKeyCallback(window, (GLFWkeyfun)i.TwEventKeyGLFW3);
	glfwSetCharCallback(window, (GLFWcharfun)i.TwEventCharGLFW3);
	reshape(window, 1024, 768);
	glewInit();
	
	v.read_raw("Modelos/raw/engine.raw", 256, 256, 256, 1); //Leemos modelo 3D y cargamos la textura
	
	v.createCube(); //Creamos el cubo que contendra el volumen

	t.initValues(); //Iniciamos los valores de la transferencia
	
	t.refreshValues(); //Creamos la textura 1D de transferencia
	t.createQuad(); //Creamos el Quad de la transferencia

	view_mat_transfer = camera.GetViewMatrix();
	while (!glfwWindowShouldClose(window)) {
		//Este es el Main loop
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		currenttime = currentFrame;
		lastFrame = currentFrame;
		
		if (currentDisplay == RayC) {
			display_Ray(); //Funcion que despliega el volumen en Ray Casting
			TwDefine("Menú/Luz visible=true");
		}
		else {
			display_2D(); //Funcion que despliega el volumen en Texturas 2D 
			TwDefine("Menú/Luz visible=false");
		}

		TwDraw(); //Ant Tweak Bar
		moverme(); //Mover camara
		checkSelectedModel(); //Se actualiza el texture3D o texture2D
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	TwTerminate();
	glfwTerminate();
	return 0;
}