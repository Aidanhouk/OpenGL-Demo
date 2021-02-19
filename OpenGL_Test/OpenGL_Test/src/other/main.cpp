#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "renderer/Renderer.h"

#include "vertexProcessing/VertexBuffer.h"
#include "vertexProcessing/IndexBuffer.h"
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
//#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define IMGUI
#define CUBES
#define LIGHTING

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const float windowWidth{ 1920.0f };
const float windowHeight{ 1080.0f };

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX{ (int)windowWidth >> 1 };
float lastY{ (int)windowHeight >> 1 };

SpotLight spotLight(&camera);

float deltaTime{ 0.0f };
float lastFrame{ 0.0f };

int main(void)
{
	GLFWwindow* window;
	
	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(windowWidth, windowHeight, "Window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSwapInterval(0);

#ifndef IMGUI
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	camera.stopOrResumeMouse();
#endif // !IMGUI

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glEnable(GL_DEPTH_TEST));
	//GLCall(glDepthFunc(GL_LESS)); // default
	GLCall(glEnable(GL_STENCIL_TEST));
	GLCall(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
	GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

	stbi_set_flip_vertically_on_load(1);

	//GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

	{
		unsigned int positions_countSquare{ 4 * 6 };
		unsigned int attr_countSquare{ 8 }; // coordinates (3) + normals (3) + texture coords (2) = 8
		float blockSize{ 1.0f };
		float positionsSquare[]{
			// front
			0.0f,		0.0f,		0.0f,			0.0f, 0.0f,	-1.0f,		0.0f, 0.0f,
			blockSize,	0.0f,		0.0f,			0.0f, 0.0f,	-1.0f,		1.0f, 0.0f,
			blockSize,	blockSize,	0.0f,			0.0f, 0.0f,	-1.0f,		1.0f, 1.0f,
			//blockSize,	blockSize,	0.0f,			0.0f, 0.0f,	-1.0f,		1.0f, 1.0f,
			0.0f,		blockSize,	0.0f,			0.0f, 0.0f,	-1.0f,		0.0f, 1.0f,
			//0.0f,		0.0f,		0.0f,			0.0f, 0.0f,	-1.0f,		0.0f, 0.0f,
			// back									
			0.0f,		0.0f,		blockSize,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
			blockSize,	0.0f,		blockSize,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
			blockSize,	blockSize,	blockSize,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
			//blockSize,	blockSize,	blockSize,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
			0.0f,		blockSize,	blockSize,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
			//0.0f,		0.0f,		blockSize,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
			// left								
			0.0f,	0.0f,		0.0f,				-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
			0.0f,	blockSize,	0.0f,				-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
			0.0f,	blockSize,	blockSize,			-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
			//0.0f,	blockSize,	blockSize,			-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
			0.0f,	0.0f,		blockSize,			-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
			//0.0f,	0.0f,		0.0f,				-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
			// right							
			blockSize,	0.0f,		0.0f,			1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
			blockSize,	blockSize,	0.0f,			1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
			blockSize,	blockSize,	blockSize,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
			//blockSize,	blockSize,	blockSize,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
			blockSize,	0.0f,		blockSize,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
			//blockSize,	0.0f,		0.0f,			1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
			// bottom							
			0.0f,		0.0f,	0.0f,				0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
			blockSize,	0.0f,	0.0f,				0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
			blockSize,	0.0f,	blockSize,			0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
			//blockSize,	0.0f,	blockSize,			0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
			0.0f,		0.0f,	blockSize,			0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
			//0.0f,		0.0f,	0.0f,				0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
			// up							
			blockSize,	blockSize,	0.0f,			0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
			0.0f,		blockSize,	0.0f,			0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
			0.0f,		blockSize,	blockSize,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
			//0.0f,		blockSize,	blockSize,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
			blockSize,	blockSize,	blockSize,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
			//blockSize,	blockSize,	0.0f,			0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
		};
		unsigned int indices_countSquare{ 6 * 6 };
		unsigned int indicesSquare[]{
			// front
			0, 1, 2,
			2, 3, 0,
			// back
			4, 5, 6,
			6, 7, 4,
			// left
			8, 9, 10,
			10, 11, 8,
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
		
		VertexArray vertArrSquare;
		VertexBuffer vertBuffSquare(positionsSquare, positions_countSquare * attr_countSquare * sizeof(float));
		VertexBufferLayout layoutSquare;
		layoutSquare.push<float>(3);
		layoutSquare.push<float>(3);
		layoutSquare.push<float>(2);
		vertArrSquare.addBuffer(vertBuffSquare, layoutSquare);
		IndexBuffer indBuffSquare(indicesSquare, indices_countSquare);

#ifdef CUBES
		Texture textureCrate("res/textures/crate500.png", GL_REPEAT);
		Texture textureCrateLight("res/textures/crate500light.png", GL_REPEAT);
		glm::vec3 scaleCubes(1.0f, 1.0f, 1.0f);
		glm::vec3 rotateCubes(0.0f, 0.0f, 0.0f);
		unsigned int cubesCount{ 2 };
		glm::vec3 cubePositions[] = {
			glm::vec3(-5.0f, -5.0f, -2.0f),
			glm::vec3(0.0f, 5.0f, -1.1f),
			//glm::vec3(5.0f, 0.0f, -3.0f)
		};
#endif // CUBES

#ifdef LIGHTING
		Shader lightSourceShader("res/shaders/LightSource.shader");
		DirectionalLight dirLight(glm::vec3(0.0f, -0.5f, 0.0f));
		PointLightsControl pointLights;
		pointLights.addPointLight(glm::vec3(-2.5f, 0.3f, -2.25f));
		pointLights.addPointLight(glm::vec3(4.0f, 0.3f, -2.25f));
#endif // LIGHTING

		Shader modelShader("res/shaders/Model.shader"); // model & textures shader
		Model ourModel("res/models/backpack/backpack.obj");
		Shader singleColorShader("res/shaders/SingleColor.shader");

		Renderer renderer;

#ifdef IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
#endif // IMGUI



		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			processInput(window);
			/* Render here */
			renderer.clear();

#ifdef IMGUI
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
#endif // IMGUI

			float currentFrame{ (float)glfwGetTime() };
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			glm::mat4 projMat{ glm::perspective(glm::radians(camera.getFOV()), windowWidth / windowHeight, 0.1f, 100.0f) };
			glm::mat4 viewMat{ camera.getViewMatrix() };

			glStencilMask(0x00);

			// setting all lighting uniforms
#ifdef LIGHTING
			{
				modelShader.bind();

				dirLight.bindToShader(modelShader);
				pointLights.bindToShader(modelShader);
				spotLight.bindToShader(modelShader);
			}
#endif // LIGHTING

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);

			// drawing cubes
#ifdef CUBES
			modelShader.bind();
			textureCrate.bind(0);
			textureCrateLight.bind(1);
			modelShader.setUniform1i("u_Material.diffuse", 0);
			modelShader.setUniform1i("u_Material.specular", 1);
			modelShader.setUniform1f("u_Material.shininess", 12.8f);

			float scaleDegree{ (float)glfwGetTime() };

			for (unsigned int i = 0; i < cubesCount; ++i) {
				glm::mat4 modelMat{ glm::mat4(1.0f) };
				// auto-rotating
				modelMat = glm::rotate(modelMat, scaleDegree * glm::radians(10.0f * i), glm::vec3(0.1f, 0.03f, 0.05f));
				modelMat = glm::translate(modelMat, cubePositions[i]);
#ifdef IMGUI
				modelMat = glm::rotate(modelMat, glm::radians(-rotateCubes.x), glm::vec3(1.0f, 0.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(-rotateCubes.y), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(-rotateCubes.z), glm::vec3(0.0f, 0.0f, 1.0f));
				modelMat = glm::scale(modelMat, scaleCubes);
#endif // IMGUI

				modelShader.setUniformMat4f("u_Model", modelMat);
				modelShader.setUniformMat4f("u_View", viewMat);
				modelShader.setUniformMat4f("u_Projection", projMat);

				modelShader.setUniformVec3("u_ViewPos", camera.getPosition());

				renderer.draw(vertArrSquare, indBuffSquare);
			}

			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);

			singleColorShader.bind();
			float scale{ 1.2f };
			float move{ (scale - 1) / 2 };

			textureCrate.bind(0);
			textureCrateLight.bind(1);

			for (unsigned int i = 0; i < cubesCount; ++i) {
				glm::mat4 modelMat{ glm::mat4(1.0f) };

				// auto-rotating
				modelMat = glm::rotate(modelMat, scaleDegree * glm::radians(10.0f * i), glm::vec3(0.1f, 0.03f, 0.05f));
				modelMat = glm::translate(modelMat, glm::vec3(cubePositions[i].x - move, cubePositions[i].y - move, cubePositions[i].z - move));

				modelMat = glm::scale(modelMat, glm::vec3(scale, scale, scale));
#ifdef IMGUI
				modelMat = glm::rotate(modelMat, glm::radians(-rotateCubes.x), glm::vec3(1.0f, 0.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(-rotateCubes.y), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(-rotateCubes.z), glm::vec3(0.0f, 0.0f, 1.0f));
				modelMat = glm::scale(modelMat, scaleCubes);
#endif // IMGUI

				singleColorShader.setUniformMat4f("u_Model", modelMat);
				singleColorShader.setUniformMat4f("u_View", viewMat);
				singleColorShader.setUniformMat4f("u_Projection", projMat);

				renderer.draw(vertArrSquare, indBuffSquare);
			}
#endif // CUBES

			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 0, 0xFF);
			glEnable(GL_DEPTH_TEST);

			// drawing model
			{
				modelShader.bind();

				glm::mat4 modelMat{ glm::mat4(1.0f) };
				modelShader.setUniformMat4f("u_Model", modelMat);
				modelShader.setUniformMat4f("u_View", viewMat);
				modelShader.setUniformMat4f("u_Projection", projMat);
				ourModel.draw(modelShader);
			}

			// drawing point light sources
#ifdef LIGHTING
			lightSourceShader.bind();
			for (const auto & pntLight : pointLights.getPointLights()) {
				if (pntLight->getIsOn()) {
					pntLight->drawSource(lightSourceShader, viewMat, projMat);
					renderer.draw(vertArrSquare, indBuffSquare);
				}
			}
#endif // LIGHTING

#ifdef IMGUI
			if (camera.getUseMouse()) {
				ImGui::Text("Press Q to hide/show this debug info");
#ifdef LIGHTING
				ImGui::Text("Press F to switch flashlight");
				ImGui::SliderFloat3("Light 1 position", &pointLights.getPosition(0).x, -20.0f, 20.0f);
				if (ImGui::Button("Switch light 1", ImVec2(270.0f, 30.0f)))
					pointLights.switchLight(0);
				ImGui::SliderFloat3("Light 2 position", &pointLights.getPosition(1).x, -20.0f, 20.0f);
				if (ImGui::Button("Switch light 2", ImVec2(270.0f, 30.0f)))
					pointLights.switchLight(1);
				if (ImGui::Button("Switch directional light", ImVec2(270.0f, 30.0f)))
					dirLight.switchLight();
#endif // LIGHTING
#ifdef CUBES
				ImGui::SliderFloat3("Cube 1 position", &cubePositions[0].x, -10.0f, 10.0f);
				ImGui::SliderFloat3("Cube 2 position", &cubePositions[1].x, -10.0f, 10.0f);
				ImGui::SliderFloat3("Cube 3 position", &cubePositions[2].x, -10.0f, 10.0f);
				ImGui::SliderFloat3("Scale", &scaleCubes.x, 0.0f, 5.0f);
				ImGui::SliderFloat3("Rotate", &rotateCubes.x, 0.0f, 360.0f);
#endif // CUBES
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