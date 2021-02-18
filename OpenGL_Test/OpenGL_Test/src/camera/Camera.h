#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// ���������� ��������� ��������� ��������� �������� ������. ������������ � �������� ����������, ����� ��������� �������� �� ����������� ��� ������� ������� ������� �����
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// ��������� ������ �� ���������
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 7.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// ����������� ����� ������, ������� ������������ ������� ������ � ��������� ��������������� �������� ����, ������� � ������� ��� ������������� � OpenGL
class Camera
{
private:
	// �������� ������
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;
	// ���� ������
	float m_Yaw;
	float m_Pitch;
	// ��������� ������
	float m_MovementSpeed;
	float m_MouseSensitivity;
	float m_Zoom;
	//
	bool m_useMouse{ 1 };
public:
	// �����������, ������������ �������
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	// �����������, ������������ �������
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	// ���������� ������� ����, ����������� � �������������� ����� ������ � LookAt-������� 
	glm::mat4 getViewMatrix() const { return glm::lookAt(m_Position, m_Position + m_Front, m_Up); }
	float getFOV() const { return m_Zoom; }
	glm::vec3 getPosition() const { return m_Position; }
	bool getUseMouse() const { return m_useMouse; }
	glm::vec3 getFront() const { return m_Front; }

	// ������������ ������� ������, ���������� �� ����� ������������������ ������� �����. ��������� ������� �������� � ���� ������������� ������� ������������ (��� ��������������� ��� �� ������� ������)
	void processKeyboard(Camera_Movement direction, float deltaTime);
	// ������������ ������� ������, ���������� �� ������� ����� � ������� ����. ������� � �������� ���������� �������� �������� ��� � ����������� X, ��� � � ����������� Y.
	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	// ������������ ������� ������, ���������� �� ������� ������ ��������� ����. ���������� ������ ������� ������ �� ������������ ��� �������� 
	void processMouseScroll(float yoffset);
	void stopOrResumeMouse() { m_useMouse = !m_useMouse; }
private:
	// ��������� ������-����� �� (�����������) ����� ������ ������
	void updateCameraVectors();
};