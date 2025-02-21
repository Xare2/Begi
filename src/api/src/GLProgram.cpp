#include "GLProgram.h"
#include "GLTexture.h"

#include "World.h"

GLSLShader::GLSLShader(std::string filename, unsigned int type) : Program(filename, type)
{
	readFile();

	//Crear id
	this->idProgram = glCreateShader((int)type);

	//Compilar código
	compile();

	//Mostrar errores
	checkErrors();
}

unsigned int GLSLShader::getIdProgram()
{
	return idProgram;
}

void GLSLShader::compile()
{
	const char* c = source.c_str();
	glShaderSource(this->idProgram, 1, &c, nullptr);
	glCompileShader(this->idProgram);

}

void GLSLShader::readFile()
{
	std::ifstream f(fileName);
	if (f.is_open())
	{
		source = std::string(std::istreambuf_iterator<char>(f), {});
	}
	else
	{
		std::cout << __FILE__ << ": " << __LINE__ << "ERROR: FICHERO NO ENCONTRADO " << __FILE__ << ":" << __LINE__ << " " << fileName << "\n";
	}
}

void GLSLShader::checkErrors()
{
	GLint fragment_compiled;
	glGetShaderiv(idProgram, GL_COMPILE_STATUS, &fragment_compiled);
	if (fragment_compiled != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(idProgram, 1024, &log_length, message);
		std::cout << __FILE__ << ": " << __LINE__ << ": " << "ERROR compiling " << "\n" << message << "\n\n";
	}
}

GLProgram::GLProgram()
{
	// Crear identificador 
	programId = glCreateProgram();
}

void GLProgram::addProgram(std::string filename)
{
	if (filename.ends_with(".vert")) { //Shader de vertices 
		shaderList[GL_VERTEX_SHADER] = new GLSLShader(filename, GL_VERTEX_SHADER);
	}
	else if (filename.ends_with(".frag")) { //Shader de fragmento
		shaderList[GL_FRAGMENT_SHADER] = new GLSLShader(filename, GL_FRAGMENT_SHADER);
	}
	else {
		std::cout << __FILE__ << ": " << __LINE__ << "ERROR: Extensión de fichero " << filename << " no válida\n";
	}
}

void GLProgram::linkProgram()
{
	for (auto& sh : shaderList)
	{
		glAttachShader(programId, sh.second->getIdProgram());
	}
	glLinkProgram(programId);
	checkLinkerErrors();
	getVarList();
}

void GLProgram::getVarList()
{
	int maxStringSize = 1024;
	int count = 0;

	// Listar variables Atributo
	glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTES, &count);

	// y por cada variable 
	while (--count >= 0)
	{
		std::string varName;
		GLsizei length;
		GLint size;
		GLenum type;
		varName.resize(maxStringSize);
		//conseguir el location
		glGetActiveAttrib(programId, (GLuint)count, maxStringSize, &length, &size, &type, varName.data());
		varName = std::string(varName.c_str());
		//interrogar con nombre
		//guardar con nombre
		varList[varName] = glGetAttribLocation(programId, varName.c_str());
	}

	// Listar variables Uniform
	glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &count);
	// y por cada variable 
	while (--count >= 0) {
		std::string varName;
		GLsizei length;
		GLint size;
		GLenum type;
		varName.resize(maxStringSize);
		//conseguir el location
		glGetActiveUniform(programId, (GLuint)count, maxStringSize, &length, &size, &type, varName.data());
		varName = std::string(varName.c_str());

		//interrogar con nombre
		if (varName[varName.length() - 1] == ']') {//si es de tipo array
			std::string arrName = varName.substr(0, varName.find('['));
			for (int i = 0; i < size; i++) //coneguir la lista completade nombres
			{
				std::string arrNameIdx = arrName + "[" + std::to_string(i) + "]";
				varList[arrNameIdx] = glGetUniformLocation(programId, arrNameIdx.c_str());
			}
		}
		else
			varList[varName] = glGetUniformLocation(programId, varName.c_str());
	}

	//getLightVarList();

	//std::cout << varList.size() << std::endl;
}

// NOTE: esto es una guarrada pero estamos a sabado, se entrega hoy, ya para otro dia enviar correo a Marcos
// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetUniformLocation.xhtml
// https://stackoverflow.com/questions/29018766/glgetprogramiv-returns-a-lower-number-of-active-uniforms-why
// https://stackoverflow.com/questions/23591264/how-to-pass-uniform-array-of-struct-to-shader-via-c-code
void GLProgram::getLightVarList()
{
	auto iaux = glGetUniformLocation(programId, "lights[1].color");
	auto iaux2 = glGetUniformLocation(programId, "lights[0].color");
	int maxStringSize = 512;
	for (auto& it : varList)
	{
		std::string name = it.first;
		size_t idx = name.find("[");
		if (idx != -1)
		{
			for (int i = 1; i < Light::N_LIGHTS; i++)
			{
				std::string varName = name.replace(idx + 1, 1, std::to_string(i));

				auto id = glGetUniformLocation(programId, varName.c_str());
				if (id == -1)
				{
					std::cout << "???" << std::endl;
				}
				varList[varName] = id;
				int a = 1;
			}
		}
	}

	return;
}

void GLProgram::use()
{
	glUseProgram(programId);
}

unsigned int GLProgram::getVarLocation(std::string varName)
{
	if (varList.find(varName) != varList.end())
		return varList[varName];
	else
	{
		std::cout << __FILE__ << ": " << __LINE__ << ": " << "ERROR: variable " << varName << " no encontrada en shader\n";
		return -1;
	}
}

void GLProgram::checkLinkerErrors()
{
	GLint isLinked = 0;
	glGetProgramiv(this->programId, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(programId, maxLength, &maxLength, &infoLog[0]);

		std::string message(infoLog.data());

		std::cout << __FILE__ << ": " << __LINE__ << ": " << "ERROR linking " << "\n" << message << "\n\n";
	}
}

void GLProgram::setMVP(glm::mat4 m)
{
	//buscar location	
	std::string key = "MVP";
	auto location = varList.find(key);
	if (location != varList.end()) { //Ha encontrado la variable
		glUniformMatrix4fv(location->second, 1, GL_FALSE, &m[0][0]);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " << "ERROR, variable " << key << " no existe\n";
	}
}

void GLProgram::setVertexPos(GLsizei stride, void* offset, GLint count, GLenum type)
{
	//buscar location	
	std::string key = "vPos";
	auto location = varList.find(key);
	if (location != varList.end()) { //Ha encontrado la variable
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " << "ERROR, variable " << key << " no existe\n";
	}
}

void GLProgram::setVertexColor(GLsizei stride, void* offset, GLint count, GLenum type)
{
	//buscar location	
	std::string key = "vColor";
	auto location = varList.find(key);
	if (location != varList.end()) { //Ha encontrado la variable
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " << "ERROR, variable " << key << " no existe\n";
	}
}

void GLProgram::setVertexNormal(GLsizei stride, void* offset, GLint count, GLenum type)
{
	//buscar location	
	std::string key = "vNormal";
	auto location = varList.find(key);
	if (location != varList.end()) { //Ha encontrado la variable
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " << "ERROR, variable " << key << " no existe\n";
	}
}

void GLProgram::setVertexCoordText(GLsizei stride, void* offset, GLint count, GLenum type)
{
	//buscar location	
	std::string key = "vCoordTex";
	auto location = varList.find(key);
	if (location != varList.end()) { //Ha encontrado la variable
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";
	}
}

void GLProgram::setVertexTangent(GLsizei stride, void* offset, GLint count, GLenum type)
{
	//buscar location	
	std::string key = "vTangent";
	auto location = varList.find(key);
	if (location != varList.end()) { //Ha encontrado la variable
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";
	}
}

void GLProgram::setVertexBonesIdx(GLsizei stride, void* offset, GLint count, GLenum type)
{
	std::string key = "vboneIndices";
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else 
	{
		std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";
	}
}

void GLProgram::setVertexBonesWeights(GLsizei stride, void* offset, GLint count, GLenum type)
{
	std::string key = "vboneWeights";
	auto location = varList.find(key);
	if (location != varList.end())
	{
		glEnableVertexAttribArray(location->second);
		glVertexAttribPointer(location->second, count, type, GL_FALSE, stride, offset);
	}
	else 
	{
		std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key << " no existe\n";
	}
}

void GLProgram::setInt(std::string name, int val)
{
	unsigned int location = getVarLocation(name);

	if (location != -1) { //Ha encontrado la variable
		glUniform1i(location, val);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " << "ERROR, variable " << name << " no existe\n";
	}
}

void GLProgram::setFloat(std::string name, float val)
{
	unsigned int location = getVarLocation(name);

	if (location != -1) { //Ha encontrado la variable
		glUniform1f(location, val);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " "ERROR, variable " << name << " no existe\n";
	}
}

void GLProgram::setVec3(std::string name, const glm::vec3& vec)
{
	unsigned int location = getVarLocation(name);

	if (location != -1) { //Ha encontrado la variable
		glUniform3fv(location, 1, &vec[0]);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " "ERROR, variable " << name << " no existe\n";
	}
}

void GLProgram::setVec4(std::string name, const glm::vec4& vec)
{
	unsigned int location = getVarLocation(name);

	if (location != -1) { //Ha encontrado la variable
		glUniform4fv(location, 1, &vec[0]);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " "ERROR, variable " << name << " no existe\n";
	}
}

void GLProgram::setMatrix(std::string name, const glm::mat4& matrix)
{
	unsigned int location = getVarLocation(name);

	if (location != -1) { //Ha encontrado la variable
		glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " "ERROR, variable " << name << " no existe\n";
	}
}

/* WITH OFFSET */

void GLProgram::setInt(std::string name, int val, int offset, int stride)
{
	unsigned int location = getVarLocation(name);

	if (location != -1) { //Ha encontrado la variable
		glUniform1i(location + offset * stride, val);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " << "ERROR, variable " << name << " no existe\n";
	}
}

void GLProgram::setFloat(std::string name, float val, int offset, int stride)
{
	unsigned int location = getVarLocation(name);

	if (location != -1) { //Ha encontrado la variable
		glUniform1f(location + offset * stride, val);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " "ERROR, variable " << name << " no existe\n";
	}
}

void GLProgram::setVec3(std::string name, const glm::vec3& vec, int offset, int stride)
{
	unsigned int location = getVarLocation(name);

	if (location != -1) { //Ha encontrado la variable
		glUniform3fv(location + offset * stride, 1, &vec[0]);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " "ERROR, variable " << name << " no existe\n";
	}
}

void GLProgram::setVec4(std::string name, const glm::vec4& vec, int offset, int stride)
{
	unsigned int location = getVarLocation(name);

	if (location != -1) { //Ha encontrado la variable
		glUniform4fv(location + offset * stride, 1, &vec[0]);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " "ERROR, variable " << name << " no existe\n";
	}
}

void GLProgram::setMatrix(std::string name, const glm::mat4& matrix, int offset, int stride)
{
	unsigned int location = getVarLocation(name);

	if (location != -1) { //Ha encontrado la variable
		glUniformMatrix4fv(location + offset * stride, 1, GL_FALSE, &matrix[0][0]);
	}
	else { //No la ha encontrado
		std::cout << __FILE__ << ": " << __LINE__ << ": " "ERROR, variable " << name << " no existe\n";
	}
}

void GLProgram::setColorTexEnable()
{
	setInt("mat.usetextureColor", 1);
}

void GLProgram::setColorTexDisable()
{
	setInt("mat.usetextureColor", 0);
}

void GLProgram::setLight(Light* l)
{
	setVec3("light.pos", l->getPos());
	setVec3("light.rot", l->getDir());
	setVec4("light.color", l->getColor());
	setInt("light.enable", 1);
}

void GLProgram::setLight(Light* l, int idx)
{
	int stride = Light::LIGHT_STRIDE;
	setVec3("lights[0].pos", l->getPos(), idx, stride);
	setVec3("lights[0].dir", l->getDir(), idx, stride);
	setVec4("lights[0].color", l->getColor(), idx, stride);
	setInt("lights[0].enable", 1, idx, stride);
	setInt("lights[0].type", l->getType(), idx, stride);
	setFloat("lights[0].attenuation", l->getLinearAttenuation(), idx, stride);
	setFloat("lights[0].cutOff", glm::cos(l->getCutOffAngle()), idx, stride);
}

void GLProgram::bindTextureSampler(int binding, Texture* tex)
{
	glActiveTexture(GL_TEXTURE0 + binding);

	if (tex->isCube())
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, ((GLTexture*)tex)->getTexId());
	}
	else if (tex->is3D())
	{
		glBindTexture(GL_TEXTURE_3D, ((GLTexture*)tex)->getTexId());
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, ((GLTexture*)tex)->getTexId());
	}

}

void GLProgram::setColorTexSampler(int idx, int use)
{
	//buscar location
	std::string key1 = "textureColor";
	std::string key2 = "mat.usetextureDepth";

	auto location1 = varList.find(key1);
	auto location2 = varList.find(key2);

	if (location1 != varList.end() && location2 != varList.end())
	{
		glUniform1i(location1->second, idx);
		glUniform1i(location2->second, use);
	}
	else {
		std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key1 << " no existe\n";
		std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key2 << " no existe\n";

	}
}

void GLProgram::setColorCubicSampler(int idx, int use)
{
	//buscar location
	std::string key1 = "cubetextureColor";
	std::string key2 = "mat.useCubeTex";

	auto location1 = varList.find(key1);
	auto location2 = varList.find(key2);

	if (location1 != varList.end() && location2 != varList.end())
	{
		glUniform1i(location1->second, idx);
		glUniform1i(location2->second, use);
	}
	else
	{
		std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key1 << " no existe\n";
		std::cout << __FILE__ << ":" << __LINE__ << "ERROR, variable " << key2 << " no existe\n";
	}
}
