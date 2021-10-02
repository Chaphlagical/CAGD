#pragma once

#include <Editor/application.h>

#include <glm/glm.hpp>

namespace Chaf
{
	class HW1Layer : public Layer
	{
	public:
		HW1Layer();
		virtual ~HW1Layer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		struct DrawData
		{
			std::vector<glm::f64vec2> pos;

			std::vector<double> inter_x;
			std::vector<double> polynomial;
			std::vector<double> RBF;

			glm::f64vec2 range;
			glm::f64vec2 width;

			double step = 0.1f;

			bool need_update = false;
		};

		DrawData m_Data;

		bool m_EnablePolynomial = true;
		bool m_EnableRBF = true;

		float m_RBFd = 1.f;
	};
}