#include "HW5Layer.h"

#include "../Homework2/BezierCurve.h"

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
				if (m_EndCondition == (int)BezierSpline::EndCondition::Close)
				{
					m_Data.control_x.front() = m_Data.control_x.back();
					m_Data.control_y.front() = m_Data.control_y.back();
				}

				m_BezierSpline.createControlPoints(m_Data.control_x, m_Data.control_y, m_Data.gen_x, m_Data.gen_y);

				m_Data.curve_x.clear();
				m_Data.curve_y.clear();

				for (size_t i = 0; i < m_Data.control_x.size() - 1; i++)
				{
					std::vector<double> x = { m_Data.gen_x[3 * i],m_Data.gen_x[3 * i + 1],m_Data.gen_x[3 * i + 2],m_Data.gen_x[3 * i + 3] };
					std::vector<double> y = { m_Data.gen_y[3 * i],m_Data.gen_y[3 * i + 1],m_Data.gen_y[3 * i + 2],m_Data.gen_y[3 * i + 3] };
					std::vector<double> control_x, control_y;
					BezierCurve::create(x, y, control_x, control_y, 100);
					m_Data.curve_x.insert(m_Data.curve_x.end(), control_x.begin(), control_x.end());
					m_Data.curve_y.insert(m_Data.curve_y.end(), control_y.begin(), control_y.end());
				}
			}
			else
			{
				m_Data.curve_x = m_Data.control_x;
				m_Data.curve_y = m_Data.control_y;
			}
			m_Updated = false;


		}
	}

	void HW5Layer::OnImGuiRender()
	{
		ImGui::Begin("Homework 5");

		ImGui::Checkbox("Show Generate Control Points", &m_ShowGeneratePoints);
		ImGui::SameLine();
		ImGui::Checkbox("Show Connection Lines", &m_ShowConnectLines);
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			m_Updated = true;
			m_Data.control_x.clear();
			m_Data.control_y.clear();
			m_Data.gen_x.clear();
			m_Data.gen_y.clear();
			m_Data.curve_x.clear();
			m_Data.curve_y.clear();
		}

		static const char* Parameterization[] = { "Uniform", "Chordal", "Centripetal" };
		if (ImGui::Combo("Parameterization", &m_Parameterization, Parameterization, IM_ARRAYSIZE(Parameterization)))
		{
			m_Updated = true;
			m_BezierSpline.parameter = static_cast<BezierSpline::Parameter>(m_Parameterization);
		}

		static const char* EndCondition[] = { "Natural", "Bessel", "Close" };
		if (ImGui::Combo("End Condition", &m_EndCondition, EndCondition, IM_ARRAYSIZE(EndCondition)))
		{
			m_Updated = true;
			m_BezierSpline.end_condition = static_cast<BezierSpline::EndCondition>(m_EndCondition);
		}

		if (ImPlot::BeginPlot("Curve", "x", "y", ImVec2(-1, -1)))
		{
			uint32_t index = 0;
			ImPlotStyle& style = ImPlot::GetStyle();

			style.LineWeight = 4.f;
			ImPlot::PushStyleColor(ImPlotCol_Line, ImPlot::GetColormapColor(0));
			ImPlot::PlotLine("Bezier Spline", m_Data.curve_x.data(), m_Data.curve_y.data(), (int)m_Data.curve_x.size());
			ImPlot::PopStyleColor();

			if (m_ShowConnectLines)
			{
				style.LineWeight = 1.f;
				ImPlot::PushStyleColor(ImPlotCol_Line, ImPlot::GetColormapColor(1));
				ImPlot::PlotLine("Control", m_Data.control_x.data(), m_Data.control_y.data(), (int)m_Data.control_x.size());
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

			for (auto i = 0; i < m_Data.control_x.size(); i++)
			{
				if (ImPlot::DragPoint(("P" + std::to_string(index++)).c_str(), &m_Data.control_x[i], &m_Data.control_y[i], true, ImVec4(0.f, 0.f, 0.f, -1.f), 8.f))
				{
					if (m_BezierSpline.end_condition == BezierSpline::EndCondition::Close)
					{
						if (i == 0)
						{
							m_Data.control_x.back() = m_Data.control_x.front();
							m_Data.control_y.back() = m_Data.control_y.front();
						}
						else if (i == m_Data.control_x.size() - 1)
						{
							m_Data.control_x.front() = m_Data.control_x.back();
							m_Data.control_y.front() = m_Data.control_y.back();
						}
					}
					m_Updated = true;
				}
			}

			if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyCtrl)
			{
				ImPlotPoint pt = ImPlot::GetPlotMousePos();
				m_Data.control_x.push_back(pt.x);
				m_Data.control_y.push_back(pt.y);
				if (m_BezierSpline.end_condition == BezierSpline::EndCondition::Close)
				{
					m_BezierSpline.end_condition = BezierSpline::EndCondition::Natural;
					m_EndCondition = 0;
				}
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