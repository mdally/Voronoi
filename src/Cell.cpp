#include "../include/Cell.h"
#include "../include/Edge.h"
#include <algorithm>
#include <limits>

inline void Cell::addHalfEdge(HalfEdge* he) {
	HalfEdge* cur = halfEdges;
	while (cur->next) {
		cur = cur->next;
	}

	cur->next = he;
}

std::vector<Cell*> Cell::getNeighbors() {
	std::vector<Cell*> neighbors;

	HalfEdge* firstEdge = halfEdges;
	HalfEdge* curEdge = firstEdge;
	
	do {
		if((curEdge->edge->lSite && curEdge->edge->rSite) || (curEdge->edge->rSite && curEdge->site != &site))
			neighbors.push_back(curEdge->site->cell);
		curEdge = curEdge->next;
	} while (curEdge != firstEdge);

	return neighbors;
}

cellBoundingBox Cell::getBoundingBox() {
	double xmin = std::numeric_limits<double>::infinity();
	double ymin = xmin;
	double xmax = -xmin;
	double ymax = xmax;

	HalfEdge* firstEdge = halfEdges;
	HalfEdge* curEdge = firstEdge;

	Point2* vert;
	do {
		vert = curEdge->startPoint();

		double vx = vert->x;
		double vy = vert->y;

		if (vx < xmin) xmin = vx;
		if (vy < ymin) ymin = vy;
		if (vx > xmax) xmax = vx;
		if (vy > ymax) ymax = vy;

		curEdge = curEdge->next;
	} while (firstEdge != curEdge);

	return cellBoundingBox(xmin, ymin, xmax, ymax);
}

// Return whether a point is inside, on, or outside the cell:
//   -1: point is outside the perimeter of the cell
//    0: point is on the perimeter of the cell
//    1: point is inside the perimeter of the cell
//
int Cell::pointIntersection(double x, double y) {
	// Check if point in polygon. Since all polygons of a Voronoi
	// diagram are convex, then:
	// http://paulbourke.net/geometry/polygonmesh/
	// Solution 3 (2D):
	//   "If the polygon is convex then one can consider the polygon
	//   "as a 'path' from the first vertex. A point is on the interior
	//   "of this polygons if it is always on the same side of all the
	//   "line segments making up the path. ...
	//   "(y - y0) (x1 - x0) - (x - x0) (y1 - y0)
	//   "if it is less than 0 then P is to the right of the line segment,
	//   "if greater than 0 it is to the left, if equal to 0 then it lies
	//   "on the line segment"
	Point2 p0;
	Point2 p1;
	double r;

	HalfEdge* firstEdge = halfEdges;
	HalfEdge* curEdge = firstEdge;

	do {
		p0 = *curEdge->startPoint();
		p1 = *curEdge->endPoint();
		r = (y - p0.y)*(p1.x - p0.x) - (x - p0.x)*(p1.y - p0.y);

		if (r == 0) {
			return 0;
		}
		if (r > 0) {
			return -1;
		}

		curEdge = curEdge->next;
	} while (firstEdge != curEdge);
	return 1;
}

bool Cell::edgesCCW(HalfEdge* a, HalfEdge* b) {
	return a->angle > b->angle;
}
