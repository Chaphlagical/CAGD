#include "HW6Layer.h"

#include "../Homework2/BezierCurve.h"

#include <implot.h>


namespace Chaf
{
	HW6Layer::HW6Layer()
	{

	}

	void HW6Layer::OnAttach()
	{
		ImPlot::CreateContext();
	}

	void HW6Layer::OnDetach()
	{
		ImPlot::DestroyContext();
	}

	void HW6Layer::OnUpdate(Timestep timestep)
	{
		if (m_Updated)
		{
			if (m_BSpline.Control_Points_X.size() > 2)
			{
				m_BSpline.genDeBoorPoints(m_Data.gen_x, m_Data.gen_y);

				m_Data.curve_x.clear();
				m_Data.curve_y.clear();

				m_BSpline.genBSpline(m_Data.gen_x, m_Data.gen_y, m_Data.curve_x, m_Data.curve_y);
			}
			else
			{
				m_Data.curve_x = m_BSpline.Control_Points_X;
				m_Data.curve_y = m_BSpline.Control_Points_Y;
			}
			m_Updated = false;


		}
	}

	void HW6Layer::OnImGuiRender()
	{
		ImGui::Begin("Homework 6");

		ImGui::Checkbox("Show Generate Control Points", &m_ShowGeneratePoints);
		ImGui::SameLine();
		ImGui::Checkbox("Show Connection Lines", &m_ShowConnectLines);
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			m_Updated = true;
			m_Data.gen_x.clear();
			m_Data.gen_y.clear();
			m_Data.curve_x.clear();
			m_Data.curve_y.clear();

			m_BSpline.clear();
		}

		static const char* Point_Type[] = { "C2", "Line", "Sharp" };
		static ImVec4 Point_Color[3] = { ImVec4(1,0,0,1), ImVec4(0,1,0,1), ImVec4(1,1,0,1) };
		static int point_type = 0;
		ImGui::Combo("Point Type", &point_type, Point_Type, 3);

		if (ImPlot::BeginPlot("Curve", "x", "y", ImVec2(-1, -1), ImPlotFlags_AntiAliased))
		{
			uint32_t index = 0;
			ImPlotStyle& style = ImPlot::GetStyle();

			style.LineWeight = 4.f;
			ImPlot::PushStyleColor(ImPlotCol_Line, ImPlot::GetColormapColor(0));
			ImPlot::PlotLine("Cubic B Spline", m_Data.curve_x.data(), m_Data.curve_y.data(), (int)m_Data.curve_x.size());
			ImPlot::PopStyleColor();

			if (m_ShowConnectLines)
			{
				style.LineWeight = 1.f;
				ImPlot::PushStyleColor(ImPlotCol_Line, ImPlot::GetColormapColor(1));
				ImPlot::PlotLine("Control", m_BSpline.Control_Points_X.data(), m_BSpline.Control_Points_Y.data(), (int)m_BSpline.Control_Points_X.size());
				ImPlot::PopStyleColor();
			}

			if (m_ShowGeneratePoints)
			{
				ImPlot::PushStyleVar(ImPlotStyleVar_MarkerSize, 8);
				ImPlot::PushStyleColor(ImPlotCol_MarkerFill, ImPlot::GetColormapColor(2));
				ImPlot::PlotScatter("Generate control", m_Data.gen_x.data(), m_Data.gen_y.data(), (int)m_Data.gen_x.size());
				ImPlot::PopStyleVar();
				ImPlot::PopStyleColor();

				if (m_ShowConnectLines)
				{
					style.MarkerSize = 4.f;
					ImPlot::PushStyleColor(ImPlotCol_Line, ImPlot::GetColormapColor(3));
					ImPlot::PlotLine("Generate", m_Data.gen_x.data(), m_Data.gen_y.data(), (int)m_Data.gen_x.size());
					ImPlot::PopStyleColor();
				}
			}

			for (auto i = 0; i < m_BSpline.Control_Points_X.size(); i++)
			{
				if (ImPlot::DragPoint(("P" + std::to_string(index++)).c_str(), &m_BSpline.Control_Points_X[i], &m_BSpline.Control_Points_Y[i], true, Point_Color[m_BSpline.Types[i] - 1], 8.f))
				{
					m_Updated = true;
				}
			}

			if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyCtrl)
			{
				ImPlotPoint pt = ImPlot::GetPlotMousePos();
				m_BSpline.addPoint(pt, static_cast<BSpline::PointType>(point_type + 1));
				m_Updated = true;
			}


			ImPlot::EndPlot();
		}

		ImGui::End();
	}

	void HW6Layer::OnEvent(Event& event)
	{

	}
}