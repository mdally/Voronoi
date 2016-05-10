#include "Point2.h"
#include "CircleEventQueue.h"
#include "Cell.h"

void CircleEventQueue::addCircleEvent(treeNode<BeachSection>* section) {
	if (!section) return;
	treeNode<BeachSection>* lSection = section->prev;
	treeNode<BeachSection>* rSection = section->next;
	if (!lSection || !rSection) { return; }

	Point2 lSite = lSection->data.site->p;
	Point2 cSite = section->data.site->p;
	Point2 rSite = rSection->data.site->p;

	// If site of left beachsection is same as site of
	// right beachsection, there can't be convergence
	if (lSection == rSection) { return; }

	// Find the circumscribed circle for the three sites associated
	// with the beachsection triplet.
	// rhill 2011-05-26: It is more efficient to calculate in-place
	// rather than getting the resulting circumscribed circle from an
	// object returned by calling Voronoi.circumcircle()
	// http://mathforum.org/library/drmath/view/55002.html
	// Except that I bring the origin at cSite to simplify calculations.
	// The bottom-most part of the circumcircle is our Fortune 'circle
	// event', and its center is a vertex potentially part of the final
	// Voronoi diagram.
	double bx = cSite.x;
	double by = cSite.y;
	double ax = lSite.x - bx;
	double ay = lSite.y - by;
	double cx = rSite.x - bx;
	double cy = rSite.y - by;

	// If points l->c->r are clockwise, then center beach section does not
	// collapse, hence it can't end up as a vertex (we reuse 'd' here, which
	// sign is reverse of the orientation, hence we reverse the test.
	// http://en.wikipedia.org/wiki/Curve_orientation#Orientation_of_a_simple_polygon
	// rhill 2011-05-21: Nasty finite precision error which caused circumcircle() to
	// return infinites: 1e-12 seems to fix the problem.
	double d = 2 * (ax*cy - ay*cx);
	if (d >= -2e-12) { return; }

	double ha = ax*ax + ay*ay;
	double hc = cx*cx + cy*cy;
	double x = (cy*ha - ay*hc) / d;
	double y = (ax*hc - cx*ha) / d;
	double ycenter = y + by;

	// Important: ybottom should always be under or at sweep, so no need
	// to waste CPU cycles by checking
	CircleEvent circleEvent(section->data.site, x + bx, ycenter + sqrt(x*x + y*y), ycenter, section);

	// find insertion point in RB-tree: circle events are ordered from
	// smallest to largest
	treeNode<CircleEvent>* predecessor = nullptr;
	treeNode<CircleEvent>* node = eventQueue.getRoot();
	while (node) {
		CircleEvent& nodeEvent = node->data;
		if (circleEvent.y < nodeEvent.y || (circleEvent.y == nodeEvent.y && circleEvent.x <= nodeEvent.x)) {
			if (node->left) {
				node = node->left;
			}
			else {
				predecessor = node->prev;
				break;
			}
		}
		else {
			if (node->right) {
				node = node->right;
			}
			else {
				predecessor = node;
				break;
			}
		}
	}
	treeNode<CircleEvent>* newEvent = eventQueue.insertSuccessor(predecessor, circleEvent);
	section->data.circleEvent = newEvent;
	if (!predecessor) {
		firstEvent = newEvent;
	}
}

void CircleEventQueue::removeCircleEvent(treeNode<BeachSection>* section) {
	//if (!section) return;
	treeNode<CircleEvent>* circleEvent = section->data.circleEvent;
	if (circleEvent) {
		if (!circleEvent->prev) {
			firstEvent = circleEvent->next;
		}
		eventQueue.removeNode(circleEvent);
		section->data.circleEvent = nullptr;
	}
}
