#include "VKProgram.h"
#include "VKTexture.h"

void VKProgram::createDescriptorPool()
{
	std::vector<VkDescriptorPoolSize> poolSizes = {
		{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
		},
		//{
		//	.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		//	.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
		//},
		{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
		},
		{
			.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
		}
	};

	if (poolSizes.size() != VKProgram::UNIFORM_OBJECT_COUNT + 1)
	{
		std::cerr << __FILE__ << ": " << __LINE__ << ": Number of descriptor pool and uniform objects not matching" << std::endl;
	}

	VkDescriptorPoolCreateInfo poolInfo
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
		.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
		.pPoolSizes = poolSizes.data(),
	};

	if (vkCreateDescriptorPool(vkc->device, &poolInfo, nullptr, &this->descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

// TODO ??
void VKProgram::createUniformBuffers()
{
	uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		uniformBuffers[i].resize(UNIFORM_OBJECT_COUNT);

		uniformBuffers[i][UNIFORM_OBJECT_VERTEX_LOCATION] = new VKUniformBufferObject(vkc->device, vkc->physicalDevice, sizeof(UniformBufferObject), "UBO");
		//uniformBuffers[i][UNIFORM_OBJECT_COLOR_LOCATION] = new VKUniformBufferObject(vkc->device, vkc->physicalDevice, sizeof(glm::vec4), "Color");
		uniformBuffers[i][UNIFORM_OBJECT_FRAGMENT_LOCATION] = new VKUniformBufferObject(vkc->device, vkc->physicalDevice, sizeof(FragmentBufferObject), "FragmentBufferObject");
	}
}

void VKProgram::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding
	{
		.binding = UNIFORM_OBJECT_VERTEX_LOCATION,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.pImmutableSamplers = nullptr,
	};

	//VkDescriptorSetLayoutBinding colorLayoutBinding
	//{
	//	.binding = UNIFORM_OBJECT_COLOR_LOCATION,
	//	.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	//	.descriptorCount = 1,
	//	.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
	//	.pImmutableSamplers = nullptr,
	//};

	VkDescriptorSetLayoutBinding fragmentUboLayoutBinding
	{
		.binding = UNIFORM_OBJECT_FRAGMENT_LOCATION,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
		.pImmutableSamplers = nullptr,
	};

	VkDescriptorSetLayoutBinding samplerLayoutBinding
	{
		.binding = UNIFORM_OBJECT_SAMPLER_LOCATION,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
		.pImmutableSamplers = nullptr,
	};

	std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding,/* colorLayoutBinding,*/ fragmentUboLayoutBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data(),
	};

	if (vkCreateDescriptorSetLayout(vkc->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
	int a = 1;
}

void VKProgram::createGraphicsPipeline()
{
	//VkPipelineShaderStageCreateInfo shaderStages[] = shaders;

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = getVertexBindingDescription();
	auto attributeDescriptions = getVertexAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE,//VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	if (vkCreatePipelineLayout(vkc->device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = (uint32_t)shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = vkc->renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.pDepthStencilState = &depthStencil;

	if (vkCreateGraphicsPipelines(vkc->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
	int b = 1;
}

std::vector<VkVertexInputAttributeDescription> VKProgram::getVertexAttributeDescriptions()
{
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions = std::vector<VkVertexInputAttributeDescription>(4);

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(vertex_t, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(vertex_t, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(vertex_t, normal);

	attributeDescriptions[3].binding = 0;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[3].offset = offsetof(vertex_t, texCoords);

	return attributeDescriptions;
}

VkVertexInputBindingDescription VKProgram::getVertexBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription
	{
		.binding = 0,
		.stride = sizeof(vertex_t),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
	};

	return bindingDescription;
}

void VKProgram::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = this->descriptorPool,
		.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
		.pSetLayouts = layouts.data(),
	};

	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(vkc->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	//std::cout << MAX_FRAMES_IN_FLIGHT << "\n";

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		//std::vector<VkDescriptorBufferInfo> bufferInfos = {};

		//VkDeviceSize offset = 0;
		//for (auto& bufferInfo : uniformBuffers[i])
		//{
		//	bufferInfos.push_back(
		//		{
		//			.buffer = bufferInfo->getBufferID(),
		//			.offset = offset,
		//			.range = bufferInfo->size
		//		}
		//	);
		//	offset = bufferInfo->size;
		//}

		VkDescriptorBufferInfo vBufferInfo
		{
			.buffer = uniformBuffers[i][UNIFORM_OBJECT_VERTEX_LOCATION]->getBufferID(),
			.offset = 0,
			.range = uniformBuffers[i][UNIFORM_OBJECT_VERTEX_LOCATION]->size,
		};

		VkDescriptorBufferInfo fBufferInfo
		{
			.buffer = uniformBuffers[i][UNIFORM_OBJECT_FRAGMENT_LOCATION]->getBufferID(),
			.offset = 0,
			.range = uniformBuffers[i][UNIFORM_OBJECT_FRAGMENT_LOCATION]->size,
		};

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureImageView;
		imageInfo.sampler = textureSampler;

		std::vector<VkWriteDescriptorSet> descriptorWrites;
		descriptorWrites.resize(3);
		descriptorWrites[0] = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSets[i],
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo = &vBufferInfo,
		};
		descriptorWrites[1] = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSets[i],
			.dstBinding = 1,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo = &fBufferInfo,
		};
		descriptorWrites[2] = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSets[i],
			.dstBinding = 2,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &imageInfo,
		};

		vkUpdateDescriptorSets(vkc->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	};
}


void VKProgram::addProgram(std::string fileName)
{
	unsigned int type = 0;

	if (fileName.ends_with(".vert"))
		type = 0;
	else if (fileName.ends_with(".frag"))
		type = 1;

	VKShader shaderCode = VKShader(fileName, type);
	shaderCode.checkErrors();

	VkShaderModule shaderModule = VKShader::createShaderModule(vkc->device, &shaderCode.compiledCode);

	VkPipelineShaderStageCreateInfo shaderStageInfo{};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = "main";

	if (fileName.ends_with(".vert"))
		shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	else if (fileName.ends_with(".frag"))
		shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

	shaderStages.push_back(shaderStageInfo);
}

void VKProgram::linkProgram()
{

}

void VKProgram::use()
{
	this->addProgram("./mapi/vk_shader.vert");
	this->addProgram("./mapi/vk_shader.frag");
}

void VKProgram::checkLinkerErrors()
{

}

void VKProgram::commitVertexBufferObject()
{
	this->uniformBuffers[this->vkc->currentFrame][UNIFORM_OBJECT_VERTEX_LOCATION]->copyDataToBuffer(
		vkc->device, vkc->physicalDevice, vkc->commandPool, vkc->graphicsQueue, sizeof(UniformBufferObject), &ubo
	);
}

void VKProgram::commitFragmentBufferObject()
{
	this->uniformBuffers[this->vkc->currentFrame][UNIFORM_OBJECT_FRAGMENT_LOCATION]->copyDataToBuffer(
		vkc->device, vkc->physicalDevice, vkc->commandPool, vkc->graphicsQueue, sizeof(FragmentBufferObject), &fbo
	);
}

void VKProgram::setMatrixM(const glm::mat4& matrix)
{
	this->ubo.model = matrix;
}

void VKProgram::setMatrixV(const glm::mat4& matrix)
{
	this->ubo.view = matrix;
}

void VKProgram::setMatrixP(const glm::mat4& matrix)
{
	this->ubo.proj = matrix;
}

void VKProgram::setColor(const glm::vec4& color)
{
	// TODO arreglar esto
	//std::vector<float> colorVec = { color.r, color.g, color.b, color.a };
	//this->uniformBuffers[this->vkc->currentFrame][1]->copyDataToBuffer(
	//	vkc->device, vkc->physicalDevice, vkc->commandPool, vkc->graphicsQueue, sizeof(glm::vec4), colorVec.data()
	//);
}

void VKProgram::turnOffLight(int idx)
{
	fbo.lights[idx] =
	{
		.data = {
			0, 0, 0, 0
		},
		.pos = glm::vec4(0),
		.dir = glm::vec4(0),
		.color = glm::vec4(0),
	};
}

void VKProgram::setLight(Light* l, int idx)
{
	fbo.lights[idx] =
	{
		.data = {
			1, l->getType(), l->getLinearAttenuation(), glm::cos(l->getCutOffAngle())
		},
		//.enable = 1,
		//.type = l->getType(),
		//.attenuation = l->getLinearAttenuation(),
		//.cutOff = glm::cos(l->getCutOffAngle()),
		.pos = glm::vec4(l->getPos(), 0),
		.dir = glm::vec4(l->getDir(), 0),
		.color = l->getColor(),
	};
}

void VKProgram::setMat(uint8_t shininess, glm::vec4 color, int enable)
{
	fbo.mat.color = color;
	fbo.mat.shinnyAndEnable = {
		enable, shininess, 0, 0
	};
	//fbo.mat.enable = enable;
	//fbo.mat.shinny = shininess;
}

void VKProgram::setScene(int nLights, glm::vec3 cameraPos, glm::vec3 ambientColor)
{
	fbo.nLights = nLights;
	fbo.cameraPos = glm::vec4(cameraPos, 0);
	fbo.ambientColor = glm::vec4(ambientColor, 0);
}

void VKProgram::setColorTex()
{
	if (fbo.useTexColor)
		setColorTexEnable();
	else
		setColorTexDisable();
}

void VKProgram::setColorTexEnable()
{
	this->fbo.useTexColor = 1;
}

void VKProgram::setColorTexDisable()
{
	this->fbo.useTexColor = 0;
}

void VKProgram::bindTextureSampler(int binding, Texture* text)
{
	this->textureSampler = ((VKTexture*)text)->getSampler();
	this->textureImageView = ((VKTexture*)text)->getImageView();
}

void VKProgram::updateSharedStructs()
{
}

void VKProgram::updateLocalStructs()
{
}
