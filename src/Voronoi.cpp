#include "Voronoi.h"
using std::priority_queue;
using std::vector;

Voronoi::Voronoi(){
	boundsSet = false;

	Site s;
	s.p.x = 1.0;
	s.p.y = 1.0;
	s.edge = NULL;
	sites.push_back(s);

	s.p.x = 2.0;
	s.p.y = 2.0;
	sites.push_back(s);

	setBounds(0, 9, 0, 9);
}

void Voronoi::setBounds(int bottomX, int topX, int leftY, int rightY){
	minX = bottomX;
	maxX = topX;
	minY = leftY;
	maxY = rightY;

	boundsSet = true;
}

void Voronoi::compute(){
	//add all sites to event queue

	//figure out bounding box if necessary
	if (!boundsSet){
		
	}

	//handle all the sites with max y in one shot 
	//add vertical boundary rays between them
	//maybe not????

	//process rest of event queue
	while (!eventQueue.empty()){
		event* e = eventQueue.pop();

		if (!e->falseAlarm){
			if (e->type == SITE)
				processSiteEvent(e);
			else
				processCircleEvent(e);
		}
	}

	//attach unfinished edges to bounding box
}

void Voronoi::processSiteEvent(event* e){
	//insert into beach line:

	//search for arc in line vertically above new site
	//if arc has a circle event, invalidate it
	beachLineNode* above = beachLine.arcAbove(e->s1);
	if (above->circleEvent) above->circleEvent->falseAlarm = true;

	//replace the leaf with a subtree having 3 leaves like so:
	//          (new, old)
	//           /      \
	//   (old, new)     old
	//    /      \
	//  old      new
	Site* oldSite = above->s1;
	Site* newSite = e->s1;

	beachLineNode* new_old = above;
	new_old->s1 = newSite;
	new_old->s2 = oldSite;
	new_old->circleEvent = nullptr;
	new_old->left = new beachLineNode();
	new_old->right = new beachLineNode();

	beachLineNode* old_new = above->left;
	old_new->s1 = oldSite;
	old_new->s2 = newSite;
	old_new->parent = above;
	old_new->left = new beachLineNode();
	old_new->right = new beachLineNode();
	old_new->circleEvent = nullptr;

	beachLineNode* oldArcLeft = old_new->left;
	oldArcLeft->s1 = oldSite;
	oldArcLeft->s2 = nullptr;
	oldArcLeft->parent = old_new;
	oldArcLeft->left = nullptr;
	oldArcLeft->right = nullptr;
	oldArcLeft->circleEvent = nullptr;

	beachLineNode* newArc = old_new->right;
	newArc->s1 = newSite;
	newArc->s2 = nullptr;
	newArc->parent = old_new;
	newArc->left = nullptr;
	newArc->right = nullptr;
	newArc->circleEvent = nullptr;

	beachLineNode* oldArcRight = new_old->right;
	oldArcRight->s1 = oldSite;
	oldArcRight->s2 = nullptr;
	oldArcRight->parent = new_old;
	oldArcRight->left = nullptr;
	oldArcRight->right = nullptr;
	oldArcRight->circleEvent = nullptr;

	//create new half-edge records that will be traced by the new breakpoints
	HalfEdge* A = new HalfEdge();
	HalfEdge* B = new HalfEdge();

	A->origin = nullptr;
	A->twin = B;
	A->next = nullptr;
	A->site = oldSite;
	if (oldSite->edge == nullptr) oldSite->edge = A;

	B->origin = nullptr;
	B->twin = A;
	B->next = nullptr;
	B->site = newSite;
	if (newSite->edge == nullptr) newSite->edge = B;

	edges.push_back(A);
	edges.push_back(B);

	//check the two triplets of consecutive arcs for which the new arc is either the new left
	//or new right arc to see if the breakpoints converge. If so, create a new circle event
	//and add it to the queue. add pointers from the middle node of the triplet to 
	//the new event
	checkArcTriplet(beachLine.leftTriplet(newArc));
	checkArcTriplet(beachLine.rightTriplet(newArc));
}

void Voronoi::checkArcTriplet(nodeTriplet& sites){
	if (breakPointsConverge(sites)){
		event* e = new event();
		e->type = CIRCLE;
		e->s1 = sites.n1->s1;
		e->s2 = sites.n2->s1;
		e->s3 = sites.n3->s1;
		e->disappearingArc = sites.n2;
		e->circleCenter = circumcenter(sites);
		e->falseAlarm = false;

		eventQueue.add(e);
		sites.n2->circleEvent = e;
	}
}

bool Voronoi::breakPointsConverge(nodeTriplet& sites){
	return false;
}

Point Voronoi::circumcenter(nodeTriplet& sites){
	Point p = { 0 };
	return p;
}

void Voronoi::processCircleEvent(event* e){
	//delete the leaf l that represents the disappearing arc a

	//update the tuples representing the breakpoints at the internal nodes
	//invalidate all circle events involving a. (i.e. those pointed to by l's predecessor
	//and successor)

	//add the center of the cirlce from the event as a vertex in the DCEL
	//create new half-edge records corresponding to the new breakpoint on the beachline

}

void Voronoi::relax(){
	//move each site to be the average of the vertices that surround it
	//then re-generate the diagram with the new site locations
}