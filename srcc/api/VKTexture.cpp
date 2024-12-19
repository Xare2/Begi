#include "VKTexture.h"


VKTexture::VKTexture() : Texture()
{
}

VKTexture::VKTexture(std::string filename) : Texture(filename)
{
	//imageBufferObject = VKImageBufferObject()
	//this->update();
}

VKTexture::VKTexture(std::string filename, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue) : Texture(fileName)
{
	updateTextureImage(device, physicalDevice, commandPool, graphicsQueue);
}

VKTexture::VKTexture(const std::string& left, const std::string& right, const std::string& front, const std::string& back, const std::string& top, const std::string& bottom)
{
	this->fileName = left;
	this->type = textureType_e::cubic;
	this->load(left, right,
		front, back,
		top, bottom);
}

void VKTexture::createImageView(VkDevice device)
{
	VkImageViewCreateInfo viewInfo
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = imageBufferObject.getTextureBufferID(),
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = VK_FORMAT_R8G8B8A8_SRGB,
	};

	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}
}

void VKTexture::createTextureSampler(VkDevice device, VkPhysicalDevice physicalDevice)
{
	const VkFilter FILTER = bilinear ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
	const VkSamplerAddressMode REPEAT = repeat ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = FILTER;
	samplerInfo.minFilter = FILTER;
	samplerInfo.addressModeU = REPEAT;
	samplerInfo.addressModeV = REPEAT;
	samplerInfo.addressModeW = REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void VKTexture::update()
{
	//VKBufferMemory::createImage()
}

void VKTexture::updateTextureImage(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
	imageBufferObject = VKImageBufferObject(device, physicalDevice, size.x, size.y, fileName + "Buffer");
	imageBufferObject.copyDataToBuffer(device, physicalDevice, commandPool, graphicsQueue, size.x * size.y * 4, texBytes[0].data());
	createImageView(device);
	createTextureSampler(device, physicalDevice);
}

void VKTexture::updateDepthImage(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
	depthBufferObject = VKDepthBufferObject(device, physicalDevice, size.x, size.y, fileName + "Buffer");
	depthBufferObject.copyDataToBuffer(device, physicalDevice, commandPool, graphicsQueue, size.x * size.y * 4, texBytes[0].data());
	createImageView(device);
	createTextureSampler(device, physicalDevice);
}
