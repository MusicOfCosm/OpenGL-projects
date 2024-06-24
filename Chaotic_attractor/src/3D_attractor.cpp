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
#ifdef __APPLE__
	#include <ctime>
	#include <chrono>
#endif

#include "../../ressources/renderer.hpp"
// #define STEREO

unsigned int seed = (unsigned int)time(0);
std::mt19937 gen(seed);

int randint(int min, int max) {
	std::uniform_int_distribution<int> rdint(min, max);
	return rdint(gen);
}

template <typename fpoint> 
fpoint uniform(fpoint min, fpoint max) {
	std::uniform_real_distribution<fpoint> rdfloat(min, max);
	return rdfloat(gen);
}


#if defined(_DEBUG) && !defined(__APPLE__)
//Error messaging from OpenGL
void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "\n\nGL CALLBACK: %s type = 0x%x, severity = 0x%x, \nmessage = %s\n\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
	//exit(EXIT_FAILURE);
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
}

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

GLFWmonitor* monitor;
const GLFWvidmode* mode;
unsigned int WIDTH;
unsigned int HEIGHT;

//camera
glm::vec3 cameraPos = { 0.0f, 0.0f, 100.0f };
glm::vec3 cameraFront = { 0.0f, 0.0f, -1.0f };//direction the camera is looking
glm::vec3 cameraUp = { 0.0f, 1.0f, 0.0f }; //the "up" direction in regards to where the camera is pointed too

//inputs
bool firstMouse = true;
bool manipulate = false;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees as 0.0 would results in a direction vector pointing to the right.
float pitch = 0.0f;
float lastX;
float lastY;
float fov = 45.0f;

float xRotation = 0.f;
float yRotation = 0.f;

//timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


//int main()
int main()
{
	if (!glfwInit())
		return -1;

	glfwSetErrorCallback(error_callback);

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	WIDTH = mode->width;
	HEIGHT = mode->height;
	lastX = WIDTH / 2.f;
	lastY = HEIGHT / 2.f;

	//Hints: Tells OpenGL what version to use, if none is specified, it uses the latest
#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); //Mac doesn't support beyond 4.1
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL testing", NULL, NULL); //Fullscreen
#endif
#ifdef _DEBUG
	/*specifies whether the context should be created in debug mode, which may
	provide additional error and diagnostic reporting functionality*/
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

#ifndef __APPLE__
	//GLFWwindow* window = glfwCreateWindow(WIDTH/2, HEIGHT/2, "OpenGL testing", NULL, NULL); //Windowed
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL testing", glfwGetPrimaryMonitor(), NULL); //Fullscreen
#endif
	if (window == NULL) { // glfwCreateWindow returns NULL if an error occurs
		fprintf(stderr, "Failed to initialize GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	GLFWimage icon[1];
	icon[0].pixels = stbi_load("../../ressources/textures/Rosace.png", &icon[0].width, &icon[0].height, 0, 4);
	glfwSetWindowIcon(window, 1, icon);
	stbi_image_free(icon[0].pixels);

	GLFWmonitor* monitor = glfwGetWindowMonitor(window);

	if (glewInit() != GLEW_OK) { //Initializes GLEW, if it's not done properly, terminate program
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(glewInit()));
		glfwTerminate();
		return -2;
	}

#if defined(_DEBUG) && !defined(__APPLE__)
	glEnable(GL_DEBUG_OUTPUT); //see the beginning of the file
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, nullptr);

	//INFO
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("Shading Language Version: %s\n\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif

	//These functions are called by glfwPollEvents
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //free movement
	glfwSetScrollCallback(window, scroll_callback);

	std::vector<GLfloat> vertices = {
		//Positions         
		//Back															
		-0.5f, -0.5f, -0.5f,  
		 0.5f, -0.5f, -0.5f,  
		 0.5f,  0.5f, -0.5f,  
		 0.5f,  0.5f, -0.5f,  
		-0.5f,  0.5f, -0.5f,  
		-0.5f, -0.5f, -0.5f,  
		//Front					
		-0.5f, -0.5f,  0.5f,  
		 0.5f, -0.5f,  0.5f,  
		 0.5f,  0.5f,  0.5f,  
		 0.5f,  0.5f,  0.5f,  
		-0.5f,  0.5f,  0.5f,  
		-0.5f, -0.5f,  0.5f,  
		//Left					
		-0.5f,  0.5f,  0.5f,  
		-0.5f,  0.5f, -0.5f,  
		-0.5f, -0.5f, -0.5f,  
		-0.5f, -0.5f, -0.5f,  
		-0.5f, -0.5f,  0.5f,  
		-0.5f,  0.5f,  0.5f,  
		//Right					
		 0.5f,  0.5f,  0.5f,  
		 0.5f,  0.5f, -0.5f,  
		 0.5f, -0.5f, -0.5f,  
		 0.5f, -0.5f, -0.5f,  
		 0.5f, -0.5f,  0.5f,  
		 0.5f,  0.5f,  0.5f,  
		 //Bottom				
		-0.5f, -0.5f, -0.5f,  
		 0.5f, -0.5f, -0.5f,  
		 0.5f, -0.5f,  0.5f,  
		 0.5f, -0.5f,  0.5f,  
		-0.5f, -0.5f,  0.5f,  
		-0.5f, -0.5f, -0.5f,  
		//Up					
		-0.5f,  0.5f, -0.5f,  
		 0.5f,  0.5f, -0.5f,  
		 0.5f,  0.5f,  0.5f,  
		 0.5f,  0.5f,  0.5f,  
		-0.5f,  0.5f,  0.5f,  
		-0.5f,  0.5f, -0.5f
	};

	////Only need 4 position vertices instead of six to draw two triangles
	//GLuint indices[] = {  //It starts from 0, and are specified by the vertexattribpointer
	//0, 1, 2,	1, 2, 3, //Front
	//4, 5, 6,	5, 6, 7, //Back
	//0, 2, 4,	2, 4, 6, //Left
	//1, 3, 5,	3, 5, 7, //Right
	//0, 1, 4,	1, 4, 5, //Bottom
	//2, 3, 6,	3, 6, 7 //Top
	//}; //If a vertex doesn't exist, it makes a new one at position 0, 0, 0

	GLint attribSizes[] = { 3 };
	Object obj(vertices, attribSizes, sizeof attribSizes / sizeof vertices[0]);

	const char attractor_vert[] =
#ifdef __APPLE__
	"#version 410 core\n"
#else
	"#version 460 core\n"
#endif
R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in float aColor;
layout(location = 2) in vec3 aChange;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float scale;

out float color;

void main()
{
	color = aColor;
	gl_Position = projection * view * model * vec4(scale * .07 * aPosition + aChange, 1.0);
})";
	obj.loadShader(obj.vertexShader, attractor_vert, false);
	const char attractor_frag[] = 
#ifdef __APPLE__
	"#version 410 core\n"
#else
	"#version 460 core\n"
#endif
R"(
out vec4 outColor;

in float color;

uniform vec3 fastColor;
uniform vec3 slowColor;

uniform float colorGradientSpeed;

vec3 result;

void main()
{
	if (color >= colorGradientSpeed)
		result = fastColor;
	else {
		float percent = color / colorGradientSpeed;
		result = slowColor + percent * (fastColor - slowColor);
	}

	outColor = vec4(result, 1.0);
})";
	obj.loadShader(obj.fragmentShader, attractor_frag, false);
	obj.loadProgram();


	IMGUI_CHECKVERSION();
	// Setup Dear ImGui context
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __APPLE__
	ImGui_ImplOpenGL3_Init("#version 410");
#else
	ImGui_ImplOpenGL3_Init("#version 460");
#endif
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	const int instances = 60000;
	std::vector<glm::dvec3> pos;
	std::vector<glm::dvec3> oldPos;
	std::vector<GLfloat> color(instances);
	//std::vector<glm::vec4> pos;
	for (int i = 0; i < instances; ++i)
		pos.push_back(glm::dvec3(uniform(-10., 10.), uniform(-10., 10.), uniform(-10., 10.)/* ,uniform(-20.f, 20.f)*/));
	oldPos = pos;

	glm::vec3 fastColor = { uniform(0.2f, 0.6f), uniform(0.2f, 0.6f), uniform(0.2f, 0.6f) };
	glm::vec3 slowColor = { uniform(0.6f, 1.0f), uniform(0.6f, 1.0f), uniform(0.6f, 1.0f) };

	GLuint colorVBO;
	glGenBuffers(1, &colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instances, &color[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);

	GLuint instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::dvec3) * instances, &pos[0], GL_STATIC_DRAW);
//(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
//(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer);
	glVertexAttribLPointer(2, 3, GL_DOUBLE, 3 * sizeof(GLdouble), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	//float delta = 1.f, 
	float sigma = 1., rho = 1., beta = 1.;
	float a = 1., b = 1., c = 1., d = 1., e = 1., f = 1.;

	float gradientSpeed = 1.f;
	float dt = .0075f;
	float scale = 1.f;
	float preDT = dt;
	float preScale = scale;
	int change = -1;
	int attractor = randint(0, 5);
	float time;
#ifdef __APPLE__
	clock_t start, finish;
    float iter_time;
	float diff, cap = 1.f/60;
#endif
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //GL_LESS is default
	while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) //Set Esc as a close command
	{
#ifdef __APPLE__
		start = clock();
#endif
		time = (float)glfwGetTime();
		deltaTime = time - lastFrame;
		lastFrame = time;
		//float iter = round(time*60);
		processInput(window);

		// Clears the color buffer with a particular color
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /* | GL_STENCIL_BUFFER_BIT*/);

		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
	
#pragma region Attractors
		if (attractor == 0) { //Lorenz
			for (int i = 0; i < instances; ++i) {

				double dx = (sigma * (pos[i].y - pos[i].x)) * dt;
				double dy = (pos[i].x * (rho - pos[i].z) - pos[i].y) * dt;
				double dz = (pos[i].x * pos[i].y - beta * pos[i].z) * dt;

				oldPos[i] = pos[i];
				pos[i].x += dx;
				pos[i].y += dy;
				pos[i].z += dz;
				if (std::isnan(pos[i].x))
					pos[i] = glm::dvec3(uniform(-10., 10.), uniform(-10., 10.), uniform(-10., 10.));
				color[i] = (float)sqrt(pow(pos[i].x - oldPos[i].x, 2) + pow(pos[i].y - oldPos[i].y, 2) + pow(pos[i].z - oldPos[i].z, 2));
			}
		}
		else if (attractor == 1) { //Halvorsen
			for (int i = 0; i < instances; ++i) {
				double dx = (-a * pos[i].x - 4 * pos[i].y - 4 * pos[i].z - (float)pow(pos[i].y, 2)) * dt;
				double dy = (-a * pos[i].y - 4 * pos[i].z - 4 * pos[i].x - (float)pow(pos[i].z, 2)) * dt;
				double dz = (-a * pos[i].z - 4 * pos[i].x - 4 * pos[i].y - (float)pow(pos[i].x, 2)) * dt;

				oldPos[i] = pos[i];
				pos[i].x += dx;
				pos[i].y += dy;
				pos[i].z += dz;
				if (std::isnan(pos[i].x))
					pos[i] = glm::dvec3(uniform(-10., 10.), uniform(-10., 10.), uniform(-10., 10.));
				color[i] = (float)sqrt(pow(pos[i].x - oldPos[i].x, 2) + pow(pos[i].y - oldPos[i].y, 2) + pow(pos[i].z - oldPos[i].z, 2));
			}
		}
		else if (attractor == 2) { //Nose-Hoover
			for (int i = 0; i < instances; ++i) {
				double dx = pos[i].y * dt;
				double dy = (-pos[i].x + pos[i].y * pos[i].z) * dt;
				double dz = (a-(float)pow(pos[i].y, 2)) * dt;

				oldPos[i] = pos[i];
				pos[i].x += dx;
				pos[i].y += dy;
				pos[i].z += dz;
				if (std::isnan(pos[i].x))
					pos[i] = glm::dvec3(uniform(-10., 10.), uniform(-10., 10.), uniform(-10., 10.));
				color[i] = (float)sqrt(pow(pos[i].x - oldPos[i].x, 2) + pow(pos[i].y - oldPos[i].y, 2) + pow(pos[i].z - oldPos[i].z, 2));
			}
		}
		else if (attractor == 3) { //Chen-Lee
			for (int i = 0; i < instances; ++i) {
				double dx = (a * pos[i].x - pos[i].y * pos[i].z) * dt;
				double dy = (b * pos[i].y + pos[i].x * pos[i].z) * dt;
				double dz = (c * pos[i].z + pos[i].x * pos[i].y/3) * dt;

				oldPos[i] = pos[i];
				pos[i].x += dx;
				pos[i].y += dy;
				pos[i].z += dz;
				if (std::isnan(pos[i].x) || std::isnan(pos[i].y) || std::isnan(pos[i].z))
					pos[i] = glm::dvec3(uniform(-10., 10.), uniform(-10., 10.), uniform(-10., 10.));
				color[i] = (float)sqrt(pow(pos[i].x - oldPos[i].x, 2) + pow(pos[i].y - oldPos[i].y, 2) + pow(pos[i].z - oldPos[i].z, 2));
			}
		}
		else if (attractor == 4) { //Thomas
			for (int i = 0; i < instances; ++i) {
				double dx = (-b * pos[i].x + sin(pos[i].y)) * dt;
				double dy = (-b * pos[i].y + sin(pos[i].z)) * dt;
				double dz = (-b * pos[i].z + sin(pos[i].x)) * dt;

				oldPos[i] = pos[i];
				pos[i].x += dx;
				pos[i].y += dy;
				pos[i].z += dz;
				if (std::isnan(pos[i].x) || std::isnan(pos[i].y) || std::isnan(pos[i].z))
					pos[i] = glm::dvec3(uniform(-10., 10.), uniform(-10., 10.), uniform(-10., 10.));
				color[i] = (float)sqrt(pow(pos[i].x - oldPos[i].x, 2) + pow(pos[i].y - oldPos[i].y, 2) + pow(pos[i].z - oldPos[i].z, 2));
			}
		}else if (attractor == 5) { //Dequan Li
			for (int i = 0; i < instances; ++i) {
				double dx = (a * (pos[i].y - pos[i].x) + c * pos[i].x * pos[i].z) * dt;
				double dy = (e * pos[i].x + f * pos[i].y - pos[i].x * pos[i].z) * dt;
				double dz = (b * pos[i].z + pos[i].x * pos[i].y - d * (float)pow(pos[i].x, 2)) * dt;

				oldPos[i] = pos[i];
				pos[i].x += dx;
				pos[i].y += dy;
				pos[i].z += dz;
				if (std::isnan(pos[i].x) || std::isnan(pos[i].y) || std::isnan(pos[i].z))
					pos[i] = glm::dvec3(uniform(-10., 10.), uniform(-10., 10.), uniform(-10., 10.));
				color[i] = (float)sqrt(pow(pos[i].x - oldPos[i].x, 2) + pow(pos[i].y - oldPos[i].y, 2) + pow(pos[i].z - oldPos[i].z, 2));
			}
		}
#pragma endregion

		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instances, &color[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::dvec3) * instances, &pos[0], GL_STATIC_DRAW);
	
		//std::cout << color[0] << "       " << '\r';
		//float hx = 0, lx = 0, hy = 0, ly = 0, hz = 0, lz = 20;

		int width, height;
        glfwGetWindowSize(window, &width, &height);

#ifdef STEREO
		float shift;
		glm::vec3 oldCam = cameraPos;
		for (int i = 0; i < 2; i++)
		{
	#ifdef __APPLE__
			if (i == 0) {
				glViewport(width, 0, width, height*2);
				shift = -1.f;
			}
			else {
				glViewport(0, 0, width, height*2);
				shift = 1.f;
			}
	#else
			if (i == 0) {
				// Set the viewport for the right eye
				glViewport(width / 2, 0, width / 2, height);
				shift = -1.f;
			}
			else {
				// Set the viewport for the left eye
				glViewport(0, 0, width / 2, height);
				shift = 1.f;
			}
	#endif
			cameraPos += shift * glm::normalize(glm::cross(cameraFront, cameraUp));
#else
	#ifdef __APPLE__
		glViewport(0, 0, width*2, height*2);
	#else
		glViewport(0, 0, width, height);
	#endif
#endif

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(xRotation), glm::vec3(1.f, 0.f, 0.f));
			model = glm::rotate(model, glm::radians(yRotation), glm::vec3(0.f, 1.f, 0.f));
			glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.01f, 1000.0f);

			SETMAT(obj.shaderProgram, "model", model);
			SETMAT(obj.shaderProgram, "view", view);
			SETMAT(obj.shaderProgram, "projection", projection);

			SET3F(obj.shaderProgram, "fastColor", glmVEC3(fastColor));
			SET3F(obj.shaderProgram, "slowColor", glmVEC3(slowColor));
			SET1F(obj.shaderProgram, "scale", scale);
			SET1F(obj.shaderProgram, "colorGradientSpeed", gradientSpeed);

			obj.DrawInstances(GL_TRIANGLES, instances);

#ifdef STEREO
			cameraPos = oldCam;
		}
#endif

#pragma region Imgui
		ImGui::Begin("Settings");
		ImGui::Text("%f fps", 1 / deltaTime);
		//ImGui::Text("Pos[0]:\nx=%f\ny=%f\nz=%f", glmVEC3(pos[0]));
		double run = glfwGetTime();
		int hours = int(run / 3600);
		int minutes = int(fmod(run, 3600) / 60); //fmod is like the % operator that can work with floats and doubles
		double seconds = fmod(fmod(run, 3600), 60);
		ImGui::Text("Time run: %d hours, %d minutes, and %f seconds", hours, minutes, seconds);

		ImGui::ColorEdit3("Fast Color", &fastColor.x);
		ImGui::ColorEdit3("Slow Color", &slowColor.x);
		ImGui::SliderFloat("Color gradient speed", &gradientSpeed, .0f, 1.5f);
		ImGui::Separator();
		ImGui::SliderFloat("Rate of change dt", &dt, .000001f, 0.05f);
		ImGui::SliderFloat("scale", &scale, .0f, 5.0f);
		ImGui::Text("");
		ImGui::Separator();
		ImGui::Text("");
		ImGui::BulletText("Choose a chaotic attractor!");
		const char* items[] = { "Lorenz", "Halvorsen", "Nose-Hoover", "Chen-Lee", "Thomas", "Dequan Li" };
		ImGui::Combo(" ", &attractor, items, IM_ARRAYSIZE(items));
		
		if (attractor == 0) { //Lorenz
			if (change != 0) {
				sigma = 10, rho = 28, beta = 8.f / 3;
				dt = preDT; scale = preScale;
			}
			change = attractor;
			preDT = dt; preScale = scale;

			ImGui::Text("dx/dt = sigma * (y - x)"
						"\ndy/dt = x * (rho - z) - y"
						"\ndz/dt = x * y - beta * z");
			ImGui::SliderFloat("sigma", &sigma, .0, 30.0);
			ImGui::SliderFloat(  "rho",   &rho, .0, 30.0);
			ImGui::SliderFloat( "beta",  &beta, .0, 30.0);
		}
		else if (attractor == 1) { //Halvorsen
			if (change != attractor) {
				a = 1.89f;
				dt = preDT; scale = preScale;
			}
			change = attractor;
			preDT = dt; preScale = scale;
			
			ImGui::Text("dx/dt = -ax - 4y - 4z - y²"
						"\ndy/dt = -ay - 4z - 4x - z²"
						"\ndz/dt = -az - 4x - 4y - x²");
			ImGui::SliderFloat("a", &a, 0., 5.0);
		}
		else if (attractor == 2) { //Nose-Hoover
			if (change != attractor) {
				a = 1.5;
				dt = preDT; scale = preScale;
			}
			change = attractor;
			preDT = dt; preScale = scale;
			
			ImGui::Text(  "dx/dt = y"
						"\ndy/dt = -x + yz"
						"\ndz/dt = a-y²");
			ImGui::SliderFloat("a", &a, 0., 10.0);
		}
		else if (attractor == 3) { //Chen-Lee
			if (change != attractor) {
				a = 5.f, b = -10.f, c = -0.38f;
				dt = preDT; scale = preScale;
			}
			change = attractor;
			preDT = dt; preScale = scale;
			
			ImGui::Text(  "dx/dt = ax - yz"
						"\ndy/dt = by + xz"
						"\ndz/dt = cz + xy/3");
			ImGui::SliderFloat("a", &a,  -5., 10.0);
			ImGui::SliderFloat("b", &b, -20.,  5.0);
			ImGui::SliderFloat("c", &c,  -1.,  1.0);
		}
		else if (attractor == 4) { //Thomas
			if (change != attractor) {
				b = 0.19f;
				dt = preDT; scale = preScale;
			}
			change = attractor;
			preDT = dt; preScale = scale;
			
			ImGui::Text(  "dx/dt = -bx + sin(y)"
						"\ndy/dt = -by + sin(z)"
						"\ndz/dt = -bz + sin(x)");
			ImGui::SliderFloat("b", &b, 0., 3.0);
		}
		else if (attractor == 5) { //Dequan Li
			if (change != attractor) {
				a = 40.f, b = 1.833f, c = 0.16f, d = 0.65f, e = 55.f, f = 20.f;
				dt = 0.00025f;
				scale = 5.f;
			}
			change = attractor;
			
			ImGui::Text(  "dx/dt = ay - x + cxz"
						"\ndy/dt = ex + fy - xz"
						"\ndz/dt = bz + xy - dx ");
			ImGui::SliderFloat("a", &a, -10., 50.0);
			ImGui::SliderFloat("b", &b,  -1.,  3.0);
			ImGui::SliderFloat("c", &c,  -1.,  1.0);
			ImGui::SliderFloat("d", &d,  -1.,  1.0);
			ImGui::SliderFloat("e", &e, -10., 80.0);
			ImGui::SliderFloat("f", &f, -10., 30.0);
		}
		ImGui::Text("");
		ImGui::Separator();
		ImGui::Text("");
		if (ImGui::Button("Reset values"))
			change = -1;
		if (ImGui::Button("Reset Positions"))
			for (int i = 0; i < instances; ++i)
				pos[i] = glm::dvec3(uniform(-10., 10.), uniform(-10., 10.), uniform(-10., 10.));

		//ImGui::Checkbox("depth", &depth);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion

		// Buffer swap and I/O processing
#ifdef __APPLE__
		finish = clock();
        iter_time = float(finish - start) / CLOCKS_PER_SEC;
        //std::cout << limit << '\n' << cap << '\n' << iter_time << "\n\n";
        if (iter_time < cap) {
            diff = cap - iter_time;
            std::this_thread::sleep_for(std::chrono::milliseconds( int(diff * 1000) ));
        }
#else
		glfwSwapInterval(1); //With this, the GPU doesn't use 90% of it's processes. 1 is 60 fps, 2 is 30...
#endif
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Terminating everyting
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	obj.~Object();

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
	}

void processInput(GLFWwindow* window)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		manipulate = true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		manipulate = false;
	}


	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && width != mode->width && height != mode->height) {
		glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	else if (glfwGetKey(window, GLFW_KEY_F11)) {
		glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetWindowMonitor(window, NULL, WIDTH / 8, HEIGHT / 4, WIDTH / 2, HEIGHT / 2, GLFW_DONT_CARE);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}


	float cameraSpeed = 15.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cameraPos += cameraUp * cameraSpeed; //up
		//xRotation += 10 * cameraSpeed;
		//cameraPos += cameraSpeed * cameraFront; //forward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cameraPos -= cameraUp * cameraSpeed; //down
		//xRotation -= 10 * cameraSpeed;
		//cameraPos -= cameraSpeed * cameraFront; //back
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; //left
		//yRotation += 10*cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; //right
		//yRotation -= 10*cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (!glfwJoystickPresent(GLFW_JOYSTICK_1)) {
		float xpos = float(xposIn);
		float ypos = float(yposIn);

		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.1f; // change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		if (manipulate == true) {
			yRotation += xoffset;
			xRotation -= yoffset;
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//fov -= (float)yoffset;
	//if (fov < 1.0f)
	//	fov = 1.0f;
	//if (fov > 45.0f)
	//	fov = 45.0f;

	cameraPos += (float)yoffset*.3f * cameraFront; //forward and backward
}

