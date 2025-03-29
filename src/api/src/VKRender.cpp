#include "VKRender.h"
#include "VKProgram.h"
#include "VKMaterial.h"
#include "Object3D.h"
#include "System.h"

#include <set>
#include <algorithm>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
	std::cerr << "[WARNING] Validation layer: " << pCallbackData->pMessage << "\n\n------------------------\n"<< std::endl;

	return VK_FALSE;
}


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}

bool VKRender::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char *layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

VKRender::VKRender()
{
	vkc = new VulkanContext();
}

void VKRender::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Hello triangle",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0};

	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	auto extensions = getRequiredExtensions();

	VkInstanceCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = 0,
		.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data()
		// .enabledExtensionCount = glfwExtensionCount,
		// .ppEnabledExtensionNames = glfwExtensions
	};

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &vkc->instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
}

void VKRender::createSurface()
{
	if (glfwCreateWindowSurface(vkc->instance, vkc->window, nullptr, &vkc->surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
}

void VKRender::pickDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vkc->instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(vkc->instance, &deviceCount, devices.data());

	for (const auto &device : devices)
	{
		if (isDeviceSuitable(device))
		{
			vkc->physicalDevice = device;
			break;
		}
	}

	if (vkc->physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

QueueFamilyIndices VKRender::selectQueue(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;
	// Logic to find queue family indices to populate struct with

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto &queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vkc->surface, &presentSupport);

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

void VKRender::createLogicalDevice()
{
	QueueFamilyIndices indices = selectQueue(vkc->physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = queueFamily,
			.queueCount = 1,
			.pQueuePriorities = &queuePriority};

		queueCreateInfos.push_back(queueCreateInfo);
	}

	// queueCreateInfo.pQueuePriorities = &queuePriority;
	VkPhysicalDeviceFeatures deviceFeatures{
		.samplerAnisotropy = VK_TRUE};

	VkDeviceCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
		.pQueueCreateInfos = queueCreateInfos.data(),
		.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data(),
		.pEnabledFeatures = &deviceFeatures};

	// createInfo.enabledExtensionCount = 0;

	// validation layers no esta implementado uwu
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(vkc->physicalDevice, &createInfo, nullptr, &vkc->device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(vkc->device, indices.presentFamily.value(), 0, &vkc->presentQueue);
	vkGetDeviceQueue(vkc->device, indices.graphicsFamily.value(), 0, &vkc->graphicsQueue);
}

void VKRender::createImages()
{
	vkc->swapChainImages.resize(System::getStepCount());

	for (size_t i = 0; i < System::getStepCount(); i++)
	{
		std::string stepOutput = System::getStepOutput(i);

		if (stepOutput == "screen")
		{
			createSwapchainImage(i);
		}
		else
		{
			createImage(i);
		}
	}
}

void VKRender::createImage(size_t step)
{
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM; // Or any format you want
	imageCreateInfo.extent = {this->getWidth(), this->getHeight(), 1};
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	vkc->swapChainImages[step].resize(1);

	vkCreateImage(vkc->device, &imageCreateInfo, nullptr, vkc->swapChainImages[step].data());
}

void VKRender::createSwapchainImage(size_t step)
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vkc->physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = vkc->surface,
		.minImageCount = imageCount,
		.imageFormat = surfaceFormat.format,
		.imageColorSpace = surfaceFormat.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
	};

	QueueFamilyIndices indices = selectQueue(vkc->physicalDevice);
	uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	// It is also possible that you'll render images to a separate image first to perform operations
	// like post-processing. In that case you may use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT
	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;	  // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(vkc->device, &createInfo, nullptr, &vkc->swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(vkc->device, vkc->swapChain, &imageCount, nullptr);
	vkc->swapChainImages[step].resize(imageCount);
	vkGetSwapchainImagesKHR(vkc->device, vkc->swapChain, &imageCount, vkc->swapChainImages[step].data());
	vkc->swapChainImageFormat = surfaceFormat.format;
	vkc->swapChainExtent = extent;
}

void VKRender::createImageViews()
{
	vkc->swapChainImageViews.resize(System::getStepCount());

	for (size_t i = 0; i < System::getStepCount(); i++)
	{
		vkc->swapChainImageViews[i].resize(vkc->swapChainImages[i].size());

		for (size_t j = 0; j < vkc->swapChainImages[i].size(); j++)
		{
			VkImageViewCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = vkc->swapChainImages[i][j],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = vkc->swapChainImageFormat,
			};

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(vkc->device, &createInfo, nullptr, &vkc->swapChainImageViews[i][j]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create image views!");
			}
		}
	}
}

void VKRender::createRenderPass()
{
	const int stepCount = System::getStepCount();
	VkAttachmentDescription colorAttachment{
		.format = vkc->swapChainImageFormat,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};

	VkAttachmentReference colorAttachmentRef{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

	VkAttachmentDescription depthAttachment{
		.format = VKDepthBufferObject::findDepthFormat(vkc->physicalDevice),
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	};

	VkAttachmentReference depthAttachmentRef{
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

	VkSubpassDescription subpass{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentRef,
		.pDepthStencilAttachment = &depthAttachmentRef,
	};

	VkSubpassDependency dependency{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
	};

	std::vector<VkAttachmentDescription> attachments = {colorAttachment, depthAttachment};

	VkRenderPassCreateInfo renderPassInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = static_cast<uint32_t>(attachments.size()),
		.pAttachments = attachments.data(),
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency,
	};

	if (vkCreateRenderPass(vkc->device, &renderPassInfo, nullptr, &vkc->renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

void VKRender::drawFrame(std::map<float, Object *> &objs)
{
	vkWaitForFences(vkc->device, 1, &vkc->inFlightFences[this->currentStep][vkc->currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(
		vkc->device, vkc->swapChain, UINT64_MAX,
		vkc->imageAvailableSemaphores[this->currentStep][vkc->currentFrame],
		VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		// recreateSwapChain();
		return;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// UPDATE UNIFORM BUFFERS
	for (auto it : objs)
	{
		for (auto &mesh : it.second->getMeshes())
		{
			// Object3D* obj = (Object3D*)objs.front();
			it.second->computeModelMatrix();
			System::setModelMatrix(it.second->getModelMatrix());
			System::setActiveObject((Object3D *)it.second);

			mesh->getMaterial()->prepare();
		}
	}

	// Only reset the fence if we are submitting work
	vkResetFences(vkc->device, 1, &vkc->inFlightFences[this->currentStep][vkc->currentFrame]);

	vkResetCommandBuffer(vkc->commandBuffers[this->currentStep][vkc->currentFrame], 0);

	if (System::getStepOutput(this->currentStep) != "screen")
	{
		imageIndex = 0;
	}

	recordCommandBuffers(objs, vkc->currentFrame, imageIndex);

	VkSemaphore waitSemaphores[] = {vkc->imageAvailableSemaphores[this->currentStep][vkc->currentFrame]};
	VkSemaphore signalSemaphores[] = {vkc->renderFinishedSemaphores[this->currentStep][vkc->currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	VkSubmitInfo submitInfo{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = waitSemaphores,
		.pWaitDstStageMask = waitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &vkc->commandBuffers[this->currentStep][vkc->currentFrame],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = signalSemaphores,
	};

	VkResult res = vkQueueSubmit(vkc->graphicsQueue, 1, &submitInfo, vkc->inFlightFences[this->currentStep][vkc->currentFrame]);
	if (res != VK_SUCCESS)
	{
		std::cerr << "upslis";
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkSwapchainKHR swapChains[] = {vkc->swapChain};

	VkPresentInfoKHR presentInfo{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signalSemaphores,
		.swapchainCount = 1,
		.pSwapchains = swapChains,
		.pImageIndices = &imageIndex,
		.pResults = nullptr // Optional
	};

	result = vkQueuePresentKHR(vkc->presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		// recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}
}

void VKRender::createCommandBuffers()
{
	const int stepCount = System::getStepCount();
	vkc->commandBuffers.resize(stepCount);
	for (size_t i = 0; i < stepCount; i++)
	{
		vkc->commandBuffers[i].resize(MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = vkc->commandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = (uint32_t)vkc->commandBuffers[i].size(),
		};

		if (vkAllocateCommandBuffers(vkc->device, &allocInfo, vkc->commandBuffers[i].data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}
}

void VKRender::recordCommandBuffers(std::list<Object *> &objs, int currentFrame, int imageIndex)
{
	VkCommandBuffer commandBuffer = vkc->commandBuffers[this->currentStep][currentFrame];

	VkCommandBufferBeginInfo beginInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = 0,					// Optional
		.pInheritanceInfo = nullptr // Optional
	};

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = vkc->renderPass,
		.framebuffer = vkc->frameBuffers[this->currentStep][imageIndex],
	};

	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = vkc->swapChainExtent;

	std::vector<VkClearValue> clearValues(2);
	clearValues[0].color = {{0.2f, 0.2f, 0.2f, 1.0f}};
	clearValues[1].depthStencil = {1.0f, 0};

	renderPassInfo.pClearValues = clearValues.data();
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	for (auto &obj : objs)
	{
		for (auto &mesh : obj->getMeshes())
		{
			VkPipeline graphicsPipeline = ((VKProgram *)(mesh->getMaterial()->getProgram()))->graphicsPipeline;
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

			// TODO hace falta esto aqui
			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)vkc->swapChainExtent.width;
			viewport.height = (float)vkc->swapChainExtent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

			// TODO hace falta esto aqui
			VkRect2D scissor{};
			scissor.offset = {0, 0};
			scissor.extent = vkc->swapChainExtent;
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			VKProgram *prg = (VKProgram *)mesh->getMaterial()->getProgram();

			VkBuffer vertexBuffers[] = {boList[mesh->getMeshID()].vertexBuffer->getBufferID()};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, boList[mesh->getMeshID()].indexBuffer->getBufferID(), 0, VK_INDEX_TYPE_UINT32);

			vkCmdBindDescriptorSets(
				commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				prg->pipelineLayout,
				0, 1,
				&prg->descriptorSets[currentFrame],
				0, nullptr);

			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh->getTriangleIdxList()->size()), 1, 0, 0, 0);
			// vkCmdDraw(commandBuffer, static_cast<uint32_t>(objs.front()->getMesh()->getVertList()->size()), 1, 0, 0);
		}
	}

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void VKRender::recordCommandBuffers(std::map<float, Object *> &objs, int currentFrame, int imageIndex)
{
	VkCommandBuffer commandBuffer = vkc->commandBuffers[this->currentStep][currentFrame];

	VkCommandBufferBeginInfo beginInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = 0,					// Optional
		.pInheritanceInfo = nullptr // Optional
	};

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = vkc->renderPass,
		.framebuffer = vkc->frameBuffers[this->currentStep][imageIndex],
	};

	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = vkc->swapChainExtent;

	std::vector<VkClearValue> clearValues(2);
	clearValues[0].color = {{0.2f, 0.2f, 0.2f, 1.0f}};
	clearValues[1].depthStencil = {1.0f, 0};

	renderPassInfo.pClearValues = clearValues.data();
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	for (auto &obj : objs)
	{
		for (auto &mesh : obj.second->getMeshes())
		{
			VkPipeline graphicsPipeline = ((VKProgram *)(mesh->getMaterial()->getProgram()))->graphicsPipeline;
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)vkc->swapChainExtent.width;
			viewport.height = (float)vkc->swapChainExtent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = {0, 0};
			scissor.extent = vkc->swapChainExtent;
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			VKProgram *prg = (VKProgram *)mesh->getMaterial()->getProgram();

			VkBuffer vertexBuffers[] = {boList[mesh->getMeshID()].vertexBuffer->getBufferID()};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, boList[mesh->getMeshID()].indexBuffer->getBufferID(), 0, VK_INDEX_TYPE_UINT32);

			vkCmdBindDescriptorSets(
				commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				prg->pipelineLayout,
				0, 1,
				&prg->descriptorSets[currentFrame],
				0, nullptr);

			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh->getTriangleIdxList()->size()), 1, 0, 0, 0);
			// vkCmdDraw(commandBuffer, static_cast<uint32_t>(objs.front()->getMesh()->getVertList()->size()), 1, 0, 0);
		}
	}

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void VKRender::initFrameBuffers(int stepCount)
{
	vkc->frameBuffers.resize(stepCount);

	for (size_t i = 0; i < stepCount; i++)
	{
		vkc->frameBuffers[i].resize(vkc->swapChainImageViews[i].size());
		for (size_t j = 0; j < vkc->swapChainImageViews[i].size(); j++)
		{
			const int ATTACHMENT_COUNT = 2;
			VkImageView attachments[ATTACHMENT_COUNT] = {
				vkc->swapChainImageViews[i][j],
				vkc->depthImageView};

			VkFramebufferCreateInfo framebufferInfo{
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = vkc->renderPass,
				.attachmentCount = ATTACHMENT_COUNT,
				.pAttachments = attachments,
				.width = vkc->swapChainExtent.width,
				.height = vkc->swapChainExtent.height,
				.layers = 1,
			};

			if (vkCreateFramebuffer(vkc->device, &framebufferInfo, nullptr, &vkc->frameBuffers[i][j]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}
}

void VKRender::createSemaphores()
{
	VkSemaphoreCreateInfo semaphoreInfo{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
	VkFenceCreateInfo fenceInfo{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT};

	const int stepCount = System::getStepCount();

	vkc->inFlightFences.resize(stepCount);
	vkc->imageAvailableSemaphores.resize(stepCount);
	vkc->renderFinishedSemaphores.resize(stepCount);

	for (int i = 0; i < stepCount; i++)
	{
		vkc->inFlightFences[i].resize(MAX_FRAMES_IN_FLIGHT);
		vkc->imageAvailableSemaphores[i].resize(MAX_FRAMES_IN_FLIGHT);
		vkc->renderFinishedSemaphores[i].resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++)
		{
			if (vkCreateSemaphore(vkc->device, &semaphoreInfo, nullptr, &vkc->imageAvailableSemaphores[i][j]) != VK_SUCCESS ||
				vkCreateSemaphore(vkc->device, &semaphoreInfo, nullptr, &vkc->renderFinishedSemaphores[i][j]) != VK_SUCCESS ||
				vkCreateFence(vkc->device, &fenceInfo, nullptr, &vkc->inFlightFences[i][j]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create semaphores!");
			}
		}
	}
}

void VKRender::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = selectQueue(vkc->physicalDevice);

	VkCommandPoolCreateInfo poolInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(),
	};

	if (vkCreateCommandPool(vkc->device, &poolInfo, nullptr, &vkc->commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

void VKRender::createDepthResources()
{
	VKDepthBufferObject depthBufferObject = VKDepthBufferObject(
		vkc->device,
		vkc->physicalDevice,
		this->getWidth(),
		this->getHeight(),
		"DepthBuffer");

	VkImageViewCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = depthBufferObject.getTextureBufferID(),
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = depthBufferObject.getDepthFormat(),
	};

	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(vkc->device, &createInfo, nullptr, &vkc->depthImageView) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image views!");
	}
}

// void VKRender::createDescriptorPool()
//{
//	std::vector<VkDescriptorPoolSize> poolSizes = {
//		{
//			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
//			.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
//		},
//		//{
//		//	.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
//		//	.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
//		//},
//		//{
//		//	.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
//		//	.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
//		//},
//		{
//			.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
//			.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
//		}
//	};
//
//	//if (poolSizes.size() != VKProgram::UNIFORM_OBJECT_COUNT)
//	//{
//	//	std::cerr << __FILE__ << ": " << __LINE__ << ": Number of descriptor pool and uniform objects not matching" << std::endl;
//	//}
//
//	VkDescriptorPoolCreateInfo poolInfo
//	{
//		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
//		.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
//		.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
//		.pPoolSizes = poolSizes.data(),
//	};
//
//	if (vkCreateDescriptorPool(vkc->device, &poolInfo, nullptr, &vkc->descriptorPool) != VK_SUCCESS) {
//		throw std::runtime_error("failed to create descriptor pool!");
//	}
// }

bool VKRender::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = selectQueue(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

bool VKRender::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto &extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void VKRender::setupDebugMessenger()
{
	if (!enableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	VkResult res = CreateDebugUtilsMessengerEXT(vkc->instance, &createInfo, nullptr, &debugMessenger);
	if (res != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void VKRender::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

std::vector<const char *> VKRender::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

VkSurfaceFormatKHR VKRender::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
	for (const auto &availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR VKRender::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
	for (const auto &availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VKRender::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(vkc->window, &width, &height);

		VkExtent2D actualExtent =
			{
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void VKRender::init()
{

	this->initWindow();

	createInstance();
	setupDebugMessenger();
	createSurface();
	pickDevice();
	createLogicalDevice();
	createImages();
	createImageViews();
	createRenderPass();
	// createDescriptorPool();
	createCommandPool();
	createDepthResources();
	initFrameBuffers(System::getStepCount());
	createCommandBuffers();
	createSemaphores();
}

void VKRender::setupObject(Object *obj)
{
	objList.push_back(obj);

	for (auto &mesh : obj->getMeshes())
	{
		VKProgram *prg = (VKProgram *)mesh->getMaterial()->getProgram();
		if (!prg->inited)
		{
			prg->setVulkanContext(vkc);
			prg->use();

			for (auto tex : mesh->getMaterial()->getTextures())
			{
				((VKTexture *)tex.second)->updateTextureImage(vkc->device, vkc->physicalDevice, vkc->commandPool, vkc->graphicsQueue);
				prg->bindTextureSampler(1, tex.second);
			}
			// TODO esto
			// prg->setVertexPositions(offsetof(vertex_t, pos), 4);
			// prg->setVertexUVs(offsetof(vertex_t, texCoords), 2);
			//
			// crear descriptores para las variables uniform
			prg->createDescriptorSetLayout();
			// crear un pipeline:
			//  - Definicion de primitivas
			//  - Transformacion de vertices (vert. shader)
			//  - Unir vertices y crear nuevos vertices "internos" (teselacion shader)
			//  - Crear nueva geometria (geometry shader)
			//  - Rasterizado (3D->2D + depth buffer)
			//  - Texturado/iluminacion/sombreado (fragment shader)
			//  - Mezclado de color entre nuevo objeto y fondo (Blending)
			prg->createGraphicsPipeline();
			// Cola/memoria de comandos a enviar cuando se renderice, con sus datos
			// mas adelante, creamos los buffers de vertices
			// prg->createVertexBuffer(sizeof(vertex_t) * obj->getMesh()->getVertList().size());
			// prg->createIndexBuffer(sizeof(vertex_t) * obj->getMesh()->getVertList().size());
			prg->createUniformBuffers();
			prg->createDescriptorPool();
			prg->createDescriptorSets();
		}
		prg->inited = true;

		// VKBufferMemory::setVulkanContext(vkc);
		bufferObjectVK_t bo{};

		bo.vertexBuffer = new VKVertexBufferObject(
			vkc->device,
			vkc->physicalDevice,
			mesh->getVertList()->size() * sizeof(vertex_t),
			"vertexBuff");

		bo.indexBuffer = new VKIndexBufferObject(
			vkc->device,
			vkc->physicalDevice,
			mesh->getTriangleIdxList()->size() * sizeof(glm::uint32_t),
			"indexBuff");

		bo.vertexBuffer->copyDataToBuffer(
			vkc->device,
			vkc->physicalDevice,
			vkc->commandPool,
			vkc->graphicsQueue,
			mesh->getVertList()->size() * sizeof(vertex_t),
			mesh->getVertList()->data());

		bo.indexBuffer->copyDataToBuffer(
			vkc->device,
			vkc->physicalDevice,
			vkc->commandPool,
			vkc->graphicsQueue,
			mesh->getTriangleIdxList()->size() * sizeof(glm::uint32_t),
			mesh->getTriangleIdxList()->data());

		boList[mesh->getMeshID()] = bo;
	}
}

void VKRender::updateObject(Object *obj)
{
}

void VKRender::removeObject(Object *obj)
{
}

void VKRender::drawObject(Object *objs)
{
}

void VKRender::drawObjects(std::map<float, Object *> *objs)
{
	drawFrame(*objs);
}

void VKRender::swapBuffer()
{
	vkc->currentFrame = (vkc->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VKRender::toggleCursor(bool value)
{
}

void VKRender::setCurrentRenderStep(int currentStep)
{
	this->currentStep = currentStep;
}

void VKRender::setupFrameBuffer()
{
	// switch (this->mode)
	// {
	// case offScreen:
	// {
	// 	glBindFramebuffer(GL_FRAMEBUFFER, this->frameBufferObjectList[this->currentStep].backBufferId);
	// 	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 	GLenum drawBuf = GL_COLOR_ATTACHMENT0;
	// 	glDrawBuffers(1, &drawBuf);
	// }
	// break;

	// case onScreen:
	// {
	// 	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// 	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// }
	// break;
	// }
}

void VKRender::setOutBuffer(std::string type, std::string bufferName)
{
	// mode = offScreen;
	// if (type == "color") // guardar los buffers de salida con un nuevo nombre
	// {
	// 	buffers[bufferName] = this->frameBufferObjectList[this->currentStep].colorBuffer;
	// }
	// else if (type == "depth")
	// {
	// 	buffers[bufferName] = this->frameBufferObjectList[this->currentStep].depthBuffer;
	// }
	// else if (type == "screen") // si se activa el modo pantalla
	// {
	// 	mode = onScreen;
	// }
}

void VKRender::setOutBuffer(int step, Texture::textureType_e type, GLTexture *tex)
{
	// frameBufferObject_t fbo = this->frameBufferObjectList[step];
	// switch (type)
	// {
	// case Texture::cubic:
	// case Texture::color2D:
	// case Texture::colorBuffer:
	// 	fbo.colorBuffer = tex;
	// 	break;
	// case Texture::depthBuffer:
	// 	fbo.depthBuffer = tex;
	// 	break;
	// default:
	// 	break;
	// }
	// glBindFramebuffer(GL_FRAMEBUFFER, fbo.backBufferId); // asociar la textura anterior a ese nuevo framebuffer

	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.colorBuffer->getTexId(), 0);
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo.depthBuffer->getTexId(), 0);

	// glDrawBuffer(GL_NONE); // no se generar�n datos de pantalla
	// glReadBuffer(GL_NONE);

	// // volver al principal
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// // check errores
	// GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	// if (status != GL_FRAMEBUFFER_COMPLETE)
	// {
	// 	std::cout << "ERROR" << std::endl;
	// 	return;
	// }

	// this->frameBufferObjectList[step] = fbo;
}

Texture *VKRender::getBuffer(std::string bufferName) const
{
	return nullptr;
}

Texture *VKRender::getBuffer(int step, Texture::textureType_e type)
{
	return nullptr;
}

void VKRender::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	vkc->window = glfwCreateWindow(getWidth(), getHeight(), "Vulkan", nullptr, nullptr);

	glfwSetInputMode(vkc->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(vkc->window, 0.0, 0.0);

	// glfwSetWindowUserPointer(vkc->window, this);
	// glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

	glfwMakeContextCurrent(vkc->window);
	gladLoadGL(glfwGetProcAddress);
}

VKRender::SwapChainSupportDetails VKRender::querySwapChainSupport(VkPhysicalDevice physicalDevice)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, vkc->surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vkc->surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vkc->surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vkc->surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vkc->surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}
