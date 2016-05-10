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
	unsigned int nPoints = 5;
	unsigned int dimension = 10;
	VoronoiDiagramGenerator* vdg = nullptr;
	std::clock_t start; 
	vdg = new VoronoiDiagramGenerator();

	////////////////////////////////////////////////////////////////////
	std::vector<Point2> sites;
	std::vector<Point2> safeSites;
	BoundingBox bbox;
	randomSites(sites, bbox, dimension, nPoints); 
	std::sort(sites.begin(), sites.end(), sitesOrdered);
	safeSites.push_back(sites[0]);
	int duplicates = -1;
	for (Point2& s : sites) {
		if (s != safeSites.back()) {
			safeSites.push_back(s);
		}
		else {
			++duplicates;
		}
	}
	Diagram* diagram = vdg->compute(safeSites, bbox);
	////////////////////////////////////////////////////////////////////

	int relaxations = 0;
	while (true) {
		if (diagram->cells.size() < 5) {
			int i = 0;
		}
		if (relaxations == 188) {
			int i = 0;
		}
		/*for (Cell* c : diagram->cells) {
			std::cout << c->site.p.x << ' ' << c->site.p.y << std::endl;
		}
		std::cout << std::endl;*/
		diagram = vdg->relax();
		++relaxations;
		/*std::cin >> nPoints;

		start = std::clock();

		vdg = new VoronoiDiagramGenerator();

		std::vector<Point2> sites;
		std::vector<Point2> safeSites;
		BoundingBox bbox;

		//square(sites, bbox);
		//siteBreakpointIntersect(sites, bbox);
		randomSites(sites, bbox, dimension, nPoints);

		std::sort(sites.begin(), sites.end(), sitesOrdered);
		safeSites.push_back(sites[0]);
		int duplicates = -1;
		for (Point2& s : sites) {
			if (s != safeSites.back()) {
				safeSites.push_back(s);
			}
			else {
				++duplicates;
			}
		}

		Diagram* diagram = vdg->compute(safeSites, bbox);
		vdg->relax();
		delete diagram;
		delete vdg;

		std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms for " << nPoints << " points" << std::endl;*/
	}
	delete diagram;
	delete vdg;

    return 0;
}