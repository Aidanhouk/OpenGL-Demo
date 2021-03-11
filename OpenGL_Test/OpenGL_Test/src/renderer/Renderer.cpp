#include "Renderer.h"

#include <iostream>

#include "vertexProcessing/VertexArray.h"
#include "vertexProcessing/ElementBuffer.h"
#include "shader/Shader.h"

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char *function, const char *file, int line)
{
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << error << "): " <<
			function << " " << file << " : LINE " << line << std::endl;
		return 0;
	}
	return 1;
}

void Renderer::draw(const VertexArray & VAO, const ElementBuffer & EBO) const
{
	VAO.bind();
	EBO.bind();
	GLCall(glDrawElements(GL_TRIANGLES, EBO.getCount(), GL_UNSIGNED_INT, nullptr));
	VAO.unbind();
}

void Renderer::draw(const VertexArray & VAO, int count) const
{
	VAO.bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, count));
	VAO.unbind();
}

void Renderer::clear() const
{
	//GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	GLCall(glClearColor(0.4f, 0.8f, 1.0f, 1.0f));
}