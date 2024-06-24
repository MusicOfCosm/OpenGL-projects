#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <thread>
#include <random>
#include <ctime>
#define _USE_MATH_DEFINES
#include <math.h>
//#include <array>

#include "../../ressources/abstractions.h"
#include "../../ressources/renderer.hpp"
#include "../../ressources/glDebug.h"

const unsigned int seed = (unsigned int)time(0);
std::mt19937 gen(seed);

int randint(int min, int max) {
	std::uniform_int_distribution<int> rdint(min, max);
	return rdint(gen);
}

float uniform(float min, float max) {
	std::uniform_real_distribution<float> rdfloat(min, max);
	return rdfloat(gen);
}

#ifdef _DEBUG
//Error messaging from OpenGL
void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	fprintf(stderr, "\n\nGL CALLBACK:\nSource:   %s\nType:     %s\nid:       %d\nSeverity: %s\nMessage:  %s\n\n",
			debugSource(source), debugType(type), id, debugSeverity(severity), message);
	exit(EXIT_FAILURE);
}
#endif