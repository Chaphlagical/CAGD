#pragma once

#include <Renderer/mesh.h>

namespace Chaf
{
	class LoopSubdivision
	{
	public:
		static Ref<TriMesh> subdivision(const Ref<TriMesh>& mesh);
	};
}