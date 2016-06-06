#include "../include/Point2.h"
#include "../include/Vector2.h"
#include "../include/VoronoiDiagramGenerator.h"
#include <vector>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits>

bool sitesOrdered(const Point2& s1, const Point2& s2) {
	if (s1.y < s2.y)
		return true;
	if (s1.y == s2.y && s1.x < s2.x)
		return true;

	return false;
}

void genRandomSites(std::vector<Point2>& sites, BoundingBox& bbox, unsigned int dimension, unsigned int numSites) {
	bbox = BoundingBox(0, dimension, dimension, 0);
	std::vector<Point2> tmpSites;

	tmpSites.reserve(numSites);
	sites.reserve(numSites);

	Point2 s;

	srand(std::clock());
	for (unsigned int i = 0; i < numSites; ++i) {
		s.x = 1 + (rand() / (double)RAND_MAX)*(dimension - 2);
		s.y = 1 + (rand() / (double)RAND_MAX)*(dimension - 2);
		tmpSites.push_back(s);
	}

	//remove any duplicates that exist
	std::sort(tmpSites.begin(), tmpSites.end(), sitesOrdered);
	sites.push_back(tmpSites[0]);
	for (Point2& s : tmpSites) {
		if (s != sites.back()) sites.push_back(s);
	}
}

//test the average time it takes to compute a Lloyd's Relaxation for a few different numbers of input sites
int main() {
	VoronoiDiagramGenerator vdg = VoronoiDiagramGenerator();
	Diagram* diagram = nullptr;
	std::vector<Point2>* sites;
	BoundingBox bbox;

	const int numTests = 35;
	int testPointCounts[numTests] = {
		10, 20, 30, 40, 50, 60, 70, 80, 90,
		100, 200, 300, 400, 500, 600, 700, 800, 900,
		1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000,
		10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000//, 90000//, 100000
	};
	const int numTestsPerRun = 100;
	double testRuns[numTests][numTestsPerRun];

	clock_t start;
	double duration;
	double average;
	for (int i = 0; i < numTests; ++i) {
		sites = new std::vector<Point2>();
		genRandomSites(*sites, bbox, 1000000, testPointCounts[i]);
		diagram = vdg.compute(*sites, bbox);
		delete sites;

		for (int j = 0; j < numTestsPerRun; ++j) {
			Diagram* oldDiagram = diagram;

			start = clock();
			diagram = vdg.relax();
			duration = 1000 * (std::clock() - start) / (double)CLOCKS_PER_SEC;

			delete oldDiagram;

			testRuns[i][j] = duration;
		}

		/*average = 0;
		for (int j = 0; j < numTestsPerRun; ++j) {
			average += testRuns[i][j];
		}
		average /= numTestsPerRun;

		std::cout << "Lloyd's relaxation of " << std::setw(5) << testPointCounts[i] << " took " << average << "ms on average.\n";*/
	}

	for (int j = 0; j < numTests; ++j) {
		std::cout << testPointCounts[j] << '\t';
	}
	std::cout << '\n';
	for (int i = 0; i < numTestsPerRun; ++i) {
		for (int j = 0; j < numTests; ++j) {
			std::cout << testRuns[j][i] << '\t';
		}
		std::cout << '\n';
	}

	delete diagram;

	//don't auto-close the window
	char c;
	std::cin >> c;
	return 0;
}
