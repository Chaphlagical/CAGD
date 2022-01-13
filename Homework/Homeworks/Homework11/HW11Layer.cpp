#include "HW11Layer.h"

#include <implot.h>

#include "Parameterization.h"

#include <glm/gtc/type_ptr.hpp>

#include <Scene/scene_layer.h>
#include <Scene/scene.h>
#include <Scene/entity.h>

#include <iostream>

#include <Editor/FileDialog/ImFileDialog.h>

#include <glad/glad.h>

namespace Chaf
{
	HW11Layer::HW11Layer()
	{

	}

	void HW11Layer::OnAttach()
	{
		ImPlot::CreateContext();

		auto* scene_layer = SceneLayer::GetInstance();
		auto& scene = scene_layer->GetScene();
		auto& entity = scene->CreateEntity();
		m_Mesh = &entity.AddComponent<MeshComponent>();

		m_Mesh->Mesh = CreateRef<TriMesh>();

		ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
			GLuint tex;

			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			return (void*)tex;
		};
		ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
			GLuint texID = (GLuint)tex;
			glDeleteTextures(1, &texID);
		};
	}

	void HW11Layer::OnDetach()
	{
		ImPlot::DestroyContext();
	}

	void HW11Layer::OnUpdate(Timestep timestep)
	{

	}

	void HW11Layer::OnImGuiRender()
	{
		auto* scene_layer = SceneLayer::GetInstance();
		auto& scene = scene_layer->GetScene();
		scene->SetLineMode(true);
		//glEnable(GL_CULL_FACE);
		scene_layer->SetShowGrid(false);

		ImGui::Begin("Homework 11");
		
		if (ImGui::Button("Open a model"))
			ifd::FileDialog::Instance().Open("ModelOpenDialog", "Open a model", "mode file (*.obj){.obj}");

		if (ifd::FileDialog::Instance().IsDone("ModelOpenDialog")) {
			if (ifd::FileDialog::Instance().HasResult()) {
				std::string res = ifd::FileDialog::Instance().GetResult().u8string();
				m_Mesh->Mesh.reset();
				m_Mesh->Mesh = TriMesh::Create(res);
				m_backup_mesh = m_Mesh->Mesh;
				printf("OPEN[%s]\n", res.c_str());
				count = 0;
			}
			ifd::FileDialog::Instance().Close();
		}

		const char* const methods[] = { "uniform", "watchspress", "mean value", "discrete harmonic"};

		ImGui::Combo("Method", &method, methods,  4);

		if (ImGui::Button("Parameterization"))
		{
			m_Mesh->Mesh = Parameterization::parameterize(m_backup_mesh, static_cast<ParameterizationMethod>(method));
		}

		ImGui::End();
	}

	void HW11Layer::OnEvent(Event& event)
	{

	}
}