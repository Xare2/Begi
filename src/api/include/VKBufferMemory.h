#pragma once

#include "common.h"
//#include "VKProgram.h"

// clase base gen�rica para tener un buffer de memoria. Puede ser implementada para buffers dev�rtices, �ndices, texturas, profundidad....
class VKBufferMemory
{
protected:
	//static inline VulkanContext* vulkanContext = nullptr;

	VkBuffer bufferId = {};
	VkImage textureImageId = {};
	VkDeviceMemory memory = {};
	VkBuffer staggingBufferId = {};
	VkDeviceMemory staggingMemory = {};
	VkFormat depthFormat = VK_FORMAT_UNDEFINED;

public:
	std::string bufferName = "";
	VkDeviceSize size = 0;
	// bufferTypes_e type;
	unsigned int width = 0;
	unsigned int height = 0;

	VKBufferMemory(std::string bufferName) : bufferName(bufferName) {};

	//static void setVulkanContext( );
	static unsigned int findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
		VkMemoryPropertyFlags properties
	);
	static void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size,
		VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer& bid, VkDeviceMemory& mem
	);
	static void createImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format,
		VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
		VkImage& image, VkDeviceMemory& imageMemory
	);
	static VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
	static void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue);

	virtual void copyDataToBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkDeviceSize size, void* data) = 0;
	virtual VkBuffer getBufferID() { return bufferId; }
	virtual VkImage getTextureBufferID() { return textureImageId; }
	virtual VkFormat getDepthFormat() { return depthFormat; }
};

// clase que implementa un bufferObject de v�rtices
class VKVertexBufferObject : public VKBufferMemory
{
protected:
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue);
public:
	VKVertexBufferObject(std::string bufferName) : VKBufferMemory(bufferName) {};
	VKVertexBufferObject(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, std::string bufferName);

	void copyDataToBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkDeviceSize size, void* data) override;
};

// clase que implementa un buffer de �ndices, reutiliza partes dde la clase anterior
class VKIndexBufferObject : public VKVertexBufferObject
{
public:
	VKIndexBufferObject(std::string bufferName) : VKVertexBufferObject(bufferName) {};
	VKIndexBufferObject(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, std::string bufferName);
};

// clase que implementa buffers para variables de tipo uniform
class VKUniformBufferObject : public VKBufferMemory
{
private:
	void* uniformBufferMap = nullptr;

public:
	VKUniformBufferObject(std::string bufferName) : VKBufferMemory(bufferName) {};
	VKUniformBufferObject(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, std::string bufferName);

	void copyDataToBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkDeviceSize size, void* data) override;
};

// Clase que implemente buffers de tipo imagen (texturas y buffers de color)
class VKImageBufferObject : public VKBufferMemory
{
public:
	VKImageBufferObject() : VKBufferMemory("") {};
	VKImageBufferObject(std::string bufferName) : VKBufferMemory(bufferName) {};
	VKImageBufferObject(VkDevice device, VkPhysicalDevice physicalDevice, int w, int h, std::string bufferName);

	void copyDataToBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkDeviceSize size, void* data) override;
	void transitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	static void copyBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};

// clase que implementa buffers de profundidad
class VKDepthBufferObject : public VKBufferMemory
{
public:
	VKDepthBufferObject() : VKBufferMemory("") {};
	VKDepthBufferObject(std::string bufferName) : VKBufferMemory(bufferName) { depthFormat = VK_FORMAT_UNDEFINED; };
	VKDepthBufferObject(VkDevice device, VkPhysicalDevice physicalDevice, int w, int h, std::string bufferName);

	void copyDataToBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkDeviceSize size, void* data) override;

	static VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	static VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);
};