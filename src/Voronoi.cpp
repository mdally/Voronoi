#include "Voronoi.h"
#define _USE_MATH_DEFINES
#include <math.h>
using std::priority_queue;
using std::vector;

Voronoi::Voronoi(){
	boundsSet = false;

	/////////junk test code
	Site s;
	s.edge = NULL;
	s.p[0] = 3.0;
	s.p[1] = 7.0;
	sites.push_back(s);

	s.p[0] = 11.0;
	s.p[1] = 6.0;
	sites.push_back(s);

	s.p[0] = 5.0;
	s.p[1] = 3.0;
	sites.push_back(s);

	s.p[0] = 9.0;
	s.p[1] = 2.0;
	sites.push_back(s);

	setBounds(0, 15, 0, 8);
}

Voronoi::~Voronoi(){
	for (Vertex* v : vertices){
		delete v;
	}
	for (HalfEdge* e : edges){
		delete e;
	}
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
	xLo = sites[0].p[0] - 1;
	xHi = sites[0].p[0] + 1;
	yLo = sites[0].p[1] - 1;
	yHi = sites[0].p[1] + 1;

	for (Site& s : sites){
		event* e = new event();
		e->type = SITE;
		e->sites[0] = &s;
		e->sites[1] = nullptr;
		e->sites[2] = nullptr;
		e->disappearingArc = nullptr;
		e->falseAlarm = false;
		e->y = s.p[1];

		eventQueue.push(e);

		if (!boundsSet){
			if (s.p[0] <= xLo) xLo = s.p[0] - 1;
			else if (s.p[0] >= xHi) xHi = s.p[0] + 1;

			if (s.p[1] <= yLo) yLo = s.p[1] - 1;
			else if (s.p[1] >= yHi) yHi = s.p[1] + 1;
		}
	}

	if (!boundsSet) setBounds((int)floor(xLo), (int)ceil(xHi), (int)floor(yLo), (int)ceil(yHi));

	//process event queue
	while (!eventQueue.empty()){
		event* e = eventQueue.top();
		eventQueue.pop();
		currentSweeplineY = e->y;

		if (!e->falseAlarm){
			if (e->type == SITE) processSiteEvent(e);
			else processCircleEvent(e);
		}
		delete e;
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
			e->y = e->circleCenter[1] - e->circleCenter.distanceTo(e->sites[0]->p);

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
	Point2 A = sites.n1->s1->p;
	Point2 B = sites.n2->s1->p;
	Point2 C = sites.n3->s1->p;

	double decision = -(A[1]*B[0]) + (A[0]*B[1]) + (A[1]*C[0]) - (B[1]*C[0]) - (A[0]*C[1]) + (B[0]*C[1]);

	return (decision < 0) ? true : false;
}

inline bool Voronoi::siteToLeft(Site* s1, Site* s2){
	if (s1->p[0] < s2->p[0] ||
		s1->p[0] == s2->p[0] && s1->p[1] < s2->p[1])
		return true;
	return false;
}

Point2 Voronoi::circumcenter(nodeTriplet& sites){
	Point2 A = sites.n1->s1->p;
	Point2 B = sites.n2->s1->p;
	Point2 C = sites.n3->s1->p;

	double dA = A.distanceFromOriginSquared();
	double dB = B.distanceFromOriginSquared();
	double dC = C.distanceFromOriginSquared();

	double denom = 2.0 * (A[0]*(C[1] - B[1]) + B[0]*(A[1] - C[1]) + C[0]*(B[1] - A[1]));

	Point2 center;
	center[0] = (dA*(C[1] - B[1]) + dB*(A[1] - C[1]) + dC*(B[1] - A[1])) / denom;
	center[1] = -(dA*(C[0] - B[0]) + dB*(A[0] - C[0]) + dC*(B[0] - A[0])) / denom;

	return center;
}

void findParabolaIntersections(Point2& focus1, Point2& focus2, double directrixHeight,
	Point2& intersection1, Point2& intersection2){

	double bmc1 = focus1[1] - directrixHeight;
	double a1 = 1 / (2.0*bmc1);
	double b1 = -focus1[0] / bmc1;
	double c1 = (focus1[0]*focus1[0] + focus1[1]*focus1[1] - directrixHeight*directrixHeight) / (2.0*bmc1);

	double bmc2 = focus2[1] - directrixHeight;
	double a2 = 1 / (2.0*bmc2);
	double b2 = -focus2[0] / bmc2;
	double c2 = (focus2[0]*focus2[0] + focus2[1]*focus2[1] - directrixHeight*directrixHeight) / (2.0*bmc2);

	double a = a2 - a1;
	double b = b2 - b1;
	double c = c2 - c1;

	intersection1[0] = (-b - sqrt(b*b - 4.0*a*c)) / (2.0*a);
	intersection2[0] = (-b + sqrt(b*b - 4.0*a*c)) / (2.0*a);

	intersection1[1] = a1*intersection1[0]*intersection1[0] + b1*intersection1[0] + c1;
	intersection2[1] = a2*intersection2[0]*intersection2[0] + b2*intersection2[0] + c2;
}


inline double Voronoi::signedAngleBetweenVectors(Vector2& v1, Vector2& v2){
	double angle = atan2(v2[1], v2[0]) - atan2(v1[1], v1[0]);
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
	Point2 p1 = breakpoint->s1->p;
	Point2 p2 = breakpoint->s2->p;
	Point2 c = circleCenter->p;

	Point2 intersect1;
	Point2 intersect2;

	findParabolaIntersections(p1, p2, currentSweeplineY, intersect1, intersect2);

	Point2* target = &intersect1;
	if (moveSweepline ^ (c.distanceTo(intersect1) < c.distanceTo(intersect2))){
		target = &intersect2;
	}
	if (moveSweepline){
		findParabolaIntersections(p1, p2, currentSweeplineY - 1, intersect1, intersect2);
	}

	double angle1 = signedAngleBetweenVectors(*target-c, p1-c);
	double angle2 = signedAngleBetweenVectors(*target-c, p2-c);

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