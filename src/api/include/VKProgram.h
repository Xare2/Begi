#pragma once

#include "VKShader.h"
#include "RenderProgram.h"
#include "VKBufferMemory.h"

struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

//struct mat_t {
//	int shinny;
//	int enable;
//	alignas(16) glm::vec4 color;
//};

struct mat_t {
	/*int shinny;
	int enable;*/
	alignas(16) glm::ivec4 shinnyAndEnable;
	alignas(16) glm::vec4 color;
};

struct FragmentBufferObject
{
	int useTexColor = 1;
	int nLights = 0;
	alignas(16) glm::vec4 cameraPos;
	alignas(16) glm::vec4 ambientColor;
	mat_t mat;
	light_t lights[Light::N_LIGHTS];
};

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

class VulkanContext
{
public:
	GLFWwindow* window;
	VkInstance instance;
	VkSurfaceKHR surface;			 // una surface para dibujar "framebuffer"
	VkPhysicalDevice physicalDevice; // una GPU "f�sica//
	VkDevice device;				 // datos de GPU "l�gica"
	VkQueue graphicsQueue;			 // cola de instrucciones
	VkQueue presentQueue;			 // y cola para ense�ar resultados
	QueueFamilyIndices queue;
	VkSwapchainKHR swapChain;
	/* --------------------------------- BUFFERS -------------------------------- */
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat; // tama�os y colores
	VkExtent2D swapChainExtent;
	/* -------------------------------------------------------------------------- */
	std::vector<VkImageView> swapChainImageViews;
	VkImageView depthImageView;
	/* -------------------------------------------------------------------------- */
	VkRenderPass renderPass; // variables para usar/actualizar uniforms//tenemos que describir la estructura uniform
	std::vector<VkFramebuffer> frameBuffers;
	VkCommandPool commandPool;
	//VkDescriptorPool descriptorPool;
	std::vector<VkCommandBuffer> commandBuffers;
	VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
	VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT];
	std::vector<VkFence> imagesInFlight;
	int currentFrame = 0;
};

class VKProgram : public RenderProgram
{
private:
	UniformBufferObject ubo{};
	FragmentBufferObject fbo{};
	VkSampler textureSampler{};
	VkImageView textureImageView{};

public:
	static inline const int UNIFORM_OBJECT_COUNT = 2;
	static inline const int UNIFORM_OBJECT_VERTEX_LOCATION = 0;
	//static inline const int UNIFORM_OBJECT_COLOR_LOCATION = 1;
	static inline const int UNIFORM_OBJECT_FRAGMENT_LOCATION = 1;
	static inline const int UNIFORM_OBJECT_SAMPLER_LOCATION = 2;

	unsigned int pipeLineID = -1;
	std::vector<std::vector<VKBufferMemory*>> uniformBuffers;	// por cada MAX_FRAMES_IN_FLIGHT creamos todos los buffers
	VulkanContext* vkc;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	// y enlazarla en el cauce grafico, para ser accesible desde partes concretas (shaders)
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	bool inited = false;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages; // ?

	~VKProgram();
	//	for (auto i : shaders)
	//		delete i;
	//	glDeleteProgram(idRenderProgram);
	//}
	// static void updateSharedUniformBuffer(uint32_t currentImage, vulkanContext* vkc);
	void addProgram(std::string fileName) override;
	void linkProgram() override;
	void use() override;
	void checkLinkerErrors() override;

	void commitVertexBufferObject();
	void commitFragmentBufferObject();

	void setVertexPositions(size_t description, int compCount);
	void setVertexColors(size_t description, int compCount);
	void setVertexNormals(size_t description, int compCount);
	void setVertexTangents(size_t description, int compCount);
	void setVertexUVs(size_t description, int compCount);
	void setMatrixM(const glm::mat4& matrix);
	void setMatrixV(const glm::mat4& matrix);
	void setMatrixP(const glm::mat4& matrix);
	void setColor(const glm::vec4& color);


	void setInt(std::string name, int val) {}
	void setFloat(std::string name, float val) {}
	void setVec3(std::string name, const glm::vec3& vec) {}
	void setVec4(std::string name, const glm::vec4& vec) {}
	void setMatrix(std::string name, const glm::mat4& matrix) {}

	void turnOffLight(int idx);

	void setLight(Light* l) override {}
	void setLight(Light* l, int idx) override;

	void setMat(uint8_t shininess, glm::vec4 color, int enable);
	void setScene(int nLights, glm::vec3 cameraPos, glm::vec3 ambientColor);

	void setColorTex();
	void setColorTexEnable() override;
	void setColorTexDisable() override;
	void bindTextureSampler(int binding, Texture* text) override;

	unsigned int getProgramId() { return programId; }

	void updateSharedStructs();
	void updateLocalStructs();

	void readVarList();
	unsigned int getVarLocation(std::string varName);
	void setAttribute(unsigned int location, size_t description, int compCount);

	/* --------------------------------- VULKAN --------------------------------- */

	void createDescriptorPool();
	void createUniformBuffers();		// crear todos los VKBufferMemory que me hagan falta
	void setVulkanContext(VulkanContext* vkc) { this->vkc = vkc; };
	void createDescriptorSetLayout();
	void createGraphicsPipeline();

	std::vector<VkVertexInputAttributeDescription> getVertexAttributeDescriptions();
	VkVertexInputBindingDescription getVertexBindingDescription();
	void createDescriptorSets();
};