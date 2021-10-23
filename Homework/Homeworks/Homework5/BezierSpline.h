#pragma once

#include <vector>

namespace Chaf
{
	class BezierSpline
	{
	public:
		enum class EndCondition : int
		{
			Natural = 0,
			Bessel = 1,
			Close = 2
		};

		enum class Parameter :int
		{
			Uniform = 0,
			Chordal = 1,
			Centripetal = 2
		};

	public:
		void createControlPoints(const std::vector<double>& input_x, const std::vector<double>& input_y, std::vector<double>& control_x, std::vector<double>& control_y);

		EndCondition end_condition = EndCondition::Natural;
		Parameter parameter = Parameter::Uniform;
		float uniform_val = 1.f;
	};
}