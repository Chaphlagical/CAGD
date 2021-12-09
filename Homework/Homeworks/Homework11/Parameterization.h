#pragma once

#include <Renderer/mesh.h>

namespace Chaf
{
	enum class ParameterizationMethod
	{
		Uniform,
		WP,
		MV,
		DH
	};

	class Parameterization
	{
	public:
		static Ref<TriMesh> parameterize(const Ref<TriMesh>& mesh, ParameterizationMethod method = ParameterizationMethod::WP);

		static std::vector<std::vector<size_t>> findBoundary(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
	};
}