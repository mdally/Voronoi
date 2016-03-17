#include "Voronoi.h"
#include <cstdlib>
#include <algorithm>
#include <iostream>
using std::cout;
using std::endl;

bool sitesOrdered(const Site& s1, const Site& s2) {
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

	srand(1);
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

void square(std::vector<Site>& sites, Voronoi* diagram){
	diagram->setBounds(0, 4, 0, 4);

	Site s;
	s.edge = NULL;

	s.p[1] = 1; s.p[0] = 1; sites.push_back(s);
	s.p[1] = 3;				sites.push_back(s);

	s.p[1] = 1; s.p[0] = 3; sites.push_back(s);
	s.p[1] = 3;				sites.push_back(s);
}

void printDiagram(Voronoi* diagram){
	for(HalfEdge* e : diagram->edges){
		cout.width(10);
		if(e->origin)
			cout << e->origin->p;
		else
			cout << "infinity";
		cout << " -> ";
		cout.width(10);
		if(e->twin->origin)
			cout << e->twin->origin->p;
		else
			cout << "infinity"; 
		cout << " | " << e->site->p << " , " << e->twin->site->p;
		cout << endl;
	}
}

int main(int argc, char** argv) {
	Voronoi* diagram = new Voronoi();

	std::vector<Site> sites;
	
	square(sites, diagram);
	//siteBreakpointIntersect(sites, diagram);
	//randomSites(sites, diagram, 100, 22);

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

	printDiagram(diagram);

	return 0;
}