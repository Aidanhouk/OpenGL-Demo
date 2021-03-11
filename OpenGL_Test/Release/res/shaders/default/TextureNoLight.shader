#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

layout(std140) uniform Matrices
{
	mat4 u_Projection;
	mat4 u_View;
};
uniform mat4 u_Model;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, TexCoords);
}