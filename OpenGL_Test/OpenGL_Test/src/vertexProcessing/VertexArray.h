#pragma once

class VertexBuffer;
class VertexBufferLayout;

class VertexArray
{
private:
	unsigned int m_RendereID;
public:
	VertexArray();
	~VertexArray();

	void addBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout);

	void bind() const;
	void unbind() const;
};