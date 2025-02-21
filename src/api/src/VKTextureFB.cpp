#include "VKTextureFB.h"

VKTextureFB::VKTextureFB(int type, int width, int height,
	VkDevice device, VkPhysicalDevice physicalDevice,
	VkCommandPool commandPool, VkQueue graphicsQueue)
{
	this->fbID = VKTextureFB::fbCount++;
	this->type = (Texture::textureType_e)type;
	this->size = { width, height };

	switch (type)
	{
	case colorBuffer:
		updateTextureImage(device, physicalDevice, commandPool, graphicsQueue);
		break;
	case depthBuffer:
		updateDepthImage(device, physicalDevice, commandPool, graphicsQueue);
		break;
	}
}
