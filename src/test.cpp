#include "Voronoi.h"

int main(int argc, char** argv){
	Voronoi diagram;

	diagram.compute();
	diagram.relax();

	return 0;
}