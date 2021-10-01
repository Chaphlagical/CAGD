#include "CAGDLayer.h"
#include "polynomial.h"
#include "RBF.h"

#include <Engine/input_system.h>

#include <implot.h>

namespace Chaf
{
	CAGDLayer::CAGDLayer()
	{
	}

	void CAGDLayer::OnAttach()
	{
		ImPlot::CreateContext();
	}

	void CAGDLayer::OnDetach()
	{
		ImPlot::DestroyContext();
	}

	void CAGDLayer::OnUpdate(Timestep timestep)
	{
		if (m_Data.need_update)
		{
			m_Data.need_update = false;

			m_Data.inter_x.resize(10000);

			for (uint32_t i = 0; i < 10000; i++)
			{
				m_Data.inter_x[i] = m_Data.width.x + (double)i * m_Data.range.x / 10000.f;
			}

			std::sort(m_Data.pos.begin(), m_Data.pos.end(), [](glm::vec2 lhs, glm::vec2 rhs) {return lhs.x < rhs.x; });

			if (m_Data.pos.size() > 1)
			{
				PolynomialInterpolation polynomial(m_Data.pos);
				m_Data.polynomial = polynomial.evaluate(m_Data.inter_x);
				RBFInterpolation RBF(m_Data.pos, static_cast<double>(m_RBFd));
				m_Data.RBF = RBF.evaluate(m_Data.inter_x);
			}

		}
	}

	void CAGDLayer::OnImGuiRender()
	{
		auto* window = Application::Get().GetWindow().GetNativeWindow();

		ImGui::Begin("Homework 1");
		auto* draw_list = ImGui::GetWindowDrawList();

		if (ImGui::Button("Clear"))
		{
			m_Data.pos.clear();
			m_Data.need_update = true;
		}

		ImGui::SameLine();
		ImGui::Text("Point number: %d", m_Data.pos.size());
		ImGui::SameLine();
		ImGui::Checkbox("Polynomial", &m_EnablePolynomial);
		ImGui::SameLine();
		ImGui::Checkbox("RBF", &m_EnableRBF);
		ImGui::SameLine();
		float old_d = m_RBFd;
		ImGui::DragFloat("RBF parameter", &m_RBFd, 0.001f);
		if (std::fabs(old_d - m_RBFd) < 10e-5)
		{
			m_Data.need_update = true;
		}

		if (ImPlot::BeginPlot("Interpretation", "x", "y", ImVec2(-1, -1)))
		{
			auto selection = ImPlot::GetPlotLimits();
			m_Data.range.x = selection.X.Max - selection.X.Min;
			m_Data.range.y = selection.Y.Max - selection.Y.Min;
			m_Data.width.x = selection.X.Min;
			m_Data.width.y = selection.X.Max;

			if (std::fabs(m_Data.step - m_Data.range.x / 1000.f) > 10e-5)
			{
				m_Data.step = m_Data.range.x / 1000.f;
				m_Data.need_update = true;
			}

			uint32_t index = 0;
			ImPlotStyle& style = ImPlot::GetStyle();

			for (auto i = 0; i < m_Data.pos.size(); i++)
			{
				if (ImPlot::DragPoint(("P" + std::to_string(index++)).c_str(), &m_Data.pos[i].x, &m_Data.pos[i].y, true, ImVec4(0.f, 0.f, 0.f, -1.f), 8.f))
				{
					m_Data.need_update = true;
				}
			}

			if (m_Data.pos.size() > 1)
			{
				if (m_EnablePolynomial)
				{
					style.LineWeight = 1.f;
					ImPlot::PlotLine("Polynomial", m_Data.inter_x.data(), m_Data.polynomial.data(), (int)m_Data.polynomial.size());
				}

				if (m_EnableRBF)
				{
					style.LineWeight = 1.f;
					ImPlot::PlotLine("RBF", m_Data.inter_x.data(), m_Data.RBF.data(), (int)m_Data.RBF.size());
				}
			}

			if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyCtrl)
			{
				ImPlotPoint pt = ImPlot::GetPlotMousePos();
				m_Data.pos.push_back({pt.x, pt.y});
				m_Data.need_update = true;
			}

			ImPlot::EndPlot();
		}

		ImGui::End();
	}

	void CAGDLayer::OnEvent(Event& event)
	{

	}
}