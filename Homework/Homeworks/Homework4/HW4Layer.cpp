#include "HW4Layer.h"


#include <implot.h>


namespace Chaf
{
	HW4Layer::HW4Layer()
	{

	}

	void HW4Layer::OnAttach()
	{
		ImPlot::CreateContext();
	}

	void HW4Layer::OnDetach()
	{
		ImPlot::DestroyContext();
	}

	void HW4Layer::OnUpdate(Timestep timestep)
	{
		if (m_Updated)
		{
			m_Updated = false;
			
			m_Ellipse.clear();
			m_Ellipse.update();
		}
	}

	void HW4Layer::OnImGuiRender()
	{
		ImGui::Begin("Homework 4");

		if (ImGui::DragFloat("a", &m_Ellipse.a, 0.01))
		{
			m_Updated = true;
		}
		if (ImGui::DragFloat("b", &m_Ellipse.b, 0.01))
		{
			m_Updated = true;
		}

		if (ImPlot::BeginPlot("Curve", "x", "y", ImVec2(-1, -1)))
		{
			uint32_t index = 0;
			ImPlotStyle& style = ImPlot::GetStyle();

			//auto& plot_map = ImPlot::GetInputMap();

			for (auto i = 0; i < m_Ellipse.ellipse.first.size(); i++)
			{
				double x = m_Ellipse.ellipse.first[i];
				double y = m_Ellipse.ellipse.second[i];
				if (ImPlot::DragPoint(("P" + std::to_string(index++)).c_str(), &m_Ellipse.ellipse.first[i], &m_Ellipse.ellipse.second[i], true, ImVec4(0.f, 0.f, 0.f, -1.f), 1.f))
				{
					//m_Updated = true;
					m_Ellipse.ellipse.first[i] = x;
					m_Ellipse.ellipse.second[i] = y;

					m_Ellipse.updateCurvature(index);
				}
			}

			style.LineWeight = 1.f;
			ImPlot::PlotLine("Ellipse", m_Ellipse.ellipse.first.data(), m_Ellipse.ellipse.second.data(), (int)m_Ellipse.ellipse.first.size());
			ImPlot::PlotLine("Evolute", m_Ellipse.evolute.first.data(), m_Ellipse.evolute.second.data(), (int)m_Ellipse.evolute.first.size());
			ImPlot::PlotLine("Curvature Circle", m_Ellipse.curvaturecircle.first.data(), m_Ellipse.curvaturecircle.second.data(), (int)m_Ellipse.curvaturecircle.first.size());

			//style.LineWeight = 4.f;
			//ImPlot::PlotLine("Bezier Curve", m_Data.curve_x.data(), m_Data.curve_y.data(), (int)m_Data.curve_x.size());

			//if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyCtrl)
			//{
			//	ImPlotPoint pt = ImPlot::GetPlotMousePos();
			//	m_Data.control_x.push_back(pt.x);
			//	m_Data.control_y.push_back(pt.y);
			//	m_Updated = true;
			//}

			ImPlot::EndPlot();

		}

		ImGui::End();
	}

	void HW4Layer::OnEvent(Event& event)
	{

	}
}