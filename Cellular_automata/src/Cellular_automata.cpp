#include "cellular_automata.hpp"

GLFWmonitor* monitor;
const GLFWvidmode* mode;
int WIDTH;
int HEIGHT;

//timing
double deltaTime = 0.0;	// time between current frame and last frame
double lastFrame = 0.0;

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
	fprintf(stderr, "\n\nGL CALLBACK:\n  Source: %s\n    Type: %s\n      id: %d\nSeverity: %s\n Message: %s\n\n",
			debugSource(source), debugType(type), id, debugSeverity(severity), message);
	glfwTerminate();
	getchar();
	exit(EXIT_FAILURE);
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
	WIDTH = width; HEIGHT = height;
}


int main()
{
	if (!glfwInit())
		return -1;

	glfwSetErrorCallback(error_callback);

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	WIDTH = mode->width;
	HEIGHT = mode->height;

	//Hints: Tells OpenGL what version to use, if none is specified, it uses the latest
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
	GLFWwindow* window = glfwCreateWindow(WIDTH / 2, HEIGHT / 2, "Conway's Game of Life", NULL, NULL); WIDTH /= 2; HEIGHT /= 2;//Windowed
#else
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Conway's Game of Life", glfwGetPrimaryMonitor(), NULL); //Fullscreen
#endif
	if (window == NULL) { // glfwCreateWindow returns NULL if an error occurs
		fprintf(stderr, "Failed to initialize GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	/*
	GLFWimage icon[1];
	icon[0].pixels = stbi_load("../../ressources/textures/Rosace.png", &icon[0].width, &icon[0].height, 0, 4);
	glfwSetWindowIcon(window, 1, icon);
	stbi_image_free(icon[0].pixels);*/

	//GLFWmonitor* monitor = glfwGetWindowMonitor(window);

	if (glewInit() != GLEW_OK) { //Initializes GLEW, if it's not done properly, terminate program
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(glewInit()));
		glfwTerminate();
		return -2;
	}

 #ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT); //see the beginning of the file
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, nullptr);

	//INFO
	printf("Last compiled on %s at %s\n\n", __DATE__, __TIME__);
	printf("GPU Information:\n");
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("Shading Language Version: %s\n\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
 #endif

	//These functions are called by glfwPollEvents
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //free movement


	std::vector<GLfloat> quadVertices = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,    0.0f, 1.0f,
		-1.0f, -1.0f,    0.0f, 0.0f,
		 1.0f, -1.0f,    1.0f, 0.0f,

		-1.0f,  1.0f,    0.0f, 1.0f,
		 1.0f, -1.0f,    1.0f, 0.0f,
		 1.0f,  1.0f,    1.0f, 1.0f
	};

	GLint quadAttribSizes[] = { 2, 2 };
	Object cell(quadVertices, quadAttribSizes, sizeof quadAttribSizes / sizeof quadVertices[0]);


	const char* vert = 
#include "GOL.vert"
	;
	const char* frag =
#include "GOL.frag"
	;

	cell.loadShader(cell.vertexShader, vert, false); cell.loadShader(cell.fragmentShader, frag, false);
	cell.loadProgram();
	SET1F(cell.shaderProgram, "width", (GLfloat)WIDTH);
	SET1F(cell.shaderProgram, "height",(GLfloat)HEIGHT);

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //if render target is the size of the texture, can use GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_tex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error - Status: 0x%x\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));


	// glm::mat4 model = glm::mat4(1.0f);
	// SETMAT(cell.shaderProgram, "model", model);
	// glm::mat4 projection = glm::ortho(0.f, (float)WIDTH, 0.f, (float)HEIGHT);
	// SETMAT(cell.shaderProgram, "projection", projection);
	//float time;

	SET1I(cell.shaderProgram, "first", 1);
	cell.Draw(GL_TRIANGLES);
	SET1I(cell.shaderProgram, "first", 0);

	double time = 0.0;
	while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) //Set Esc as a close command
	{
		time = glfwGetTime();
		deltaTime = time - lastFrame;
		lastFrame = time;
		//float iter = round(time * 60);
		double run = glfwGetTime();

		int hours = int(run / 3600);
		int minutes = int(fmod(run, 3600) / 60); //fmod is like the % operator that can work with floats and doubles
		double seconds = fmod(fmod(run, 3600), 60);
		printf("%f fps | Time run: %d hours, %d minutes, and %f seconds    \r", 1 / deltaTime, hours, minutes, seconds);

		// if (glfwGetTime() < 10) {
		// glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		// }
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
		

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// cell.Use();
		cell.Draw(GL_TRIANGLES);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// Buffer swap and I/O processing
		glfwSwapInterval(2); //With this, the GPU doesn't use 90% of it's processes. 1 is 60 fps, 2 is 30...
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cell.~Object();

	glfwDestroyWindow(window);
	glfwTerminate();
	printf("\n");
	exit(EXIT_SUCCESS);
}