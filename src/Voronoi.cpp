#include "Voronoi.h"
using std::priority_queue;
using std::vector;

Voronoi::Voronoi(){
	beachLine = NULL;

	Site s;
	s.p.x = 1.0;
	s.p.y = 1.0;
	s.edge = NULL;
	sites.push_back(s);

	s.p.x = 2.0;
	s.p.y = 2.0;
	sites.push_back(s);

	boxMinX = boxMinY = 0;
	boxMaxX = boxMaxY = 10;
}

void Voronoi::compute(){
	//add all sites to event queue
	event e;
	e.type = SITE;
	e.node = NULL;
	e.falseAlarm = false;
	for (Site& s : sites){
		e.site = &s;

		events.push_back(e);
		eventQueue.push(&e);
	}

	//handle all sites with max y in one shot

	//process event queue
	while (!eventQueue.empty()){
		event* e = eventQueue.top();

		if (e->type == SITE) processSiteEvent(e);
		else processCircleEvent(e);

		eventQueue.pop();
	}
}

void Voronoi::processSiteEvent(event* e){

}

void Voronoi::processCircleEvent(event* e){

}

void Voronoi::addArcToBeachLine(Site* s){

}

void Voronoi::relax(){

}