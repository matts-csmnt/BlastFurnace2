#include "CORE\BF_Graphics.h"
#include "Utils/BF_Consts.h"
#include "Utils/BF_Error.h"

#pragma comment(lib, "vulkan-1")

#define GLFW_INCLUDE_VULKAN	//includes vulkan\vulkan.h
#include <GLFW/glfw3.h>

Graphics::Graphics() : m_exit(false),
#ifdef _DEBUG
	m_enableValidationLayers(true)
#else
	m_enableValidationLayers(false)
#endif
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

	//init vulkan api stuff
	vulkanSetup();
}

void Graphics::shutdown()
{
	vulkanShutdown();

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

//--- HERE THERE BE DRAGONS... ---
//- VULKAN SETUP & API CALLS -

#define CHECK_RET(x) if(!x){return 0;}

int Graphics::vulkanSetup()
{
	int check(0);
	
	m_validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	m_deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	CHECK_RET(createInstance());

	return 1;
}

int Graphics::vulkanShutdown()
{
	vkDestroyInstance(m_instance, nullptr);

	return 1;
}

int Graphics::createInstance()
{
	if (m_enableValidationLayers && !checkValidationLayerSupport())
	{
		errorF("Vulkan VAlidation layer support requested, but not available.");
	}

	//create application info
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = kWindowTitle;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = kWindowTitle;
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	//create instance info
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//Pass required extensions to vulkan
	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	//Validation layers
	if (m_enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	//Create instance and check for errors
	VkResult res;
	res = vkCreateInstance(&createInfo, nullptr, &m_instance);

	if (res != VK_SUCCESS) {
		panicF("failed to create vulkan instance!");
		return 0;
	}

#if _DEBUG
	printf("vulkan instance created\n");
	//List vulkan extensions
	printf("available vk extensions:\n");
	for (const auto& extension : m_deviceExtensions) {
		printf("\t %s\n", extension);
	}
#endif

	return 1;
}


//Get Validation layer and extensions...
bool Graphics::checkValidationLayerSupport()
{
	//Look for layers
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	//Add layer data
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	//check requested layers exist and are available
	for (const char* layerName : m_validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char*> Graphics::getRequiredExtensions() const
{
	uint32_t glfwExtensionCount = 0;

	//Pass glfw window extensions to vulkan
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	if(!glfwExtensions)
	{
		//If Vulkan is not available on the machine, this function returns NULL and generates a 
		//GLFW_API_UNAVAILABLE error. Call glfwVulkanSupported to check whether Vulkan is at least minimally available.
		bool glfwRes = glfwVulkanSupported();
		const char* res = glfwRes ? "Vulkan supported..." : "Vulkan unsupported";
		glfwRes ?
			errorF("GLFW required extensions failed...\n%s", res) :
			panicF("GLFW required extensions failed...\n%s", res);
	}

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//Vulkan debug exts
	if (m_enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}