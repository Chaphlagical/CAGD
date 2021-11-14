#pragma once

#include <Editor/application.h>

#include <glm/glm.hpp>

struct ImPlotPoint;

namespace Chaf
{
	class HW7Layer : public Layer
	{
	public:
		HW7Layer();
		virtual ~HW7Layer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		bool m_Updated = true;

		std::unordered_map<std::string, bool> m_questions;

		struct
		{
			std::vector<double> x1;
			std::vector<double> y1;
			std::vector<double> x2;
			std::vector<double> y2;
		}m_DrawData;
	};
}