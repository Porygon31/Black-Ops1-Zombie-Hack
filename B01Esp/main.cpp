#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <iostream>

#include <GLFW/glfw3.h>
#include <MemMan.h>
#include "DataTypes.h"
#include "Helper.h"
#include "Offset.h"
using namespace Zombies;

int main(void)
{
	GLFWwindow* window;
	ShowWindow(GetConsoleWindow(), SW_SHOW);	

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_FLOATING, true);
	//glfwWindowHint(GLFW_RESIZABLE, false);
	//glfwWindowHint(GLFW_MAXIMIZED, true);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
	glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, true);

	window = glfwCreateWindow(1024, 768, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	//Get Process
	uintptr_t Process = MemMan::GetProcess(L"BlackOps.exe");
	if (Process == 0)
	{
		std::cout << "Process is not found\n";
		return -1;
	}

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{

			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);

			//Get ViewMatrix
			ViewMatrix Matrix = MemMan::ReadMem<ViewMatrix>(AViewMatrix);

			for (short int i = 0; i < 40; i++)
			{
				uintptr_t Entity = MemMan::ReadMem<uintptr_t>(AEntityList + i * Offsets::ODistanceBetween);
			
				if (Entity == NULL) continue;
				
				// Check if Health is >= 0
				int Health = MemMan::ReadMem<int>(Entity + Offsets::OHealth);
				if (Health <= 0) continue;

				// Get Entity Location
				Vec3 Location = MemMan::ReadMem<Vec3>(Entity + Offsets::OLocation);

				// W2S Location
				Vec2 ScreenCoords;
				if (!WorldToScreen(Location, ScreenCoords, Matrix.Matrix)) continue;


				// Draw to screen
				glBegin(GL_LINES);
				glVertex2f(0.0f, -1.0f);
				glVertex2f(ScreenCoords.X, ScreenCoords.Y);
				glEnd();
				//std::cout << "[main] Zombie #" << i << " screen coords: X=" << ScreenCoords.X << " Y=" << ScreenCoords.Y << std::endl;
				//std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}

	glfwTerminate();
	return 0;
}