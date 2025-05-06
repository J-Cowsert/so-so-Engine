#pragma once
#include "so-so/Core/Timestep.h"
#include "so-so/Events/Event.h"
#include "so-so/Events/MouseEvent.h"

#include "Camera.h"

namespace soso {

	enum class CameraControlMode {
		NONE, FLYCAM, ARCBALL
	};

	// TODO: Implement
	enum class CameraProjectionType {
		NONE, ORTHOGRAPHIC, PERSPECTIVE
	};

	class SceneCamera : public Camera {
	public:
		SceneCamera() = default;
		SceneCamera(const float degFov, const float width, const float height, const float nearP, const float farP);
		void Init();

		void Focus(const glm::vec3& focusPoint);
		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		bool IsActive() const { return m_IsActive; }
		void SetActive(bool active) { m_IsActive = active; }

		CameraControlMode GetCurrentMode() const { return m_CameraControlMode; }

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		const glm::vec3& GetFocalPoint() const { return m_FocalPoint; }

		void SetViewportSize(uint32_t width, uint32_t height);

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjection() const { return GetProjection() * m_ViewMatrix; }
		

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;

		const glm::vec3& GetPosition() const { return m_Position; }

		glm::quat GetOrientation() const;

		[[nodiscard]] float GetVerticalFOV() const { return m_VerticalFOV; }
		[[nodiscard]] float GetAspectRatio() const { return m_AspectRatio; }
		[[nodiscard]] float GetNearClip() const { return m_NearClip; }
		[[nodiscard]] float GetFarClip() const { return m_FarClip; }
		[[nodiscard]] float GetPitch() const { return m_Pitch; }
		[[nodiscard]] float GetYaw() const { return m_Yaw; }
		[[nodiscard]] float GetCameraSpeed() const;
	private:
		void UpdateCameraView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

	private:
		CameraProjectionType m_ProjectionType{ CameraProjectionType::PERSPECTIVE };
		CameraControlMode m_CameraControlMode{ CameraControlMode::ARCBALL };

	private:
		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Position, m_Direction, m_FocalPoint;

		// Projection params
		float m_VerticalFOV, m_AspectRatio, m_NearClip, m_FarClip;

		bool m_IsActive = false;
		bool m_Panning, m_Rotating;
		glm::vec2 m_InitialMousePosition{};
		glm::vec3 m_InitialFocalPoint, m_InitialRotation;

		float m_Distance;
		float m_NormalSpeed{ 0.002f };

		float m_Pitch, m_Yaw;
		float m_PitchDelta{}, m_YawDelta{};
		glm::vec3 m_PositionDelta{};
		glm::vec3 m_RightDirection{};


		float m_MinFocusDistance{ 100.0f };

		uint32_t m_ViewportWidth{ 1280 }, m_ViewportHeight{ 720 };

		constexpr static float MIN_SPEED{ 0.0005f }, MAX_SPEED{ 2.0f };
	};
}