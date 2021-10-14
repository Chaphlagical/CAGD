#pragma once

#include <Editor/application.h>

#include <glm/glm.hpp>

#include "Ellipse.h"

struct ImPlotPoint;

namespace Chaf
{
	class HW4Layer : public Layer
	{
	public:
		HW4Layer();
		virtual ~HW4Layer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		Ellipse m_Ellipse;

		bool m_Updated = true;
		float m_a = 10;
		float m_b = 20;
	};
}