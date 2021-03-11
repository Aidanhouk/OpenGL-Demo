#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

layout(std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};
uniform mat4 u_Model;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = projection * view * u_Model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_Texture1;

void main()
{
	// for grass
	//vec4 texColor = texture(u_Texture1, TexCoords);
	//if (texColor.a < 0.1)
	//	discard;
	//FragColor = texColor;
	FragColor = texture(u_Texture1, TexCoords);
}