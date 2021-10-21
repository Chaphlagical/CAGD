#include "BezierSpline.h"

#include <Eigen/Eigen>

namespace Chaf
{
	void BezierSpline::createControlPoints(const std::vector<double>& input_x, const std::vector<double>& input_y, std::vector<double>& control_x, std::vector<double>& control_y)
	{
		size_t n = input_x.size();

		std::vector<double> delta(n - 1);
		Eigen::MatrixXd A(3 * n + 1, 3 * n + 1);
		Eigen::MatrixXd b(3 * n + 1, 2);
		A.setZero();
		b.setZero();

		// Parameter -> delta
		switch (parameter)
		{
		case Parameter::Uniform:
			std::fill(delta.begin(), delta.end(), 1.0);
			break;
		case Parameter::Chordal:
			for (size_t i = 0; i < n - 1; i++)
			{
				delta[i] = sqrt((input_x[i] - input_x[i + 1]) * (input_x[i] - input_x[i + 1]) + (input_y[i] - input_y[i + 1]) * (input_y[i] - input_y[i + 1]));
			}
			break;
		case Parameter::Centripetal:
			for (size_t i = 0; i < n - 1; i++)
			{
				delta[i] = sqrt(sqrt((input_x[i] - input_x[i + 1]) * (input_x[i] - input_x[i + 1]) + (input_y[i] - input_y[i + 1]) * (input_y[i] - input_y[i + 1])));
			}
			break;
		}

		// Control point interpolation: 0~n-1
		for (size_t i = 0; i < n; i++)
		{
			A(i, 3 * i) = 1;
			b(i, 0) = input_x[i];
			b(i, 1) = input_y[i];
		}

		// C1 continuity: n~2n-3
		for (size_t i = 1; i < n - 1; i++)
		{
			A(n + i - 1, 3 * i - 1) = -delta[i];
			A(n + i - 1, 3 * i) = delta[i - 1] + delta[i];
			A(n + i - 1, 3 * i + 1) = -delta[i - 1];
		}

		// C2 continuity: 2n - 2  ~ 3n-4
		for (size_t i = 0; i < n - 1; i++)
		{
			A(2 * n + i - 2, 3 * i - 2) = delta[i] * delta[i];
			A(2 * n + i - 2, 3 * i - 1) = 2 * delta[i] * delta[i];
			A(2 * n + i - 2, 3 * i) = delta[i] * delta[i] - delta[i - 1] * delta[i - 1];
			A(2 * n + i - 2, 3 * i + 1) = 2 * delta[i - 1] * delta[i - 1];
			A(2 * n + i - 2, 3 * i + 2) = delta[i - 1] * delta[i - 1];
		}

		// End condition: 3n-3, 3n-2
		switch (end_condition)
		{
		case EndCondition::Natural:
			A(3 * n - 3, 0) = 1;
			A(3 * n - 3, 1) = -2;
			A(3 * n - 3, 2) = 1;

			A(3 * n, 3 * n - 2) = 1;
			A(3 * n, 3 * n - 1) = -2;
			A(3 * n, 3 * n) = 1;
			break;
		case EndCondition::Bessel:


			break;
		case EndCondition::Close:
			break;
		}

		Eigen::VectorXd x = A.colPivHouseholderQr().solve(b);

		control_x.resize(3 * n + 1);
		control_y.resize(3 * n + 1);

		for (size_t i = 0; i < 3 * n + 1; i++)
		{
			control_x[i] = x(i, 0);
			control_y[i] = x(i, 1);
		}
	}
}