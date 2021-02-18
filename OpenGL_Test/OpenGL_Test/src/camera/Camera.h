#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Определяет несколько возможных вариантов движения камеры. Используется в качестве абстракции, чтобы держаться подальше от специфичных для оконной системы методов ввода
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Параметры камеры по умолчанию
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 7.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// Абстрактный класс камеры, который обрабатывает входные данные и вычисляет соответствующие Эйлеровы углы, векторы и матрицы для использования в OpenGL
class Camera
{
private:
	// Атрибуты камеры
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;
	// углы Эйлера
	float m_Yaw;
	float m_Pitch;
	// Настройки камеры
	float m_MovementSpeed;
	float m_MouseSensitivity;
	float m_Zoom;
	//
	bool m_useMouse{ 1 };
public:
	// Конструктор, использующий векторы
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	// Конструктор, использующие скаляры
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	// Возвращает матрицу вида, вычисленную с использованием углов Эйлера и LookAt-матрицы 
	glm::mat4 getViewMatrix() const { return glm::lookAt(m_Position, m_Position + m_Front, m_Up); }
	float getFOV() const { return m_Zoom; }
	glm::vec3 getPosition() const { return m_Position; }
	bool getUseMouse() const { return m_useMouse; }
	glm::vec3 getFront() const { return m_Front; }

	// Обрабатываем входные данные, полученные от любой клавиатуроподобной системы ввода. Принимаем входной параметр в виде определенного камерой перечисления (для абстрагирования его от оконных систем)
	void processKeyboard(Camera_Movement direction, float deltaTime);
	// Обрабатываем входные данные, полученные от системы ввода с помощью мыши. Ожидаем в качестве параметров значения смещения как в направлении X, так и в направлении Y.
	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	// Обрабатывает входные данные, полученные от события колеса прокрутки мыши. Интересуют только входные данные на вертикальную ось колесика 
	void processMouseScroll(float yoffset);
	void stopOrResumeMouse() { m_useMouse = !m_useMouse; }
private:
	// Вычисляет вектор-прямо по (обновленным) углам Эйлера камеры
	void updateCameraVectors();
};