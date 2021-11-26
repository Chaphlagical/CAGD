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
	class HW9Layer : public Layer
	{
	public:
		HW9Layer();
		virtual ~HW9Layer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		bool m_Updated = true;

		MeshComponent* m_Mesh = nullptr;
		Ref<PhongMaterial> m_Material = nullptr;

		bool m_p1 = false;
		bool m_p2 = false;
	};
}