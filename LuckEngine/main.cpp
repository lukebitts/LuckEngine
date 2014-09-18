#include <iostream>
#include <GLFW\glfw3.h>
#include <anax\anax.hpp>

#include <assimp\cimport.h>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

int main()
{
	aiImportFile("file.obj", aiProcessPreset_TargetRealtime_MaxQuality);;
	anax::World w;

	anax::Entity e = w.createEntity();

	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window))
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}