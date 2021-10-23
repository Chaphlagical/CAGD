#include "BezierSpline.h"

__pragma(warning(push, 0))
#include <Eigen/Eigen>
__pragma(warning(pop))

#include <iostream>

#include <omp.h>

namespace Chaf
{
	void BezierSpline::createControlPoints(const std::vector<double>& input_x, const std::vector<double>& input_y, std::vector<double>& control_x, std::vector<double>& control_y)
	{
		size_t n = input_x.size() - 1;

		std::vector<double> delta(n);
		Eigen::MatrixXd A(3 * n + 1, 3 * n + 1);
		Eigen::MatrixXd b(3 * n + 1, 2);
		A.setZero();
		b.setZero();

		// Parameter -> delta
		switch (parameter)
		{
		case Parameter::Uniform:
			std::fill(delta.begin(), delta.end(), static_cast<double>(uniform_val));
			break;
		case Parameter::Chordal:
			for (size_t i = 0; i < n; i++)
			{
				delta[i] = sqrt((input_x[i] - input_x[i + 1]) * (input_x[i] - input_x[i + 1]) + (input_y[i] - input_y[i + 1]) * (input_y[i] - input_y[i + 1]));
			}
			break;
		case Parameter::Centripetal:
			for (size_t i = 0; i < n; i++)
			{
				delta[i] = sqrt(sqrt((input_x[i] - input_x[i + 1]) * (input_x[i] - input_x[i + 1]) + (input_y[i] - input_y[i + 1]) * (input_y[i] - input_y[i + 1])));
			}
			break;
		}

		size_t rows = 0;
		// Control point interpolation: 0~n-1
#pragma omp parallel for
		for (int64_t i = 0; i <= (int64_t)n; i++)
		{
			A(i, 3 * i) = 1;
			b(i, 0) = input_x[i];
			b(i, 1) = input_y[i];
		}

		// C1 continuity: n~2n-3
#pragma omp parallel for
		for (int64_t i = 1; i <= (int64_t)(n - 1); i++)
		{
			A(n + i , 3 * i - 1) = -delta[i];
			A(n + i , 3 * i) = delta[i - 1] + delta[i];
			A(n + i , 3 * i + 1) = -delta[i - 1];
		}

		// C2 continuity: 2n - 2  ~ 3n-5
#pragma omp parallel for
		for (int64_t i = 1; i <= (int64_t)(n - 1); i++)
		{
			A(2 * n + i - 1, 3 * i - 2) = delta[i] * delta[i];
			A(2 * n + i - 1, 3 * i - 1) = -2 * delta[i] * delta[i];
			A(2 * n + i - 1, 3 * i) = delta[i] * delta[i] - delta[i - 1] * delta[i - 1];
			A(2 * n + i - 1, 3 * i + 1) = 2 * delta[i - 1] * delta[i - 1];
			A(2 * n + i - 1, 3 * i + 2) = -delta[i - 1] * delta[i - 1];
		}

		// End condition: 3n-3, 3n-2
		switch (end_condition)
		{
		case EndCondition::Natural:
			A(3 * n - 1, 0) = 1;
			A(3 * n - 1, 1) = -2;
			A(3 * n - 1, 2) = 1;

			rows++;

			A(3 * n, 3 * n - 2) = 1;
			A(3 * n, 3 * n - 1) = -2;
			A(3 * n, 3 * n) = 1;
			break;
		case EndCondition::Bessel:
			A(3 * n - 1, 0) = -1;
			A(3 * n - 1, 1) = 1;
			b(3 * n - 1, 0) = delta[0] / 3.f * (-(delta[1] + 2 * delta[0]) / ((delta[0] + delta[1]) * delta[0]) * input_x[0] + (delta[0] + delta[1]) / (delta[0] * delta[1]) * input_x[1] - delta[0] / (delta[1] * (delta[0] + delta[1])) * input_x[2]);
			b(3 * n - 1, 1) = delta[0] / 3.f * (-(delta[1] + 2 * delta[0]) / ((delta[0] + delta[1]) * delta[0]) * input_y[0] + (delta[0] + delta[1]) / (delta[0] * delta[1]) * input_y[1] - delta[0] / (delta[1] * (delta[0] + delta[1])) * input_y[2]);

			rows++;

			A(3 * n, 3 * n - 1) = -1;
			A(3 * n, 3 * n) = 1;
			b(3 * n, 0) = delta[n - 1] / 3.f * ((delta[n - 2] + 2 * delta[n - 1]) / ((delta[n - 2] + delta[n - 1]) * delta[n - 2]) * input_x[n] - (delta[n - 2] + delta[n - 1]) / (delta[n - 2] * delta[n - 1]) * input_x[n - 1] + delta[n - 1] / (delta[n - 2] * (delta[n - 2] + delta[n - 1])) * input_x[n - 2]);
			b(3 * n, 1) = delta[n - 1] / 3.f * ((delta[n - 2] + 2 * delta[n - 1]) / ((delta[n - 2] + delta[n - 1]) * delta[n - 2]) * input_y[n] - (delta[n - 2] + delta[n - 1]) / (delta[n - 2] * delta[n - 1]) * input_y[n - 1] + delta[n - 1] / (delta[n - 2] * (delta[n - 2] + delta[n - 1])) * input_y[n - 2]);
			break;
		case EndCondition::Close:
			A(3 * n - 1, 0) = delta[0] + delta[n-1];
			A(3 * n - 1, 1) = -delta[n-1];
			A(3 * n - 1, 3*n - 1) = -delta[0];

			rows++;

			A(3 * n, 0) = delta[0]*delta[0]-delta[n-1]*delta[n-1];
			A(3 * n, 1) = 2*delta[n-1]*delta[n-1];
			A(3 * n, 2) = -delta[n-1] * delta[n-1];
			A(3 * n, 3 * n - 2) = delta[0] * delta[0];
			A(3 * n, 3 * n - 1) = -2 * delta[0] * delta[0];

			break;
		}

		Eigen::MatrixXd x = A.colPivHouseholderQr().solve(b);

		control_x.resize(3 * n + 1);
		control_y.resize(3 * n + 1);

		for (size_t i = 0; i < 3 * n + 1; i++)
		{
			control_x[i] = x(i, 0);
			control_y[i] = x(i, 1);
		}
	}
}