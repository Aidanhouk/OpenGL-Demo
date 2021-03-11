#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
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
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
	vs_out.TexCoords = aTexCoords;
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material u_Material;

float ShadowCalculation();

void main()
{
	vec3 color = texture(u_Material.diffuse, fs_in.TexCoords).rgb;
	vec3 colorSpecular = texture(u_Material.specular, fs_in.TexCoords).rgb;
	vec3 normal = normalize(fs_in.Normal);
	vec3 lightColor = vec3(1.0);

	// Фоновая составляющая
	vec3 ambient = 0.05 * color;

	// Диффузная составляющая
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;

	// Отраженная составляющая
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess);
	vec3 specular = spec * lightColor;

	// Вычисляем тень
	float shadow = ShadowCalculation();
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse * color + specular * colorSpecular));

	lighting = pow(lighting, vec3(1.0 / 2.2));

	FragColor = vec4(lighting, 1.0);
}

float ShadowCalculation()
{
	// Выполняем деление перспективы
	vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
	// Преобразуем в диапазон [0,1]
	projCoords = projCoords * 0.5 + 0.5;
	// Получаем наиболее близкое значение глубины исходя из перспективы глазами источника света (используя диапазон [0,1] fragPosLight в качестве координат)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// Получаем глубину текущего фрагмента исходя из перспективы глазами источника света
	float currentDepth = projCoords.z;

	// Вычисляем смещение (на основе разрешения карты глубины и наклона)
	vec3 normal = normalize(fs_in.Normal);
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

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