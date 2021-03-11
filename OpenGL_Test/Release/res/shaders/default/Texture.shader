#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(std140) uniform Matrices
{
	mat4 u_Projection;
	mat4 u_View;
};
uniform mat4 u_Model;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoords;

void main()
{
	v_FragPos = vec3(u_Model * vec4(aPosition, 1.0));
	v_Normal = mat3(transpose(inverse(u_Model))) * aNormal;
	v_TexCoords = aTexCoords;
	gl_Position = u_Projection * u_View * u_Model * vec4(aPosition, 1.0);
}

#shader fragment
#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material u_Material;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoords;

uniform vec3 u_ViewPos;

out vec4 out_FragColor;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	int isOn;
};
uniform DirLight u_DirLight;
vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir);

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	int isOn;
};
#define POINT_LIGHTS_COUNT 2
uniform PointLight u_PointLights[POINT_LIGHTS_COUNT];
vec3 calcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir);

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	int isOn;
};
uniform SpotLight u_SpotLight;
vec3 calcSpotLight(SpotLight spotLight, vec3 normal, vec3 v_FragPos, vec3 viewDir);

void main()
{
	vec3 normal = normalize(v_Normal);
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	vec3 output = vec3(0.0);

	if (u_DirLight.isOn == 1)
		output = calcDirLight(u_DirLight, normal, viewDir);

	for (int i = 0; i < POINT_LIGHTS_COUNT; ++i)
		if (u_PointLights[i].isOn == 1)
			output += calcPointLight(u_PointLights[i], normal, v_FragPos, viewDir);

	if (u_SpotLight.isOn == 1)
		output += calcSpotLight(u_SpotLight, normal, v_FragPos, viewDir);

	output = pow(output, vec3(1.0 / 2.2));
	out_FragColor = vec4(output, 1.0);
}

vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-dirLight.direction);

	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);

	// specular
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess);

	vec3 ambient = dirLight.ambient * vec3(texture(u_Material.diffuse, v_TexCoords));
	vec3 diffuse = dirLight.diffuse * diff * vec3(texture(u_Material.diffuse, v_TexCoords));
	vec3 specular = dirLight.specular * spec * vec3(texture(u_Material.specular, v_TexCoords));
	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(pointLight.position - fragPos);

	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);

	// specular
	//vec3 reflectDir = reflect(-lightDir, -normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess);

	// attenuation
	float distance = length(pointLight.position - fragPos) * 0.1;
	//float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));
	float attenuation = 1.0 / pow((pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance)), 2);

	vec3 ambient = pointLight.ambient * vec3(texture(u_Material.diffuse, v_TexCoords));
	vec3 diffuse = pointLight.diffuse * diff * vec3(texture(u_Material.diffuse, v_TexCoords));
	vec3 specular = pointLight.specular * spec * vec3(texture(u_Material.specular, v_TexCoords));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(spotLight.position - fragPos);

	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);

	// specular
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess);

	// attenuation
	float distance = length(spotLight.position - fragPos) * 0.1;
	//float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));
	float attenuation = 1.0 / pow((spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance)), 2);

	// intensity
	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = spotLight.cutOff - spotLight.outerCutOff;
	float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = spotLight.ambient * texture(u_Material.diffuse, v_TexCoords).rgb;
	vec3 diffuse = spotLight.diffuse * diff * texture(u_Material.diffuse, v_TexCoords).rgb;
	vec3 specular = spotLight.specular * spec * texture(u_Material.specular, v_TexCoords).rgb;
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return (ambient + diffuse + specular);
}