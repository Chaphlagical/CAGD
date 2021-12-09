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
	class HW11Layer : public Layer
	{
	public:
		HW11Layer();
		virtual ~HW11Layer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		bool m_Updated = true;

		MeshComponent* m_Mesh = nullptr;
		Ref<TriMesh> m_backup_mesh = nullptr;

		int method = 0;

		uint32_t count = 0;
	};
}