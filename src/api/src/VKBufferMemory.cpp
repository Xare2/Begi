#include "VKBufferMemory.h"
#include "vertex.h"
//void VKBufferMemory::setVulkanContext(VulkanContext* vulkanContext)
//{
//	VKBufferMemory::vulkanContext = vulkanContext;
//}

unsigned int VKBufferMemory::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void VKBufferMemory::createBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
	VkDeviceSize size, VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties, VkBuffer& bid,
	VkDeviceMemory& mem
)
{
	VkBufferCreateInfo bufferInfo
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &bid) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, bid, &memRequirements);

	VkMemoryAllocateInfo allocInfo
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties),
	};

	if (vkAllocateMemory(device, &allocInfo, nullptr, &mem) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, bid, mem, 0);
}

void VKBufferMemory::createImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width,
	uint32_t height, VkFormat format, VkImageTiling tiling,
	VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
	VkImage& image, VkDeviceMemory& imageMemory
)
{
	VkExtent3D extent = { .width = width,.height = height, .depth = 1 };

	VkImageCreateInfo createInfo
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = format,
		.extent = extent,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	if (vkCreateImage(device, &createInfo, nullptr, &image) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device, image, imageMemory, 0);
}

VkCommandBuffer VKBufferMemory::beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool)
{
	VkCommandBuffer vkCommandBuffer;

	VkCommandBufferAllocateInfo allocInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	if (vkAllocateCommandBuffers(device, &allocInfo, &vkCommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	VkCommandBufferBeginInfo beginInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};

	vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);

	return vkCommandBuffer;
}

void VKBufferMemory::endSingleTimeCommands(VkCommandBuffer commandBuffer, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer,
	};

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void VKVertexBufferObject::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);

	VkBufferCopy copyRegion
	{
		.srcOffset = 0, // Optional
		.dstOffset = 0, // Optional
		.size = size,
	};

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer, device, commandPool, graphicsQueue);
}

VKVertexBufferObject::VKVertexBufferObject(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, std::string bufferName) : VKBufferMemory(bufferName)
{
	this->size = size;
	createBuffer(
		device,
		physicalDevice,
		size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staggingBufferId,
		staggingMemory
	);

	createBuffer(
		device,
		physicalDevice,
		size,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferId,
		memory
	);
}

void VKVertexBufferObject::copyDataToBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkDeviceSize size, void* data)
{
	void* copyData;
	vkMapMemory(device, staggingMemory, 0, size, 0, &copyData);
	memcpy(copyData, data, (size_t)size);
	vkUnmapMemory(device, staggingMemory);

	copyBuffer(staggingBufferId, bufferId, size, device, commandPool, graphicsQueue);

	//delete copyData;
	//vkDestroyBuffer(device, staggingBufferId, nullptr);
	//vkFreeMemory(device, staggingMemory, nullptr);
}

VKIndexBufferObject::VKIndexBufferObject(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, std::string bufferName)
	: VKVertexBufferObject(bufferName)
{
	this->size = size;
	createBuffer(
		device,
		physicalDevice,
		size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staggingBufferId,
		staggingMemory
	);


	createBuffer(
		device,
		physicalDevice,
		size,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		bufferId,
		memory
	);
}

VKUniformBufferObject::VKUniformBufferObject(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, std::string bufferName) : VKBufferMemory(bufferName)
{
	this->size = size;
	createBuffer(
		device,
		physicalDevice,
		size,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		bufferId,
		memory
	);

	vkMapMemory(device, memory, 0, size, 0, &uniformBufferMap);
}

void VKUniformBufferObject::copyDataToBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkDeviceSize size, void* data)
{
	//UniformBufferObject ubo = reinterpret_cast<UniformBufferObject>(data);

	//memcpy(uniformBufferMap, &ubo, size);
	memcpy(uniformBufferMap, data, size);
}

VKImageBufferObject::VKImageBufferObject(VkDevice device, VkPhysicalDevice physicalDevice, int w, int h, std::string bufferName) : VKBufferMemory(bufferName)
{
	VKBufferMemory::createBuffer(
		device,
		physicalDevice,
		(VkDeviceSize)(w * h * 4),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staggingBufferId,
		staggingMemory
	);

	this->width = w;
	this->height = h;
	this->size = (VkDeviceSize)(w * h * 4);
	VKBufferMemory::createImage(
		device,
		physicalDevice,
		w,
		h,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		this->textureImageId,
		this->memory
	);
}

void VKImageBufferObject::copyDataToBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkDeviceSize size, void* data)
{
	//stbi_uc* pixels = reinterpret_cast<UniformBufferObject>(data);
	createBuffer(
		device,
		physicalDevice,
		size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staggingBufferId,
		staggingMemory
	);

	void* copyData;
	vkMapMemory(device, staggingMemory, 0, size, 0, &copyData);
	memcpy(copyData, data, static_cast<size_t>(size));
	vkUnmapMemory(device, staggingMemory);

	VKImageBufferObject::transitionImageLayout(device, commandPool, graphicsQueue, 
		textureImageId, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, 
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
	);
	copyBufferToImage(device, commandPool, graphicsQueue, 
		staggingBufferId, textureImageId,
		static_cast<uint32_t>(width), static_cast<uint32_t>(height)
	);
	VKImageBufferObject::transitionImageLayout(device, commandPool, graphicsQueue, 
		textureImageId, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);
}

void VKImageBufferObject::transitionImageLayout(VkDevice device, VkCommandPool commandPool,
	VkQueue graphicsQueue, VkImage image, VkFormat format,
	VkImageLayout oldLayout, VkImageLayout newLayout
)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		//if (hasStencilComponent(format)) {
		if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	endSingleTimeCommands(commandBuffer, device, commandPool, graphicsQueue);
}

void VKImageBufferObject::copyBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommands( commandBuffer,device, commandPool, graphicsQueue);
}

VKDepthBufferObject::VKDepthBufferObject(VkDevice device, VkPhysicalDevice physicalDevice, int w, int h, std::string bufferName) : VKBufferMemory(bufferName)
{
	this->width = w;
	this->height = h;
	this->size = w * h;

	depthFormat = VKDepthBufferObject::findDepthFormat(physicalDevice);

	VKBufferMemory::createImage(
		device,
		physicalDevice,
		w,
		h,
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		this->textureImageId,
		this->memory
	);
}

void VKDepthBufferObject::copyDataToBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkDeviceSize size, void* data)
{
	createBuffer(
		device,
		physicalDevice,
		size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staggingBufferId,
		staggingMemory
	);

	void* copyData;
	vkMapMemory(device, staggingMemory, 0, size, 0, &copyData);
	memcpy(copyData, data, static_cast<size_t>(size));
	vkUnmapMemory(device, staggingMemory);
}

VkFormat VKDepthBufferObject::findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

VkFormat VKDepthBufferObject::findDepthFormat(VkPhysicalDevice physicalDevice)
{
	return findSupportedFormat(
		physicalDevice,
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}
