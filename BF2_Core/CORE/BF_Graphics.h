#pragma once

#include <memory>
#include <vector>
#include "../Graphics & Window/BF_Window.h"
#include "../Graphics & Window/VK_QueueFamilyIndices.h"
#include "../Graphics & Window/VK_Swapchain.h"

#include <vulkan/vulkan.h>

//---

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

	//debug
	VkResult createDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*);
	void destroyDebugUtilsMessengerEXT(const VkAllocationCallbacks*);

	//init
	int createVkInstance();
	int createVkDebugMsgr();
	int createVkSurface();
	int pickVkPhysicalDevice();
	int createVkLogicalDevice();
	int createSwapchain();

	//cleanup
	int cleanupSwapchain();

	bool checkValidationLayerSupport();
	bool isDeviceSuitable(const VkPhysicalDevice&);
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice&);
	bool checkDeviceExtensionSupport(const VkPhysicalDevice&);
	
	SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice&);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>&);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&);

	std::vector<const char*> getRequiredExtensions() const;

	//Helpers
	VkImageView createVkImageView(VkDevice, VkImage, VkFormat, VkImageAspectFlags);

	VkDebugUtilsMessengerEXT	m_debugMsgr;

	VkInstance					m_instance;
	VkSurfaceKHR				m_surface;
	VkPhysicalDevice			m_physDevice;
	VkDevice					m_device;

	VkQueue						m_graphicsQueue;
	VkQueue						m_presentQueue;

	Swapchain					m_swapchain;

	bool m_enableValidationLayers;
	std::vector<const char*> m_validationLayers;
	std::vector<const char*> m_deviceExtensions;

	std::unique_ptr<Window> m_pWindow;

	//exit flag
	bool m_exit;
};