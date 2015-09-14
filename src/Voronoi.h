#ifndef _VORONOI_H_
#define _VORONOI_H_

#include "DCEL.h"

#include <vector>
#include <queue>

class Voronoi{
public:
	std::vector<Vertex> vertices;
	std::vector<HalfEdge> edges;
	std::vector<Site> sites;

	Voronoi();
	void compute();
	void relax();
private:
	int boxMinX, boxMaxX, boxMinY, boxMaxY;

	struct beachLineNode{
		Site* s1;
		Site* s2;

		beachLineNode* left;
		beachLineNode* right;
	};

	enum eventType { SITE, CIRCLE };
	struct event{
		eventType type;
		Site* site;
		beachLineNode* node;
		bool falseAlarm;
	};

	class compareEvents{
	public:
		bool operator()(event* e1, event* e2){
			Point& p1 = (e1->type == SITE) ? e1->site->p : e1->node->s1->p;
			Point& p2 = (e2->type == SITE) ? e2->site->p : e2->node->s1->p;

			if (p1.y < p2.y) return true;
			if (p1.y == p2.y && p1.x > p2.x) return true;
			return false;
		}
	};

	beachLineNode* beachLine;
	std::vector<event> events;
	std::priority_queue<event*, std::vector<event*>, compareEvents> eventQueue;

	void processSiteEvent(event* e);
	void processCircleEvent(event* e);
	void addArcToBeachLine(Site* s);
};

#endif