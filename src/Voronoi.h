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

	beachLineNode() : s1(nullptr), s2(nullptr), parent(nullptr), left(nullptr), 
		right(nullptr), circleEvent(nullptr), edge(nullptr) {}
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

	void printLine();
	void printNode(beachLineNode* n);
};

enum eventType { SITE, CIRCLE };

struct event{
	eventType type;
	Site* sites[3];
	beachLineNode* disappearingArc;
	Point2 circleCenter;
	double y;
	bool falseAlarm;
};

class compareEvents{
public:
	bool operator()(event* e1, event* e2){
		Point2 p1;
		Point2 p2;

		if (e1->type == SITE){
			p1 = e1->sites[0]->p;
		}
		else{
			p1[0] = e1->circleCenter[0];
			p1[1] = e1->y;
		}
		if (e2->type == SITE){
			p2 = e2->sites[0]->p;
		}
		else{
			p2[0] = e2->circleCenter[0];
			p2[1] = e2->y;
		}

		if (p1[1] < p2[1]){
			return true;
		}
		if (p1[1] == p2[1] && p1[0] > p2[0]){
			return true;
		}
		return false;
	}
};

//TODO: how to pass in sites?
enum boundary { TOP, BOTTOM, LEFT, RIGHT };
class Voronoi{
public:
	std::vector<Site> sites;
	std::vector<Vertex*> vertices;
	std::vector<HalfEdge*> edges;

	Voronoi();
	~Voronoi();
	void setBounds(int bottomX, int topX, int leftY, int rightY);
	void compute();
	void relax();

private:
	int minX, maxX, minY, maxY;
	bool boundsSet;
	double currentSweeplineY;

	std::priority_queue<event*, std::vector<event*>, compareEvents> eventQueue;
	BeachLine beachLine;
	std::vector<HalfEdge*> danglingEdges;

	void processSiteEvent(event* e);
	void processCircleEvent(event* e);

	void checkArcTripletForCircleEvent(nodeTriplet& sites);
	void attachEdgeToCircleCenter(beachLineNode* breakpoint, Vertex* circleCenter, bool moveSweepline);
	void matchEdges(HalfEdge* edge, HalfEdge* faceEdges[], event* circleEvent);

	inline bool breakPointsConverge(nodeTriplet& sites);
	inline bool siteToLeft(Site* s1, Site* s2);
	Point2 circumcenter(nodeTriplet& sites);
	friend void findParabolaIntersections(Point2& focus1, Point2& focus2, double directrixY, 
		Point2& intersection1, Point2& intersection2);
	inline double signedAngleBetweenVectors(Vector2& v1, Vector2& v2);

	void trimOutsideEdges();
	void attachEdgesToBoundingBox();
	void findIntersectionWithBoundaries(Point2& src, Vector2& direction, double& t, boundary& b);
};

#endif