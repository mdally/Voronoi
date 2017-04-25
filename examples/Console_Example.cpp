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

	const int numTests = 5;
	const int maxSites = 3000000;

	std::ofstream outFile = std::ofstream();
	outFile.open("test_data.txt");
	outFile.clear();

	std::chrono::time_point<std::chrono::steady_clock> start, stop;
	for (int i = 0; i < numTests; ++i) {
		sites = new std::vector<Point2>();

		unsigned int nSites = maxSites; // (unsigned int)round((rand() / (double)RAND_MAX)*(maxSites - 2)) + 2u;
		genRandomSites(*sites, bbox, 100000, nSites);

		start = std::chrono::high_resolution_clock::now();
		diagram = vdg.compute(*sites, bbox);
		stop = std::chrono::high_resolution_clock::now();
		int64_t durationNS = (std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count());

		delete diagram;
		delete sites;

		std::cout << nSites << "\t" << durationNS << std::endl;
		outFile << nSites << "\t" << durationNS << std::endl;
	}
	outFile.close();

	return 0;
}
