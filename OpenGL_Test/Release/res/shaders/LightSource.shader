#shader vertex
#version 330 core

layout(location = 0) in vec4 aPosition;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * aPosition;
}

#shader fragment
#version 330 core

out vec4 out_Color;

uniform vec3 u_LightColor;

void main()
{
	out_Color = vec4(u_LightColor, 1.0);
}