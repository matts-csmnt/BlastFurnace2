#include "CORE\BF_Graphics.h"
#include "Utils/BF_Consts.h"
#include "Utils/BF_Error.h"

#pragma comment(lib, "vulkan-1")

#define GLFW_INCLUDE_VULKAN	//includes vulkan\vulkan.h
#include <GLFW/glfw3.h>

Graphics::Graphics() : m_exit(false)
{
}

void Graphics::init()
{
	//init the window
	m_pWindow = std::make_unique<Window>();
	if (!m_pWindow)
	{
		panicF("Failed to create the Window module.");
	}
	m_pWindow->init(Window::s_width, Window::s_height, kWindowTitle);
}

void Graphics::shutdown()
{
	m_pWindow->shutdown();
}

void Graphics::frame()
{

	//check for exit conditions
	m_exit |= m_pWindow->shouldClose();
}

const double Graphics::queryTimer() const
{
	return m_pWindow->queryTime();
}

const bool Graphics::getExitFlag() const
{
	return m_exit;
}
