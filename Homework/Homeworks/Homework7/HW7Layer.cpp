#include "HW7Layer.h"

#include <implot.h>

#include "../Homework3/BezierCurve.h"

namespace Chaf
{
	HW7Layer::HW7Layer()
	{

	}

	void HW7Layer::OnAttach()
	{
		ImPlot::CreateContext();

		m_questions = {
			{"1-1", false},
			{"1-2", false},
			{"1-3", false},
			{"3-1", false},
			{"3-2", false},
		};

		for (double i = 2.0; i <= 4.0; i += 0.01)
		{
			m_DrawData.x1.push_back(-7.0 / 8.0 * i * i * i + 9.0 * i * i - 57.0 / 2.0 * i + 30.0);
			m_DrawData.y1.push_back(-5.0 / 8.0 * i * i * i + 15.0 / 4.0 * i * i - 9.0 / 2.0 * i - 1.0);
		}

		std::vector<double> x = { -2.0, -2.0 / 3.0, 8.0 / 3.0, 4.0 };
		std::vector<double> y = { 0.5, 3.0, 4.0, 2.5 };

		BezierCurve::create(x, y, m_DrawData.x2, m_DrawData.y2, 200);
	}

	void HW7Layer::OnDetach()
	{
		ImPlot::DestroyContext();
	}

	void HW7Layer::OnUpdate(Timestep timestep)
	{

	}

	void HW7Layer::OnImGuiRender()
	{
		ImGui::Begin("Homework 7");

		for (auto& [key, val] : m_questions)
		{
			ImGui::Checkbox(key.c_str(), &val);
			ImGui::SameLine();
		}

		ImGui::NewLine();

		if (ImPlot::BeginPlot("Curve", "x", "y", ImVec2(-1, -1), ImPlotFlags_AntiAliased))
		{
			uint32_t index = 0;
			ImPlotStyle& style = ImPlot::GetStyle();

			style.LineWeight = 4.f;
			style.MarkerSize = 6.f;

			if (m_questions["1-1"])
			{
				double x[] = { 2, 0, 3, 4, 2 };
				double y[] = { 0, 2, 4, 1, 0 };

				ImPlot::PlotLine("1-1", x, y, 5);
				ImPlot::PlotScatter("1-1", x, y, 4);
			}

			if (m_questions["1-2"])
			{
				double x[] = { 85.0 / 64.0, 15.0 / 8.0, 191.0 / 64.0 };
				double y[] = { 91.0 / 64.0, 19.0 / 8.0, 153.0 / 64.0 };

				ImPlot::PlotScatter("1-2", x, y, 3);
			}

			if (m_questions["1-3"])
			{
				double x[] = { 2.0 , 1.0 , 1.25 , 1.875 , 2.1875 , 2.59375 , 2.984375 , 3.375 , 3.75 , 4.0, 2.0 };
				double y[] = { 0.0 , 1.0 , 2.0 , 2.375 , 2.5625 , 2.59375 , 2.390625 , 2.1875 , 1.75 , 1.0, 0.0 };

				ImPlot::PlotLine("1-3", x, y, 11);
				ImPlot::PlotScatter("1-3", x, y, 10);
				ImPlot::PlotLine("1-3", m_DrawData.x1.data(), m_DrawData.y1.data(), m_DrawData.x1.size());
			}

			if (m_questions["3-1"])
			{
				double x1[] = { -2, -4, 6, 4, -2 };
				double y1[] = { -10, 2, 5, -7, -10 };

				double x2[] = { -11.0 / 3.0, 1.0, 17.0 / 3.0, -1.0 / 6.0, 13.0 / 6.0, 1.0 };
				double y2[] = { 0.0, 7.0 / 2.0, 3.0, 21.0 / 8.0, 27.0 / 8.0, 3.0 };

				ImPlot::PlotLine("3-1", x1, y1, 5);
				ImPlot::PlotScatter("3-1", x2, y2, 6);
			}

			if (m_questions["3-2"])
			{
				double x[] = { -2.0, -2.0 / 3.0, 8.0 / 3.0, 4.0 };
				double y[] = { 0.5, 3.0, 4.0, 2.5 };

				ImPlot::PlotLine("3-2", m_DrawData.x2.data(), m_DrawData.y2.data(), m_DrawData.x2.size());
				ImPlot::PlotScatter("3-2", x, y, 4);
			}

			ImPlot::EndPlot();
		}

		ImGui::End();
	}

	void HW7Layer::OnEvent(Event& event)
	{

	}
}