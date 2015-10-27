#include "Voronoi.h"
#include <cstdlib>

#define dimension 10
#define numSites 4

int main(int argc, char** argv){
	Voronoi diagram;
	
	diagram.setBounds(0, dimension, 0, dimension);

	Site s;
	s.edge = NULL;

	for (int i = 0; i < numSites; ++i){
		s.p[0] = (rand() % (dimension-1)) + 1;
		s.p[1] = (rand() % (dimension-1)) + 1;
		diagram.sites.push_back(s);
	}

	diagram.compute();
	diagram.relax();

	return 0;
}