#include "Voronoi.h"
#include <cstdlib>
#include <vector>

#define dimension 100000
#define numSites 100

bool sitesOrdered(Site& s1, Site& s2){
	if (s1.p[1] < s2.p[1])
		return true;
	if (s1.p[1] == s2.p[1] && s1.p[0] < s2.p[0])
		return true;

	return false;
}

int main(int argc, char** argv){
	Voronoi diagram;
	
	diagram.setBounds(0, dimension, 0, dimension);

	std::vector<Site> sites;

	Site s;
	s.edge = NULL;

	for (int i = 0; i < numSites; ++i){
		s.p[0] = (rand() % (dimension-1)) + 1;
		s.p[1] = (rand() % (dimension-1)) + 1;
		sites.push_back(s);
	}

	std::sort(sites.begin(), sites.end(), sitesOrdered);
	diagram.sites.push_back(sites[0]);
	int duplicates = -1;
	for (Site& s : sites){
		if (s.p != diagram.sites.back().p){
			diagram.sites.push_back(s);
		}
		else{
			++duplicates;
		}
	}

	diagram.compute();
	diagram.relax();

	return 0;
}