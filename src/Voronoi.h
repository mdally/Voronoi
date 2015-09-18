#ifndef _VORONOI_H_
#define _VORONOI_H_

#include "DCEL.h"
#include "BeachLine.h"
#include "EventQueue.h"

#include <vector>
#include <queue>

class Voronoi{
public:
	std::vector<Site> sites;
	std::vector<Vertex> vertices;
	std::vector<HalfEdge*> edges;

	Voronoi();
	void setBounds(int bottomX, int topX, int leftY, int rightY);
	void compute();
	void relax();
private:
	int minX, maxX, minY, maxY;
	bool boundsSet;

	EventQueue eventQueue;
	BeachLine beachLine;

	void processSiteEvent(event* e);
	void processCircleEvent(event* e);

	void checkArcTriplet(nodeTriplet& sites);
	bool breakPointsConverge(nodeTriplet& sites);
	Point circumcenter(nodeTriplet& sites);
};

#endif