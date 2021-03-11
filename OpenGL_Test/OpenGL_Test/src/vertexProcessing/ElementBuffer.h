#pragma once

class ElementBuffer
{
private:
	unsigned int m_RenererID;
	unsigned int m_Count;
public:
	ElementBuffer(const unsigned int *data, unsigned int count);
	~ElementBuffer();

	void bind() const;
	void unbind() const;

	inline unsigned int getCount() const { return m_Count; }
};