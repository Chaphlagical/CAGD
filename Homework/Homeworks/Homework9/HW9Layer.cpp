#include "HW9Layer.h"

#include "Sphere.h"
#include "Ellipsoid.h"

#include <implot.h>

#include <glm/gtc/type_ptr.hpp>

#include <Scene/scene_layer.h>
#include <Scene/scene.h>
#include <Scene/entity.h>

#include <iostream>

namespace Chaf
{
	HW9Layer::HW9Layer()
	{

	}

	void HW9Layer::OnAttach()
	{
		ImPlot::CreateContext();

		auto* scene_layer = SceneLayer::GetInstance();
		auto& scene = scene_layer->GetScene();
		auto& entity = scene->CreateEntity();
		m_Mesh = &entity.AddComponent<MeshComponent>();
		m_Material = CastRef<PhongMaterial>(entity.AddComponent<MaterialComponent>(MaterialType::Material_Phong).MaterialSrc);
		m_Material->DiffuseTexture = Texture2D::Create(std::string(PROJECT_SOURCE_DIR) + "../assets/texture/texture.png");
		auto& light = entity.AddComponent<LightComponent>();
		light.SetLight(LightType::LightType_Basic);

		m_Mesh->Mesh = CreateRef<TriMesh>();
	}

	void HW9Layer::OnDetach()
	{
		ImPlot::DestroyContext();
	}

	void HW9Layer::OnUpdate(Timestep timestep)
	{

	}

	void HW9Layer::OnImGuiRender()
	{
		auto* scene_layer = SceneLayer::GetInstance();
		auto& scene = scene_layer->GetScene();
		scene->SetLineMode(false);
		scene_layer->SetShowGrid(false);

		ImGui::Begin("Homework 9");

		if (ImGui::Checkbox("1", &m_p1))
		{
			m_Mesh->Mesh = CreateRef<TriMesh>();
			if (m_p1)m_p2 = false;
		}
		if (ImGui::Checkbox("2", &m_p2))
		{
			m_Mesh->Mesh = CreateRef<TriMesh>();
			if (m_p2)m_p1 = false;
		}

		if (m_Mesh->Mesh->GetVerticesNum() == 0)
		{
			if (m_p1)
			{
				m_Mesh->Mesh = Sphere::create(100);
			}
			if (m_p2)
			{
				m_Mesh->Mesh = Ellipsoid::create(100);
			}
		}


		ImGui::End();
	}

	void HW9Layer::OnEvent(Event& event)
	{

	}
}