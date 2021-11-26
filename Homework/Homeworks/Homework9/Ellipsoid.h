#pragma once

#include <Renderer/mesh.h>

namespace Chaf
{
	class Ellipsoid
	{
	public:
		static Ref<TriMesh> create(uint32_t sample);

	private:
		static Vertex Impl(float u, float v);
	};
}