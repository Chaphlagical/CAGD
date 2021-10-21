#pragma once

#include <vector>

namespace Chaf
{
	class BezierSpline
	{
	public:
		enum class EndCondition
		{
			Natural,
			Bessel,
			Close
		};

		enum class Parameter
		{
			Uniform,
			Chordal,
			Centripetal
		};

	public:
		void createControlPoints(const std::vector<double>& input_x, const std::vector<double>& input_y, std::vector<double>& control_x, std::vector<double>& control_y);

		EndCondition end_condition = EndCondition::Natural;
		Parameter parameter = Parameter::Uniform;
		double uniform_val = 1;
	};
}