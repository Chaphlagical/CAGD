#include "Ellipse.h"

#include <omp.h>

#define PI 3.1415926535898

namespace Chaf
{
	Ellipse::Ellipse()
	{
		double step = 2 * PI / 10000;
		t.resize(10000);
		for (uint32_t i = 0; i < 10000; i++)
		{
			t[i]=(static_cast<double>(i) * step);
		}
	}

	void Ellipse::update()
	{
		ellipse.first.resize(t.size());
		ellipse.second.resize(t.size());

		evolute.first.resize(t.size());
		evolute.second.resize(t.size());

		std::pair< std::vector<double>, std::vector<double>>c1, c2, e1;

		c1.first.resize(t.size());
		c1.second.resize(t.size());

		c2.first.resize(t.size());
		c2.second.resize(t.size());

		e1.first.resize(t.size());
		e1.second.resize(t.size());

		e2.first.resize(t.size());
		e2.second.resize(t.size());

		k.resize(t.size());

#pragma omp parallel for
		for (int i = 0; i < 10000; i++)
		{
			ellipse.first[i]=static_cast<double>(a) * cos(t[i]);
			ellipse.second[i]=static_cast<double>(b) * sin(t[i]);

			c1.first[i]=-static_cast<double>(a) * sin(t[i]);
			c1.second[i]=static_cast<double>(b) * cos(t[i]);
			double c1_norm = sqrt(c1.first[i] * c1.first[i] + c1.second[i] * c1.second[i]);

			c2.first[i]=-static_cast<double>(a) * cos(t[i]);
			c2.second[i]=-static_cast<double>(b) * sin(t[i]);

			e1.first[i]=c1.first[i] / c1_norm;
			e1.second[i]=c1.second[i] / c1_norm;

			double dot = c2.first[i] * e1.first[i] + c2.second[i] * e1.second[i];
			e2.first[i]=c2.first[i] - dot * e1.first[i];
			e2.second[i]=c2.second[i] - dot * e1.second[i];
			double e2_norm = sqrt(e2.first[i] * e2.first[i] + e2.second[i] * e2.second[i]);
			e2.first[i] /= e2_norm;
			e2.second[i] /= e2_norm;

			double cross = c1.first[i] * c2.second[i] - c1.second[i] * c2.first[i];
			k[i]=cross / (c1_norm * c1_norm * c1_norm);

			evolute.first[i]=ellipse.first[i] + 1 / k[i] * e2.first[i];
			evolute.second[i]=ellipse.second[i] + 1 / k[i] * e2.second[i];
		}
	}

	void Ellipse::updateCurvature(uint32_t index)
	{
		curvaturecircle.first.clear();
		curvaturecircle.second.clear();

		double radius = 1 / k[index];

		double x = evolute.first[index];
		double y = evolute.second[index];

		curvaturecircle.first.resize(10000);
		curvaturecircle.second.resize(10000);

#pragma omp parallel for
		for (int i = 0; i < 10000; i++)
		{
			curvaturecircle.first[i]=x + cos(t[i]) / k[index];
			curvaturecircle.second[i]=y + sin(t[i]) / k[index];
		}
	}

	void Ellipse::clear()
	{
		e2.first.clear();
		e2.second.clear();
		k.clear();;
		curvaturecircle.first.clear();
		curvaturecircle.second.clear();
		ellipse.first.clear();
		ellipse.second.clear();
		evolute.first.clear();
		evolute.second.clear();
	}
}