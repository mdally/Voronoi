#ifndef _VORONOI_H_
#define _VORONOI_H_

#include "DCEL.h"

#include <vector>
#include <queue>

//forward declarations
struct event;

struct beachLineNode{
	Site* s1;
	Site* s2;

	beachLineNode* parent;
	beachLineNode* left;
	beachLineNode* right;

	event* circleEvent;
	HalfEdge* edge;
};

struct nodeTriplet{
	beachLineNode* n1;
	beachLineNode* n2;
	beachLineNode* n3;
};

class BeachLine{
	friend class Voronoi;
public:
	beachLineNode* root;

	BeachLine();
	~BeachLine();

	beachLineNode* arcAbove(Site* s);

	beachLineNode* min(beachLineNode* n);
	beachLineNode* max(beachLineNode* n);

	beachLineNode* predecessor(beachLineNode* n);
	beachLineNode* successor(beachLineNode* n);

	beachLineNode* prevArc(beachLineNode* n);
	beachLineNode* nextArc(beachLineNode* n);

	nodeTriplet leftTriplet(beachLineNode* n);
	nodeTriplet rightTriplet(beachLineNode* n);

	void destroy(beachLineNode* n);
};

enum eventType { SITE, CIRCLE };

struct event{
	eventType type;
	Site* sites[3];
	beachLineNode* disappearingArc;
	Point circleCenter;
	double y;
	bool falseAlarm;
};

class compareEvents{
public:
	bool operator()(event* e1, event* e2){
		Point p1;
		Point p2;

		if (e1->type == SITE){
			p1 = e1->sites[0]->p;
		}
		else{
			p1.x = e1->circleCenter.x;
			p1.y = e1->y;
		}
		if (e2->type == SITE){
			p2 = e2->sites[0]->p;
		}
		else{
			p2.x = e2->circleCenter.x;
			p2.y = e2->y;
		}

		if (p1.y < p2.y) return true;
		if (p1.y == p2.y && p1.x > p2.x) return true;
		return false;
	}
};

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

	std::priority_queue<event*, std::vector<event*>, compareEvents> eventQueue;
	BeachLine beachLine;

	void processSiteEvent(event* e);
	void processCircleEvent(event* e);

	void checkArcTripletForCircleEvent(nodeTriplet& sites);
	void attachEdgeToCircleCenter(beachLineNode* breakpoint, Vertex* circleCenter, bool moveSweepline);
	void matchEdges(HalfEdge* edge, HalfEdge* faceEdges[], event* circleEvent);

	inline bool breakPointsConverge(nodeTriplet& sites);
	inline bool siteToLeft(Site* s1, Site* s2);
	inline Point circumcenter(nodeTriplet& sites);
	friend inline void findParabolaIntersections(Point& focus1, Point& focus2, double directrixY, 
		Point& intersection1, Point& intersection2);
	inline double dist(Point& p1, Point& p2);
	inline double signedAngleBetweenVectors(Point& src, Point& p1, Point& p2);
};

#endif