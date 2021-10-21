#include "HW5Layer.h"


#include <implot.h>


namespace Chaf
{
	HW5Layer::HW5Layer()
	{

	}

	void HW5Layer::OnAttach()
	{
		ImPlot::CreateContext();
	}

	void HW5Layer::OnDetach()
	{
		ImPlot::DestroyContext();
	}

	void HW5Layer::OnUpdate(Timestep timestep)
	{
		if (m_Updated)
		{
			if (m_Data.control_x.size() > 2)
			{
				m_BezierSpline.createControlPoints(m_Data.control_x, m_Data.control_y, m_Data.gen_x, m_Data.gen_y);
			}

			m_Updated = false;


		}
	}

	void HW5Layer::OnImGuiRender()
	{
		ImGui::Begin("Homework 5");

		if (ImPlot::BeginPlot("Curve", "x", "y", ImVec2(-1, -1)))
		{
			uint32_t index = 0;
			ImPlotStyle& style = ImPlot::GetStyle();

			//auto& plot_map = ImPlot::GetInputMap();

			for (auto i = 0; i < m_Data.control_x.size(); i++)
			{
				if (ImPlot::DragPoint(("P" + std::to_string(index++)).c_str(), &m_Data.control_x[i], &m_Data.control_y[i], true, ImVec4(0.f, 0.f, 0.f, -1.f), 8.f))
				{
					m_Updated = true;
				}
			}

			style.LineWeight = 1.f;
			ImPlot::PlotLine("Control", m_Data.control_x.data(), m_Data.control_y.data(), (int)m_Data.control_x.size());
			style.LineWeight = 2.f;
			ImPlot::PlotLine("Generate", m_Data.gen_x.data(), m_Data.gen_y.data(), (int)m_Data.gen_x.size());
			style.LineWeight = 4.f;
			// TODO:
			//ImPlot::PlotLine("Bezier Spline", m_Data.curve_x.data(), m_Data.curve_y.data(), (int)m_Data.curve_x.size());


			if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyCtrl)
			{
				ImPlotPoint pt = ImPlot::GetPlotMousePos();
				m_Data.control_x.push_back(pt.x);
				m_Data.control_y.push_back(pt.y);
				m_Updated = true;
			}

			ImPlot::EndPlot();

		}

		ImGui::End();
	}

	void HW5Layer::OnEvent(Event& event)
	{

	}
}