#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(aPosition, 1.0);
}

#shader fragment
#version 330 core

out vec4 out_FragColor;

void main()
{
	out_FragColor = vec4(0.04, 0.28, 0.26, 1.0);
}