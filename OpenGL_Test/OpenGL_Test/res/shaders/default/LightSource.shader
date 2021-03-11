#shader vertex
#version 330 core

layout(location = 0) in vec4 aPosition;

layout(std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};
uniform mat4 u_Model;

void main()
{
	gl_Position = projection * view * u_Model * aPosition;
}

#shader fragment
#version 330 core

out vec4 out_Color;

uniform vec3 u_LightColor;

void main()
{
	out_Color = vec4(u_LightColor, 1.0);
}