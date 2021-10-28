#pragma once

#include <vector>

#include <implot.h>

namespace Chaf
{
	class BSpline
	{
	public:
		enum PointType : size_t
		{
			C2 = 1,
			Line = 2,
			Sharp = 3
		};

	public:
		BSpline();

		~BSpline() = default;

		double genBasis(const std::vector<double>& T, double t, size_t i, size_t k);

		void addPoint(const ImPlotPoint& p, PointType type = PointType::Line);

		void genDeBoorPoints(std::vector<double>& x, std::vector<double>& y);

		void genBSpline(const std::vector<double>& px, const std::vector<double>& py, std::vector<double>& x, std::vector<double>& y, size_t sample = 1000);

		void clear();

	public:
		std::vector<double> Control_Points_X;
		std::vector<double> Control_Points_Y;
		std::vector<PointType> Types;

	private:
		ImPlotPoint genBSpline(const std::vector<double>& px, const std::vector<double>& py, double t);

	private:
		std::vector<double> m_T;

	};
}