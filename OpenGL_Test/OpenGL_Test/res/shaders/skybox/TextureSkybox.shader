#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 v_Normal;
out vec3 v_Position;

layout(std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};
uniform mat4 u_Model;

void main()
{
	v_Normal = mat3(transpose(inverse(u_Model))) * aNormal;
	v_Position = vec3(u_Model * vec4(aPos, 1.0));
	gl_Position = projection * view * vec4(v_Position, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 v_Normal;
in vec3 v_Position;

uniform vec3 u_CameraPos;
uniform samplerCube u_Skybox;

void main()
{
	//vec3 I = normalize(v_Position - u_CameraPos);
	//vec3 R = reflect(I, normalize(v_Normal));
	//FragColor = vec4(texture(u_Skybox, R).rgb, 1.0);
	float ratio = 1.00 / 1.52;
	vec3 I = normalize(v_Position - u_CameraPos);
	vec3 R = refract(I, normalize(v_Normal), ratio);
	FragColor = vec4(texture(u_Skybox, R).rgb, 1.0);
}