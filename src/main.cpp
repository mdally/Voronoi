#include "MemoryPool/C-11/MemoryPool.h"
#include "../include/Point2.h"
#include "../include/Vector2.h"
#include "../include/VoronoiDiagramGenerator.h"
#include "RBTree.h"
#include <vector>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <limits>
#include "Epsilon.h"

bool sitesOrdered(const Point2& s1, const Point2& s2) {
	if (s1.y < s2.y)
		return true;
	if (s1.y == s2.y && s1.x < s2.x)
		return true;

	return false;
}

void randomSites(std::vector<Point2>& sites, BoundingBox& bbox, unsigned int dimension, unsigned int numSites) {
	bbox = BoundingBox(0, dimension, dimension, 0);

	Point2 s;

	srand(std::clock());
	for (unsigned int i = 0; i < numSites; ++i) {
		s.x = 1 + (rand() / (double)RAND_MAX)*(dimension - 2);
		s.y = 1 + (rand() / (double)RAND_MAX)*(dimension - 2);
		sites.push_back(s);
	}
}

int main() {
	unsigned int nPoints;
	unsigned int dimension = 100;
	VoronoiDiagramGenerator vdg = VoronoiDiagramGenerator();
	Diagram* diagram = nullptr;
	
	std::vector<Point2>* sites;
	std::vector<Point2>* safeSites;
	BoundingBox bbox;

	nPoints = 100;
	sites = new std::vector<Point2>();
	safeSites = new std::vector<Point2>();
	randomSites(*sites, bbox, dimension, nPoints);
	std::sort(sites->begin(), sites->end(), sitesOrdered);
	safeSites->push_back((*sites)[0]);
	for (Point2& s : *sites) {
		if (s != safeSites->back()) 
			safeSites->push_back(s);
	}
	diagram = vdg.compute(*safeSites, bbox);

	while (true) {
		diagram = vdg.relax();
	}

	delete diagram;
	return 0;
}
