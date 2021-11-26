#include "HW8Layer.h"

#include <implot.h>

#include "../Homework3/BezierCurve.h"

#include <glm/gtc/type_ptr.hpp>

#include <Scene/scene_layer.h>
#include <Scene/scene.h>
#include <Scene/entity.h>

#include <iostream>

namespace Chaf
{
	HW8Layer::HW8Layer()
	{

	}

	void HW8Layer::OnAttach()
	{
		ImPlot::CreateContext();

		m_questions = {
			{"1", false},
			{"2-1", false},
			{"2-2", false},
			{"3-1", false},
			{"3-2", false}
		};
		m_DrawData.T.resize(3, 101);
		for (uint32_t i = 0; i <= 100; i++)
		{
			double t = static_cast<double>(i) / 100.0;
			m_DrawData.T(0, i) = (1 - t) * (1 - t);
			m_DrawData.T(1, i) = 2 * t * (1 - t);
			m_DrawData.T(2, i) = t * t;
		}

		auto* scene_layer = SceneLayer::GetInstance();
		auto& scene = scene_layer->GetScene();
		m_Mesh = &scene->CreateEntity().AddComponent<MeshComponent>();
	}

	void HW8Layer::OnDetach()
	{
		ImPlot::DestroyContext();
	}

	void HW8Layer::OnUpdate(Timestep timestep)
	{

	}

	void HW8Layer::OnImGuiRender()
	{
		auto* scene_layer = SceneLayer::GetInstance();
		auto& scene = scene_layer->GetScene();
		scene->SetLineMode(true);
		scene_layer->SetShowGrid(false);

		ImGui::Begin("Homework 8");

		for (auto& [key, val] : m_questions)
		{
			if (ImGui::Checkbox(key.c_str(), &val))
			{
				m_Mesh->Reset();
			}
			ImGui::SameLine();
		}
		ImGui::NewLine();
		if (m_questions["1"])
		{
			ImGui::DragFloat3("x", glm::value_ptr(m_DrawData.x), 0.1f, -std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), "%.2f");
			ImGui::DragFloat("d", &m_DrawData.d, 0.1f, 0.f, std::numeric_limits<float>::max(), "%.2f");
		}
		if (m_questions["2-1"] || m_questions["3-1"])
		{
			ImGui::PushID(2);
			if (ImGui::DragFloat("ellipse a", &m_DrawData.a1, 0.1f, 0.f, std::numeric_limits<float>::max(), "%.2f"))
			{
				m_Mesh->Reset();
			}

			if (ImGui::DragFloat("ellipse b", &m_DrawData.b1, 0.1f, 0.f, std::numeric_limits<float>::max(), "%.2f"))
			{
				m_Mesh->Reset();
			}
			ImGui::PopID();
		}

		if (m_questions["2-2"] || m_questions["3-2"])
		{
			ImGui::PushID(2);
			if(ImGui::DragFloat("hyperbola a", &m_DrawData.a2, 0.1f, 0.f, std::numeric_limits<float>::max(), "%.2f"))
				m_Mesh->Reset();

			if(ImGui::DragFloat("hyperbola b", &m_DrawData.b2, 0.1f, 0.f, std::numeric_limits<float>::max(), "%.2f"))
				m_Mesh->Reset();
			ImGui::PopID();
		}

		if (ImPlot::BeginPlot("Curve", "x", "y", ImVec2(-1, -1), ImPlotFlags_AntiAliased))
		{
			ImPlotStyle& style = ImPlot::GetStyle();

			style.LineWeight = 4.f;
			style.MarkerSize = 6.f;

			if (m_questions["1"])
			{
				Eigen::MatrixXf X(8, 4);
				X(0, 0) = 1; X(0, 1) = 1; X(0, 2) = -1; X(0, 3) = 1;
				X(1, 0) = -1; X(1, 1) = 1; X(1, 2) = -1; X(1, 3) = 1;
				X(2, 0) = 1; X(2, 1) = -1; X(2, 2) = -1; X(2, 3) = 1;
				X(3, 0) = -1; X(3, 1) = -1; X(3, 2) = -1; X(3, 3) = 1;
				X(4, 0) = 1; X(4, 1) = 1; X(4, 2) = 1; X(4, 3) = 1;
				X(5, 0) = -1; X(5, 1) = 1; X(5, 2) = 1; X(5, 3) = 1;
				X(6, 0) = 1; X(6, 1) = -1; X(6, 2) = 1; X(6, 3) = 1;
				X(7, 0) = -1; X(7, 1) = -1; X(7, 2) = 1; X(7, 3) = 1;

				Eigen::MatrixXf M(4, 4);
				M(0, 0) = m_DrawData.d; M(0, 1) = 0; M(0, 2) = 0; M(0, 3) = m_DrawData.x.x;
				M(1, 0) = 0; M(1, 1) = m_DrawData.d; M(1, 2) = 0; M(1, 3) = m_DrawData.x.y;
				M(2, 0) = 0; M(2, 1) = 0; M(2, 2) = m_DrawData.d; M(2, 3) = m_DrawData.x.z;
				M(3, 0) = 0; M(3, 1) = 0; M(3, 2) = 0; M(3, 3) = 1;

				Eigen::MatrixXf P(4, 4);
				P.setZero();
				P(0, 0) = 1; P(1, 1) = 1; P(2, 2) = 1; P(3, 2) = 1;

				Eigen::MatrixXf V(4, 4);
				V.setZero();
				V(0, 0) = 1; V(1, 1) = 1; V(2, 2) = -1; V(3, 3) = 1;

				Eigen::MatrixXf X_ = (P * V * M * X.transpose()).transpose();
				std::vector<float> result_x;
				std::vector<float> result_y;
				for (uint32_t i = 0; i < 8; i++)
				{
					result_x.push_back(X_(i, 0) / X_(i, 3));
					result_y.push_back(X_(i, 1) / X_(i, 3));
				}
				std::vector<float> draw_x1 = { result_x[0], result_x[1], result_x[3], result_x[2], result_x[0] };
				std::vector<float> draw_y1 = { result_y[0], result_y[1], result_y[3], result_y[2], result_y[0] };

				std::vector<float> draw_x2 = { result_x[4], result_x[5], result_x[7], result_x[6], result_x[4] };
				std::vector<float> draw_y2 = { result_y[4], result_y[5], result_y[7], result_y[6], result_y[4] };

				std::vector<float> draw_x3 = { result_x[0], result_x[4] };
				std::vector<float> draw_y3 = { result_y[0], result_y[4] };

				std::vector<float> draw_x4 = { result_x[1], result_x[5] };
				std::vector<float> draw_y4 = { result_y[1], result_y[5] };

				std::vector<float> draw_x5 = { result_x[2], result_x[6] };
				std::vector<float> draw_y5 = { result_y[2], result_y[6] };

				std::vector<float> draw_x6 = { result_x[3], result_x[7] };
				std::vector<float> draw_y6 = { result_y[3], result_y[7] };

				ImPlot::PlotLine("1", draw_x1.data(), draw_y1.data(), 5);
				ImPlot::PlotLine("1", draw_x2.data(), draw_y2.data(), 5);
				ImPlot::PlotLine("1", draw_x3.data(), draw_y3.data(), 2);
				ImPlot::PlotLine("1", draw_x4.data(), draw_y4.data(), 2);
				ImPlot::PlotLine("1", draw_x5.data(), draw_y5.data(), 2);
				ImPlot::PlotLine("1", draw_x6.data(), draw_y6.data(), 2);
			}

			if (m_questions["2-1"])
			{
				Eigen::MatrixXd P(3, 3);
				P(0, 0) = (double)m_DrawData.a1; P(0, 1) = (double)m_DrawData.a1; P(0, 2) = 0.0;
				P(1, 0) = 0.0; P(1, 1) = (double)m_DrawData.b1; P(1, 2) = 2 * (double)m_DrawData.b1;
				P(2, 0) = 1.0; P(2, 1) = 1.0; P(2, 2) = 2.0;

				Eigen::MatrixXd X = P * m_DrawData.T;

				P(0, 1) = -(double)m_DrawData.a1; P(1, 1) = -(double)m_DrawData.b1; P(2, 1) = -1.0;

				Eigen::MatrixXd X_ = P * m_DrawData.T;

				std::vector<double> result_x;
				std::vector<double> result_y;
				std::vector<double> result_x_;
				std::vector<double> result_y_;
				for (uint32_t i = 0; i <= 100; i++)
				{
					result_x.push_back(X(0, i) / X(2, i));
					result_y.push_back(X(1, i) / X(2, i));
				}
				for (uint32_t i = 0; i <= 100; i++)
				{
					result_x_.push_back(X_(0, i) / X_(2, i));
					result_y_.push_back(X_(1, i) / X_(2, i));
				}

				ImPlot::PlotLine("2-1", result_x.data(), result_y.data(), (int)result_x.size());
				ImPlot::PlotLine("2-1", result_x_.data(), result_y_.data(), (int)result_x.size());
			}

			if (m_questions["2-2"])
			{
				Eigen::MatrixXd P(3, 3);
				P(0, 0) = (double)m_DrawData.a2; P(0, 1) = (double)m_DrawData.a2; P(0, 2) = 2 * (double)m_DrawData.a2;
				P(1, 0) = 0.0; P(1, 1) = (double)m_DrawData.b2; P(1, 2) = 2 * (double)m_DrawData.b2;
				P(2, 0) = 1.0; P(2, 1) = 1.0; P(2, 2) = 0.0;

				Eigen::MatrixXd X = P * m_DrawData.T;

				P(0, 1) = -(double)m_DrawData.a2; P(1, 1) = -(double)m_DrawData.b2; P(2, 1) = -1.0;

				Eigen::MatrixXd X_ = P * m_DrawData.T;

				std::vector<double> result_x;
				std::vector<double> result_y;
				std::vector<double> result_x_1;
				std::vector<double> result_y_1;
				std::vector<double> result_x_2;
				std::vector<double> result_y_2;
				for (uint32_t i = 0; i <= 100; i++)
				{
					if (X(2, i) != 0.0)
					{
						result_x.push_back(X(0, i) / X(2, i));
						result_y.push_back(X(1, i) / X(2, i));
					}
				}
				for (uint32_t i = 0; i <= 100; i++)
				{
					if (X_(0, i) / X_(2, i) > 0)
					{
						if (X(2, i) != 0.0)
						{
							result_x_1.push_back(X_(0, i) / X_(2, i));
							result_y_1.push_back(X_(1, i) / X_(2, i));
						}
					}
					else
					{
						if (X(2, i) != 0.0)
						{
							result_x_2.push_back(X_(0, i) / X_(2, i));
							result_y_2.push_back(X_(1, i) / X_(2, i));
						}
					}
				}

				ImPlot::PlotLine("2-2", result_x.data(), result_y.data(), (int)result_x.size());
				ImPlot::PlotLine("2-2", result_x_1.data(), result_y_1.data(), (int)result_x.size());
				ImPlot::PlotLine("2-2", result_x_2.data(), result_y_2.data(), (int)result_x.size());
			}

			if (m_questions["3-1"])
			{
				Eigen::MatrixXd P(3, 3);
				P(0, 0) = (double)m_DrawData.a1; P(0, 1) = (double)m_DrawData.a1; P(0, 2) = 0.0;
				P(1, 0) = 0.0; P(1, 1) = (double)m_DrawData.b1; P(1, 2) = 2 * (double)m_DrawData.b1;
				P(2, 0) = 1.0; P(2, 1) = 1.0; P(2, 2) = 2.0;

				Eigen::MatrixXd X = P * m_DrawData.T;

				P(0, 1) = -(double)m_DrawData.a1; P(1, 1) = -(double)m_DrawData.b1; P(2, 1) = -1.0;

				Eigen::MatrixXd X_ = P * m_DrawData.T;

				std::vector<Vertex> vertices;
				std::vector<uint32_t> indices;

				for (uint32_t i = 0; i <= 100; i++)
				{
					Vertex vertex;
					vertex.m_Position = { X(0,i), X(1,i), X(2,i) };
					vertices.push_back(vertex);
				}
				for (uint32_t i = 0; i <= 100; i++)
				{
					Vertex vertex;
					vertex.m_Position = { X_(0,i), X_(1,i), X_(2,i) };
					vertices.push_back(vertex);
				}

				for (uint32_t i = 0; i < 100; i++)
				{
					indices.push_back(i);
					indices.push_back(i + 1);
					indices.push_back(i);
				}

				for (uint32_t i = 0; i < 100; i++)
				{
					indices.push_back(i+ 101);
					indices.push_back(i + 102);
					indices.push_back(i+ 101);
				}

				auto* scene_layer = SceneLayer::GetInstance();
				auto& scene = scene_layer->GetScene();

				if (m_Mesh->Mesh->GetVerticesNum() == 0)
				{
					m_Mesh->Mesh = TriMesh::Create(vertices, indices);
				}
			}

			if (m_questions["3-2"])
			{
				Eigen::MatrixXd P(3, 3);
				P(0, 0) = (double)m_DrawData.a2; P(0, 1) = (double)m_DrawData.a2; P(0, 2) = 2 * (double)m_DrawData.a2;
				P(1, 0) = 0.0; P(1, 1) = (double)m_DrawData.b2; P(1, 2) = 2 * (double)m_DrawData.b2;
				P(2, 0) = 1.0; P(2, 1) = 1.0; P(2, 2) = 0.0;

				Eigen::MatrixXd X = P * m_DrawData.T;

				P(0, 1) = -(double)m_DrawData.a2; P(1, 1) = -(double)m_DrawData.b2; P(2, 1) = -1.0;

				Eigen::MatrixXd X_ = P * m_DrawData.T;

				std::vector<Vertex> vertices;
				std::vector<uint32_t> indices;

				for (uint32_t i = 0; i <= 100; i++)
				{
					Vertex vertex;
					vertex.m_Position = { X(0,i), X(1,i), X(2,i) };
					vertices.push_back(vertex);
				}
				for (uint32_t i = 0; i <= 100; i++)
				{
					Vertex vertex;
					vertex.m_Position = { X_(0,i), X_(1,i), X_(2,i) };
					vertices.push_back(vertex);
				}

				for (uint32_t i = 0; i < 100; i++)
				{
					indices.push_back(i);
					indices.push_back(i + 1);
					indices.push_back(i);
				}

				for (uint32_t i = 0; i < 100; i++)
				{
					indices.push_back(i + 101);
					indices.push_back(i + 102);
					indices.push_back(i + 101);
				}

				if (m_Mesh->Mesh->GetVerticesNum() == 0)
				{
					m_Mesh->Mesh = TriMesh::Create(vertices, indices);
				}
			}

			ImPlot::EndPlot();
		}

		ImGui::End();
	}

	void HW8Layer::OnEvent(Event& event)
	{

	}
}