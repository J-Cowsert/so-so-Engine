#include "sspch.h"
#include "SceneCamera.h"

#include "so-so/Core/Input.h"
#include "so-so/Core/KeyCodes.h"
#include <glfw/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace soso {

	static void DisableMouse() {

		Input::SetCursorMode(CursorMode::Locked);
	}

	static void EnableMouse() {

		Input::SetCursorMode(CursorMode::Normal);
	}

	SceneCamera::SceneCamera(const float degFov, const float width, const float height, const float nearP, const float farP)
		: Camera(glm::perspectiveFov(glm::radians(degFov), width, height, nearP, farP)), m_FocalPoint(0.0f), m_VerticalFOV(glm::radians(degFov)), m_NearClip(nearP), m_FarClip(farP) 
	{
		Init();
	}

	void SceneCamera::Init() {

		constexpr glm::vec3 position = { -5, 5, 5 };
		m_Distance = glm::distance(position, m_FocalPoint);

		m_Yaw = 3.0f * glm::pi<float>() / 4.0f;
		m_Pitch = glm::pi<float>() / 4.0f;

		m_Position = CalculatePosition();
		const glm::quat orientation = GetOrientation();
		m_Direction = glm::eulerAngles(orientation) * (180.0f / glm::pi<float>());
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	void SceneCamera::OnUpdate(const Timestep ts) {

		const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		const glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.002f;

		if (Input::IsMouseButtonPressed(MouseCode::Right) && !Input::IsKeyPressed(KeyCode::LeftAlt)) {

			m_CameraControlMode = CameraControlMode::FLYCAM;
			DisableMouse();
			const float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;

			const float speed = GetCameraSpeed();

			if (Input::IsKeyPressed(KeyCode::Q))
				m_PositionDelta -= ts.GetMilliseconds() * speed * glm::vec3{ 0.f, yawSign, 0.f };
			if (Input::IsKeyPressed(KeyCode::E))
				m_PositionDelta += ts.GetMilliseconds() * speed * glm::vec3{ 0.f, yawSign, 0.f };
			if (Input::IsKeyPressed(KeyCode::S))
				m_PositionDelta -= ts.GetMilliseconds() * speed * m_Direction;
			if (Input::IsKeyPressed(KeyCode::W))
				m_PositionDelta += ts.GetMilliseconds() * speed * m_Direction;
			if (Input::IsKeyPressed(KeyCode::A))
				m_PositionDelta -= ts.GetMilliseconds() * speed * m_RightDirection;
			if (Input::IsKeyPressed(KeyCode::D))
				m_PositionDelta += ts.GetMilliseconds() * speed * m_RightDirection;

			constexpr float maxRate{ 0.12f };
			m_YawDelta += glm::clamp(yawSign * delta.x * RotationSpeed(), -maxRate, maxRate);
			m_PitchDelta += glm::clamp(delta.y * RotationSpeed(), -maxRate, maxRate);

			m_RightDirection = glm::cross(m_Direction, glm::vec3{ 0.f, yawSign, 0.f });

			m_Direction = glm::rotate(glm::normalize(glm::cross(glm::angleAxis(-m_PitchDelta, m_RightDirection),
				glm::angleAxis(-m_YawDelta, glm::vec3{ 0.f, yawSign, 0.f }))), m_Direction);

			const float distance = glm::distance(m_FocalPoint, m_Position);
			m_FocalPoint = m_Position + GetForwardDirection() * distance;
			m_Distance = distance;
		}

		else if (Input::IsKeyPressed(KeyCode::LeftAlt)) {

			m_CameraControlMode = CameraControlMode::ARCBALL;

			if (Input::IsMouseButtonPressed(MouseCode::Middle)) {

				DisableMouse();
				MousePan(delta);
			}
			else if (Input::IsMouseButtonPressed(MouseCode::Left)) {

				DisableMouse();
				MouseRotate(delta);
			}
			else if (Input::IsMouseButtonPressed(MouseCode::Right)) {

				DisableMouse();
				MouseZoom((delta.x + delta.y) * 0.1f);
			}
			else
				EnableMouse();
		}
		else {
			EnableMouse();
		}

		m_InitialMousePosition = mouse;
		m_Position += m_PositionDelta;
		m_Yaw += m_YawDelta;
		m_Pitch += m_PitchDelta;

		if (m_CameraControlMode == CameraControlMode::ARCBALL)
			m_Position = CalculatePosition();

		UpdateCameraView();
	}

	float SceneCamera::GetCameraSpeed() const {

		float speed = m_NormalSpeed;

		if (Input::IsKeyPressed(KeyCode::LeftControl))
			speed /= 2 - glm::log(m_NormalSpeed);

		if (Input::IsKeyPressed(KeyCode::LeftShift))
			speed *= 2 - glm::log(m_NormalSpeed);

		return glm::clamp(speed, MIN_SPEED, MAX_SPEED);
	}

	void SceneCamera::UpdateCameraView() {

		const float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;

		// Extra step to handle the problem when the camera direction is the same as the up vector
		const float cosAngle = glm::dot(GetForwardDirection(), GetUpDirection());
		if (cosAngle * yawSign > 0.99f)
			m_PitchDelta = 0.f;

		const glm::vec3 lookAt = m_Position + GetForwardDirection();
		m_Direction = glm::normalize(lookAt - m_Position);
		m_Distance = glm::distance(m_Position, m_FocalPoint);
		m_ViewMatrix = glm::lookAt(m_Position, lookAt, glm::vec3{ 0.f, yawSign, 0.f });

		//damping for smooth camera
		m_YawDelta *= 0.6f;
		m_PitchDelta *= 0.6f;
		m_PositionDelta *= 0.8f;
	}

	void SceneCamera::Focus(const glm::vec3& focusPoint) {

		m_FocalPoint = focusPoint;
		m_CameraControlMode = CameraControlMode::FLYCAM;

		if (m_Distance > m_MinFocusDistance) {

			m_Distance -= m_Distance - m_MinFocusDistance;
			m_Position = m_FocalPoint - GetForwardDirection() * m_Distance;
		}

		m_Position = m_FocalPoint - GetForwardDirection() * m_Distance;
		UpdateCameraView();
	}

	std::pair<float, float> SceneCamera::PanSpeed() const {

		const float x = glm::min(float(m_ViewportWidth) / 1000.0f, 2.4f); // max = 2.4f
		const float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		const float y = glm::min(float(m_ViewportHeight) / 1000.0f, 2.4f); // max = 2.4f
		const float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float SceneCamera::RotationSpeed() const {
		return 0.3f;
	}

	float SceneCamera::ZoomSpeed() const {

		float distance = m_Distance * 0.2f;
		distance = glm::max(distance, 0.0f);
		float speed = distance * distance;
		speed = glm::min(speed, 50.0f); // max speed = 50
		return speed;
	}

	void SceneCamera::OnEvent(Event& event) {

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& e) { return OnMouseScroll(e); });
	}

	bool SceneCamera::OnMouseScroll(MouseScrolledEvent& e) {

		if (Input::IsMouseButtonPressed(MouseCode::Right)) {

			m_NormalSpeed += e.GetYOffset() * 0.3f * m_NormalSpeed;
			m_NormalSpeed = std::clamp(m_NormalSpeed, MIN_SPEED, MAX_SPEED);
		}
		else {

			MouseZoom(e.GetYOffset() * 0.1f);
			UpdateCameraView();
		}

		return true;
	}

	void SceneCamera::MousePan(const glm::vec2& delta) {

		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint -= GetRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void SceneCamera::MouseRotate(const glm::vec2& delta) {

		const float yawSign = GetUpDirection().y < 0.0f ? -1.0f : 1.0f;
		m_YawDelta += yawSign * delta.x * RotationSpeed();
		m_PitchDelta += delta.y * RotationSpeed();
	}

	void SceneCamera::MouseZoom(float delta) {

		m_Distance -= delta * ZoomSpeed();
		const glm::vec3 forwardDir = GetForwardDirection();
		m_Position = m_FocalPoint - forwardDir * m_Distance;

		if (m_Distance < 1.0f) {

			m_FocalPoint += forwardDir * m_Distance;
			m_Distance = 1.0f;
		}

		m_PositionDelta += delta * ZoomSpeed() * forwardDir;
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height) {

		if (m_ViewportWidth == width && m_ViewportHeight == height)
			return;

		SetPerspectiveProjection(m_VerticalFOV, (float)width, (float)height, m_NearClip, m_FarClip);
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}

	glm::vec3 SceneCamera::GetUpDirection() const {

		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 SceneCamera::GetRightDirection() const {

		return glm::rotate(GetOrientation(), glm::vec3(1.f, 0.f, 0.f));
	}

	glm::vec3 SceneCamera::GetForwardDirection() const {

		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 SceneCamera::CalculatePosition() const {

		return m_FocalPoint - GetForwardDirection() * m_Distance + m_PositionDelta;
	}

	glm::quat SceneCamera::GetOrientation() const {

		return glm::quat(glm::vec3(-m_Pitch - m_PitchDelta, -m_Yaw - m_YawDelta, 0.0f));
	}
}