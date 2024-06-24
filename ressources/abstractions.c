#include "abstractions.h"

static GLuint vertexShader; //static means that this variable can only be seen in this file (even though it's global)
static GLuint fragmentShader;

static const char *vert;
static const char *frag;

char* shaderContent(const char* filePath, GLenum shaderType) {
	FILE *fp;
#ifdef _MSC_VER //MSVC or Intel compiler 
	errno_t err = fopen_s(&fp, filePath, "rb");
	if (err != 0) {
#else
	fp = fopen(filePath, "rb");
	if (fp == NULL) {
#endif
		// handle error
		fprintf(stderr, "ERROR: %s SHADER - FILE FAILED TO LOAD\n",
				(shaderType == GL_VERTEX_SHADER ? "VERTEX" :
				shaderType == GL_GEOMETRY_SHADER ? "GEOMETRY" :
				shaderType == GL_FRAGMENT_SHADER ? "FRAGMENT" : "UNKNOWN"));
		printf("Path: %s\n", filePath);
		exit(EXIT_FAILURE);
	}
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	rewind(fp);
	char *shaderContent = malloc(size + 1);
	if (shaderContent == NULL) {
		// handle error
		fprintf(stderr, "ERROR: %s SHADER - FILE EMPTY",
				(shaderType == GL_VERTEX_SHADER ? "VERTEX" :
				shaderType == GL_GEOMETRY_SHADER ? "GEOMETRY" :
				shaderType == GL_FRAGMENT_SHADER ? "FRAGMENT" : "UNKNOWN"));
		printf("Path: %s\n", filePath);
		exit(EXIT_FAILURE);
	}
	fread(shaderContent, size, 1, fp);
	shaderContent[size] = '\0';
	fclose(fp);
	return shaderContent;
}

GLuint loadShader(GLenum shaderType, const char *shaderSourceCode, GLuint shaderProgram, bool loadFromFile)
{
	if (!glIsProgram(shaderProgram)) fprintf(stderr, "\nERROR: SHADER -> PROGRAM - INVALID\nValue must be set with glCreateProgram()\n\n");
	//Reading and loading a shader (glsl file) as a const char* string
	if (loadFromFile == true) {
		shaderSourceCode = shaderContent(shaderSourceCode, shaderType);
	}
	// printf("\n%s\n\n", shaderSourceCode);
	if (shaderProgram)
	// fileContent[strlen(fileContent)-1] = '\0';
	if (shaderType == GL_VERTEX_SHADER) vert = shaderSourceCode;
	if (shaderType == GL_FRAGMENT_SHADER) frag = shaderSourceCode;

	//Creating the shader object
	GLuint shaderObject = glCreateShader(shaderType);
	shaderType == GL_VERTEX_SHADER ? vertexShader = shaderObject : shaderType == GL_FRAGMENT_SHADER ? fragmentShader = shaderObject : 0;

	//Compiling the shader code
	glShaderSource(shaderObject, 1, &shaderSourceCode, NULL); // (int) the source code string is null terminated
	glCompileShader(shaderObject);

	//Checking for compiling errors for the shader code
	GLint success;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLchar infoLog[1024];
		glGetShaderInfoLog(shaderObject, 1024, NULL, infoLog);
		fprintf(stderr, "\nERROR: %s SHADER - COMPILATION FAILED\n%s\n\n",
				(shaderType == GL_VERTEX_SHADER ? "VERTEX" :
				 shaderType == GL_GEOMETRY_SHADER ? "GEOMETRY" :
				 shaderType == GL_FRAGMENT_SHADER ? "FRAGMENT" :
				 shaderType == GL_COMPUTE_SHADER ? "COMPUTE" : "UNKNOWN"),
				 infoLog);
		if (shaderType == GL_VERTEX_SHADER) printf("%s", vert);
		if (shaderType == GL_FRAGMENT_SHADER) printf("%s", frag);
		glDeleteShader(shaderObject);
		exit(EXIT_FAILURE);
	}
	else glAttachShader(shaderProgram, shaderObject);
	return shaderObject;
}
//const char* ext = strrchr(texFilePath, '.');

void loadProgram(GLuint shaderProgram)
{
	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (success) {
		fprintf(stderr, "\nWARNING: SHADER -> PROGRAM - ALREADY LINKED");
		glUseProgram(shaderProgram);
	}
	else {
		//Linking all the attached shaders to the program
		glLinkProgram(shaderProgram);

		//Checking for errors in the linking process
		GLchar infoLog[1024];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 1024, NULL, infoLog);
			fprintf(stderr, "\nERROR: SHADER -> PROGRAM - LINKING_FAILED\n%s", infoLog);
		}
		else //Binding the shader program to tell OpenGL to use it to draw
			glUseProgram(shaderProgram);
		if (vertexShader && fragmentShader) {
			glDetachShader(shaderProgram, vertexShader);
			glDetachShader(shaderProgram, fragmentShader);
			glDeleteShader(vertexShader); glDeleteShader(fragmentShader);
		}
	}
}

GLuint loadTexture2D(GLenum textureUnit, const char* texFilePath) {
	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(textureUnit); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_REPEAT, GL_CLAMP_TO_EDGE/BORDER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_LINEAR(_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST

	stbi_set_flip_vertically_on_load(true);
	int imgWidth, imgHeight, nColorChannels;
	unsigned char* texData = stbi_load(texFilePath, &imgWidth, &imgHeight, &nColorChannels, 0); //4 because we want RGBA
	if (texData) {					   //format used by OpenGL			 //actual format of the image
		GLenum format = 0;
		if (nColorChannels == 1)
			format = GL_RED;
		else if (nColorChannels == 3)
			format = GL_RGB;
		else if (nColorChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else fprintf(stderr, "Failed to load texture: %s\n", texFilePath);
	stbi_image_free(texData);

	return texture;
}



void bindFramerate(clock_t start, int cap)
{
	if (cap <= 0) cap = 60;
	float Cap = 1.f/cap;
	clock_t finish = clock();
	float iter_time = (float)(finish - start) / CLOCKS_PER_SEC;
	printf("%f fps     \r", (float)(finish - start)*1000);
	if (iter_time < Cap) {
		float diff = Cap - iter_time;
#ifdef _WIN32
		//the *1000 converts seconds to milliseconds, I learned way too late that the nano version doesn't work on Windows
		Sleep((int)(diff * 1000));
#else
		//The *1000000 converts seconds to microseconds
		usleep((int)(diff * 1000000));
#endif
	}
}

// ivec2 ndcToScr(float x, float y, int scrWidth, int scrHeight)
// {
// 	ivec2 newCoord = {
// 		(int)((x / 2 + 0.5) * scrWidth ),
// 		(int)((y / 2 + 0.5) * scrHeight)
// 	};
// 	return newCoord;
// }
// vec2 scrToNdc(int x, int y, int scrWidth, int scrHeight)
// {
// 	vec2 newCoord = {
// 		x/(scrWidth  / 2.f) - 1.f,
// 		y/(scrHeight / 2.f) - 1.f
// 	};
// 	return newCoord;
// }