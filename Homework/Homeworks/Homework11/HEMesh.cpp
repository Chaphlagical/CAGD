#pragma once

#include "HEMesh.h"

#include <unordered_set>

namespace Chaf
{
	HEMesh::HEMesh(TriMesh& mesh)
	{
		// Add all vertices
		for (auto& v : mesh.m_Vertices)
		{
			HVertex* hv = create<HVertex>();
			hv->data = v;
			vertices.push_back(hv);
		}

		std::unordered_set<HalfEdge*> vertex_set;
		for (uint32_t i = 0; i < mesh.m_Indices.size(); i += 3)
		{
			uint32_t indices[3] = { mesh.m_Indices[i], mesh.m_Indices[i + 1], mesh.m_Indices[i + 2] };

			HalfEdge* he[3];

			for (uint32_t j = 0; j < 3; j++)
			{
				he[j] = createEdge(vertices[indices[j]], vertices[indices[(j + 1) % 3]]);
				if (vertex_set.find(he[j]) == vertex_set.end())
				{
					vertex_set.insert(he[j]);
					this->edges.push_back(he[j]);
				}
			}

			auto* face = createFace(vertices[indices[0]], vertices[indices[1]], vertices[indices[2]]);
			this->faces.push_back(face);
		}
	}

	HEMesh::~HEMesh()
	{
		m_pool.release();
	}

	Ref<TriMesh> HEMesh::toTriMesh()
	{
		std::vector<Vertex> vertices(this->vertices.size());
		std::vector<uint32_t> indices;
		std::unordered_map<HVertex*, uint32_t> vertex_index;
		for (uint32_t i = 0; i < vertices.size(); i++)
		{
			vertices[i] = this->vertices[i]->data;
			vertex_index[this->vertices[i]] = i;
		}
		for (uint32_t i = 0; i < faces.size(); i++)
		{
			auto* he = faces[i]->he;
			for (uint32_t j = 0; j < 3; j++)
			{
				indices.push_back(vertex_index[he->v]);
				he = he->next;
			}
		}

		return CreateRef<TriMesh>(std::move(vertices), std::move(indices));
	}

	HalfEdge* HEMesh::createEdge(HVertex* v1, HVertex* v2)
	{
		if (m_edge_map.find(EdgePair(v1, v2)) != m_edge_map.end())
		{
			return m_edge_map[EdgePair(v1, v2)];
		}

		auto he = create<HalfEdge>();
		v1->he = he;
		he->v = v2;
		if (m_edge_map.find(EdgePair(v2, v1)) != m_edge_map.end())
		{
			he->pair = m_edge_map[EdgePair(v2, v1)];
			m_edge_map[EdgePair(v2, v1)]->pair = he;
		}

		m_edge_map[EdgePair(v1, v2)] = he;

		return he;
	}

	HFace* HEMesh::createFace(HVertex* v1, HVertex* v2, HVertex* v3)
	{
		auto he1 = createEdge(v1, v2);
		auto he2 = createEdge(v2, v3);
		auto he3 = createEdge(v3, v1);

		auto face = create<HFace>();
		face->he = he1;

		he1->next = he2;
		he2->next = he3;
		he3->next = he1;

		he1->face = face;
		he2->face = face;
		he3->face = face;

		if (m_edge_map.find(EdgePair(v2, v1)) != m_edge_map.end())
		{
			he1->pair = m_edge_map[EdgePair(v2, v1)];
			m_edge_map[EdgePair(v2, v1)]->pair = he1;
		}

		if (m_edge_map.find(EdgePair(v3, v2)) != m_edge_map.end())
		{
			he2->pair = m_edge_map[EdgePair(v3, v2)];
			m_edge_map[EdgePair(v3, v2)]->pair = he2;
		}

		if (m_edge_map.find(EdgePair(v1, v3)) != m_edge_map.end())
		{
			he3->pair = m_edge_map[EdgePair(v1, v3)];
			m_edge_map[EdgePair(v1, v3)]->pair = he3;
		}

		return face;
	}
}