#include "Parameterization.h"

#include <unordered_set>

#include <Eigen/Eigen>

#include <iostream>

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

	Ref<TriMesh> Parameterization::parameterize(const Ref<TriMesh>& mesh, ParameterizationMethod method)
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

		std::vector<std::unordered_set<size_t>> neigborhood(vertices.size());
		std::unordered_map<std::pair<size_t, size_t>, size_t, pair_hash> triangle_map;
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			neigborhood[indices[i]].insert(indices[i + 1]);
			neigborhood[indices[i + 1]].insert(indices[i + 2]);
			neigborhood[indices[i + 2]].insert(indices[i]);
			triangle_map[std::make_pair(indices[i], indices[i + 1])] = i;
			triangle_map[std::make_pair(indices[i + 1], indices[i + 2])] = i;
			triangle_map[std::make_pair(indices[i + 2], indices[i])] = i;
		}

		auto& boundaries = findBoundary(vertices, indices);
		if (boundaries.empty())
		{
			return mesh;
		}
		auto& boundary = boundaries[0];

		Eigen::SparseMatrix<float> Laplace_matrix(vertices.size(), vertices.size());
		Eigen::MatrixXf b(vertices.size(), 2);

		Laplace_matrix.setZero();
		b.setZero();

		std::vector<Eigen::Triplet<float>> Lij;

		for (size_t i = 0; i < vertices.size(); i++)
		{
			Lij.push_back(Eigen::Triplet<float>(i, i, 1.f));

			if (std::find(boundary.begin(), boundary.end(), i) == boundary.end())
			{
				std::unordered_map<size_t, float> wi;
				for (auto& idx : neigborhood[i])
				{
					if (method == ParameterizationMethod::Uniform)
					{
						wi[idx] = 1.f;
					}
					else
					{
						size_t t1 = triangle_map[std::make_pair(i, idx)];
						size_t t2 = triangle_map[std::make_pair(idx, i)];

						glm::vec3 v = vertices[i].m_Position;
						glm::vec3 vi = vertices[idx].m_Position;

						glm::vec3 vi1, vi2;

						for (size_t k = 0; k < 3; k++)
						{
							if (vertices[indices[t1 + k]].m_Position != v && vertices[indices[t1 + k]].m_Position != vi)
							{
								vi1 = vertices[indices[t1 + k]].m_Position;
							}
							if (vertices[indices[t2 + k]].m_Position != v && vertices[indices[t2 + k]].m_Position != vi)
							{
								vi2 = vertices[indices[t2 + k]].m_Position;
							}
						}

						if (method == ParameterizationMethod::WP)
						{
							float r = glm::length(v - vi);
							float cos1 = glm::dot(v - vi, vi1 - vi);
							float cos2 = glm::dot(v - vi, vi2 - vi);

							float cot1 = cos1 / (sqrtf(1 - cos1 * cos1));
							float cot2 = cos2 / (sqrtf(1 - cos2 * cos2));

							wi[idx] = (cot1 + cot2) / (r * r);
						}
						else if (method == ParameterizationMethod::MV)
						{
							float r = glm::length(v - vi);
							float cos1 = glm::dot(v - vi, v - vi1);
							float cos2 = glm::dot(v - vi, v - vi2);
							float alpha1 = acosf(cos1);
							float alpha2 = acosf(cos2);

							wi[idx] = (tanf(alpha1 / 2.f) + tanf(alpha2 / 2.f)) / r;
						}
						else if (method == ParameterizationMethod::DH)
						{
							float cos1 = glm::dot(vi1 - v, vi1 - vi);
							float cos2 = glm::dot(vi2 - v, vi2 - vi);
							float alpha1 = acosf(cos1);
							float alpha2 = acosf(cos2);

							wi[idx] = (1.f / tanf(alpha1) + tanf(1.f / alpha2));
						}
					}
				}
				float wi_sum = 0.f;
				for (auto& [idx, w] : wi)
				{
					wi_sum += w;
				}
				for (auto& [idx, w] : wi)
				{
					Lij.push_back(Eigen::Triplet<float>(i, idx, -w / wi_sum));
				}
			}
		}

		for (uint32_t i = 0; i < boundary.size(); i++)
		{
			b(boundary[i], 0) = std::cos((float)i * glm::pi<float>() * 2.f / static_cast<float>(boundary.size()));
			b(boundary[i], 1) = std::sin((float)i * glm::pi<float>() * 2.f / static_cast<float>(boundary.size()));
		}

		Laplace_matrix.setFromTriplets(Lij.begin(), Lij.end());

		Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;

		solver.compute(Laplace_matrix);

		Eigen::MatrixXf result = solver.solve(b);

		std::vector<Vertex> new_vertices(vertices.size());

		for (size_t i = 0; i < vertices.size(); i++)
		{
			new_vertices[i].m_Position.x = result(i, 0);
			new_vertices[i].m_Position.y = result(i, 1);
			new_vertices[i].m_Position.z = 0.f;
		}

		return CreateRef<TriMesh>(std::move(new_vertices), std::move(indices));
	}

	std::vector<std::vector<size_t>> Parameterization::findBoundary(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		std::vector<std::vector<size_t>> mesh_graph(vertices.size(), std::vector<size_t>(vertices.size()));
		for (auto& g : mesh_graph)
		{
			std::fill(g.begin(), g.end(), 0);
		}

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			mesh_graph[indices[i]][indices[i + 1]]++;
			mesh_graph[indices[i + 1]][indices[i]] ++;
			mesh_graph[indices[i + 1]][indices[i + 2]] ++;
			mesh_graph[indices[i + 2]][indices[i + 1]] ++;
			mesh_graph[indices[i + 2]][indices[i]]++;
			mesh_graph[indices[i]][indices[i + 2]] ++;
		}

		std::unordered_set<size_t> found;
		std::vector<std::vector<size_t>> boundaries;

		for (size_t i = 0; i < vertices.size(); i++)
		{
			size_t currect_vertex = i;
			std::vector<size_t> boundary;
			while (true)
			{
				bool has = false;
				for (size_t j = 0; j < vertices.size(); j++)
				{
					if (j != currect_vertex && mesh_graph[currect_vertex][j] == 1 && found.find(j) == found.end())
					{
						found.insert(j);
						boundary.push_back(j);
						currect_vertex = j;
						has = true;
					}
				}
				if (currect_vertex == i || !has)
				{
					break;
				}
			}
			if (!boundary.empty())
			{
				boundaries.push_back(boundary);
			}
		}

		std::sort(boundaries.begin(), boundaries.end(), [](const std::vector<size_t>& lhs, const std::vector<size_t>& rhs) {return lhs.size() > rhs.size(); });
		return boundaries;
	}
}