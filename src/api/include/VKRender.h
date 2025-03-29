#pragma once
// #define NDEBUG

#include "common.h"

#include "Render.h"
#include "Object.h"
#include "VKProgram.h"
#include "VKBufferMemory.h"

class VKRender : public Render
{	
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	typedef struct bufferObjectVK_t
	{
		VKBufferMemory *vertexBuffer;
		VKBufferMemory *indexBuffer;
	} bufferObjectVK_t;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
	const std::vector<const char *> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	std::map<int, bufferObjectVK_t> boList;
	VulkanContext *vkc = nullptr;
	std::list<Object *> objList;

	VkDebugUtilsMessengerEXT debugMessenger;

	bool checkValidationLayerSupport();

public:
	VKRender();

	void createInstance();
	void createSurface();
	void pickDevice();
	QueueFamilyIndices selectQueue(VkPhysicalDevice device);
	void createLogicalDevice();
	void createImages();
	void createImage(size_t step);
	void createSwapchainImage(size_t step);
	void createImageViews();
	void createRenderPass();
	void drawFrame(std::map<float, Object *> &objs);
	void createCommandBuffers();
	void addCommands(int cbId, Mesh3D *mesh);
	void recordCommandBuffers(std::list<Object *> &objs, int currentFrame, int imageIndex);
	void recordCommandBuffers(std::map<float, Object *> &objs, int currentFrame, int imageIndex);
	void createFramebuffers();
	void createSemaphores();
	void createCommandPool();
	void createDepthResources();
	// void createDescriptorPool();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
	std::vector<const char *> getRequiredExtensions();
	// VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
	// void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

	// OVERRIDES
	virtual void init() override;
	virtual void setupObject(Object *obj) override;
	virtual void updateObject(Object *obj) override;

	virtual void removeObject(Object *obj) override;
	virtual void drawObject(Object *obj) override;
	virtual void drawObjects(std::map<float, Object *> *objs) override;
	virtual bool isClosed() override { return glfwWindowShouldClose(vkc->window); };
	virtual void swapBuffer() override;

	virtual void toggleCursor(bool value) override;

	virtual void setCurrentRenderStep(int currentStep) override;
	virtual void initFrameBuffers(int stepCount) override;
	virtual void setupFrameBuffer() override;
	virtual void setOutBuffer(std::string type, std::string bufferName) override;
	virtual void setOutBuffer(int step, Texture::textureType_e type, GLTexture *tex) override;
	virtual Texture *getBuffer(std::string bufferName) const override;
	virtual Texture *getBuffer(int step, Texture::textureType_e type) override;

	GLFWwindow *getWindow() override { return vkc->window; }
	void initWindow();
	VKRender::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice);
	~VKRender();
};
