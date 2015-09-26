#include "Voronoi.h"
#define _USE_MATH_DEFINES
#include <math.h>
using std::priority_queue;
using std::vector;

Voronoi::Voronoi(){
	boundsSet = false;

	/////////junk test code
	Site s;
	s.p.x = 3.0;
	s.p.y = 7.0;
	s.edge = NULL;
	sites.push_back(s);

	s.p.x = 11.0;
	s.p.y = 6.0;
	sites.push_back(s);

	s.p.x = 5.0;
	s.p.y = 3.0;
	sites.push_back(s);

	s.p.x = 9.0;
	s.p.y = 2.0;
	sites.push_back(s);

	setBounds(0, 15, 0, 8);
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
	double xLo, xHi, yLo, yHi;
	xLo = sites[0].p.x - 1;
	xHi = sites[0].p.x + 1;
	yLo = sites[0].p.y - 1;
	yHi = sites[0].p.y + 1;

	for (Site& s : sites){
		event* e = new event();
		e->type = SITE;
		e->sites[0] = &s;
		e->sites[1] = nullptr;
		e->sites[2] = nullptr;
		e->disappearingArc = nullptr;
		e->falseAlarm = false;
		e->y = s.p.y;

		eventQueue.push(e);

		if (!boundsSet){
			if (s.p.x <= xLo) xLo = s.p.x - 1;
			else if (s.p.x >= xHi) xHi = s.p.x + 1;

			if (s.p.y <= yLo) yLo = s.p.y - 1;
			else if (s.p.y >= yHi) yHi = s.p.y + 1;
		}
	}

	if (!boundsSet) setBounds((int)floor(xLo), (int)ceil(xHi), (int)floor(yLo), (int)ceil(yHi));

	//process event queue
	while (!eventQueue.empty()){
		event* e = eventQueue.top();
		eventQueue.pop();
		currentSweeplineY = e->y;

		if (!e->falseAlarm){
			if (e->type == SITE)
				processSiteEvent(e);
			else
				processCircleEvent(e);
		}
	}

	//TODO: attach remaining unfinished edges to bounding box

	//TODO: clean up all resources necessary
}

void Voronoi::processSiteEvent(event* e){
	//insert into beach line:
	if (beachLine.root == nullptr){
		beachLineNode* root = new beachLineNode();
		root->s1 = e->sites[0];
		root->s2 = nullptr;
		root->parent = nullptr;
		root->left = nullptr;
		root->right = nullptr;
		root->circleEvent = nullptr;
		root->edge = nullptr;

		beachLine.root = root;

		return;
	}

	//search for arc in line vertically above new site
	//if arc has a circle event, invalidate it
	beachLineNode* above = beachLine.arcAbove(e->sites[0]);
	if (above->circleEvent){
		above->circleEvent->falseAlarm = true;
	}

	//replace the leaf with a subtree having 3 leaves like so:
	//          (new, old)
	//           /      \
	//   (old, new)     old
	//    /      \
	//  old      new
	Site* oldSite = above->s1;
	Site* newSite = e->sites[0];

	beachLineNode* new_old = above;
	new_old->s1 = newSite;
	new_old->s2 = oldSite;
	new_old->circleEvent = nullptr;
	new_old->left = new beachLineNode();
	new_old->right = new beachLineNode();
	new_old->edge = nullptr;

	beachLineNode* old_new = above->left;
	old_new->s1 = oldSite;
	old_new->s2 = newSite;
	old_new->parent = above;
	old_new->left = new beachLineNode();
	old_new->right = new beachLineNode();
	old_new->circleEvent = nullptr;
	old_new->edge = nullptr;

	beachLineNode* oldArcLeft = old_new->left;
	oldArcLeft->s1 = oldSite;
	oldArcLeft->s2 = nullptr;
	oldArcLeft->parent = old_new;
	oldArcLeft->left = nullptr;
	oldArcLeft->right = nullptr;
	oldArcLeft->circleEvent = nullptr;
	oldArcLeft->edge = nullptr;

	beachLineNode* newArc = old_new->right;
	newArc->s1 = newSite;
	newArc->s2 = nullptr;
	newArc->parent = old_new;
	newArc->left = nullptr;
	newArc->right = nullptr;
	newArc->circleEvent = nullptr;
	newArc->edge = nullptr;

	beachLineNode* oldArcRight = new_old->right;
	oldArcRight->s1 = oldSite;
	oldArcRight->s2 = nullptr;
	oldArcRight->parent = new_old;
	oldArcRight->left = nullptr;
	oldArcRight->right = nullptr;
	oldArcRight->circleEvent = nullptr;
	oldArcRight->edge = nullptr;

	//create new half-edge records that will be traced by the new breakpoints
	HalfEdge* A = new HalfEdge();
	HalfEdge* B = new HalfEdge();
	new_old->edge = A;
	old_new->edge = A;

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
	checkArcTripletForCircleEvent(beachLine.leftTriplet(newArc));
	checkArcTripletForCircleEvent(beachLine.rightTriplet(newArc));
}

void Voronoi::checkArcTripletForCircleEvent(nodeTriplet& sites){
	if (sites.n1 && sites.n2 && sites.n3){
		if (breakPointsConverge(sites)){
			event* e = new event();
			e->type = CIRCLE;
			e->sites[0] = sites.n1->s1;
			e->sites[1] = sites.n2->s1;
			e->sites[2] = sites.n3->s1;
			e->disappearingArc = sites.n2;
			e->circleCenter = circumcenter(sites);
			e->falseAlarm = false;
			e->y = e->circleCenter.y - dist(e->sites[0]->p, e->circleCenter);

			if (e->y < currentSweeplineY){
				eventQueue.push(e);
				sites.n2->circleEvent = e;
			}
			else{
				delete e;
			}
		}
	}
}

inline bool Voronoi::breakPointsConverge(nodeTriplet& sites){
	Point A = sites.n1->s1->p;
	Point B = sites.n2->s1->p;
	Point C = sites.n3->s1->p;

	double decision = -(A.y*B.x) + (A.x*B.y) + (A.y*C.x) - (B.y*C.x) - (A.x*C.y) + (B.x*C.y);

	return (decision < 0) ? true : false;
}

inline bool Voronoi::siteToLeft(Site* s1, Site* s2){
	if (s1->p.x < s2->p.x ||
		s1->p.x == s2->p.x && s1->p.y < s2->p.y)
		return true;
	return false;
}

inline Point Voronoi::circumcenter(nodeTriplet& sites){
	Point A = sites.n1->s1->p;
	Point B = sites.n2->s1->p;
	Point C = sites.n3->s1->p;

	double dA = A.x*A.x + A.y*A.y;
	double dB = B.x*B.x + B.y*B.y;
	double dC = C.x*C.x + C.y*C.y;

	double denom = 2.0 * (A.x*(C.y - B.y) + B.x*(A.y - C.y) + C.x*(B.y - A.y));

	Point center;
	center.x = (dA*(C.y - B.y) + dB*(A.y - C.y) + dC*(B.y - A.y)) / denom;
	center.y = -(dA*(C.x - B.x) + dB*(A.x - C.x) + dC*(B.x - A.x)) / denom;

	return center;
}

inline void findParabolaIntersections(Point& focus1, Point& focus2, double directrixHeight,
	Point& intersection1, Point& intersection2){

	double bmc1 = focus1.y - directrixHeight;
	double a1 = 1 / (2.0*bmc1);
	double b1 = -focus1.x / bmc1;
	double c1 = (focus1.x*focus1.x + focus1.y*focus1.y - directrixHeight*directrixHeight) / (2.0*bmc1);

	double bmc2 = focus2.y - directrixHeight;
	double a2 = 1 / (2.0*bmc2);
	double b2 = -focus2.x / bmc2;
	double c2 = (focus2.x*focus2.x + focus2.y*focus2.y - directrixHeight*directrixHeight) / (2.0*bmc2);

	double a = a2 - a1;
	double b = b2 - b1;
	double c = c2 - c1;

	intersection1.x = (-b - sqrt(b*b - 4.0*a*c)) / (2.0*a);
	intersection2.x = (-b + sqrt(b*b - 4.0*a*c)) / (2.0*a);

	intersection1.y = a1*intersection1.x*intersection1.x + b1*intersection1.x + c1;
	intersection2.y = a2*intersection2.x*intersection2.x + b2*intersection2.x + c2;
}

inline double Voronoi::dist(Point& p1, Point& p2){
	return sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
}


inline double Voronoi::signedAngleBetweenVectors(Point& src, Point& p1, Point& p2){
	double angle = atan2(p2.y - src.y, p2.x - src.x) - atan2(p1.y - src.y, p1.x - src.x);
	if (angle < 0) angle += 2 * M_PI;
	
	return angle;
}

void Voronoi::processCircleEvent(event* e){
	//delete the leaf l that represents the disappearing arc a:
	//update the tuples representing the breakpoints at the internal nodes
	//invalidate all circle events involving a. (i.e. those pointed to by l's predecessor
	//and successor)
	beachLineNode* disappearing = e->disappearingArc;
	beachLineNode* prevArc = beachLine.prevArc(disappearing);
	if (prevArc && prevArc->circleEvent) prevArc->circleEvent->falseAlarm = true;

	beachLineNode* nextArc = beachLine.nextArc(disappearing);
	if (nextArc && nextArc->circleEvent) nextArc->circleEvent->falseAlarm = true;

	//add the center of the cirlce from the event as a vertex in the DCEL
	//create new half-edge records corresponding to the new breakpoint on the beachline
	Vertex* center = new Vertex();
	center->p = e->circleCenter;

	//attach existing half-edges to new vertex
	beachLineNode* breakpoint1 = beachLine.predecessor(e->disappearingArc);
	beachLineNode* breakpoint2 = beachLine.successor(e->disappearingArc);

	attachEdgeToCircleCenter(breakpoint1, center, false);
	attachEdgeToCircleCenter(breakpoint2, center, false);

	HalfEdge* faceEdges[6] = { 0 };

	matchEdges(breakpoint1->edge, faceEdges, e);
	matchEdges(breakpoint1->edge->twin, faceEdges, e);
	matchEdges(breakpoint2->edge, faceEdges, e);
	matchEdges(breakpoint2->edge->twin, faceEdges, e);

	//fixup beach line: delete disappearing arc, merge bordering breakpoints, add new half-edges
	beachLineNode* merge;
	beachLineNode* destroy;

	if (breakpoint1 == disappearing->parent){
		merge = breakpoint2;
		destroy = breakpoint1;
	}
	else{
		merge = breakpoint1;
		destroy = breakpoint2;
	}

	beachLineNode* relocate;
	if (destroy->left == disappearing) {
		relocate = destroy->right;
	}
	else {
		relocate = destroy->left;
	}

	Site** replace = &merge->s1;
	Site** replaceWith = &destroy->s1;
	if (*replace != disappearing->s1){
		replace = &merge->s2;
		replaceWith = &destroy->s2;
	}

	*replace = *replaceWith;

	relocate->parent = destroy->parent;
	if (destroy == destroy->parent->left){
		destroy->parent->left = relocate;
	}
	else{
		destroy->parent->right = relocate;
	}

	delete disappearing;
	delete destroy;

	HalfEdge* A = new HalfEdge();
	HalfEdge* B = new HalfEdge();

	merge->edge = A;
	merge->circleEvent = nullptr;

	A->origin = nullptr;
	A->next = nullptr;
	A->twin = B;
	A->site = merge->s1;
	if (merge->s1->edge == nullptr) merge->s1->edge = A;

	B->origin = nullptr;
	B->next = nullptr;
	B->twin = A;
	B->site = merge->s2;
	if (merge->s2->edge == nullptr) merge->s2->edge = B;

	edges.push_back(A);
	edges.push_back(B);

	attachEdgeToCircleCenter(merge, center, true);

	matchEdges(A, faceEdges, e);
	matchEdges(B, faceEdges, e);

	for (int i = 0; i < 3; ++i){
		if (faceEdges[i * 2]->origin == center)
			faceEdges[i * 2 + 1]->next = faceEdges[i * 2];
		else
			faceEdges[i * 2]->next = faceEdges[i * 2 + 1];
	}

	//check the new triple of consecutive arcs that has the former left neighbor of the deleted arc
	//as its middle arc for a circle event. Same for former right neighbor
	checkArcTripletForCircleEvent(beachLine.leftTriplet(nextArc));
	checkArcTripletForCircleEvent(beachLine.rightTriplet(prevArc));
}

//TODO: verify - this might not work correctly in cases where one end is already attached to a vertex
void Voronoi::attachEdgeToCircleCenter(beachLineNode* breakpoint, Vertex* circleCenter, bool moveSweepline){
	Point p1 = breakpoint->s1->p;
	Point p2 = breakpoint->s2->p;
	Point c = circleCenter->p;

	Point intersect1;
	Point intersect2;

	findParabolaIntersections(p1, p2, currentSweeplineY, intersect1, intersect2);

	Point* target = &intersect1;
	if (moveSweepline ^ (dist(intersect1, c) < dist(intersect2, c))){
		target = &intersect2;
	}
	if (moveSweepline){
		findParabolaIntersections(p1, p2, currentSweeplineY - 1, intersect1, intersect2);
	}

	double angle1 = signedAngleBetweenVectors(c, *target, p1);
	double angle2 = signedAngleBetweenVectors(c, *target, p2);

	Site* originFace = breakpoint->s1;
	if (angle2 < angle1){
		originFace = breakpoint->s2;
	}

	if (breakpoint->edge->site == originFace) {
		breakpoint->edge->origin = circleCenter;
	}
	else {
		breakpoint->edge->twin->origin = circleCenter;
	}
}

void Voronoi::matchEdges(HalfEdge* edge, HalfEdge* faces[], event* circleEvent){
	for (int i = 0; i < 3; ++i){
		if (edge->site == circleEvent->sites[i]){
			if (!faces[2 * i]){
				faces[2 * i] = edge;
			}
			else {
				faces[2 * i + 1] = edge;
			}
			break;
		}
	}
}

//TODO
void Voronoi::relax(){
	//move each site to be the average of the vertices that surround it
	//then re-generate the diagram with the new site locations
}