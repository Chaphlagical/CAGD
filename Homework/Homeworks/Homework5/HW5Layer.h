#pragma once

#include <Editor/application.h>

#include <glm/glm.hpp>

#include "BezierSpline.h"

struct ImPlotPoint;

namespace Chaf
{
	class HW5Layer : public Layer
	{
	public:
		HW5Layer();
		virtual ~HW5Layer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		struct DrawData
		{
			std::vector<double> control_x;
			std::vector<double> control_y;
			std::vector<double> gen_x;
			std::vector<double> gen_y;
			std::vector<double> curve_x;
			std::vector<double> curve_y;
			size_t select = 0;
		};

		bool m_ShowGeneratePoints = false;
		bool m_ShowConnectLines = false;

		int m_Parameterization = 0;
		int m_EndCondition = 0;

		DrawData m_Data;
		BezierSpline m_BezierSpline;
		bool m_Updated = true;

	};
}