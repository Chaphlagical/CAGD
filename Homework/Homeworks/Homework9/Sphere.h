#pragma once

#include <Renderer/mesh.h>

namespace Chaf
{
	class Sphere
	{
	public:
		static Ref<TriMesh> create(uint32_t sample);

	private:
		static Vertex Impl(float u, float v);
	};
}