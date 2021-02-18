#include "IndexBuffer.h"

#include "renderer/Renderer.h"

IndexBuffer::IndexBuffer(const unsigned int *data, unsigned int count)
	: m_Count{ count }
{
	//ASSERT(sizeof(unsigned int) == sizeof(GLuint));

	GLCall(glGenBuffers(1, &m_RenererID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenererID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_RenererID));
}

void IndexBuffer::bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenererID));
}

void IndexBuffer::unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}