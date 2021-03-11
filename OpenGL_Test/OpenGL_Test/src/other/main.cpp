#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <map>

#include "renderer/Renderer.h"

#include "vertexProcessing/VertexBuffer.h"
#include "vertexProcessing/ElementBuffer.h"
#include "vertexProcessing/VertexArray.h"
#include "vertexProcessing/VertexBufferLayout.h"

#include "shader/Shader.h"
#include "camera/Camera.h"
#include "texture/Texture.h"
#include "model/Model.h"

#include "lighting/DirectionalLight.h"
#include "lighting/PointLight.h"
#include "lighting/PointLightsControl.h"
#include "lighting/SpotLight.h"

#include "stb_image/stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define IMGUI
#define CUBES
#define LIGHTING
#define SQUARE
#define ROOM
//#define TRANSPARENT
//#define PLANET
//#define SKYBOX
#define MODEL

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadCubemap(std::vector<std::string> &faces);

float windowWidth{ 1920.0f };
float windowHeight{ 1080.0f };

Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX{ windowWidth / 2 };
float lastY{ windowHeight / 2 };

SpotLight spotLight(&camera);

float deltaTime{ 0.0f };
float lastFrame{ 0.0f };

int main(void)
{
	GLFWwindow* window;
	
	/* Initialize the library */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(windowWidth, windowHeight, "Window", NULL, NULL);
	//window = glfwCreateWindow(windowWidth, windowHeight, "Window", glfwGetPrimaryMonitor(), NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	GLCall(glfwSetFramebufferSizeCallback(window, framebuffer_size_callback));
	GLCall(glfwSetCursorPosCallback(window, mouse_callback));
	GLCall(glfwSetScrollCallback(window, scroll_callback));
	GLCall(glfwSwapInterval(0));

#ifndef IMGUI
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	camera.stopOrResumeMouse();
#endif // !IMGUI

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	GLCall(glEnable(GL_DEPTH_TEST));

	GLCall(glEnable(GL_CULL_FACE));
	GLCall(glCullFace(GL_BACK));
	GLCall(glFrontFace(GL_CCW));

	GLCall(glEnable(GL_PROGRAM_POINT_SIZE));

	//GLCall(glEnable(GL_MULTISAMPLE));

	stbi_set_flip_vertically_on_load(1);

	//GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

	{
		Renderer renderer;

#ifdef CUBES
		unsigned int positions_countCube{ 4 * 6 };
		unsigned int attr_countCube{ 8 }; // coordinates (3) + normals (3) + texture coords (2) = 8
		float positionsCube[]{
			// front
			0.0f, 0.0f, 0.0f,	0.0f, 0.0f,	-1.0f,	0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,	0.0f, 0.0f,	-1.0f,	1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,	0.0f, 0.0f,	-1.0f,	1.0f, 1.0f,
			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	-1.0f,	0.0f, 1.0f,
			// back															
			0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
			1.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
			1.0f, 1.0f, 1.0f,	0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
			0.0f, 1.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
			// left															
			0.0f, 0.0f,	0.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
			0.0f, 1.0f,	0.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
			0.0f, 1.0f,	1.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
			0.0f, 0.0f,	1.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
			// right														
			1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
			1.0f, 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
			// bottom														
			0.0f, 0.0f, 0.0f,	0.0f, -1.0f, 0.0f,	0.0f, 1.0f,
			1.0f, 0.0f, 0.0f,	0.0f, -1.0f, 0.0f,	1.0f, 1.0f,
			1.0f, 0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	0.0f, 0.0f,
			// up															
			1.0f, 1.0f,	0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
			0.0f, 1.0f,	0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
			0.0f, 1.0f,	1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
			1.0f, 1.0f,	1.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
		};
		unsigned int indices_countCube{ 6 * 6 };
		unsigned int indicesCube[]{
			// front
			0, 3, 2,
			2, 1, 0,
			// back
			4, 5, 6,
			6, 7, 4,
			// left
			8, 11, 10,
			10, 9, 8,
			// right
			12, 13, 14,
			14, 15, 12,
			// bottom
			16, 17, 18,
			18, 19, 16,
			// up
			20, 21, 22,
			22, 23, 20,
		};

		VertexArray VAO_Cube;
		VertexBuffer VBO_Cube(positionsCube, positions_countCube * attr_countCube * sizeof(float));
		VertexBufferLayout layoutCube;
		layoutCube.push<float>(3);
		layoutCube.push<float>(3);
		layoutCube.push<float>(2);
		VAO_Cube.addBuffer(VBO_Cube, layoutCube);
		ElementBuffer EBO_Cube(indicesCube, indices_countCube);

		Texture textureCrate("res/textures/crate500.png", GL_CLAMP_TO_BORDER);
		Texture textureCrateSpecular("res/textures/crate500light.png", GL_CLAMP_TO_BORDER);
		unsigned int cubesCount{ 2 };
		glm::vec3 cubePositions[] = {
			glm::vec3(-4.0f, 1.5f, -2.0f),
			glm::vec3(2.0f, 7.0f, -1.0f),
		};
#endif // CUBES

#ifdef LIGHTING
		//Shader shaderLightSource("res/shaders/default/LightSource.shader");
		//unsigned int uniformBlockIndexLightSource = glGetUniformBlockIndex(shaderLightSource.getRedndererID(), "Matrices");
		//GLCall(glUniformBlockBinding(shaderLightSource.getRedndererID(), uniformBlockIndexLightSource, 0));

		DirectionalLight dirLight(glm::vec3(0.0f, 30.0f, 0.0001f));
		//PointLightsControl pointLights(&shaderLightSource, &renderer, &VAO_Cube, &EBO_Cube);
		//pointLights.addPointLight(glm::vec3(0.0f, 9.0f, 0.001f));
		//pointLights.addPointLight(glm::vec3(4.0f, 0.3f, -2.25f));
#endif // LIGHTING

#ifdef MODEL
		Model modelBackpack("res/models/backpack/backpack.obj");
		modelBackpack.setPosition(glm::vec3(0.0f, 4.0f, 0.0f));
#endif // MODEL

#ifdef SKYBOX
		std::vector<std::string> faces
		{
			"res/textures/skybox/right.png",
			"res/textures/skybox/left.png",
			"res/textures/skybox/top.png",
			"res/textures/skybox/bottom.png",
			"res/textures/skybox/front.png",
			"res/textures/skybox/back.png"
		};
		unsigned int cubemapTexture{ loadCubemap(faces) };
		float positionsSkybox[]{
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		VertexArray VAO_Skybox;
		VertexBuffer VBO_Skybox(positionsSkybox, 36 * 3 * sizeof(float));
		VertexBufferLayout layoutSkybox;
		layoutSkybox.push<float>(3);
		VAO_Skybox.addBuffer(VBO_Skybox, layoutSkybox);

		Shader shaderSkybox("res/shaders/skybox/Skybox.shader");
#endif // SKYBOX

#ifdef PLANET
		Shader shaderPlanet("res/shaders/planet/Planet.shader");
		Model modelPlanet("res/models/planet/planet.obj");
		unsigned int uniformBlockIndexPlanet = glGetUniformBlockIndex(shaderPlanet.getRedndererID(), "Matrices");
		GLCall(glUniformBlockBinding(shaderPlanet.getRedndererID(), uniformBlockIndexPlanet, 0));

		Shader shaderRock("res/shaders/planet/Rock.shader");
		Model modelRock("res/models/rock/rock.obj");
		unsigned int uniformBlockIndexRock = glGetUniformBlockIndex(shaderRock.getRedndererID(), "Matrices");
		GLCall(glUniformBlockBinding(shaderRock.getRedndererID(), uniformBlockIndexRock, 0));

		// generate a large list of semi-random model transformation matrices
		// ------------------------------------------------------------------
		unsigned int amount = 10000;
		glm::mat4* modelMatrices;
		modelMatrices = new glm::mat4[amount];
		srand(glfwGetTime()); // initialize random seed	
		float radius = 50.0f;
		float offset = 5.0f;
		for (unsigned int i = 0; i < amount; ++i) {
			glm::mat4 model = glm::mat4(1.0f);
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)amount * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement;
			model = glm::translate(model, glm::vec3(x, y, z));

			// 2. scale: Scale between 0.05 and 0.25f
			float scale = (rand() % 20) / 100.0f + 0.05;
			model = glm::scale(model, glm::vec3(scale));

			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (rand() % 360);
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			// 4. now add to list of matrices
			modelMatrices[i] = model;
		}

		// configure instanced array
		// -------------------------
		unsigned int buffer;
		GLCall(glGenBuffers(1, &buffer));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
		GLCall(glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW));

		// set transformation matrices as an instance vertex attribute (with divisor 1)
		// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
		// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
		// -----------------------------------------------------------------------------------------------------------------------------------
		for (unsigned int i = 0; i < modelRock.getMeshes().size(); ++i) {
			unsigned int VAO = modelRock.getMeshes()[i].getVAO();
			GLCall(glBindVertexArray(VAO));
			// set attribute pointers for matrix (4 times vec4)
			GLCall(glEnableVertexAttribArray(3));
			GLCall(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0));
			GLCall(glEnableVertexAttribArray(4));
			GLCall(glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))));
			GLCall(glEnableVertexAttribArray(5));
			GLCall(glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))));
			GLCall(glEnableVertexAttribArray(6));
			GLCall(glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))));

			GLCall(glVertexAttribDivisor(3, 1));
			GLCall(glVertexAttribDivisor(4, 1));
			GLCall(glVertexAttribDivisor(5, 1));
			GLCall(glVertexAttribDivisor(6, 1));

			GLCall(glBindVertexArray(0));
		}
#endif // PLANET

#ifdef SQUARE
		// Координаты
		glm::vec3 pos1(0.0f, 1.0f, 0.0f);
		glm::vec3 pos2(0.0f, 0.0f, 0.0f);
		glm::vec3 pos3(1.0f, 0.0f, 0.0f);
		glm::vec3 pos4(1.0f, 1.0f, 0.0f);

		//glm::vec3 pos1(0.0f, 0.0f, 0.0f);
		//glm::vec3 pos2(1.0f, 0.0f, 0.0f);
		//glm::vec3 pos3(1.0f, 1.0f, 0.0f);
		//glm::vec3 pos4(0.0f, 1.0f, 0.0f);

		// Текстурные координаты
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);

		//glm::vec2 uv1(0.0f, 0.0f);
		//glm::vec2 uv2(1.0f, 0.0f);
		//glm::vec2 uv3(1.0f, 1.0f);
		//glm::vec2 uv4(0.0f, 1.0f);

		// Вектор нормали
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		// Вычисляем касательные/бикасательные векторы обоих треугольников
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;

		// Треугольник №1
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		// Треугольник №2
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		unsigned int positions_countSquare{ 6 };
		// coordinates (3) + normals (3) + texture coords (2) + tangents (3) + bitangent (3) = 14
		unsigned int attr_countSquare{ 14 };
		float positionsSquare[] = {
			// координаты           // нормали        // текст. координаты      // касательные          // бикасательные
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		//float positionsSquare[]{
		//	0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
		//	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
		//	1.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
		//	0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f
		//};
		unsigned int indices_countSquare{ 6 };
		unsigned int indicesSquare[]{
			0, 1, 2,
			2, 3, 0
		};

		VertexArray VAO_Square;
		VertexBuffer VBO_Square(positionsSquare, positions_countSquare * attr_countSquare * sizeof(float));
		VertexBufferLayout layoutSquare;
		layoutSquare.push<float>(3);
		layoutSquare.push<float>(3);
		layoutSquare.push<float>(2);
		layoutSquare.push<float>(3);
		layoutSquare.push<float>(3);
		VAO_Square.addBuffer(VBO_Square, layoutSquare);
		//ElementBuffer EBO_Square(indicesSquare, indices_countSquare);

#ifdef ROOM
		//Texture textureFloor("res/textures/room/floor.jpg", GL_CLAMP_TO_EDGE);

		stbi_set_flip_vertically_on_load(0);
		//Texture textureWall("res/textures/room/wall/brickwall.jpg", GL_CLAMP_TO_EDGE);
		//Texture textureWallNormal("res/textures/room/wall/brickwall_normal.jpg", GL_CLAMP_TO_EDGE);
		Texture textureWall("res/textures/room/wall2/bricks2.jpg", GL_CLAMP_TO_EDGE);
		Texture textureWallNormal("res/textures/room/wall2/bricks2_normal.jpg", GL_CLAMP_TO_EDGE);
		Texture textureWallDepth("res/textures/room/wall2/bricks2_disp.jpg", GL_CLAMP_TO_EDGE);
		//Texture textureWall("res/textures/room/toy/wood.png", GL_CLAMP_TO_EDGE);
		//Texture textureWallNormal("res/textures/room/toy/toy_box_normal.png", GL_CLAMP_TO_EDGE);
		//Texture textureWallDepth("res/textures/room/toy/toy_box_disp.png", GL_CLAMP_TO_EDGE);
		stbi_set_flip_vertically_on_load(1);

		//Texture textureCeil("res/textures/room/ceil.jpg", GL_CLAMP_TO_EDGE);
		glm::vec3 groundPos(-5.0f, -0.01f, 5.0f);
#endif // ROOM

#endif // SQUARE

#ifdef TRANSPARENT
		Shader shaderTransparentText("res/shaders/transparent/TransparentTexture.shader");
		unsigned int uniformBlockIndexTranspTexture = glGetUniformBlockIndex(shaderTransparentText.getRedndererID(), "Matrices");
		GLCall(glUniformBlockBinding(shaderTransparentText.getRedndererID(), uniformBlockIndexTranspTexture, 0));

		// glass
		Texture textureGlass("res/textures/glass.png", GL_CLAMP_TO_EDGE);

		std::vector<glm::vec3> glasses;
		glasses.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
		glasses.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
		glasses.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
		glasses.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
		glasses.push_back(glm::vec3(0.5f, 0.0f, -0.6f));
#endif // TRANSPARENT



		Shader shaderDirectionalShadowsDepth("res/shaders/shadows/directionalLight/DirectionalShadowsDepth.shader");

		Shader shaderDefault("res/shaders/default/Default.shader");
		unsigned int uniformBlockIndexDefault = glGetUniformBlockIndex(shaderDefault.getRedndererID(), "Matrices");
		GLCall(glUniformBlockBinding(shaderDefault.getRedndererID(), uniformBlockIndexDefault, 0));

		Shader shaderDefaultNormals("res/shaders/default/DefaultNormalsParallax.shader");
		unsigned int uniformBlockIndexDefaultNormals = glGetUniformBlockIndex(shaderDefaultNormals.getRedndererID(), "Matrices");
		GLCall(glUniformBlockBinding(shaderDefaultNormals.getRedndererID(), uniformBlockIndexDefaultNormals, 0));




		// depthmap for shadows
		unsigned int depthMapFBO;
		GLCall(glGenFramebuffers(1, &depthMapFBO));

		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

		unsigned int depthMap;
		GLCall(glGenTextures(1, &depthMap));
		GLCall(glBindTexture(GL_TEXTURE_2D, depthMap));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO));
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0));
		GLCall(glDrawBuffer(GL_NONE));
		GLCall(glReadBuffer(GL_NONE));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));



		// shaders configuration
		shaderDefaultNormals.bind();
		shaderDefaultNormals.setUniform1i("shadowMap", 0);
		shaderDefaultNormals.setUniform1i("u_Material.diffuse", 1);
		shaderDefaultNormals.setUniform1i("u_Material.normal", 3);
		shaderDefaultNormals.setUniform1i("u_Material.depth", 4);
		shaderDefaultNormals.setUniform1f("u_Material.shininess", 128.0f);
		shaderDefault.bind();
		shaderDefault.setUniform1i("shadowMap", 0);
		shaderDefault.setUniform1i("u_Material.diffuse", 1);
		shaderDefault.setUniform1i("u_Material.specular", 2);
		shaderDefault.setUniform1f("u_Material.shininess", 128.0f);



		// configure a uniform buffer object
		unsigned int uboMatrices;
		GLCall(glGenBuffers(1, &uboMatrices));
		GLCall(glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices));
		GLCall(glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW));
		GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
		// define the range of the buffer that links to a uniform binding point
		GLCall(glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4)));

#ifdef IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
#endif // IMGUI



		// render loop
		while (!glfwWindowShouldClose(window))
		{
			processInput(window);

			renderer.clear();
			//GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			//GLCall(glClearColor(0.4f, 0.8f, 1.0f, 1.0f));

#ifdef IMGUI
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
#endif // IMGUI

			// per-frame time logic
			float currentFrame{ (float)glfwGetTime() };
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;



			// configure transformation matrices
			glm::mat4 projMat{ glm::perspective(glm::radians(camera.getFOV()), (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f) };
			glm::mat4 viewMat{ camera.getViewMatrix() };

			// bind view and projection matrices to uniform blocks
			{
				GLCall(glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices));

				GLCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewMat)));
				GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projMat)));

				GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
			}

			// 1. Рендеринг глубины сцены в текстуру (вид - с позиции источника света)
			glm::mat4 lightProjection, lightView;
			glm::mat4 lightSpaceMatrix;
			glm::vec3 sunightDirection{ dirLight.getDirection() };
			float near_plane = 0.0f, far_plane = 100.0f;
			lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
			lightView = glm::lookAt(sunightDirection, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
			lightSpaceMatrix = lightProjection * lightView;

			// Рендеринг сцены глазами источника света
			shaderDirectionalShadowsDepth.bind();
			shaderDirectionalShadowsDepth.setUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);

			GLCall(glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT));
			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO));
			GLCall(glClear(GL_DEPTH_BUFFER_BIT));

			// draw model
#ifdef MODEL
			{
				glm::mat4 modelMat{ glm::mat4(1.0f) };
				modelMat = glm::translate(modelMat, modelBackpack.getPosition());
				shaderDirectionalShadowsDepth.setUniformMat4f("model", modelMat);
				modelBackpack.draw(shaderDirectionalShadowsDepth);
			}
#endif // MODEL

			// draw room
#ifdef ROOM
			{
				// floor
				{
					glm::mat4 modelMat{ glm::mat4(1.0f) };
					modelMat = glm::translate(modelMat, groundPos);
					modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					modelMat = glm::scale(modelMat, glm::vec3(10.0f, 10.0f, 1.0f));

					shaderDirectionalShadowsDepth.setUniformMat4f("model", modelMat);

					//renderer.draw(VAO_Square, EBO_Square);
					renderer.draw(VAO_Square, 6);
				}
				// ceil
				{
					glm::mat4 modelMat{ glm::mat4(1.0f) };
					glm::vec3 translate{ groundPos };
					translate.y += 10.0f;
					translate.z -= 10.0f;
					modelMat = glm::translate(modelMat, translate);
					modelMat = glm::rotate(modelMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					modelMat = glm::scale(modelMat, glm::vec3(10.0f, 10.0f, 1.0f));

					shaderDirectionalShadowsDepth.setUniformMat4f("model", modelMat);

					//renderer.draw(VAO_Square, EBO_Square);
					renderer.draw(VAO_Square, 6);
				}
				// walls
				{
					for (unsigned int i = 0; i < 4; ++i) {
						glm::mat4 modelMat{ glm::mat4(1.0f) };
						glm::vec3 translate{ groundPos };
						switch (i)
						{
						case 0:
							translate.z -= 10.0f;
							break;
						case 1:
							break;
						case 2:
							translate.x += 10.0f;
							break;
						case 3:
							translate.x += 10.0f;
							translate.z -= 10.0f;
							break;
						}
						modelMat = glm::translate(modelMat, translate);
						modelMat = glm::rotate(modelMat, glm::radians(90.0f * i), glm::vec3(0.0f, 1.0f, 0.0f));
						modelMat = glm::scale(modelMat, glm::vec3(10.0f, 10.0f, 1.0f));

						shaderDirectionalShadowsDepth.setUniformMat4f("model", modelMat);

						//renderer.draw(VAO_Square, EBO_Square);
						renderer.draw(VAO_Square, 6);
					}
				}
			}
#endif // ROOM

			// draw cubes
#ifdef CUBES
			float scaleDegree{ (float)glfwGetTime() };

			for (unsigned int i = 0; i < cubesCount; ++i) {
				glm::mat4 modelMat{ glm::mat4(1.0f) };
				modelMat = glm::translate(modelMat, cubePositions[i]);
				// auto-rotating
				modelMat = glm::rotate(modelMat, scaleDegree * glm::radians(10.0f * i), glm::vec3(0.1f, 0.03f, 0.05f));

				shaderDirectionalShadowsDepth.setUniformMat4f("model", modelMat);

				renderer.draw(VAO_Cube, EBO_Cube);
			}
#endif // CUBES

			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

			// Сброс настроек области просмотра
			GLCall(glViewport(0, 0, windowWidth, windowHeight));
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			// 2. Рендерим сцену как обычно, но используем при этом сгенерированную карту глубины/тени 
			shaderDefaultNormals.bind();
			// Устанавливаем uniform-переменные освещения
			shaderDefaultNormals.setUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
			shaderDefaultNormals.setUniformVec3("viewPos", camera.getPosition());
			shaderDefaultNormals.setUniformVec3("lightPos", dirLight.getDirection());
			GLCall(glActiveTexture(GL_TEXTURE0));
			GLCall(glBindTexture(GL_TEXTURE_2D, depthMap));

			shaderDefaultNormals.setUniform1f("height_scale", 0.0f);

			// draw model
#ifdef MODEL
			{
				glm::mat4 modelMat{ glm::mat4(1.0f) };
				modelMat = glm::translate(modelMat, modelBackpack.getPosition());
				shaderDefaultNormals.setUniformMat4f("model", modelMat);
				modelBackpack.draw(shaderDefaultNormals);
			}
#endif // MODEL

			shaderDefaultNormals.setUniform1f("height_scale", 0.1f);

			// draw room
#ifdef ROOM
			{
				shaderDefaultNormals.setUniform1i("u_Material.specular", 1);
				textureWall.bind(1);
				textureWallNormal.bind(3);
				textureWallDepth.bind(4);

				// floor
				{
					//textureFloor.bind(1);

					glm::mat4 modelMat{ glm::mat4(1.0f) };
					modelMat = glm::translate(modelMat, groundPos);
					modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					modelMat = glm::scale(modelMat, glm::vec3(10.0f, 10.0f, 1.0f));

					shaderDefaultNormals.setUniformMat4f("model", modelMat);

					//renderer.draw(VAO_Square, EBO_Square);
					renderer.draw(VAO_Square, 6);
				}
				// ceil
				{
					//textureCeil.bind(1);

					glm::mat4 modelMat{ glm::mat4(1.0f) };
					glm::vec3 translate{ groundPos };
					translate.y += 10.0f;
					translate.z -= 10.0f;
					modelMat = glm::translate(modelMat, translate);
					modelMat = glm::rotate(modelMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					modelMat = glm::scale(modelMat, glm::vec3(10.0f, 10.0f, 1.0f));

					shaderDefaultNormals.setUniformMat4f("model", modelMat);

					//renderer.draw(VAO_Square, EBO_Square);
					renderer.draw(VAO_Square, 6);
				}
				// walls
				{
					//textureWall.bind(1);

					for (unsigned int i = 0; i < 4; ++i) {
						glm::mat4 modelMat{ glm::mat4(1.0f) };
						glm::vec3 translate{ groundPos };
						switch (i)
						{
						case 0:
							translate.z -= 10.0f;
							break;
						case 1:
							break;
						case 2:
							translate.x += 10.0f;
							break;
						case 3:
							translate.x += 10.0f;
							translate.z -= 10.0f;
							break;
						}
						modelMat = glm::translate(modelMat, translate);
						modelMat = glm::rotate(modelMat, glm::radians(90.0f * i), glm::vec3(0.0f, 1.0f, 0.0f));
						modelMat = glm::scale(modelMat, glm::vec3(10.0f, 10.0f, 1.0f));

						shaderDefaultNormals.setUniformMat4f("model", modelMat);

						//renderer.draw(VAO_Square, EBO_Square);
						renderer.draw(VAO_Square, 6);
					}
				}
			}
#endif // ROOM

			shaderDefault.bind();
			shaderDefault.setUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
			shaderDefault.setUniformVec3("viewPos", camera.getPosition());
			shaderDefault.setUniformVec3("lightPos", dirLight.getDirection());

			// draw cubes
#ifdef CUBES
			textureCrate.bind(1);
			textureCrateSpecular.bind(2);

			for (unsigned int i = 0; i < cubesCount; ++i) {
				glm::mat4 modelMat{ glm::mat4(1.0f) };
				modelMat = glm::translate(modelMat, cubePositions[i]);
				// auto-rotating
				modelMat = glm::rotate(modelMat, scaleDegree * glm::radians(10.0f * i), glm::vec3(0.1f, 0.03f, 0.05f));

				shaderDefault.setUniformMat4f("model", modelMat);

				renderer.draw(VAO_Cube, EBO_Cube);
			}
#endif // CUBES





			/*
			shadowMappingShader.bind();
			// set all lighting uniforms
#ifdef LIGHTING
			{
				dirLight.bindToShader(shadowMappingShader);
				pointLights.bindToShader(shadowMappingShader);
				spotLight.bindToShader(shadowMappingShader);
			}
#endif // LIGHTING
			*/

			// draw point light sources
#ifdef LIGHTING
			//shaderLightSource.bind();
			//pointLights.drawAllPntLights();
#endif // LIGHTING

			// draw transparent objects
#ifdef TRANSPARENT
			{
				// sort windows
				std::map<float, glm::vec3> sorted;
				for (unsigned int i = 0; i < glasses.size(); ++i) {
					float distance{ glm::length(camera.getPosition() - glasses[i]) };
					sorted[distance] = glasses[i];
				}

				shaderTransparentText.bind();

				textureGlass.bind(0);
				for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
				{
					glm::mat4 modelMat{ glm::mat4(1.0f) };
					modelMat = glm::translate(modelMat, it->second);

					shaderTransparentText.setUniformMat4f("u_Model", modelMat);

					shaderTransparentText.setUniform1i("u_Texture1", 0);

					renderer.draw(VAO_Square, EBO_Square);
				}
			}
#endif // TRANSPARENT

			// draw planet and rocks
#ifdef PLANET
			{
				shaderPlanet.bind();
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
				model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
				shaderPlanet.setUniformMat4f("u_Model", model);
				modelPlanet.draw(shaderPlanet);

				shaderRock.bind();
				shaderRock.setUniform1i("texture_diffuse1", 0);
				GLCall(glActiveTexture(GL_TEXTURE0));
				GLCall(glBindTexture(GL_TEXTURE_2D, modelRock.getTexturesLoaded()[0].m_ID));

				for (unsigned int i = 0; i < modelRock.getMeshes().size(); ++i) {
					GLCall(glBindVertexArray(modelRock.getMeshes()[i].getVAO()));
					GLCall(glDrawElementsInstanced(GL_TRIANGLES, modelRock.getMeshes()[i].m_Indices.size(), GL_UNSIGNED_INT, 0, amount));
					GLCall(glBindVertexArray(0));
				}
			}
#endif // PLANET

			// draw skybox as last
#ifdef SKYBOX
			{
				GLCall(glDepthFunc(GL_LEQUAL)); // change depth function so depth test passes when values are equal to depth buffer's content
				shaderSkybox.bind();

				GLCall(glActiveTexture(GL_TEXTURE0));
				GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture));

				shaderSkybox.setUniformMat4f("projection", projMat);
				shaderSkybox.setUniformMat4f("view", glm::mat4(glm::mat3(camera.getViewMatrix())));

				renderer.draw(VAO_Skybox, 36);

				GLCall(glDepthFunc(GL_LESS));
			}
#endif // SKYBOX
			
#ifdef IMGUI
			if (camera.getUseMouse()) {
				ImGui::Text("Press Q to hide/show this debug window");
				ImGui::Text("Press Space/LShift to go up/down");
#ifdef LIGHTING
				//ImGui::Text("Press F to switch flashlight");
				//ImGui::SliderFloat3("Light 1 position", &pointLights.getPosition(0).x, -5.0f, 15.0f);
				//if (ImGui::Button("Switch light 1", ImVec2(270.0f, 30.0f)))
				//	pointLights.switchLight(0);
				//ImGui::SliderFloat3("Light 2 position", &pointLights.getPosition(1).x, -20.0f, 20.0f);
				//if (ImGui::Button("Switch light 2", ImVec2(270.0f, 30.0f)))
				//	pointLights.switchLight(1);
				ImGui::SliderFloat3("Sunlight direction", &dirLight.getDirectionAlt().x, -30.0f, 30.0f);
#endif // LIGHTING
#ifdef CUBES
				ImGui::SliderFloat3("Cube 1 position", &cubePositions[0].x, -10.0f, 10.0f);
				ImGui::SliderFloat3("Cube 2 position", &cubePositions[1].x, -10.0f, 10.0f);
#endif // CUBES
#ifdef MODEL
				ImGui::SliderFloat3("Backpack position", &modelBackpack.getPositionAlt().x, -10.0f, 10.0f);
#endif // MODEL
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif // IMGUI

			/* Swap front and back buffers */
			glfwSwapBuffers(window);
			/* Poll for and process events */
			glfwPollEvents();
		}
	}
#ifdef IMGUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
#endif // IMGUI
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.processKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.processKeyboard(DOWN, deltaTime);

	static float imguiDeltaTime{ 0.0f };
	static float imguiLastTime{ 0.0f };
	float currentFrame{ (float)glfwGetTime() };
	imguiDeltaTime = currentFrame - imguiLastTime;
	if (imguiDeltaTime > 0.3f) {
#ifdef IMGUI
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			imguiLastTime = currentFrame;
			camera.stopOrResumeMouse();
			if (!camera.getUseMouse())
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			else
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
#endif // IMGUI
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			imguiLastTime = currentFrame;
			spotLight.switchLight();
		}
	}
}

void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
	static bool firstTime{ true };
	if (firstTime)
	{
		lastX = xpos;
		lastY = ypos;
		firstTime = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	if (!camera.getUseMouse())
		camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	camera.processMouseScroll(2 * yoffset);
}

unsigned int loadCubemap(std::vector<std::string> &faces)
{
	stbi_set_flip_vertically_on_load(0);

	unsigned int textureID;
	GLCall(glGenTextures(1, &textureID));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, textureID));

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); ++i) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 3);
		if (data) {
			GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
		}
		stbi_image_free(data);
	}
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

	stbi_set_flip_vertically_on_load(1);

	return textureID;
}