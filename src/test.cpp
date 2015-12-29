#include "Voronoi.h"
#include <cstdlib>

//#define dimension 20000000
//#define numSites 10000

bool sitesOrdered(Site& s1, Site& s2) {
	if (s1.p[1] < s2.p[1])
		return true;
	if (s1.p[1] == s2.p[1] && s1.p[0] < s2.p[0])
		return true;

	return false;
}

void randomSites(std::vector<Site>& sites, Voronoi* diagram, int dimension, int numSites) {
	diagram->setBounds(0, dimension, 0, dimension);

	Site s;
	s.edge = NULL;

	for (int i = 0; i < numSites; ++i) {
		s.p[0] = (rand() / (double)RAND_MAX)*dimension;
		s.p[1] = (rand() / (double)RAND_MAX)*dimension;
		sites.push_back(s);
	}

}

void siteBreakpointIntersect(std::vector<Site>& sites, Voronoi* diagram) {
	diagram->setBounds(0, 6, 0, 6);

	Site s;
	s.edge = NULL;

	s.p[1] = 1; s.p[0] = 1; sites.push_back(s);
	s.p[1] = 3;				sites.push_back(s);
	s.p[1] = 5;				sites.push_back(s);

	s.p[1] = 1; s.p[0] = 3; sites.push_back(s);
	s.p[1] = 5;				sites.push_back(s);

	s.p[1] = 1; s.p[0] = 5; sites.push_back(s);
	s.p[1] = 3;				sites.push_back(s);
	s.p[1] = 5;				sites.push_back(s);
}

int main(int argc, char** argv){
	Voronoi* diagram = new Voronoi();

	std::vector<Site> sites;

	siteBreakpointIntersect(sites, diagram);

	std::sort(sites.begin(), sites.end(), sitesOrdered);
	diagram->sites.push_back(sites[0]);
	int duplicates = -1;
	for (Site& s : sites) {
		if (s.p != diagram->sites.back().p) {
			diagram->sites.push_back(s);
		}
		else {
			++duplicates;
		}
	}

	diagram->compute();
	diagram->relax();

	return 0;
}