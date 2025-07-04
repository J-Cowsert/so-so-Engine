#pragma once
#include <glm/ext.hpp>

namespace soso {

	class Camera {
	public:

		Camera() = default;
		Camera(const glm::mat4& projection);
		Camera(const float degreeFOV, const float width, const float height, const float nearP, const float farP);
		
		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }

		void SetPerspectiveProjection(const float degreeFOV, const float width, const float height, const float nearP, const float farP) {
			m_Projection = glm::perspectiveFov(glm::radians(degreeFOV), width, height, nearP, farP);
		}

		void SetOrthographicProjection(const float width, const float height, const float nearP, const float farP) {
			m_Projection = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, nearP, farP);
		}

		float GetExposure() const { return m_Exposure; }
		float& GetExposure() { return m_Exposure; }

	protected:
		float m_Exposure = 1.0f;
		
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};

}