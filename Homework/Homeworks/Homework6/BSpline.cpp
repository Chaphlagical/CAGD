#include "BSpline.h"

__pragma(warning(push, 0))
#include <Eigen/Eigen>
__pragma(warning(pop))

#include <iostream>

#include <omp.h>

namespace Chaf
{
	BSpline::BSpline()
	{
		m_T = { 0, 0, 0 };
	}

	double BSpline::genBasis(const std::vector<double>& T, double t, size_t i, size_t k)
	{
		if (k == 1)
		{
			if ((t >= T[i] && t < T[i + 1]) || (t >= T[i] && t <= T[i + 1] && T[i + 1] == T.back()))
			{
				return 1.0;
			}
			else
			{
				return 0.0;
			}
		}
		double left = 0.0;
		left = T[i + k - 1] - T[i] == 0.0 ? 0.0 : (t - T[i]) / (T[i + k - 1] - T[i]) * genBasis(T, t, i, k - 1);

		double right = 0.0;
		right = T[i + k] - T[i + 1] == 0.0 ? 0.0 : (T[i + k] - t) / (T[i + k] - T[i + 1]) * genBasis(T, t, i + 1, k - 1);

		return  left + right;
	}

	void BSpline::addPoint(const ImPlotPoint& p, PointType type)
	{
		Types.push_back(type);
		Control_Points_X.push_back(p.x);
		Control_Points_Y.push_back(p.y);
	}

	void BSpline::genDeBoorPoints(std::vector<double>& x, std::vector<double>& y)
	{
		std::vector<size_t> indices = { 1 };
		std::vector<double> s;
		for (size_t i = 0; i < Types.size(); i++)
		{
			indices.push_back(Types[i] + indices.back());
			s.push_back(static_cast<double>(i));
		}

		m_T.clear();
		m_T.push_back(0);
		m_T.push_back(0);
		m_T.push_back(0);
		for (size_t i = 0; i < Types.size(); i++)
		{
			for (size_t k = 0; k < Types[i]; k++)
			{
				m_T.push_back(static_cast<double>(i));
			}
		}
		m_T.push_back(static_cast<double>(Types.size() - 1));
		m_T.push_back(static_cast<double>(Types.size() - 1));
		m_T.push_back(static_cast<double>(Types.size() - 1));

		size_t n = m_T.size() - 4;

		x.resize(n);
		y.resize(n);
		Eigen::MatrixXd A(n, n);
		Eigen::MatrixXd b(n, 2);


		A.setZero();
		b.setZero();

		// Begin
		switch (Types[0])
		{
		case PointType::C2:
			A(0, 0) = 1;

			A(1, 0) = 2;
			A(1, 1) = -3;
			A(1, 2) = 1;

			b(0, 0) = Control_Points_X[0];
			b(0, 1) = Control_Points_Y[0];
			break;
		case PointType::Line:
			A(0, 0) = 1;

			A(1, 0) = 2;
			A(1, 1) = -3;
			A(1, 2) = 1;

			A(2, 2) = 1;

			b(0, 0) = Control_Points_X[0];
			b(0, 1) = Control_Points_Y[0];

			b(2, 0) = Control_Points_X[0];
			b(2, 1) = Control_Points_Y[0];
			break;
		case PointType::Sharp:
			A(0, 0) = 1;

			A(1, 0) = 2;
			A(1, 1) = -3;
			A(1, 2) = 1;

			A(2, 2) = 1;

			A(3, 3) = 1;

			b(0, 0) = Control_Points_X[0];
			b(0, 1) = Control_Points_Y[0];

			b(2, 0) = Control_Points_X[0];
			b(2, 1) = Control_Points_Y[0];

			b(3, 0) = Control_Points_X[0];
			b(3, 1) = Control_Points_Y[0];
			break;
		default:
			break;
		}

		// Inner
#pragma omp parallel for
		for (int i = 1; i < Types.size() - 1; i++)
		{
			size_t index = indices[i];

			if (Types[i] == PointType::C2)
			{
				A(index, index - 1) = genBasis(m_T, s[i], index - 1, 4);
				A(index, index) = genBasis(m_T, s[i], index, 4);
				A(index, index + 1) = genBasis(m_T, s[i], index + 1, 4);
				b(index, 0) = Control_Points_X[i];
				b(index, 1) = Control_Points_Y[i];
			}
			else if (Types[i] == PointType::Line)
			{
				if (Types[i - 1] != PointType::Line)
				{
					A(index, index) = 2;
					A(index, index + 1) = -3;
					A(index, index + 2) = 1;

					A(index + 1, index + 1) = 1;

					b(index + 1, 0) = Control_Points_X[i];
					b(index + 1, 1) = Control_Points_Y[i];
				}
				else if (Types[i + 1] != PointType::Line)
				{
					A(index, index) = 1;

					A(index + 1, index - 1) = 1;
					A(index + 1, index) = -3;
					A(index + 1, index + 1) = 2;

					b(index, 0) = Control_Points_X[i];
					b(index, 1) = Control_Points_Y[i];
				}
				else
				{
					A(index, index) = 1;

					A(index + 1, index + 1) = 1;

					b(index, 0) = Control_Points_X[i];
					b(index, 1) = Control_Points_Y[i];

					b(index + 1, 0) = Control_Points_X[i];
					b(index + 1, 1) = Control_Points_Y[i];
				}
			}
			else if (Types[i] == PointType::Sharp)
			{
				A(index, index - 1) = 2;
				A(index, index) = -3;
				A(index, index + 1) = 1;

				A(index + 1, index + 1) = 1;

				A(index + 2, index + 1) = 1;
				A(index + 2, index + 2) = -3;
				A(index + 2, index + 3) = 2;

				b(index + 1, 0) = Control_Points_X[i];
				b(index + 1, 1) = Control_Points_Y[i];
			}
		}


		// End
		switch (Types.back())
		{
		case PointType::C2:
			A(n - 2, n - 3) = 1;
			A(n - 2, n - 2) = -3;
			A(n - 2, n - 1) = 2;

			A(n - 1, n - 1) = 1;

			b(n - 1, 0) = Control_Points_X.back();
			b(n - 1, 1) = Control_Points_Y.back();
			break;
		case PointType::Line:
			A(n - 3, n - 3) = 1;

			A(n - 2, n - 3) = 1;
			A(n - 2, n - 2) = -3;
			A(n - 2, n - 1) = 2;

			A(n - 1, n - 1) = 1;

			b(n - 3, 0) = Control_Points_X.back();
			b(n - 3, 1) = Control_Points_Y.back();

			b(n - 1, 0) = Control_Points_X.back();
			b(n - 1, 1) = Control_Points_Y.back();
			break;
		case PointType::Sharp:
			A(n - 4, n - 4) = 1;

			A(n - 3, n - 3) = 1;

			A(n - 2, n - 1) = 2;
			A(n - 2, n - 2) = -3;
			A(n - 2, n - 3) = 1;

			A(n - 1, n - 1) = 1;

			b(n - 4, 0) = Control_Points_X.back();
			b(n - 4, 1) = Control_Points_Y.back();

			b(n - 3, 0) = Control_Points_X.back();
			b(n - 3, 1) = Control_Points_Y.back();

			b(n - 1, 0) = Control_Points_X.back();
			b(n - 1, 1) = Control_Points_Y.back();
		default:
			break;
		}

		Eigen::MatrixXd res = A.colPivHouseholderQr().solve(b);

		for (size_t i = 0; i < x.size(); i++)
		{
			x[i] = res(i, 0);
			y[i] = res(i, 1);
		}
	}

	void BSpline::genBSpline(const std::vector<double>& px, const std::vector<double>& py, std::vector<double>& x, std::vector<double>& y, size_t sample)
	{
		x.resize(sample+1);
		y.resize(sample+1);

#pragma omp parallel for
		for (int i = 0; i <= sample; i++)
		{
			double t = m_T.back() / static_cast<double>(sample) * static_cast<double>(i);
			auto p = genBSpline(px, py, t);
			x[i] = p.x;
			y[i] = p.y;
		}
	}

	void BSpline::clear()
	{
		m_T.clear();
		Control_Points_X.clear();
		Control_Points_Y.clear();
		Types.clear();
	}

	ImPlotPoint BSpline::genBSpline(const std::vector<double>& px, const std::vector<double>& py, double t)
	{
		ImPlotPoint result = { 0.0, 0.0 };
		for (size_t i = 0; i < px.size(); i++)
		{
			auto basis = genBasis(m_T, t, i, 4);
			result.x += basis * px[i];
			result.y += basis * py[i];
		}

		return result;
	}



}