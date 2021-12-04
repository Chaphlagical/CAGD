#include "LoopSubdivision.h"

#include <unordered_set>
#include <map>

namespace Chaf
{
	struct pair_hash {
		template <class T1, class T2>
		std::size_t operator () (const std::pair<T1, T2>& p) const {
			auto h1 = std::hash<T1>{}(p.first);
			auto h2 = std::hash<T2>{}(p.second);
			return h1 ^ h2;
		}
	};

	Ref<TriMesh> LoopSubdivision::subdivision(const Ref<TriMesh>& mesh)
	{
		std::vector<Vertex> raw_vertices = mesh->m_Vertices;
		std::vector<uint32_t> raw_indices = mesh->m_Indices;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		//optimize mesh
		std::unordered_map<glm::vec3, size_t> vertices_map;
		for (auto& v : raw_vertices)
		{
			vertices_map[v.m_Position] = 0;
		}

		for (auto& [key, index] : vertices_map)
		{
			index = vertices.size();
			Vertex v = {};
			v.m_Position = key;
			vertices.push_back(v);
		}

		for (auto& idx : raw_indices)
		{
			indices.push_back(vertices_map[raw_vertices[idx].m_Position]);
		}

		size_t old_vertices_count = vertices.size();
		size_t old_triangle_count = indices.size() / 3;

		std::map<uint32_t, std::unordered_set<uint32_t>> vertex_connect_map;
		std::map<uint32_t, std::vector<uint32_t>> triangle_connect_map;
		std::unordered_map<std::pair<uint32_t, uint32_t>, uint32_t, pair_hash> new_vertex_map;

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			for (size_t j = 0; j < 3; j++)
			{
				// Adding vertex connection
				vertex_connect_map[indices[i + j]].insert(indices[i + (j + 1) % 3]);
				vertex_connect_map[indices[i + (j + 1) % 3]].insert(indices[i + j]);

				// Add triangle connection
				triangle_connect_map[indices[i + j]].push_back(i / 3);

				// Adding new vertices
				uint32_t nv = 0;
				if (new_vertex_map.find(std::make_pair(indices[i + j], indices[i + (j + 1) % 3])) == new_vertex_map.end())
				{
					Vertex v;
					v.m_Position = (vertices[indices[i + j]].m_Position + vertices[indices[i + (j + 1) % 3]].m_Position) * 0.5f;
					vertices.push_back(v);
					new_vertex_map[std::make_pair(indices[i + j], indices[i + (j + 1) % 3])] = vertices.size() - 1;
					new_vertex_map[std::make_pair(indices[i + (j + 1) % 3], indices[i + j])] = vertices.size() - 1;
					nv = static_cast<uint32_t>(vertices.size() - 1);
				}
				else
				{
					nv = new_vertex_map[std::make_pair(indices[i + j], indices[i + (j + 1) % 3])];
				}

				vertex_connect_map[nv].insert(indices[i + j]);
				vertex_connect_map[nv].insert(indices[i + (j + 1) % 3]);
				triangle_connect_map[nv].push_back(i / 3); 
			}
		}

		// Update new vertex position
		// 3/8 *(A+B)+1/8*(C+D)
		//				D
		//           /   \
		//         A-n-B
		//			 \    /
		//            C
		std::vector<Vertex> new_vertices(vertices.size());
		for (uint32_t i = old_vertices_count; i < vertices.size(); i++)
		{
			Vertex new_pos = {};
			for (auto t : triangle_connect_map[i])
			{
				for (size_t j = 0; j < 3; j++)
				{
					new_pos.m_Position += 0.125f * vertices[indices[3 * t + j]].m_Position;
				}
			}

			for (auto v : vertex_connect_map[i])
			{
				new_pos.m_Position += 0.125f * vertices[v].m_Position;
			}

			new_vertices[i] = new_pos;
		}

		// Update old vertex position
		for (uint32_t i = 0; i < old_vertices_count; i++)
		{
			Vertex new_pos = {};
			size_t degree = vertex_connect_map[i].size();
			float u = 0.f;
			if (degree == 3)
			{
				u = 3.f / 16.f;
			}
			else
			{
				u = 3.f / (8.f * static_cast<float>(degree));
			}

			for (auto v : vertex_connect_map[i])
			{
				new_pos.m_Position += u * vertices[v].m_Position;
			}
			new_pos.m_Position += (1 - static_cast<float>(degree) * u) * vertices[i].m_Position;
			new_vertices[i] = new_pos;
		}

		// Reconstruct mesh
		std::vector<uint32_t> new_indices;
		for (uint32_t i = 0; i < indices.size(); i += 3)
		{
			uint32_t v0 = indices[i];
			uint32_t v1 = indices[i + 1];
			uint32_t v2 = indices[i + 2];

			uint32_t nv0 = new_vertex_map[std::make_pair(v0, v1)];
			uint32_t nv1 = new_vertex_map[std::make_pair(v1, v2)];
			uint32_t nv2 = new_vertex_map[std::make_pair(v2, v0)];

			std::vector<uint32_t> triangle_indices = {
				v0, nv0, nv2,
				nv0, v1, nv1,
				nv2, nv1, v2,
				nv1, nv2, nv0
			};

			new_indices.insert(new_indices.end(), triangle_indices.begin(), triangle_indices.end());
		}

		return CreateRef<TriMesh>(std::move(new_vertices), std::move(new_indices));
	}
}