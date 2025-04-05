#pragma once

#include <glm/ext.hpp>

namespace soso {

	class Camera {
	public:

		Camera() = default;
		Camera(const glm::mat4& projection) : m_Projection(projection) {}
		Camera(const float degreeFOV, const float width, const float height, const float nearP, const float farP) 
			: m_Projection(glm::perspectiveFov(glm::radians(degreeFOV), width, height, nearP, farP))
		{}
		
		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }

		void SetPerspectiveProjection(const float degreeFOV, const float width, const float height, const float nearP, const float farP) {
			m_Projection = glm::perspectiveFov(glm::radians(degreeFOV), width, height, nearP, farP);
		}

		void SetOrthographicProjection(const float width, const float height, const float nearP, const float farP) {
			m_Projection = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, nearP, farP);
		}

	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};

}