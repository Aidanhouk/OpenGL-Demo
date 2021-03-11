#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} vs_out;

layout(std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
	vs_out.FragPos = vec3(model * vec4(aPosition, 1.0));
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
	vs_out.TexCoords = aTexCoords;
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}

#shader fragment
#version 330 core

out vec4 out_FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} fs_in;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material u_Material;

uniform sampler2D shadowMap;

uniform vec3 viewPos;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	//int isOn;
};
uniform DirLight u_DirLight;
vec3 calcDirLight(vec3 normal, vec3 viewDir);

float shadowCalculation();

void main()
{
	vec3 normal = normalize(fs_in.Normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 output = vec3(0.0);

	//if (u_DirLight.isOn > 0)
	output += calcDirLight(normal, viewDir);
	
	output = pow(output, vec3(1.0 / 2.2));
	out_FragColor = vec4(output, 1.0);
}

vec3 calcDirLight(vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(u_DirLight.direction);

	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);

	// specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess);

	float shadow = shadowCalculation();
	vec3 ambient = u_DirLight.ambient * vec3(texture(u_Material.diffuse, fs_in.TexCoords));
	vec3 diffuse = u_DirLight.diffuse * diff * vec3(texture(u_Material.diffuse, fs_in.TexCoords));
	vec3 specular = u_DirLight.specular * spec * vec3(texture(u_Material.specular, fs_in.TexCoords));

	//return ambient + (diffuse + specular) * (1.0 - shadow);
	//return ambient + diffuse + specular * (1.0 - shadow);
	return ambient + diffuse + specular;
	//return vec3(1.0) * (1.0 - shadow);
}

float shadowCalculation()
{
	// ¬ыполн€ем деление перспективы
	vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
	// ѕреобразуем в диапазон [0,1]
	projCoords = projCoords * 0.5 + 0.5;
	// ѕолучаем наиболее близкое значение глубины исход€ из перспективы глазами источника света (использу€ диапазон [0,1] fragPosLight в качестве координат)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// ѕолучаем глубину текущего фрагмента исход€ из перспективы глазами источника света
	float currentDepth = projCoords.z;

	// ¬ычисл€ем смещение (на основе разрешени€ карты глубины и наклона)
	vec3 normal = normalize(fs_in.Normal);
	float bias = max(0.05 * (1.0 - dot(normal, -u_DirLight.direction)), 0.005);
	
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	
	if (projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}