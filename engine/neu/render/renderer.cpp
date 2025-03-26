#include "renderer.h"

#include "core.h"

#include "scene/scene.h"
#include "scene/game_object.h"

#include "utility/profiler.h"
#include "application.h"
#include "shader_manager.h"

#include "components/mesh.h"
#include "components/transform.h"

using namespace Neu;

void Renderer::renderScene(Scene* scene)
{
	NEU_PROFILE_FUNCTION;

	//PROFILE_START;
	Vector3f backgroundColor = scene->getBackgroundColor();

	//glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderManager* shaderManager = Application::getInstance()->getShaderManager();

	for (GameObject* gameObject : scene->getGameObjects())
	{
		if (gameObject->hasComponent<TransformComponent>() 
			&& gameObject->hasComponent<MeshComponent>())
		{
			auto& transform = gameObject->getComponent<TransformComponent>();
			auto& mesh = gameObject->getComponent<MeshComponent>();

			// TODO: DEFINE WHICH SHADER TO USE
			Shader* shader = shaderManager->getShader("simple");

			shader->use();
			shader->setMatrix4x4f("model_matrix", transform.transform);
			shader->setVector3f("color", gameObject->color);

			glBindVertexArray(mesh.getVAO());
			glDrawElements(GL_TRIANGLES, (GLsizei)(mesh.getFaceCount() * 3), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

		}
	}

	// RENDER PHYSICS (MOVE DEBUG RENDERER HERE)

	//PROFILE_STOP;
}

void Renderer::toggleWireframe()
{
	if (wireframeMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	wireframeMode = !wireframeMode;
}

void Renderer::drawLine(Vector3f start, Vector3f end, Vector4f color)
{
	//Logger::debug("RENDERING LINE FROM (%f, %f, %f) TO (%f, %f, %f)\n", start.x, start.y, start.z, end.x, end.y, end.z);

	unsigned int VAO;
	unsigned int VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	float vertices[] = {
		start.x, start.y, start.z,
		end.x, end.y, end.z
	};

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Shader* lineShader = Application::getInstance()->getShaderManager()->getShader("line");

	lineShader->use();
	lineShader->setVector3f("color", color);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	// CLEAR RESOURCES
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Renderer::drawTriangle(Vector3f v1, Vector3f v2, Vector3f v3, Vector4f color)
{
	//Logger::debug("RENDERING TRIANGLE\n");

	unsigned int VAO;
	unsigned int VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	float vertices[] = { 
		v1.x, v1.y, v1.z,
		v2.x, v2.y, v2.z,
		v3.x, v3.y, v3.z
	};

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Shader* lineShader = Application::getInstance()->getShaderManager()->getShader("line");

	lineShader->use();
	lineShader->setVector3f("color", color);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	// CLEAR RESOURCES
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}