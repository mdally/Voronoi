#include "MemoryPool.h"
#include "Point2.h"
#include "Vector2.h"
#include "VoronoiDiagramGenerator.h"
#include "RBTree.h"
#include <vector>
#include <ctime>
#include <iostream>
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

	srand(0);
	//srand(std::clock());
	for (unsigned int i = 0; i < numSites; ++i) {
		s.x = 1 + (rand() / (double)RAND_MAX)*(dimension - 2);
		s.y = 1 + (rand() / (double)RAND_MAX)*(dimension - 2);
		sites.push_back(s);
	}

}
void siteBreakpointIntersect(std::vector<Point2>& sites, BoundingBox& bbox) {
	bbox = BoundingBox(0, 6, 6, 0);

	Point2 s;

	s.y = 1; s.x = 1; sites.push_back(s);
	s.y = 3;		  sites.push_back(s);
	s.y = 5;		  sites.push_back(s);

	s.y = 1; s.x = 3; sites.push_back(s);
	s.y = 5;		  sites.push_back(s);

	s.y = 1; s.x = 5; sites.push_back(s);
	s.y = 3;		  sites.push_back(s);
	s.y = 5;		  sites.push_back(s);
}
void square(std::vector<Point2>& sites, BoundingBox& bbox) {
	bbox = BoundingBox(0, 4, 4, 0);

	Point2 s;

	s.y = 1; s.x = 1; sites.push_back(s);
	s.y = 3;		  sites.push_back(s);

	s.y = 1; s.x = 3; sites.push_back(s);
	s.y = 3;		  sites.push_back(s);
}

int main() {
	unsigned int nPoints = 9;
	unsigned int dimension = 10;
	VoronoiDiagramGenerator* vdg = nullptr;
	vdg = new VoronoiDiagramGenerator();

	std::vector<Point2> sites;
	std::vector<Point2> safeSites;
	BoundingBox bbox;
	randomSites(sites, bbox, dimension, nPoints); 
	std::sort(sites.begin(), sites.end(), sitesOrdered);
	safeSites.push_back(sites[0]);
	for (size_t i = 1; i < nPoints; ++i) {
		if (safeSites[i] != safeSites.back()) {
			safeSites.push_back(safeSites[i]);
		}
	}
	Diagram* diagram = vdg->compute(safeSites, bbox);

	int relaxations = 0;
	while (true) {
		diagram = vdg->relax();
		++relaxations;
	}
	delete diagram;
	delete vdg;

    return 0;
}