#include "GL4Render.h"
#include "System.h"

void GL4Render::drawGL(Object* obj)
{
	for (auto& mesh : obj->getMeshes())
	{
		std::vector <vertex_t>* triangleVertex = mesh->getVertList();
		std::vector <unsigned int>* triangleIdList = mesh->getTriangleIdxList();
		obj->computeModelMatrix();
		System::setModelMatrix(obj->getModelMatrix());
		//std::cout << obj->getModelMatrix()[0][0] << std::endl;
		System::setActiveObject(obj);

		auto buffer = bufferObjectList[mesh->getMeshID()];
		//activar
		glBindBuffer(GL_ARRAY_BUFFER, buffer.vertexBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.indexBufferId);

		//usar
		mesh->getMaterial()->prepare();

		glDrawElements(GL_TRIANGLES, (GLsizei)mesh->getTriangleIdxList()->size(), GL_UNSIGNED_INT, nullptr);
	}
}

void GL4Render::setupObject(Object* obj)
{
	for (auto& mesh : obj->getMeshes())
	{
		if (bufferObjectList.count(mesh->getMeshID()))
		{
			//std::cout << "buffer created, we skip\n";
			continue;
		}

		bufferObject_t buffer{};

		//reservar identificadores
		glGenVertexArrays(1, &buffer.bufferId);
		glGenBuffers(1, &buffer.vertexBufferId);
		glGenBuffers(1, &buffer.indexBufferId);

		//copiar datos de vertices
		glBindBuffer(GL_ARRAY_BUFFER, buffer.vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * mesh->getVertList()->size(),
			mesh->getVertList()->data(), GL_STATIC_DRAW);

		//copiar datos de identificadores
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.indexBufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->getTriangleIdxList()->size(),
			mesh->getTriangleIdxList()->data(), GL_STATIC_DRAW);

		bufferObjectList[mesh->getMeshID()] = buffer;
	}
}

void GL4Render::updateObject(Object* obj)
{
	for (auto& mesh : obj->getMeshes())
	{
		bufferObject_t buffer = bufferObjectList[mesh->getMeshID()];

		//copiar datos de vertices
		glBindBuffer(GL_ARRAY_BUFFER, buffer.vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * mesh->getVertList()->size(),
			mesh->getVertList()->data(), GL_STATIC_DRAW);

		//copiar datos de identificadores
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.indexBufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->getTriangleIdxList()->size(),
			mesh->getTriangleIdxList()->data(), GL_STATIC_DRAW);
	}
}

void GL4Render::removeObject(Object* obj)
{
	for (auto& mesh : obj->getMeshes())
	{
		std::string meshName = mesh->getName();

		MSH_t* msh_t = MeshManager::getMSH(meshName);

		if (msh_t != nullptr)
		{
			msh_t->numberOfUses--;
			if (msh_t->numberOfUses > 0)
				continue;
		}

		bufferObject_t buffer = bufferObjectList[mesh->getMeshID()];
		glDeleteVertexArrays(1, &buffer.bufferId);
		glDeleteBuffers(1, &buffer.vertexBufferId);
		glDeleteBuffers(1, &buffer.indexBufferId);
	}
}
