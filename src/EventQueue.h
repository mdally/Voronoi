#ifndef _EVENT_QUEUE_H_
#define _EVENT_QUEUE_H_

#include "DCEL.h"
#include "BeachLine.h"

enum eventType { SITE, CIRCLE };

struct event{
	eventType type;
	Site* s1;
	Site* s2;
	Site* s3;
	beachLineNode* disappearingArc;
	Point circleCenter;
	bool falseAlarm;
};

class compareEvents{
public:
	bool operator()(event* e1, event* e2){
		Point& p1 = (e1->type == SITE) ? e1->s1->p : e1->circleCenter;
		Point& p2 = (e2->type == SITE) ? e2->s1->p : e2->circleCenter;

		if (p1.y < p2.y) return true;
		if (p1.y == p2.y && p1.x > p2.x) return true;
		return false;
	}
};

class EventQueue{
public:
	EventQueue();
	~EventQueue();

	bool empty();

	void add(event* e);
	event* pop();
private:
};

#endif