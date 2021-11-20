#pragma once

#include <Editor/application.h>

#include <glm/glm.hpp>

__pragma(warning(push, 0))
#include <Eigen/Eigen>
__pragma(warning(pop))

#include <Scene/components.h>

struct ImPlotPoint;

namespace Chaf
{
	class HW8Layer : public Layer
	{
	public:
		HW8Layer();
		virtual ~HW8Layer() = default;

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
			glm::vec3 x = { 0.f, 0.f, 3.f };
			float d = 1.f;
			float a1 = 3;
			float b1 = 3;
			float a2 = 3;
			float b2 = 3;
			Eigen::MatrixXd T;
		}m_DrawData;

		MeshComponent* m_Mesh = nullptr;
	};
}