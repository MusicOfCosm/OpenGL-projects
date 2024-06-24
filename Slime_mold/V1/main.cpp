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
#include <array>

//#include "../../ressources/abstractions.h"
#include "../../ressources/renderer.hpp"
#include "../../ressources/glDebug.h"
//#include "../../ressources/materials.h"

unsigned int seed = (unsigned int)time(0);
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
MessageCallback(GLenum source,
				GLenum type,
				GLuint id,
				GLenum severity,
				GLsizei length,
				const GLchar* message,
				const void* userParam)
{
	fprintf(stderr, "\n\nGL CALLBACK:\nSource:   %s\nType:     %s\nid:       %d\nSeverity: %s\nMessage:  %s\n\n",
			debugSource(source), debugType(type), id, debugSeverity(severity), message);
	scanf_s("");
	exit(EXIT_FAILURE);
}
#endif

//glfw error callback
void error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error: %s\n", description);
}


GLFWmonitor* monitor;
const GLFWvidmode* mode;
int WIDTH;
int HEIGHT;

//Enables to resizes what is drawn along the window, if not used, the shape drawn would always stay the same size
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	WIDTH = width; HEIGHT = height;
}

//timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float SPEED = uniform(1.f, 2.5f);
int SENSOR_RANGE = randint(5, 30); // Sensor Offset
int SENSOR_SIZE = randint(0, 2);
float sa = uniform(30.0f, 60.0f);
float SA = (float)M_PI / 180.f * sa; //uniform(pi / 3.5f, pi / 2.0f); //Sensor Angle
float ra = uniform(5.0f, 60.0f);
float RA = (float)M_PI / 180.f * ra; //uniform(0, pi / 4.5f); //Rotation Angle
bool BOUNDARY = randint(0, 1); //0 and 1 are false and true
int START = randint(0, 3); //0: random, 1: centre, 2: circle, 3: corner
bool BLUR = randint(0, 1);
//int SPECIES = randint(1, 3);
//bool IGNORE = randint(0, 1);

std::vector<GLfloat> canvas;

void init(glm::vec3 &agent);

void Boundary(float & x, float & y);

void movement(glm::vec3 &agent);


int main()
{
	if (!glfwInit())
		return -1;

	glfwSetErrorCallback(error_callback);

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	WIDTH = mode->width;
	HEIGHT = mode->height;

	canvas.resize(WIDTH * HEIGHT);

	//Hints: Tells OpenGL what version to use, if none is specified, it uses the latest
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); //Mac doesn't support beyond 4.1
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#ifdef _DEBUG
	/*specifies whether the context should be created in debug mode, which may
	provide additional error and diagnostic reporting functionality*/
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	//GLFWwindow* window = glfwCreateWindow(WIDTH / 2, HEIGHT / 2, "Slime mold simulation", NULL, NULL); WIDTH /= 2; HEIGHT /= 2;//Windowed
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Slime mold simulation", glfwGetPrimaryMonitor(), NULL); //Fullscreen
	if (window == NULL) { // glfwCreateWindow returns NULL if an error occurs
		fprintf(stderr, "Failed to initialize GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//GLFWimage icon[1];
	//icon[0].pixels = stbi_load("../../../ressources/textures/Rosace.png", &icon[0].width, &icon[0].height, 0, 4);
	//glfwSetWindowIcon(window, 1, icon);
	//stbi_image_free(icon[0].pixels);

	GLFWmonitor* monitor = glfwGetWindowMonitor(window);

	if (glewInit() != GLEW_OK) { //Initializes GLEW, if it's not done properly, terminate program
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(glewInit()));
		glfwTerminate();
		return -2;
	}

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT); //see the beginning of the file
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	//INFO
	printf("GPU Information:\n");
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("Shading Language Version: %s\n\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif

	//These functions are called by glfwPollEvents
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //free movement


	IMGUI_CHECKVERSION();
	// Setup Dear ImGui context
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	//Texture max dimension size: 16384
	int instances = 100000;
	std::vector<glm::vec3> agents(instances);
	for (int i = 0; i < instances; ++i)
		init(agents[i]);

	GLint attribSizes[] = { 1 };
	Object obj(agents, attribSizes, (3 * sizeof attribSizes) / sizeof agents[0], GL_DYNAMIC_DRAW, instances);

	obj.loadShader(obj.vertexShader, "../V1.vert"); obj.loadShader(obj.fragmentShader, "../V1.frag");
	obj.loadProgram();

	glm::mat4 model = glm::mat4(1.0f);
	SETMAT(obj.shaderProgram, "model", model);
	glm::mat4 projection = glm::ortho(0.f, (float)WIDTH, 0.f, (float)HEIGHT);
	SETMAT(obj.shaderProgram, "projection", projection);
	//float time;


	std::vector<GLfloat> quadVertices = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	GLint quadAttribSizes[] = { 2, 2 };
	Object screen(quadVertices, quadAttribSizes, sizeof quadAttribSizes / sizeof quadVertices[0], GL_STATIC_DRAW);

	screen.loadShader(screen.vertexShader, "../scr_V1.vert"); screen.loadShader(screen.fragmentShader, "../scr_V1.frag");
	screen.loadProgram();
	SET1I(screen.shaderProgram, "width", WIDTH);
	SET1I(screen.shaderProgram, "height", HEIGHT);

	//screen.Use();
	//SET1I(screen.shaderProgram, "screenTexture", 0);
	//https://learnopengl.com/Advanced-OpenGL/Framebuffers
	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//creating a color attachement texture
	GLuint fbo_tex;
	glGenTextures(1, &fbo_tex);
	glBindTexture(GL_TEXTURE_2D, fbo_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //if render target is the size of the texture, can use GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_tex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error - Status: 0x%x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));

	//std::vector<glm::vec3> test(100 * 100);
	//for (int i = 0; i < 10000; ++i) {
	//	if      (i < 2500) test[i] = glm::vec3(1.f, 0.f, 0.f);
	//	else if (i < 5000) test[i] = glm::vec3(0.f, 1.f, 0.f);
	//	else if (i < 7500) test[i] = glm::vec3(0.f, 0.f, 1.f);
	//	else               test[i] = glm::vec3(0.f);
	//}
	//GLuint agentData;
	//glActiveTexture(GL_TEXTURE1);
	//glGenTextures(1, &agentData);
	//glBindTexture(GL_TEXTURE_2D, agentData);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 100, 100, 0, GL_RGB, GL_FLOAT, test.data());
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //if render target is the size of the texture, can use GL_NEAREST
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//SET1I(screen.shaderProgram, "agentData", 1);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) //Set Esc as a close command
	{
		float time = (float)glfwGetTime();
		deltaTime = time - lastFrame;
		lastFrame = time;
		//float iter = round(time * 60);
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		SET1I(screen.shaderProgram, "width",  WIDTH);
		SET1I(screen.shaderProgram, "height", HEIGHT);
		SET1I(screen.shaderProgram, "blur", (int)BLUR);

		//glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);

		obj.Use();
		//glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * agents.size(), &agents[0], GL_STATIC_DRAW);
		obj.Update(GLint(sizeof(glm::vec3)*agents.size()), &agents[0]);

		glDrawArraysInstanced(GL_POINTS, 0, 1, instances);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		screen.Use();
		screen.Draw(GL_TRIANGLES);

		{
			ImGui::Begin("Settings");
			ImGui::Text("%f fps", 1 / deltaTime);
			////ImGui::Text("Pos[0]:\nx=%f\ny=%f\nz=%f", glmVEC3(pos[0]));
			double run = glfwGetTime();
			int hours = int(run / 3600);
			int minutes = int(fmod(run, 3600) / 60); //fmod is like the % operator that can work with floats and doubles
			double seconds = fmod(fmod(run, 3600), 60);
			ImGui::Text("Time run: %d hours, %d minutes, and %f seconds", hours, minutes, seconds);

			ImGui::Separator();

			ImGui::SliderFloat("Speed", &SPEED, 1.0f, 2.5f);
			ImGui::SliderInt("Sensor range", &SENSOR_RANGE, 1, 30);
			ImGui::SliderInt("Sensor size", &SENSOR_SIZE, 0, 2);
			ImGui::SliderFloat("Sensor angle", &sa, 0.0f, 180.0f);
			SA = (float)M_PI / 180.f * sa;
			ImGui::SliderFloat("Rotation angle", &sa, 0.0f, 180.0f);
			RA = (float)M_PI / 180.f * sa;
			ImGui::Checkbox("Blur", &BLUR);

			ImGui::End();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glReadPixels(0, 0, WIDTH, HEIGHT, GL_RED, GL_FLOAT, canvas.data());


		for (int i = 0; i < instances; ++i)
			movement(agents[i]);

		// Buffer swap and I/O processing
		glfwSwapInterval(1); //With this, the GPU doesn't use 90% of it's processes. 1 is 60 fps, 2 is 30...
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


void init(glm::vec3 &agent)
{
	if (START == 0) { //random, the matrix goes from 0 to the designated values-1
		agent.x = uniform(0, float(WIDTH));
		agent.y = uniform(0, float(HEIGHT));
		agent.z = uniform(-(float)M_PI, (float)M_PI);
	}
	else if (START == 1) { //centre
		agent.x = WIDTH / 2.0f;
		agent.y = HEIGHT / 2.0f;
		agent.z = uniform(-(float)M_PI, (float)M_PI);
	}
	else if (START == 2) { //circle
		float tmp_pos = uniform(-(float)M_PI, (float)M_PI);
		agent.x = WIDTH / 2.0f + cos(tmp_pos) * uniform(0, float(HEIGHT) / 2.f);
		agent.y = HEIGHT / 2.0f + sin(tmp_pos) * uniform(0, float(HEIGHT) / 2.f);
		agent.z = (float)acos((float(WIDTH) / 2.f - agent.x) / sqrt(pow((float(WIDTH) / 2.f - agent.x), 2) + pow((float(HEIGHT) / 2.f - agent.y), 2)));
		if (agent.y > float(HEIGHT) / 2.f) { agent.z = -agent.z; }
	}
	else { //corner
		agent.x = (BOUNDARY == true) ? float(randint(0, 1) * WIDTH) : 0.F;
		agent.y = (BOUNDARY == true) ? float(randint(0, 1) * HEIGHT) : 0.F;
		agent.z = uniform(-(float)M_PI, (float)M_PI);
	}
}


void Boundary(float & x, float & y)
{
	if (BOUNDARY == true) {
		if (x > WIDTH) {
			x = float(WIDTH);
		}
		else if (x < 0) {
			x = 0;
		}

		if (y > HEIGHT) {
			y = float(HEIGHT);
		}
		else if (y < 0) {
			y = 0;
		}
	}

	else {
		if (x > WIDTH) {
			x -= WIDTH;
		}
		else if (x < 0) {
			x += WIDTH;
		}

		if (y > HEIGHT) {
			y -= HEIGHT;
		}
		else if (y < 0) {
			y += HEIGHT;
		}
	}
}
void Boundary(int & x, int & y)
{
		if (BOUNDARY == true) {
		if (x > WIDTH) {
			x = WIDTH;
		}
		else if (x < 0) {
			x = 0;
		}

		if (y > HEIGHT) {
			y = HEIGHT;
		}
		else if (y < 0) {
			y = 0;
		}
	}

	else {
		if (x > WIDTH) {
			x -= WIDTH;
		}
		else if (x < 0) {
			x += WIDTH;
		}

		if (y > HEIGHT) {
			y -= HEIGHT;
		}
		else if (y < 0) {
			y += HEIGHT;
		}
	}
}



float sense(float sensorAngleOffset, glm::vec3 agent) {
	float sensorAngle = agent.z + sensorAngleOffset;
	float sensorDirX = cos(sensorAngle);
	float sensorDirY = sin(sensorAngle);
	int sensorCentreX = (int)round(agent.x + sensorDirX * SENSOR_RANGE);
	int sensorCentreY = (int)round(agent.y + sensorDirY * SENSOR_RANGE);
	float sum = 0.0f;

	for (int offsetX = -SENSOR_SIZE; offsetX <= SENSOR_SIZE; offsetX++) { //Goes from -SENSOR_SIZE to SENSOR_SIZE
		for (int offsetY = -SENSOR_SIZE; offsetY <= SENSOR_SIZE; offsetY++) {
			int posX = sensorCentreX + offsetX;
			int posY = sensorCentreY + offsetY;
			Boundary(posX, posY); //resetting posX and posY to be inside the bounds
			
			int pos = (posY) * (WIDTH) + (posX); //Convert from 2D array to 1D array
			if (pos >= WIDTH * HEIGHT) pos -= (WIDTH+1);
			//if (pos >= WIDTH * HEIGHT || pos <= 0) printf("posX: %d, posY: %d, pos: %d\n%d\n\n", posX, posY, pos, WIDTH*HEIGHT);

			sum += canvas[pos];
			//if (canvas[0].x > 0) printf("red = %f", canvas[0]);
			//if (sum > 0) printf("sum = %f     \r", sum);

			//if (IMAGE == true)
			//{
			//	sum += Gray[index].at<uint8_t>(posY, posX);
			//	//sum += img_art.at<uint8_t>(posY, posX);
			//}
			//else
			//{
			//	for (int i = 0; i < SPECIES; i++)
			//	{
			//		if (Gray_ref[i].at<uint8_t>(posY, posX) == Gray_ref[index].at<uint8_t>(posY, posX)) {
			//			sum += Gray_ref[i].at<uint8_t>(posY, posX) * (255 / gray_val[index]); //Order is important!!
			//		}
			//		else if (IGNORE == false) { sum -= Gray_ref[i].at<uint8_t>(posY, posX) * (255 / gray_val[index]); }
			//	}
			//}
		}
	}

	return sum;
}

void movement(glm::vec3 &agent /*, int index, int number*/)
{
	//if (
	//    Grey.at<uint8_t>(int(round(self.y)), int(round(self.x))) == gray_val[0] ||
	//    Grey.at<uint8_t>(int(round(self.y)), int(round(self.x))) == gray_val[1] ||
	//    Grey.at<uint8_t>(int(round(self.y)), int(round(self.x))) == gray_val[2]
	//    )
	//{
	//    self.orientation = uniform(-M_PI, M_PI);
	//}
	//
	////Placing pixel on canvas
	//else if (SPECIES && number <= NUMBER && number != 0)
	//{
	    //Canvas.at<cv::Vec3b>(int(round(self.y)), int(round(self.x))) = { color[index][0], color[index][1], color[index][2] };
	    //Gray_ref[index].at<uint8_t>(int(round(self.y)), int(round(self.x))) = gray_val[index]; //Went from Gray to Gray_ref
	    
	
	    //}
	
	    //Sensing other pixels
	    float forward = sense(0.0f, agent);
	    float left = sense(SA, agent);
	    float right = sense(-SA, agent);
	
	    if (forward >= left && forward >= right) {
	        ;
	    }
	
	    else if (forward < left && forward < right) {
	        agent.z += uniform(-RA, RA);
	    }
	
	    else if (left > right) {
	        agent.z += RA;
	    }
	
	    else if (left < right) {
	        agent.z += -RA;
	    }
	//}
	
	//moving
	agent.x += SPEED * cos(agent.z);
	agent.y += SPEED * sin(agent.z);
	if (agent.x < 0 || agent.x > WIDTH - 1 || agent.y < 0 || agent.y > HEIGHT - 1) {
		if (BOUNDARY == true) {
			agent.z = uniform(-(float)M_PI, (float)M_PI);
		}
		Boundary(agent.x, agent.y);
	}
}