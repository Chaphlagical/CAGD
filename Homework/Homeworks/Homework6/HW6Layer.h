#pragma once

#include <Editor/application.h>

#include <glm/glm.hpp>

#include "BSpline.h"

struct ImPlotPoint;

namespace Chaf
{
	class HW6Layer : public Layer
	{
	public:
		HW6Layer();
		virtual ~HW6Layer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		struct DrawData
		{
			std::vector<double> gen_x;
			std::vector<double> gen_y;
			std::vector<double> curve_x;
			std::vector<double> curve_y;
		};

		bool m_ShowGeneratePoints = false;
		bool m_ShowConnectLines = false;

		BSpline m_BSpline;

		DrawData m_Data;
		bool m_Updated = true;

	};
}