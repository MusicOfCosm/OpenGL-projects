#include "slime.hpp"

//glfw error callback
void error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

GLFWmonitor* monitor;
const GLFWvidmode* mode;
int WIDTH, HEIGHT;
GLuint texWidth, texHeight;

//Enables to resizes what is drawn along the window, if not used, the shape drawn would always stay the same size
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	WIDTH = width; HEIGHT = height;
}

//timing
double deltaTime = 0.0;	// time between current frame and last frame
double lastFrame = 0.0;

float SPEED = uniform(1.f, 2.5f);
int SENSOR_RANGE = randint(5, 30); // Sensor Offset
int SENSOR_SIZE = 0;// randint(0, 2);
float sa = uniform(30.0f, 60.0f);
float ra = uniform(5.0f, 60.0f);
float SA = (float)M_PI / 180.f * sa; //uniform(pi / 3.5f, pi / 2.0f); //Sensor Angle
float RA = (float)M_PI / 180.f * ra; //uniform(0, pi / 4.5f); //Rotation Angle
bool BOUNDARY = randint(0, 1); //0 and 1 are false and true
int START = randint(0, 3); //0: random, 1: centre, 2: circle, 3: corner
bool BLUR = randint(0, 1);
//int SPECIES = randint(1, 3);
//bool IGNORE = randint(0, 1);

void init(glm::vec3 &agent);

int main()
{
	if (!glfwInit())
		return -1;

	glfwSetErrorCallback(error_callback);

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	WIDTH = mode->width;
	HEIGHT = mode->height;

#ifdef __APPLE__
	//Hints: Tells OpenGL what version to use, if none is specified, it uses the latest
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); //Mac doesn't support beyond 4.1
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
	//icon[0].pixels = stbi_load("../../ressources/textures/Rosace.png", &icon[0].width, &icon[0].height, 0, 4);
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

	//INFO
	printf("Last compiled on %s at %s\n", __DATE__, __TIME__);
	printf("GPU Information:\n");
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("Shading Language Version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	//Texture max dimension size: 16384

	int work_grp_cnt[3];
	int work_grp_size[3];
	int work_grp_inv;

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);

	printf("max global (total) work group counts x:%d y:%d z:%d\n",
		   work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);
	printf("max local (in one shader) work group sizes x:%d y:%d z:%d\n",
		   work_grp_size[0], work_grp_size[1], work_grp_size[2]);
	printf("max local work group invocations %d\n\n\n", work_grp_inv);


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
	Object screen(quadVertices, quadAttribSizes, sizeof quadAttribSizes / sizeof quadVertices[0], GL_STATIC_COPY);

	screen.loadShader(screen.vertexShader, "../slime.vert"); screen.loadShader(screen.fragmentShader, "../slime.frag");
	screen.loadProgram();

	GLuint computeProgram = glCreateProgram();
	GLuint computeShader = loadShader(GL_COMPUTE_SHADER, "../slime.comp", computeProgram, true);
	loadProgram(computeProgram);
	glDetachShader(computeProgram, computeShader);	glDeleteShader(computeShader);
	

	texWidth = 2560, texHeight = 1440;
	SET1I(computeProgram, "WIDTH", texWidth);
	SET1I(computeProgram, "HEIGHT", texHeight);

	GLuint compTex;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &compTex);
	glBindTexture(GL_TEXTURE_2D, compTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texWidth, texHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //if render target is the size of the texture, can use GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindImageTexture(0, compTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//SET1I(computeProgram, "imgOut", 0);

	//16:9 aspect ratio
	int agent_w = WIDTH, agent_h = HEIGHT;
	SET2I(computeProgram, "NUMBER", glmVEC2(glm::ivec2(agent_w, agent_h)));
	{
		int instances = agent_w * agent_h;
		std::vector<glm::vec3> agents(instances);
		for (int i = 0; i < instances; ++i)
			init(agents[i]);

	GLuint Agent;
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &Agent);
	glBindTexture(GL_TEXTURE_2D, Agent);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, agent_w, agent_h, 0, GL_RGB, GL_FLOAT, agents.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //if render target is the size of the texture, can use GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindImageTexture(1, Agent, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//SET1I(computeProgram, "agents", 1);
	}


	//glm::mat4 model = glm::mat4(1.0f);
	//glm::mat4 projection = glm::ortho(0.f, (float)WIDTH, 0.f, (float)HEIGHT);
	//SETMAT(screen.shaderProgram, "model", model);
	//SETMAT(screen.shaderProgram, "projection", projection);
	//glm::mat4 matrix = projection * model;
	//SETMAT(screen.shaderProgram, "matrix", matrix);

	//int frame = 0;
	double time;
	//glfwSwapInterval(1); //With this, the GPU doesn't use 90% of it's processes. 1 is 60 fps, 2 is 30...
	while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) //Set Esc as a close command
	{
		time = glfwGetTime();
		deltaTime = time - lastFrame;
		lastFrame = time;
		////float iter = round(time * 60);
		//double run = glfwGetTime();
		//int hours = int(run / 3600);
		//int minutes = int(fmod(run, 3600) / 60); //fmod is like the % operator that can work with floats and doubles
		//double seconds = fmod(fmod(run, 3600), 60);
		//
		//if (frame == 60)
		//{ printf("%f fps | Time run: %d hours, %d minutes, and %f seconds          \r", 1 / deltaTime, hours, minutes, seconds); frame = 0; }

		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		//glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(computeProgram);
		glDispatchCompute(texWidth/32, texHeight/32, 1);

		// make sure writing to image has finished before read
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//glMemoryBarrier(GL_ALL_BARRIER_BITS);
		SET1F(computeProgram, "time", (GLfloat)time);
		SET1F(computeProgram, "SPEED", SPEED);
		SET1I(computeProgram, "SENSOR_RANGE", SENSOR_RANGE);
		SET1I(computeProgram, "SENSOR_SIZE", SENSOR_SIZE);
		SET1F(computeProgram, "SA", SA);
		SET1F(computeProgram, "RA", RA);
		SET1I(computeProgram, "BOUNDARY", BOUNDARY);
		SET1I(computeProgram, "BLUR", BLUR);

		screen.Use();
		screen.Draw(GL_TRIANGLES);


		ImGui::Begin("Settings");
		ImGui::Text("%f fps", 1 / deltaTime);
		ImGui::Separator();
		ImGui::SliderFloat("Speed", &SPEED, 1.0f, 2.5f);
		ImGui::SliderInt("Sensor range", &SENSOR_RANGE, 1, 30);
		ImGui::SliderInt("Sensor size", &SENSOR_SIZE, 0, 2);
		ImGui::Separator();
		ImGui::SliderFloat("Sensor angle", &sa, 0.0f, 180.0f);
		SA = (float)M_PI / 180.f * sa;
		ImGui::SliderFloat("Rotation angle", &ra, 0.0f, 180.0f);
		RA = (float)M_PI / 180.f * ra;
		ImGui::Separator();
		ImGui::Checkbox("Bounds", &BOUNDARY);
		ImGui::Checkbox("Blur", &BLUR);

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Buffer swap and I/O processing
		glfwSwapBuffers(window);
		glfwPollEvents();
		//++frame;
	}

	//Terminating everyting
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	screen.~Object();

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

void init(glm::vec3 &agent)
{
	if (START == 0) { //random, the matrix goes from 0 to the designated values-1
		agent.x = uniform(0, float(texWidth));
		agent.y = uniform(0, float(texHeight));
		agent.z = uniform(-(float)M_PI, (float)M_PI);
	}
	else if (START == 1) { //centre
		agent.x = texWidth / 2.0f;
		agent.y = texHeight / 2.0f;
		agent.z = uniform(-(float)M_PI, (float)M_PI);
	}
	else if (START == 2) { //circle
		float tmp_pos = uniform(-(float)M_PI, (float)M_PI);
		agent.x = texWidth / 2.0f + cos(tmp_pos) * uniform(0, float(texHeight) / 2.f);
		agent.y = texHeight / 2.0f + sin(tmp_pos) * uniform(0, float(texHeight) / 2.f);
		agent.z = (float)acos((float(texWidth / 2.f) - agent.x) / sqrt(pow((float(texWidth) / 2.f - agent.x), 2) + pow((float(texHeight) / 2.f - agent.y), 2)));
		if (agent.y > float(texHeight) / 2.f) { agent.z = -agent.z; }
	}
	else { //corner
		agent.x = (BOUNDARY == true) ? float(randint(0, 1) * texWidth) : 0.F;
		agent.y = (BOUNDARY == true) ? float(randint(0, 1) * texHeight) : 0.F;
		agent.z = uniform(-(float)M_PI, (float)M_PI);
	}
}