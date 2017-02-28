#include "../include/Point2.h"
#include "../include/Vector2.h"
#include "../include/VoronoiDiagramGenerator.h"
#include <vector>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <fstream>
#include <chrono>

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

	const int numTests = 100;
	const int numTestsPerRun = 100;
	int64_t testRuns[numTests][numTestsPerRun];

	std::ofstream outFile = std::ofstream();
	outFile.open("test_data.txt");
	outFile.clear();

	std::chrono::time_point<std::chrono::steady_clock> start, stop;
	double average;
	for (int i = 0; i < numTests; ++i) {
		for (int j = 0; j < numTestsPerRun; ++j) {
			sites = new std::vector<Point2>();
			genRandomSites(*sites, bbox, 100, 1000 * (i + 1));

			start = std::chrono::high_resolution_clock::now();
			diagram = vdg.compute(*sites, bbox);
			stop = std::chrono::high_resolution_clock::now();
			testRuns[i][j] = (std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count());

			delete diagram;
			delete sites;
		}

		average = 0;
		for (int j = 0; j < numTestsPerRun; ++j) {
			average += testRuns[i][j];
		}
		average /= numTestsPerRun;
		average /= 1000000;

		std::cout << "Computing diagram of " << std::setw(5) << 1000 * (i + 1) << " points took " << average << "ms on average.\n";

		outFile << 1000 * (i + 1) << "\t";
		for (int j = 0; j < numTestsPerRun; ++j) {
			outFile << (j != 0 ? "\t" : "") << testRuns[i][j];
		}
		outFile << std::endl;
	}
	outFile.close();

	return 0;
}
