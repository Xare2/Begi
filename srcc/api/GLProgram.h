#pragma once
#include "common.h"
#include "Program.h"
#include "RenderProgram.h"

class GLSLShader : public Program
{
private:
	std::string source;

public:
	GLSLShader(std::string filename, unsigned int type);

	unsigned int getIdProgram();

	void compile() override;
	void readFile() override;
	void checkErrors() override;
};

class GLProgram : public RenderProgram
{
public:
	std::map<unsigned int, GLSLShader*> shaderList;
	//std::map<std::string, unsigned int> varList;

	GLProgram();
	void addProgram(std::string filename) override;
	void checkLinkerErrors() override;
	void linkProgram() override;
	void use() override;

	void getVarList();
	void getLightVarList();
	unsigned int getVarLocation(std::string varName);

	//funciones set de datos
	void setMVP(glm::mat4 m);
	void setVertexPos(GLsizei stride, void* offset, GLint count, GLenum type);
	void setVertexColor(GLsizei stride, void* offset, GLint count, GLenum type);
	void setVertexNormal(GLsizei stride, void* offset, GLint count, GLenum type);
	void setVertexCoordText(GLsizei stride, void* offset, GLint count, GLenum type);
	void setVertexTangent(GLsizei stride, void* offset, GLint count, GLenum type);
	void setVertexBonesIdx(GLsizei stride, void* offset, GLint count, GLenum type);
	void setVertexBonesWeights(GLsizei stride, void* offset, GLint count, GLenum type);

	void setInt(std::string name, int val) override;
	void setFloat(std::string name, float val) override;
	void setVec3(std::string name, const glm::vec3& vec) override;
	void setVec4(std::string name, const glm::vec4& vec) override;
	void setMatrix(std::string name, const glm::mat4& matrix) override;

	void setInt(std::string name, int val, int offset, int stride);
	void setFloat(std::string name, float val, int offset, int stride);
	void setVec3(std::string name, const glm::vec3& vec, int offset, int stride);
	void setVec4(std::string name, const glm::vec4& vec, int offset, int stride);
	void setMatrix(std::string name, const glm::mat4& matrix, int offset, int stride);

	void setLight(Light* l) override;
	void setLight(Light* l, int idx) override;
	void setColorTexEnable() override;
	void setColorTexDisable() override;
	void bindTextureSampler(int binding, Texture* tex) override;

	void setColorTexSampler(int idx, int use);
	void setColorCubicSampler(int idx, int use);
};