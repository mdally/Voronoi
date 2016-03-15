#include "Voronoi.h"
#include "Geometry.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>
#include <utility>
#include <algorithm>
#include <cassert>
using std::priority_queue;
using std::vector;
using std::pair;

Point2 diagramCenter;

#define _PRINT_BEACHLINE 1

Voronoi::Voronoi(){
	boundsSet = false;
}

Voronoi::~Voronoi(){
	for (Vertex* v : vertices){
		delete v;
	}
	for (HalfEdge* e : edges){
		delete e;
	}
}

void Voronoi::setBounds(int leftX, int rightX, int bottomY, int topY){
	minX = leftX;
	maxX = rightX;
	minY = bottomY;
	maxY = topY;

	diagramCenter = Point2((leftX+rightX)/2.0, (bottomY+topY)/2.0);
	boundsSet = true;
}

void genRandomSites(int numSites, float maxVal){

}

void Voronoi::compute(){
	//add all sites to event queue
	//figure out bounding box if necessary
	double xLo, xHi, yLo, yHi;
	xLo = sites[0].p[0] - 1;
	xHi = sites[0].p[0] + 1;
	yLo = sites[0].p[1] - 1;
	yHi = sites[0].p[1] + 1;

	//HalfEdge** junk = &(sites[20].edge);

	int numSites = sites.size();
	for (int i = 0; i < numSites; ++i){
		Site* s = &(sites[i]);
		event* e = new event();
		e->type = SITE;
		e->sites[0] = s;
		e->sites[1] = nullptr;
		e->sites[2] = nullptr;
		e->disappearingArc = nullptr;
		e->falseAlarm = false;
		e->y = s->p[1];

		eventQueue.push(e);

		if (!boundsSet){
			if (s->p[0] <= xLo) xLo = s->p[0] - 1;
			else if (s->p[0] >= xHi) xHi = s->p[0] + 1;

			if (s->p[1] <= yLo) yLo = s->p[1] - 1;
			else if (s->p[1] >= yHi) yHi = s->p[1] + 1;
		}
	}

	if (!boundsSet){
		setBounds((int)floor(xLo), (int)ceil(xHi), (int)floor(yLo), (int)ceil(yHi));
	}

	//handle all sites at max Y value
	processMaxYSites();

	//process remaining event queue
	while (!eventQueue.empty()){
		event* e = eventQueue.top();
		eventQueue.pop();
		currentSweeplineY = e->y;

		if (!e->falseAlarm){
			if (e->type == SITE){
				processSiteEvent(e);
			}
			else{
				processCircleEvent(e);
			}
		}
		delete e;
	}

	//trim vertices & edges that fall outside the box
	//trimOutsideEdges();
	//attachEdgesToBoundingBox();

	//verifyDiagram();

	//TODO: clean up any resources necessary
}

void Voronoi::processMaxYSites() {
	event* e = eventQueue.top();
	double maxSiteY = e->sites[0]->p[1];
	
	vector<Site*> topSites;

	while (e && e->sites[0]->p[1] == maxSiteY) {
		topSites.push_back(e->sites[0]);
		eventQueue.pop();
		e = eventQueue.top();
	}

	for (Site* s : topSites) {
		if (beachLine.root == nullptr) {
			beachLineNode* root = new beachLineNode();
			root->s1 = s;

			beachLine.root = root;
		}
		else {
			beachLineNode* prev = beachLine.max(beachLine.root);

			/****************************************************************************
			 * replace the leaf with a subtree having 2 leaves like so:
			 *          (old, new)
			 *           /      \
			 *         old      new
			 ****************************************************************************/
			Site* oldSite = prev->s1;
			Site* newSite = s;

			beachLineNode* old_new = prev;
			old_new->s2 = newSite;
			old_new->left = new beachLineNode();
			old_new->right = new beachLineNode();

			beachLineNode* left = old_new->left;
			left->s1 = oldSite;
			left->parent = old_new;

			beachLineNode* right = old_new->right;
			right->s1 = newSite;
			right->parent = old_new;

#if _PRINT_BEACHLINE
			beachLine.printLine();
#endif

			HalfEdge* A = new HalfEdge();
			HalfEdge* B = new HalfEdge();
			old_new->edge = A;

			A->twin = B;
			A->site = oldSite;
			if (oldSite->edge == nullptr) {
				oldSite->edge = A;
			}

			B->twin = A;
			B->site = newSite;
			if (newSite->edge == nullptr) {
				newSite->edge = B;
			}

			edges.push_back(A);
			edges.push_back(B);
		}
	}

}

void Voronoi::processSiteEvent(event* e){
	//search for arc in line vertically above new site
	//if arc has a circle event, invalidate it
	beachLineNode* above = beachLine.arcAbove(e->sites[0]);
	if (above->circleEvent){
		above->circleEvent->falseAlarm = true;
	}

	beachLineNode* newArc;
	if (above->s2 == nullptr) {
		/****************************************************************************
		 * replace the leaf with a subtree having 3 leaves like so:
		 *          (new, old)
		 *           /      \
		 *   (old, new)     old
		 *    /      \
		 *  old      new
		 ****************************************************************************/
		Site* oldSite = above->s1;
		Site* newSite = e->sites[0];

		beachLineNode* new_old = above;
		new_old->s1 = newSite;
		new_old->s2 = oldSite;
		new_old->left = new beachLineNode();
		new_old->right = new beachLineNode();

		beachLineNode* old_new = above->left;
		old_new->s1 = oldSite;
		old_new->s2 = newSite;
		old_new->parent = above;
		old_new->left = new beachLineNode();
		old_new->right = new beachLineNode();

		beachLineNode* oldArcLeft = old_new->left;
		oldArcLeft->s1 = oldSite;
		oldArcLeft->parent = old_new;

		newArc = old_new->right;
		newArc->s1 = newSite;
		newArc->parent = old_new;

		beachLineNode* oldArcRight = new_old->right;
		oldArcRight->s1 = oldSite;
		oldArcRight->parent = new_old;

		//create new half-edge records that will be traced by the new breakpoint
		HalfEdge* A = new HalfEdge();
		HalfEdge* B = new HalfEdge();
		new_old->edge = A;
		old_new->edge = A;

		A->twin = B;
		A->site = oldSite;
		if (oldSite->edge == nullptr) {
			oldSite->edge = A;
		}

		B->twin = A;
		B->site = newSite;
		if (newSite->edge == nullptr) {
			newSite->edge = B;
		}

		edges.push_back(A);
		edges.push_back(B);
	}
	else { //new site is directly above an existing breakpoint in the beachline
		//invalidate existing circle events
		beachLineNode* leftArc = beachLine.predecessor(above);
		beachLineNode* rightArc = beachLine.successor(above);

		if (leftArc->circleEvent){
			leftArc->circleEvent->falseAlarm = true;
		}
		if (rightArc->circleEvent){
			rightArc->circleEvent->falseAlarm = true;
		}

		/****************************************************************************
		 * update beachline like so:
		 *      (left, right)                    (left, new)
		 *      /           \                    /         \
		 *    ...           ...       -->      ...         ...
		 *    /               \                /             \
		 * left               right         left             (new, right)
		 *                                                   /          \
		 *                                                 new          right
		 ****************************************************************************/
		beachLineNode* rightBP = rightArc;
		beachLineNode* leftBP = above;
		HalfEdge* prevEdge = leftBP->edge;
		Site* newSite = e->sites[0];
		
		leftBP->s2 = newSite;

		rightBP->s2 = rightBP->s1;
		rightBP->s1 = newSite;
		rightBP->left = new beachLineNode();
		rightBP->right = new beachLineNode();

		newArc = rightBP->left;
		newArc->s1 = newSite;
		newArc->parent = rightBP;

		beachLineNode* newRightArc = rightBP->right;
		newRightArc->s1 = rightBP->s2;
		newRightArc->parent = rightBP;

		//add new vertex
		Point2 ixt1, ixt2;
		findParabolaIntersections(newSite->p, newRightArc->s1->p, currentSweeplineY, ixt1, ixt2);

		Vertex* v = new Vertex(ixt1);
		vertices.push_back(v);

		//join existing edge to vertex
		if (prevEdge->site->p[0] < prevEdge->twin->site->p[0]) {
			prevEdge->origin = v;
			v->leaving = prevEdge;
		}
		else {
			prevEdge->twin->origin = v;
			v->leaving = prevEdge->twin;
		}

		HalfEdge* sixEdges[6] = { 0 };
		Site* involvedSites[3] = {leftBP->s1, leftBP->s2, rightBP->s2};
		matchEdges(prevEdge, sixEdges, involvedSites);
		matchEdges(prevEdge->twin, sixEdges, involvedSites);

		//add two new edges for new breakpoints
		HalfEdge* A = new HalfEdge();
		HalfEdge* B = new HalfEdge();
		A->twin = B;
		B->twin = A;
		A->site = leftBP->s1;
		if (A->site->edge == nullptr){
			A->site->edge = A;
		}
		B->site = leftBP->s2;
		if (B->site->edge == nullptr){
			B->site->edge = B;
		}
		B->origin = v;

		leftBP->edge = A;
		matchEdges(A, sixEdges, involvedSites);
		matchEdges(B, sixEdges, involvedSites);

		edges.push_back(A);
		edges.push_back(B);

		A = new HalfEdge();
		B = new HalfEdge();
		A->twin = B;
		B->twin = A;
		A->site = rightBP->s1;
		if (A->site->edge == nullptr){
			A->site->edge = A;
		}
		B->site = rightBP->s2;
		if (B->site->edge == nullptr){
			B->site->edge = B;
		}
		B->origin = v;

		rightBP->edge = A;
		matchEdges(A, sixEdges, involvedSites);
		matchEdges(B, sixEdges, involvedSites);

		edges.push_back(A);
		edges.push_back(B);

		//set 6 half-edges' next pointers
		for (int i = 0; i < 3; ++i) {
			if (sixEdges[i * 2]->origin == v)
				sixEdges[i * 2 + 1]->next = sixEdges[i * 2];
			else
				sixEdges[i * 2]->next = sixEdges[i * 2 + 1];
		}
	}

#if _PRINT_BEACHLINE
	beachLine.printLine();
#endif

	//check the two triplets of consecutive arcs for which the new arc is either the new left
	//or new right arc to see if the breakpoints converge. If so, create a new circle event
	//and add it to the queue. add pointers from the middle node of the triplet to 
	//the new event
	nodeTriplet left = beachLine.leftTriplet(newArc);
	nodeTriplet right = beachLine.rightTriplet(newArc);

	checkArcTripletForCircleEvent(left);
	checkArcTripletForCircleEvent(right);
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
			e->circleCenter = circumcenter(sites.n1->s1->p, sites.n2->s1->p, sites.n3->s1->p);
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

void Voronoi::processCircleEvent(event* e){
	//delete the leaf l that represents the disappearing arc a:
	//update the tuples representing the breakpoints at the internal nodes
	//invalidate all circle events involving a. (i.e. those pointed to by l's predecessor
	//and successor)
	beachLineNode* disappearing = e->disappearingArc;
	beachLineNode* prevArc = beachLine.prevArc(disappearing);
	if (prevArc && prevArc->circleEvent){
		prevArc->circleEvent->falseAlarm = true;
	}

	beachLineNode* nextArc = beachLine.nextArc(disappearing);
	if (nextArc && nextArc->circleEvent){
		nextArc->circleEvent->falseAlarm = true;
	}

	//add the center of the cirlce from the event as a vertex in the DCEL
	//create new half-edge records corresponding to the new breakpoint on the beachline
	Vertex* center = new Vertex(e->circleCenter);

	//attach existing half-edges to new vertex
	beachLineNode* breakpoint1 = beachLine.predecessor(disappearing);
	beachLineNode* breakpoint2 = beachLine.successor(disappearing);

	attachEdgeToCircleCenter(breakpoint1, center, false);
	attachEdgeToCircleCenter(breakpoint2, center, false);

	HalfEdge* faceEdges[6] = { 0 };

	matchEdges(breakpoint1->edge, faceEdges, e->sites);
	matchEdges(breakpoint1->edge->twin, faceEdges, e->sites);
	matchEdges(breakpoint2->edge, faceEdges, e->sites);
	matchEdges(breakpoint2->edge->twin, faceEdges, e->sites);

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

#if _PRINT_BEACHLINE
	beachLine.printLine();
#endif

	HalfEdge* A = new HalfEdge();
	HalfEdge* B = new HalfEdge();

	merge->edge = A;
	merge->circleEvent = nullptr;

	A->twin = B;
	A->site = merge->s1;
	if (merge->s1->edge == nullptr){
		merge->s1->edge = A;
	}

	B->twin = A;
	B->site = merge->s2;
	if (merge->s2->edge == nullptr){
		merge->s2->edge = B;
	}

	edges.push_back(A);
	edges.push_back(B);

	attachEdgeToCircleCenter(merge, center, true);

	matchEdges(A, faceEdges, e->sites);
	matchEdges(B, faceEdges, e->sites);

	for (int i = 0; i < 3; ++i){
		if (faceEdges[i * 2]->origin == center)
			faceEdges[i * 2 + 1]->next = faceEdges[i * 2];
		else
			faceEdges[i * 2]->next = faceEdges[i * 2 + 1];
	}

	//check the new triple of consecutive arcs that has the former left neighbor of the deleted arc
	//as its middle arc for a circle event. Same for former right neighbor
	nodeTriplet left = beachLine.leftTriplet(prevArc);
	nodeTriplet right = beachLine.rightTriplet(nextArc);

	checkArcTripletForCircleEvent(left);
	checkArcTripletForCircleEvent(right);

	vertices.push_back(center);
}

//trim vertices which fall outside the bounding box from the diagram
void Voronoi::trimOutsideEdges(){
	vector<Vertex*> cleanVerts;
	for (Vertex* v : vertices){
		Point2& p = v->p;

		if (p[0] < minX || p[0] > maxX || p[1] < minY || p[1] > maxY){
			HalfEdge* e = v->leaving;

			if (e){
				e->origin = nullptr;
				if (e->twin->origin)
					danglingEdges.push_back(e);
				e = e->twin->next;
			}

			if (e){
				e->origin = nullptr;
				if (e->twin->origin)
					danglingEdges.push_back(e);
				e = e->twin->next;
			}

			if (e){
				e->origin = nullptr;
				if (e->twin->origin)
					danglingEdges.push_back(e);
			}

			delete v;
		}
		else{
			cleanVerts.push_back(v);
		}
	}
	vertices.clear();
	vertices = cleanVerts;

	//now remove dangling edges from trimmed vertices
	int len = edges.size()/2;
	vector<HalfEdge*> cleanEdges;
	for (int i = 0; i < len; ++i){
		HalfEdge* e = edges[2 * i];

		if (e->origin || e->twin->origin){
			cleanEdges.push_back(e);
			cleanEdges.push_back(e->twin);
		}
	}

	edges.clear();
	edges = cleanEdges;
}

void Voronoi::attachEdgeToCircleCenter(beachLineNode* breakpoint, Vertex* circleCenter, bool moveSweepline){
	Point2 p1 = breakpoint->s1->p;
	Point2 p2 = breakpoint->s2->p;
	Point2 c = circleCenter->p;

	Point2* target;
	Point2 intersect1;
	Point2 intersect2;

	if (p1[1] == p2[1]){
		target = &intersect1;
		intersect1[0] = (p1[0] + p2[0]) / 2.0;
		intersect1[1] = (p1[1] + p2[1]) / 2.0;
	}
	else{
		findParabolaIntersections(p1, p2, currentSweeplineY, intersect1, intersect2);

		target = &intersect1;
		if (moveSweepline ^ (c.distanceTo(intersect1) < c.distanceTo(intersect2))){
			target = &intersect2;
		}
		if (moveSweepline){
			findParabolaIntersections(p1, p2, currentSweeplineY - 1, intersect1, intersect2);
		}
	}

	Vector2 vT = *target-c;
	Vector2 v1 = p1-c;
	Vector2 v2 = p2-c;

	double angle1 = Vector2::signedAngle(vT, v1);
	double angle2 = Vector2::signedAngle(vT, v2);

	Site* originFace = breakpoint->s1;
	if (angle2 < angle1){
		originFace = breakpoint->s2;
	}

	if (breakpoint->edge->site == originFace) {
		breakpoint->edge->origin = circleCenter;
		if (circleCenter->leaving == nullptr){
			circleCenter->leaving = breakpoint->edge;
		}
	}
	else {
		breakpoint->edge->twin->origin = circleCenter;
		if (circleCenter->leaving == nullptr){
			circleCenter->leaving = breakpoint->edge->twin;
		}
	}
}

void Voronoi::matchEdges(HalfEdge* edge, HalfEdge* faces[], Site* sites[]){
	for (int i = 0; i < 3; ++i){
		if (edge->site == sites[i]){
			if (!faces[2 * i]){
				faces[2 * i] = edge;
			}
			else {
				faces[2 * i + 1] = edge;
			}
			return;
		}
	}
}

bool orderedClockwise(const pair<HalfEdge*, boundary>& e1, const pair<HalfEdge*, boundary>& e2){
	Vector2 direction1 = e1.first->origin->p - diagramCenter;
	Vector2 direction2 = e2.first->origin->p - diagramCenter;

	double angle1 = atan2(direction1[1], direction1[0]);
	double angle2 = atan2(direction2[1], direction2[0]);

	if (angle1 < 0){
		angle1 += 2 * M_PI;
	}
	if (angle2 < 0){
		angle2 += 2 * M_PI;
	}

	return angle1 > angle2;
}

void Voronoi::attachEdgesToBoundingBox(){
	vector<pair<HalfEdge*,boundary>> orderedEdges;
	currentSweeplineY -= 1;
	
	beachLineNode* n = beachLine.min(beachLine.root);
	n = beachLine.successor(n);
	
	while (n && n->s2){
		danglingEdges.push_back(n->edge);
		n = beachLine.successor(beachLine.successor(n));
	}
	
	for(HalfEdge* e : danglingEdges){
		if (e->origin || e->twin->origin){
			Point2 src, p1, p2;
			if (e->origin){
				src = e->origin->p;
				p1 = e->site->p;
				p2 = e->twin->site->p;
			}
			else{
				src = e->twin->origin->p;
				p1 = e->twin->site->p;
				p2 = e->site->p;
			}

			Point2 intersect1, intersect2;
			findParabolaIntersections(p1, p2, currentSweeplineY, intersect1, intersect2);

			Vector2 vIxt = intersect1 - src;
			Vector2 v1 = p1-src;
			Vector2 v2 = p2-src;

			double angle1 = Vector2::signedAngle(vIxt, v1);
			double angle2 = Vector2::signedAngle(vIxt, v2);
			Point2 target;
			if (angle1 < angle2){
				target = intersect1;
			}
			else{
				target = intersect2;
			}

			Vector2 direction = target - src;
			direction = unit(direction);

			boundary b;
			double t;
			findIntersectionWithBoundaries(src, direction, t, b);

			Vertex* v = new Vertex();
			v->p = src + t*direction;

			if (e->origin){
				e->twin->origin = v;
				v->leaving = e->twin;
				orderedEdges.push_back(pair<HalfEdge*, boundary>(e->twin, b));
			}
			else{
				e->origin = v;
				v->leaving = e;
				orderedEdges.push_back(pair<HalfEdge*, boundary>(e, b));
			}
			assert(v->leaving->site != nullptr);
		}
		else{
			delete e->twin;
			delete e;
		}
	}

	std::sort(orderedEdges.begin(), orderedEdges.end(), orderedClockwise);

	//now link them up
	int numEdges = orderedEdges.size();
	for (int i = 0; i < numEdges; ++i){
		pair<HalfEdge*, boundary>* currEdge = &orderedEdges.at(i);
		pair<HalfEdge*, boundary>* nextEdge = &orderedEdges.at((i + 1) % numEdges);
		pair<HalfEdge*, boundary> tmp;
		tmp.first = currEdge->first;
		tmp.second = currEdge->second;

		HalfEdge* prevIn = nullptr;

		while (tmp.second != nextEdge->second){
			Vertex* v = new Vertex();

			switch (tmp.second){
				case RIGHT:{
					v->p = Point2(maxX, minY);
					tmp.second = BOTTOM;
				} break;
				case BOTTOM:{
					v->p = Point2(minX, minY);
					tmp.second = LEFT;
				} break;
				case LEFT:{
					v->p = Point2(minX, maxY);
					tmp.second = TOP;
				} break;
				case TOP:{
					v->p = Point2(maxX, maxY);
					tmp.second = RIGHT;
				} break;
			}

			HalfEdge* A = new HalfEdge();
			HalfEdge* B = new HalfEdge();

			HalfEdge* in;
			if (tmp.first->site == currEdge->first->site){
				in = tmp.first;
			}
			else{
				in = tmp.first->twin;
			}

			A->next = in;
			A->origin = v;
			A->site = in->site;
			A->twin = B;
			prevIn = A;

			B->origin = in->origin;
			B->twin = A;

			edges.push_back(A);
			edges.push_back(B);

			v->leaving = A;
			vertices.push_back(v);

			tmp.first = A;
		}

		if (prevIn == nullptr){
			prevIn = currEdge->first;
		}

		HalfEdge* A = new HalfEdge();
		HalfEdge* B = new HalfEdge();

		HalfEdge* in;
		HalfEdge* out;
		if (nextEdge->first->site == currEdge->first->site){
			in = nextEdge->first;
			out = nextEdge->first->twin;
		}
		else{
			in = nextEdge->first->twin;
			out = nextEdge->first;
		}

		A->next = prevIn;
		A->origin = out->origin;
		A->site = in->site;
		A->twin = B;

		B->origin = prevIn->origin;
		B->twin = A;

		in->next = A;

		edges.push_back(A);
		edges.push_back(B);
	}
}

void Voronoi::findIntersectionWithBoundaries(Point2& src, Vector2& direction, double& t, boundary& b){
	t = std::numeric_limits<double>::max();
	double tmp;

	if (direction[0] != 0.0){
		tmp = (minX - src[0]) / direction[0];
		if (tmp > 0 && tmp < t){
			t = tmp;
			b = LEFT;
		}

		tmp = (maxX - src[0]) / direction[0];
		if (tmp > 0 && tmp < t){
			t = tmp;
			b = RIGHT;
		}
	}

	if (direction[1] != 0.0){
		tmp = (minY - src[1]) / direction[1];
		if (tmp > 0 && tmp < t){
			t = tmp;
			b = BOTTOM;

		}

		tmp = (maxY - src[1]) / direction[1];
		if (tmp > 0 && tmp < t){
			t = tmp;
			b = TOP;
		}
	}
}

void Voronoi::verifyDiagram() {
	for (Vertex* v : vertices) {
		bool b = (v->leaving->origin == v);
		assert(b);
	}

	for (HalfEdge* e : edges) {
		HalfEdge* tmp = e;
		HalfEdge* first = tmp;
		while (tmp->next && tmp != first) {
			assert((tmp->site == tmp->next->site));
			assert((tmp->next->origin == tmp->twin->origin));

			tmp = tmp->next;
		}
	}

	int i = 0;
	for (Site& s : sites){
		HalfEdge* e = s.edge;
		HalfEdge* first = e;

		bool closed = false;
		while (e){
			assert((e->site == &s));
			e = e->next;

			if (e == first){
				closed = true;
				break;
			}
		}
		assert(closed);
		++i;
	}

}

//TODO
void Voronoi::relax(){
	//move each site to be the average of the vertices that surround it
	//then re-generate the diagram with the new site locations
}
