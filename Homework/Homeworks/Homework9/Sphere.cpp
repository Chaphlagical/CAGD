#include "Sphere.h"

namespace Chaf
{
	inline uint32_t Cnk(uint32_t n, uint32_t k)
	{
		if (n == k || k == 0)
		{
			return 1;
		}

		std::vector<uint32_t> dp(k + 1);
		for (uint32_t i = 0; i <= n; i++)
		{
			for (int64_t j = static_cast<int64_t>(std::min(i, k)); j >= 0; j--)
			{
				if (i == j || j == 0)
				{
					dp[j] = 1;
				}
				else
				{
					dp[j] = dp[j] + dp[j - 1];
				}
			}
		}

		return dp[k];
	}

	inline float Bernstein(uint32_t n, uint32_t i, float t)
	{
		return (float)Cnk(n, i) * std::powf(t, (float)i) * std::powf(1 - t, (float)(n - i));
	}

	Vertex Sphere::Impl(float u, float v)
	{
		glm::vec3 p[3][3];
		float w[3][3];

		p[0][0] = glm::vec3(0, 0, 1);
		p[0][1] = glm::vec3(1, 0, 1);
		p[0][2] = glm::vec3(1, 0, 0);
		p[1][0] = glm::vec3(0, 0, 1);
		p[1][1] = glm::vec3(1, 1, 1);
		p[1][2] = glm::vec3(1, 1, 0);
		p[2][0] = glm::vec3(0, 0, 1);
		p[2][1] = glm::vec3(0, 1, 1);
		p[2][2] = glm::vec3(0, 1, 0);

		w[0][0] = 1;
		w[0][1] = 1.f / sqrtf(2.f);
		w[0][2] = 1;
		w[1][0] = 1.f;
		w[1][1] = 1 / 2.f;
		w[1][2] = 1.f / sqrtf(2.f);
		w[2][0] = 1;
		w[2][1] = 1.f / sqrtf(2.f);
		w[2][2] = 1;

		glm::vec3 position = glm::vec3{ 0.f };
		float w_ = 0.f;

		for (uint32_t i = 0; i < 3; i++)
		{
			for (uint32_t j = 0; j < 3; j++)
			{
				position += Bernstein(2, i, u) * Bernstein(2, j, v) * w[i][j] * p[i][j];
				w_ += Bernstein(2, i, u) * Bernstein(2, j, v) * w[i][j];
			}
		}
		Vertex vertex;
		vertex.m_Position = position / w_;
		vertex.m_TexCoord = { u,v };
		vertex.m_Normal = vertex.m_Position;

		return vertex;
	}

	Ref<TriMesh> Sphere::create(uint32_t sample)
	{
		std::vector<Vertex> vertices1;
		std::vector<uint32_t> indices1;
		for (uint32_t i = 0; i <= sample; i++)
		{
			for (uint32_t j = 0; j <= sample; j++)
			{
				vertices1.push_back(Impl((float)i / (float)sample, (float)j / (float)sample));
			}
		}

		uint32_t indices_[6] = { 0, sample + 1, sample + 2, 0, sample + 2,1 };
		for (int k = 0; k < (sample + 1) * sample; k++)
			for (int i = 0; i < 6; i++)
				if ((k + 1) % (sample + 1) > 0)
					indices1.push_back(indices_[i] + k);

		size_t vertices_count = vertices1.size();
		size_t indices_count = indices1.size();

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		vertices.reserve(vertices_count * 8);
		indices1.reserve(indices_count * 8);

		// 1
		for (const auto& vertex : vertices1)
		{
			vertices.push_back(vertex);
		}
		for (const auto& index : indices1)
		{
			indices.push_back(index);
		}

		//2
		for (const auto& vertex : vertices1)
		{
			Vertex tmp = vertex;
			tmp.m_Position.x = -tmp.m_Position.x;

			vertices.push_back(tmp);
		}
		for (int32_t i = indices1.size() - 1; i >= 0; i--)
		{
			indices.push_back(indices1[i] + vertices_count);
		}

		//3
		for (const auto& vertex : vertices1)
		{
			Vertex tmp = vertex;
			tmp.m_Position.y = -tmp.m_Position.y;

			vertices.push_back(tmp);
		}
		for (int32_t i = indices1.size() - 1; i >= 0; i--)
		{
			indices.push_back(indices1[i] + vertices_count * 2);
		}

		//4
		for (const auto& vertex : vertices1)
		{
			Vertex tmp = vertex;
			tmp.m_Position.z = -tmp.m_Position.z;

			vertices.push_back(tmp);
		}
		for (int32_t i = indices1.size() - 1; i >= 0; i--)
		{
			indices.push_back(indices1[i] + vertices_count * 3);
		}

		//5
		for (const auto& vertex : vertices1)
		{
			Vertex tmp = vertex;
			tmp.m_Position.x = -tmp.m_Position.x;
			tmp.m_Position.y = -tmp.m_Position.y;

			vertices.push_back(tmp);
		}
		for (const auto& index : indices1)
		{
			indices.push_back(index + vertices_count * 4);
		}

		//6
		for (const auto& vertex : vertices1)
		{
			Vertex tmp = vertex;
			tmp.m_Position.x = -tmp.m_Position.x;
			tmp.m_Position.z = -tmp.m_Position.z;

			vertices.push_back(tmp);
		}
		for (const auto& index : indices1)
		{
			indices.push_back(index + vertices_count * 5);
		}

		//7
		for (const auto& vertex : vertices1)
		{
			Vertex tmp = vertex;
			tmp.m_Position.y = -tmp.m_Position.y;
			tmp.m_Position.z = -tmp.m_Position.z;

			vertices.push_back(tmp);
		}
		for (const auto& index : indices1)
		{
			indices.push_back(index + vertices_count * 6);
		}

		//8
		for (const auto& vertex : vertices1)
		{
			Vertex tmp = vertex;
			tmp.m_Position.x = -tmp.m_Position.x;
			tmp.m_Position.y = -tmp.m_Position.y;
			tmp.m_Position.z = -tmp.m_Position.z;

			vertices.push_back(tmp);
		}
		for (int32_t i = indices1.size() - 1; i >= 0; i--)
		{
			indices.push_back(indices1[i] + vertices_count * 7);
		}

		return TriMesh::Create(vertices, indices);
	}
}