#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>

#ifdef __APPLE__
	#include <libgen.h>
#endif

#include "../ressources/abstractions.h"
#include "../ressources/glDebug.h"


GLFWmonitor* monitor;
const GLFWvidmode* mode;
GLint WIDTH, HEIGHT;

//timing
double deltaTime = 0.0;	// time between current frame and last frame
double lastFrame = 0.0;


#if defined(_DEBUG) && !defined(__APPLE__)
// // Error messaging from OpenGL
void GLAPIENTRY
MessageCallback(GLenum source,
				GLenum type,
				GLuint id,
				GLenum severity,
				GLsizei length,
				const GLchar* message,
				const void* userParam)
{
	fprintf(stderr, "\n\nGL CALLBACK:\n  Source: %s\n    Type: %s\n      id: %d\nSeverity: %s\n Message: %s\n\n",
			debugSource(source), debugType(type), id, debugSeverity(severity), message);
	//glfwTerminate();
}
#endif

//glfw error callback
void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}


//Enables to resizes what is drawn along the window, if not used, the shape drawn would always stay the same size
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	WIDTH = width, HEIGHT = height;
}


int main(int argc, char const *argv[])
{
	if (!glfwInit())
		return -1;

	glfwSetErrorCallback(error_callback);

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	WIDTH = mode->width;
	HEIGHT = mode->height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); //Mac doesn't support beyond 4.1
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

#ifdef _DEBUG
	/*specifies whether the context should be created in debug mode, which may
	provide additional error and diagnostic reporting functionality*/
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(WIDTH / 2, HEIGHT / 2, "Julia Set", NULL, NULL); WIDTH /= 2; HEIGHT /= 2;//Windowed
	//GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Julia Set", glfwGetPrimaryMonitor(), NULL); //Fullscreen
#else
	// GLFWwindow* window = glfwCreateWindow(WIDTH / 2, HEIGHT / 2, "Julia Set", NULL, NULL); WIDTH /= 2; HEIGHT /= 2;//Windowed
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Julia Set", glfwGetPrimaryMonitor(), NULL); //Fullscreen
#endif
	if (window == NULL) { // glfwCreateWindow returns NULL if an error occurs
		fprintf(stderr, "Failed to initialize GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//These functions are called by glfwPollEvents
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// int w, h;
	// glfwGetFramebufferSize(window, &w, &h);
	// glViewport(0, 0, w, h);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //free movement

	/*
	GLFWimage icon[1];
	icon[0].pixels = stbi_load("../ressources/textures/Rosace.png", &icon[0].width, &icon[0].height, 0, 4);

	glfwSetWindowIcon(window, 1, icon);
	stbi_image_free(icon[0].pixels);*/

	//GLFWmonitor* monitor = glfwGetWindowMonitor(window);

	if (glewInit() != GLEW_OK) { //Initializes GLEW, if it's not done properly, terminate program
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(glewInit()));
		glfwTerminate();
		return -2;
	}

	#ifdef _DEBUG
	//INFO
	printf("Last compiled on %s at %s\n\n", __DATE__, __TIME__);
	printf("GPU Information:\n");
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("Shading Language Version: %s\n\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	#endif

	GLfloat quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	GLuint VAO; //Vertex array object
	glGenVertexArrays(1, &VAO);
	/* Any subsequent VBO, EBO, glVertexAttribPointer, and 
	glEnableVertexAttribArray will be stored inside the bound VAO */
	glBindVertexArray(VAO);
	
    GLuint VBO;
	glGenBuffers(1, &VBO); //Generating 1 buffer with this id.
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Making VBO the active array buffer to enable it to receive data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	
    GLuint shaderProgram;
	shaderProgram = glCreateProgram();
#ifdef WIN32
	const char *vert = "../Julia.vert";
	const char *frag = "../Julia.frag";
#elif defined(__APPLE__)
	const int size = strlen(argv[0]);
	char exe_path[size];
    strncpy(exe_path, argv[0], sizeof(exe_path));
    char *exe_dir = dirname(exe_path);
    // printf("Executable directory: %s\n", exe_dir);
    char vert[size+15];
    strcpy(vert, exe_dir);
    strcat(vert, "/Julia.vert");
    // printf("Vertex shader path: %s\n", vert);
    char frag[size+15];
    strcpy(frag, exe_dir);
    strcat(frag, "/Julia.frag");
    // printf("Fragment shader path: %s\n", frag);
#endif
// 	const char *vert = 
// "#version 410 core\n"
// "\n"
// "layout(location = 0) in vec2 aPosition;\n"
// "layout(location = 1) in vec2 aTexCoords;\n"
// "\n"
// "out vec2 TexCoords;\n"
// "\n"
// "void main()\n"
// "{\n"
// "    TexCoords = aTexCoords;\n"
// "    gl_Position = vec4(aPosition, 0.0, 1.0);\n"
// "}";

// 	const char *frag =
// "#version 410 core\n"
// "\n"
// "out vec4 outColor;\n"
// "\n"
// "uniform int width;\n"
// "uniform int height;\n"
// "\n"
// "uniform double xpos;\n"
// "uniform double ypos;\n"
// "\n"
// "vec2 xbounds = vec2(-2, 2);\n"
// "vec2 ybounds = vec2(-2, 2);\n"
// "\n"
// "void bind(inout double x, inout double y)\n"
// "{\n"
// "	x = x / width;\n"
// "	x = (((x - 0) * (xbounds.y - xbounds.x)) / (1 - 0)) + (xbounds.x);\n"
// "	y = y / height;\n"
// "	y = (((y - 0) * (ybounds.y - ybounds.x)) / (1 - 0)) + (ybounds.x);\n"
// "}\n"
// "\n"
// "void main()\n"
// "{\n"
// "	double x0 = gl_FragCoord.x; double y0 = gl_FragCoord.y;\n"
// "	bind(x0, y0);\n"
// "	dvec2 coords = dvec2(xpos, ypos);\n"
// "	bind(coords.x, coords.y);\n"
// "\n"
// "	double x = x0;\n"
// "	double y = y0;\n"
// "\n"
// "	int iter = 0;\n"
// "	int max_iter = 100;\n"
// "	while (x*x + y*y <= 4 && iter < max_iter) {\n"
// "	    double xtemp = x;\n"
// "        x = x*x - y*y + xpos;\n"
// "        y = 2*xtemp*y + ypos;\n"
// "        iter++;\n"
// "	}\n"
// "\n"
// "	vec3 color = vec3(1);\n"
// "    if (iter == max_iter) \n"
// "        outColor = vec4(vec3(0), 1);\n"
// "    else\n"
// "	    outColor = vec4(vec3(float(iter)/max_iter)*color, 1);\n"
// "}";
	
	loadShader(GL_VERTEX_SHADER, vert, shaderProgram, true);
	loadShader(GL_FRAGMENT_SHADER, frag, shaderProgram, true);
	loadProgram(shaderProgram);
	glUseProgram(shaderProgram); 

	double time;
	glfwSwapInterval(1); //With this, the GPU doesn't use 90% of it's processes. 1 is 60 fps, 2 is 30...
	while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) //Set Esc as a close command
	{
		time = glfwGetTime();
		deltaTime = time - lastFrame;
		lastFrame = time;

		double xpos, ypos;
		//getting cursor position
		glfwGetCursorPos(window, &xpos, &ypos);
		printf("Cursor Position at (%f : %f)                 \r", xpos, ypos);

		glClear(GL_COLOR_BUFFER_BIT);

		// SET1F(computeProgram, "time", (GLfloat)time);
		// SET1I(computeProgram, "BOUNDARY", BOUNDARY);
		// SET1F(computeProgram, "rmin", rmin);
		// SET1F(computeProgram, "rmax", rmax);
		#ifdef __APPLE__
		SET1I(shaderProgram, "width", WIDTH*2);
		SET1I(shaderProgram, "height", HEIGHT*2);
		#else
		SET1I(shaderProgram, "width", WIDTH);
		SET1I(shaderProgram, "height", HEIGHT);
		#endif
		SET1D(shaderProgram, "xpos", xpos / WIDTH);
		SET1D(shaderProgram, "ypos", ypos / HEIGHT);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Buffer swap and I/O processing
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
