#pragma once

#include <shaderc/shaderc.hpp>
#include <optional>

#include "RenderProgram.h"
#include "common.h"
#include "vertex.h"

#define MAX_FRAMES_IN_FLIGHT 2

class VKShader : public Program
{
public:
    typedef struct shaderCode_t
    {
        size_t length;
        uint32_t *data;
    } shaderCode_t;

    shaderc::SpvCompilationResult module;
    std::string code = "";
    shaderCode_t compiledCode;
    unsigned int vkType = -1;

    VKShader(std::string fileName, unsigned int type);

    void readFile() override;
    void compile() override;
    void checkErrors() override;

    static VkShaderModule createShaderModule(VkDevice device, volatile VKShader::shaderCode_t *code);

    ~VKShader();
};