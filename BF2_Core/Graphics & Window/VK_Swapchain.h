#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct Swapchain {
	VkSurfaceFormatKHR			m_surfaceFormat;
	VkSwapchainKHR				m_swapChain;
	VkExtent2D					m_extent;
	std::vector<VkImage>		m_images;
	std::vector<VkImageView>	m_imageViews;
	std::vector<VkFramebuffer>	m_frameBuffers;

	SwapChainSupportDetails		m_supportDetails;
};