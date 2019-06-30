#pragma once

#include <memory>

struct GLFWwindow;

class Window {
public:
	Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void init(uint32_t w, uint32_t h, const char* title);
	void shutdown();

	const bool shouldClose() const;

	static bool s_resized;
	static uint32_t s_width, s_height;

	const double queryTime() const;

	GLFWwindow* getGLFWwindow() const;

private:

	GLFWwindow* m_pWnd;

	const char* m_titleStr;
};

void framebufferResizeCallback(GLFWwindow* window, int width, int height);