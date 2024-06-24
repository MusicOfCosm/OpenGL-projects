#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//#define BASICS
//#define LIGHTING
//#define MODEL
//#define ADVANCED

#include <GL/glew.h>
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <time.h>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif


#ifndef ABSTRACTIONS
#define ABSTRACTIONS
#define glmVEC2(vec2) vec2.x, vec2.y
#define glmVEC3(vec3) vec3.x, vec3.y, vec3.z
#define glmVEC4(vec4) vec4.x, vec4.y, vec4.z, vec4.w
#define VEC2(n) n, n
#define VEC3(n) n, n, n
#define VEC4(n) n, n, n, n
#define combVEC2(glm, n) glm.x*n, glm.y*n
#define combVEC3(glm, n) glm.x*n, glm.y*n, glm.z*n
#define combVEC4(glm, n) glm.x*n, glm.y*n, glm.z*n, glm.w*n

#define SET1I(program, varName, value ) glUniform1i(glGetUniformLocation(program, varName), value)
#define SET2I(program, varName, values) glUniform2i(glGetUniformLocation(program, varName), values)
#define SET3I(program, varName, values) glUniform3i(glGetUniformLocation(program, varName), values)

#define SET1F(program, varName, value ) glUniform1f(glGetUniformLocation(program, varName), value)
#define SET2F(program, varName, valueX, valueY) glUniform2f(glGetUniformLocation(program, varName), valueX, valueY)
#define SET3F(program, varName, values) glUniform3f(glGetUniformLocation(program, varName), values)
#define SET4F(program, varName, values) glUniform4f(glGetUniformLocation(program, varName), values)

#define SET1D(program, varName, value ) glUniform1d(glGetUniformLocation(program, varName), value)
#define SET2D(program, varName, values) glUniform2d(glGetUniformLocation(program, varName), values)
#define SET3D(program, varName, values) glUniform3d(glGetUniformLocation(program, varName), values)

#define SET3FV(program, varName, instances, values) glUniform3fv(glGetUniformLocation(program, varName), instances, glm::value_ptr(values))
#define SET4FV(program, varName, instances, values) glUniform4fv(glGetUniformLocation(program, varName), instances, glm::value_ptr(values))
#define SETMAT(program, varName, values) glUniformMatrix4fv(glGetUniformLocation(program, varName), 1, GL_FALSE, glm::value_ptr(values))
#endif



/**
 * \brief Compiles a given shader and attaches it with the program, checks for errors, and returns the ID of the shader.
 *
 * \param shaderType: The type of the shader such as GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
 * \param shaderSourceCode: The filepath of the shader code written in glsl.
 * \param shaderProgram: The ID of the program to which the compiled shaders will be linked to.
 * \return The ID of the shader, which can be used as parameter for other OpenGL functions.
 */
GLuint loadShader(GLenum shaderType, const char* shaderSourceCode, GLuint shaderProgram, bool loadFromFile);

/** 
 * @brief Links all the attached shaders to the program, checks for errors, and binds the shader program to tell OpenGL to use it to draw.
 * @param shaderProgram The program to which the loaded shaders will be linked to.
 */
void loadProgram(GLuint shaderProgram);

/**
 * @brief Binds a texture to a texture unit.
 * @param textureUnit GL_TEXTUREx, where x is a number beginning at 0. The number of available texture units depends on your GPU.
 * @param texFilePath The filepath of the image that will be used to generate the texture.
 * @return The ID of the texture unit.
 */
GLuint loadTexture2D(GLenum textureUnit, const char* texFilePath);



/**
 * @brief Make the program wait to set a framerate
 * @param start Value used to calculate the amount of time to wait. It must be set at the start of your while loop.
 * @param cap The framerate cap. If 0, will default to 60.
 */
void bindFramerate(clock_t start, int cap);

// typedef struct { int x; int y; } ivec2;
// typedef struct { float x; float y; } vec2;
// ivec2 ndcToScr(float x, float y, int scrWidth, int scrHeight);
// vec2  scrToNdc(  int x,   int y, int scrWidth, int scrHeight);

#ifdef __cplusplus
}
#endif
