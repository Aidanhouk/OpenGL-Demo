#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out VS_OUT{
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform vec3 lightPos;
uniform vec3 viewPos;

layout(std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
	vec3 fragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.TexCoords = aTexCoords;	
	vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));

	vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentViewPos = TBN * viewPos;
	vs_out.TangentFragPos = TBN * fragPos;

	gl_Position = projection * view * vec4(fragPos, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

in VS_OUT{
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} fs_in;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D depth;
	float shininess;
};
uniform Material u_Material;

uniform float height_scale;
vec2 parallaxMapping(vec2 texCoords, vec3 viewDir);

uniform sampler2D shadowMap;
float shadowCalculation(vec3 normal);

void main()
{
	vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	vec2 texCoords;
	// �������� ���������� ��������� ��� ����������� ����������
	if (height_scale > 0) {
		texCoords = parallaxMapping(fs_in.TexCoords, viewDir);
		//if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) discard;
	}
	else
		texCoords = fs_in.TexCoords;

	vec3 diffuseMap = texture(u_Material.diffuse, texCoords).rgb;
	vec3 specularMap = texture(u_Material.specular, texCoords).rgb;
	vec3 normalMap = texture(u_Material.normal, texCoords).rgb;
	normalMap = normalize(normalMap * 2.0 - 1.0);

	// ������� ������������
	vec3 ambient = 0.05 * diffuseMap;

	// ��������� ������������
	float diff = max(dot(lightDir, normalMap), 0.0);
	vec3 diffuse = diff * diffuseMap;

	// ���������� ������������
	float spec = pow(max(dot(normalMap, halfwayDir), 0.0), u_Material.shininess);
	vec3 specular = spec * specularMap;

	// ��������� ����
	float shadow = shadowCalculation(normalMap);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

	FragColor = vec4(lighting, 1.0);
}

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir)
{
	// ���������� ����� �������
	const float minLayers = 8.0;
	const float maxLayers = 32.0;
	float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));

	// ��������� ������ ������� ����
	float layerDepth = 1.0 / numLayers;

	// ������� �������� ����
	float currentLayerDepth = 0.0;

	// �������� ������ ���������� ��������� ��� ������� ���� (�� ������� P)
	vec2 P = viewDir.xy * height_scale;
	vec2 deltaTexCoords = P / numLayers;

	// �������� ��������� ��������
	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = texture(u_Material.depth, currentTexCoords).r;

	while (currentLayerDepth < currentDepthMapValue)
	{
		// �������� ���������� ���������� ����� ����������� ������� P
		currentTexCoords -= deltaTexCoords;

		// �������� �������� ������� �� ����� ������� ��� ������� ���������� ���������
		currentDepthMapValue = texture(u_Material.depth, currentTexCoords).r;

		// �������� �������� ������� ���������� ����
		currentLayerDepth += layerDepth;
	}

	// �������� ���������� ���������� �� �����������
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// �������� �������� ������� �� � ����� ����������� (��� �������� ������������)
	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(u_Material.depth, prevTexCoords).r - currentLayerDepth + layerDepth;

	// ������������ ���������� ���������
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return currentTexCoords;
}

float shadowCalculation(vec3 normal)
{
	// ��������� ������� �����������
	vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
	// ����������� � �������� [0,1]
	projCoords = projCoords * 0.5 + 0.5;
	// �������� �������� ������� �������� ������� ������ �� ����������� ������� ��������� ����� (��������� �������� [0,1] fragPosLight � �������� ���������)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// �������� ������� �������� ��������� ������ �� ����������� ������� ��������� �����
	float currentDepth = projCoords.z;

	// ��������� �������� (�� ������ ���������� ����� ������� � �������)
	vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
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