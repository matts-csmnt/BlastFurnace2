#include "BF_Window.h"
#include "../Utils/BF_Error.h"

#define GLFW_INCLUDE_VULKAN	//includes vulkan\vulkan.h
#include <GLFW/glfw3.h>

//statics
bool Window::s_resized = false;
uint32_t Window::s_width = 1024;
uint32_t Window::s_height = 768;

Window::Window()
{
}

void Window::init(uint32_t w, uint32_t h, const char * title)
{
	m_titleStr = title;

	//Init glfw library
	if (!glfwInit())
	{
		panicF("Failed to initialize GLFW!");
		return;
	}

	//Window to be created WITHOUT openGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Create and store our window
	m_pWnd = glfwCreateWindow(w, h, title, nullptr, nullptr);

	if (!m_pWnd)
	{
		panicF("Failed to create GLFW window!");
		return;
	}

	//Set the framebuffer resize callback
	glfwSetWindowUserPointer(m_pWnd, this);
	glfwSetFramebufferSizeCallback(m_pWnd, framebufferResizeCallback);
}

void Window::shutdown()
{
	glfwDestroyWindow(m_pWnd);
	glfwTerminate();
}

const bool Window::shouldClose() const
{
	return glfwWindowShouldClose(m_pWnd);
}

const double Window::queryTime() const
{
	return glfwGetTime();
}

GLFWwindow* Window::getGLFWwindow() const
{
	return m_pWnd;
}

void framebufferResizeCallback(GLFWwindow * window, int width, int height)
{
	Window::s_resized = true;
}
