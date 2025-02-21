#pragma once
#include "common.h"

#include "Render.h"
#include "Object.h"
#include "VKProgram.h"
#include "VKBufferMemory.h"

class VKRender : public Render
{
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	//const bool enableValidationLayers = true;
	const bool enableValidationLayers = false;
#endif
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	// Extensiones que necesitaremos activar en nuestro motor gr�fico
	//- SwapChain: Alojamiento de framebuffers que se dibujar�n
	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	

	// n�mero m�ximo de im�genes renderizadas "a la vez"
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	typedef struct bufferObject_t
	{
		VKBufferMemory* vertexBuffer;
		VKBufferMemory* indexBuffer;
	} bufferObject_t;

	std::map<int, bufferObject_t> boList;
	VulkanContext* vkc = nullptr;
	std::list<Object*> objList;

	VkDebugUtilsMessengerEXT debugMessenger;
	VKDepthBufferObject depthBufferObject;

public:
	VKRender();

	void createInstance();
	void createSurface();
	void pickDevice();
	QueueFamilyIndices selectQueue(VkPhysicalDevice device);
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void drawFrame(std::list<Object*>& objs);
	void createCommandBuffers();
	void addCommands(int cbId, Mesh3D* mesh);
	void recordCommandBuffers(std::list<Object*>& objs, int currentFrame, int imageIndex);
	void createFramebuffers();
	void createSemaphores();
	void createCommandPool();
	void createDepthResources();
	//void createDescriptorPool();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	std::vector<const char*> getRequiredExtensions();
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);


	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	// OVERRIDES
	virtual void init() override;
	virtual void setupObject(Object* obj) override;
	virtual void updateObject(Object* obj) override {};

	virtual void removeObject(Object* obj) override {};
	virtual void drawObject(Object* objs) override {};
	virtual void drawObjects(std::list<Object*>* objs) override;
	virtual void drawObjects(std::map<float, Object*>* objs) override {};
	virtual bool isClosed() override { return glfwWindowShouldClose(vkc->window); };
	virtual void swapBuffer() override {};

	void toggleCursor(bool value) override {};

	GLFWwindow* getWindow() override { return vkc->window; }
	void initWindow();
	VKRender::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice);
	~VKRender();

};
