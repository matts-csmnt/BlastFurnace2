#include "BF_Graphics.h"
#include "../Utils/BF_Consts.h"
#include "../Utils/BF_Error.h"

#pragma comment(lib, "vulkan-1")

#define GLFW_INCLUDE_VULKAN	//includes vulkan\vulkan.h
#include <GLFW/glfw3.h>

#include <set>

Graphics::Graphics() : m_exit(false), m_instance(nullptr),
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

//vkCreateDebugUtilsMessengerEXT function to create the VkDebugUtilsMessengerEXT object. 
//extension function, not automatically loaded. 
//We have to look up its address ourselves using vkGetInstanceProcAddr
VkResult Graphics::createDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(m_instance, pCreateInfo, pAllocator, &m_debugMsgr);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
};

void Graphics::destroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(m_instance, m_debugMsgr, pAllocator);
	}
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, VkDebugUtilsMessageTypeFlagsEXT msgType, const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData, void * pUserData)
{
	errorF("VALIDATION LAYER: %s\n", pCallbackData->pMessage);

	return VK_FALSE;
}

#define CHECK_RET(x) if(!x){return 0;}

int Graphics::vulkanSetup()
{
	int check(0);
	
	m_validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	m_deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	//Vulkan debug exts
	//if (m_enableValidationLayers) {
	//	m_deviceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	//}

	//start creating the objects
	CHECK_RET(createVkInstance());
	CHECK_RET(createVkDebugMsgr());
	CHECK_RET(createVkSurface());
	CHECK_RET(pickVkPhysicalDevice());

	return 1;
}

int Graphics::vulkanShutdown()
{
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

	destroyDebugUtilsMessengerEXT(nullptr);

	vkDestroyInstance(m_instance, nullptr);

	return 1;
}

int Graphics::createVkInstance()
{
	if (m_enableValidationLayers && !checkValidationLayerSupport())
	{
		errorF("Vulkan Validation layer support requested, but not available.");
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

	//Validation layers
	if (m_enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_validationLayers.data();
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	//Create instance and check for errors
	VkResult res;
	res = vkCreateInstance(&createInfo, nullptr, &m_instance);

	if (res != VK_SUCCESS) {
		panicF("failed to create vulkan instance! - VkResult %i", res);
		return 0;
	}

#if _DEBUG
	printf("vulkan instance created\n");
	//List vulkan extensions
	printf("available vk extensions:\n");
	for (const auto& extension : extensions) {
		printf("\t %s\n", extension);
	}
#endif

	return 1;
}

int Graphics::createVkDebugMsgr()
{
	//Early out if Valid Layers turned off
	if (!m_enableValidationLayers) return 1;

	//Create debug messenger info
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr; // Optional

	VkResult res = createDebugUtilsMessengerEXT(&createInfo, nullptr);
	if (res != VK_SUCCESS) {
		errorF("failed to set up debug messenger! - VkResult %i", res);
		//not vital error just yet
	}

	return 1;
}

int Graphics::createVkSurface()
{
	//glfw handles multiplat surface creation
	VkResult res = glfwCreateWindowSurface(m_instance, m_pWindow->getGLFWwindow(), nullptr, &m_surface);
	if (res != VK_SUCCESS) {
		panicF("failed to create window surface! - VkResult %i");
		return 0;
	}

	return 1;
}

int Graphics::pickVkPhysicalDevice()
{
	//Look for GFX devices
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		panicF("failed to find GPUs with Vulkan support!");
		return 0;
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

	//Check our devices are suitable
	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			m_physDevice = device;
			break;
		}
	}

	if (m_physDevice == VK_NULL_HANDLE) {
		panicF("failed to find a suitable GPU!");
		return 0;
	}

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

bool Graphics::isDeviceSuitable(const VkPhysicalDevice& device)
{
	//Get device properties & features
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	//TODO: Check features here...

	//Check for devices that can handle commands we want to use
	QueueFamilyIndices indices = findQueueFamilies(device);

	//Check supported extensions
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	//Check for swap chain support
	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate && deviceFeatures.samplerAnisotropy;
}

QueueFamilyIndices Graphics::findQueueFamilies(const VkPhysicalDevice& device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {

		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		//Window surface support
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

bool Graphics::checkDeviceExtensionSupport(const VkPhysicalDevice& device)
{
	//Enumerate extensions
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	//Check all required extensions in available extensions
	std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());
	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

SwapChainSupportDetails Graphics::querySwapChainSupport(const VkPhysicalDevice& device)
{
	SwapChainSupportDetails details;

	//Basic surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

	//Formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
	}

	//Presentation modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
	}

	return details;
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
		
		errorF("GLFW required extensions failed...\n%s", res);
	}

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	return extensions;
}