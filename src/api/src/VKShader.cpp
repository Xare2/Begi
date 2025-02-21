#include "VKShader.h"

VKShader::VKShader(std::string fileName, unsigned int type) : Program(fileName, type)
{
	if (fileName.ends_with(".vert"))
		vkType = 0;
	else if (fileName.ends_with(".frag"))
		vkType = 1;

	readFile();
	compile();
}

void VKShader::readFile()
{
	std::ifstream f(fileName);
	if (f.is_open())
	{
		code = std::string(std::istreambuf_iterator<char>(f), {});
	}
	else
	{
		std::cout << "ERROR: FICHERO NO ENCONTRADO " << __FILE__ << ":" << __LINE__ << " " << fileName << "\n";
	}
}

void VKShader::compile()
{
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	// Like -DMY_DEFINE=1
	//options.AddMacroDefinition("MY_DEFINE", "1");
	//if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

	module = compiler.CompileGlslToSpv(code, (shaderc_shader_kind)vkType, fileName.c_str(), options);

	if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << module.GetErrorMessage();
	}

	std::vector<uint32_t> code = { module.cbegin(), module.cend() };

	//compiledCode = { .length = code.size() * sizeof(uint32_t), .data = code.data() };
	//compiledCode = { .length = code.size(), .data = code.data() };

	compiledCode.length = code.size() * sizeof(uint32_t);
	compiledCode.data = new uint32_t[code.size()];

	memcpy(compiledCode.data, code.data(), code.size() * sizeof(uint32_t));

}

void VKShader::checkErrors()
{
	//TODO
}

VkShaderModule VKShader::createShaderModule(VkDevice device, volatile VKShader::shaderCode_t* code)
{
	VkShaderModule shaderModule;

	VkShaderModuleCreateInfo createInfo
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = code->length,
		.pCode = code->data,
	};

	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

VKShader::~VKShader()
{
	delete[] compiledCode.data;
}
