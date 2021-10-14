#pragma once

#include <vector>

namespace Chaf
{
	struct Ellipse
	{
	public:
		float a = 20;
		float b = 10;
		std::pair< std::vector<double>, std::vector<double>>ellipse;
		std::pair< std::vector<double>, std::vector<double>>evolute;
		std::pair< std::vector<double>, std::vector<double>>curvaturecircle;


	private:
		std::vector<double> t;
		std::pair< std::vector<double>, std::vector<double>>e2;
		std::vector<double> k;

	public:
		Ellipse();

		void update();

		void updateCurvature(uint32_t index);

		void clear();


	};
}