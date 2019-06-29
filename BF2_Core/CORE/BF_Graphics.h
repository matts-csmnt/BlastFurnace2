#pragma once

#include <memory>
#include <vector>
#include "../Graphics & Window/BF_Window.h"

#include <vulkan/vulkan.h>

class Graphics {
public:
	Graphics();
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void init();
	void shutdown();

	void frame();

	const double queryTimer() const;

	const bool getExitFlag() const;

private:

	//VK API
	int vulkanSetup();
	int vulkanShutdown();

	int createInstance();

	bool checkValidationLayerSupport();

	std::vector<const char*> getRequiredExtensions() const;

	VkInstance m_instance;

	bool m_enableValidationLayers;
	std::vector<const char*> m_validationLayers;
	std::vector<const char*> m_deviceExtensions;

	std::unique_ptr<Window> m_pWindow;

	//exit flag
	bool m_exit;
};