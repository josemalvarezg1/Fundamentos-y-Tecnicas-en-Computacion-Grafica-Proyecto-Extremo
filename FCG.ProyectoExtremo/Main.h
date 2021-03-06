#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include <iostream>
#include <cmath>
#include <AntTweakBar.h>
#include <IL/il.h> 
#include <climits>
#include <vector>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <Commdlg.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GLSLProgram.h"

//Para las normales y su offset
#define BUFFER_OFFSET(offset) ((char*)NULL + (offset))

typedef enum { Engine = 0, Head, Torso, Walnut} theVolume;
typedef enum { RayC = 0, T2D } theDisplay;

void reshape(GLFWwindow* window, int w, int h);
void moverme();
int main(int argc, char* argv[]);