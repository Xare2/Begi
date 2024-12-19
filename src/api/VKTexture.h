#pragma once
#include "Texture.h"

#include "common.h"
#include "VKBufferMemory.h"

class VKTexture :
	public Texture
{
private:
	VkSampler sampler{};
	VkImageView imageView{};
	VKImageBufferObject imageBufferObject;
	VKDepthBufferObject depthBufferObject;
	//VKImageBufferObject cubicImageBufferObjects[6];

	void createImageView(VkDevice device);
	void createTextureSampler(VkDevice device, VkPhysicalDevice physicalDevice);

public:
	VKTexture();
	VKTexture(std::string filename);
	VKTexture(std::string filename, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
	VKTexture(const std::string& left, const std::string& right,
		const std::string& front, const std::string& back,
		const std::string& top, const std::string& bottom);


	void update() override;
	void updateTextureImage(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
	void updateDepthImage(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);

	VkSampler getSampler(){ return sampler; }
	VkImageView getImageView() { return imageView; }
};


