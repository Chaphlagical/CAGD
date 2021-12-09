#pragma once

#include <Renderer/mesh.h>
#include <memory_resource>
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

	struct HalfEdge;

	struct HVertex
	{
		Vertex data;
		HalfEdge* he = nullptr;
	};

	struct EdgePair
	{
		HVertex* v1 = nullptr;
		HVertex* v2 = nullptr;

		EdgePair(HVertex* v1, HVertex* v2) :
			v1(v1), v2(v2) {}

		inline bool operator==(const EdgePair& rhs) const
		{
			return v1 == rhs.v1 && v2 == rhs.v2;
		}

		inline bool operator<(const EdgePair& rhs) const
		{
			if (v1 < rhs.v1) return true;
			if (v1 > rhs.v1) return false;

			if (v2 < rhs.v2) return true;
			if (v2 > rhs.v2) return false;

			return false;
		}
	};

	struct HFace
	{
		HalfEdge* he;
	};

	struct HalfEdge
	{
		HVertex* v = nullptr;
		HalfEdge* pair = nullptr;
		HalfEdge* next = nullptr;
		HFace* face = nullptr;
	};

	struct HEMesh
	{
		std::vector<HVertex*> vertices;
		std::vector<HalfEdge*> edges;
		std::vector<HFace*> faces;

		HEMesh() = default;

		HEMesh(TriMesh& mesh);

		~HEMesh();

		Ref<TriMesh> toTriMesh();

		HalfEdge* createEdge(HVertex* v1, HVertex* v2);

		HFace* createFace(HVertex* v1, HVertex* v2, HVertex* v3);

		template<typename T, typename... Args>
		inline T* create(Args&&...args)
		{
			T* ptr = (T*)m_pool.allocate(sizeof(T), alignof(T));
			new (ptr) T(std::forward<Args>(args)...);
			return ptr;
		}

		template<typename T>
		inline void destroy(T* ptr)
		{
			m_pool.deallocate(ptr, sizeof(T), alignof(T));
		}

	private:
		std::pmr::synchronized_pool_resource m_pool;

		std::map<EdgePair, HalfEdge*> m_edge_map;
	};
}