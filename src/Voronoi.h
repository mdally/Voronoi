#ifndef _VORONOI_H_
#define _VORONOI_H_

#include "DCEL.h"
#include "BeachLine.h"
#include "EventQueue.h"

#include <vector>
#include <queue>

//TODO: how to pass in sites?
class Voronoi{
public:
	std::vector<Site> sites;
	std::vector<Vertex*> vertices;
	std::vector<HalfEdge*> edges;

	Voronoi();
	void setBounds(int bottomX, int topX, int leftY, int rightY);
	void compute();
	void relax();

private:
	int minX, maxX, minY, maxY;
	bool boundsSet;
	double currentSweeplineY;

	EventQueue eventQueue;
	BeachLine beachLine;

	void processSiteEvent(event* e);
	void processCircleEvent(event* e);

	void checkArcTripletForCircleEvent(nodeTriplet& sites);
	void attachEdgeToCircleCenter(beachLineNode* breakpoint, Vertex* circleCenter, bool moveSweepline);
	void matchEdges(HalfEdge* edge, std::vector<HalfEdge*>* faces, event* circleEvent);

	inline bool breakPointsConverge(nodeTriplet& sites);
	inline bool siteToLeft(Site* s1, Site* s2);
	inline Point circumcenter(nodeTriplet& sites);
	inline void findParabolaIntersections(Point& focus1, Point& focus2, double directrixY, 
		Point& intersection1, Point& intersection2);
	inline double dist(Point& p1, Point& p2);
	inline double signedAngleBetweenVectors(Point& src, Point& p1, Point& p2);
};

#endif