#include "Renderer.h"

#include <iostream>

#include "vertexProcessing/VertexArray.h"
#include "vertexProcessing/IndexBuffer.h"
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

void Renderer::draw(const VertexArray & vertArr, const IndexBuffer & indBuff) const
{
	vertArr.bind();
	indBuff.bind();
	GLCall(glDrawElements(GL_TRIANGLES, indBuff.getCount(), GL_UNSIGNED_INT, nullptr));
	vertArr.unbind();
}

void Renderer::clear() const
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}