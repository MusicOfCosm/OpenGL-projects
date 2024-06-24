#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>

int main()
{
	glfwInit();

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(1, 1, "I love you", NULL, NULL);
	glfwMakeContextCurrent(window);

	std::ofstream file;
	file.open("GPU_Info.txt");
	file << "Information about your GPU (Graphical Processing Unit):\n\n";
	file << "Vendor: " << glGetString(GL_VENDOR) << '\n';
	file << "Renderer: " << glGetString(GL_RENDERER) << '\n';
	file << "Version: " << glGetString(GL_VERSION) << '\n';
	file << "Shading Language Version:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n\n";
	file.close();

	return 0;
}
