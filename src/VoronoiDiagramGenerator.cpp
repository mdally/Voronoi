#include "../include/VoronoiDiagramGenerator.h"
#include "../include/Vector2.h"
#include "Epsilon.h"
#include <algorithm>
#include <iostream>
using std::cout;
using std::cin;
using std::endl;

void VoronoiDiagramGenerator::printBeachLine() {
	treeNode<BeachSection>* section = beachLine->getFirst(beachLine->getRoot());

	while (section) {
		cout << section->data.site->p << endl;
		section = section->next;
	}
	if(section) cout << section->data.site->p << endl;
	cout << endl << endl;
}

bool pointComparator(std::pair<Point2,Color>* a, std::pair<Point2,Color>* b) {
	double r = b->first.y - a->first.y;
	if (r < 0) return true;
	else if (r == 0) {
		if (b->first.x - a->first.x < 0) return true;
		else return false;
	}
	else return false;
}

Diagram* VoronoiDiagramGenerator::compute(std::vector<std::pair<Point2,Color>>& sites, BoundingBox bbox) {
	siteEventQueue = new std::vector<std::pair<Point2,Color>*>();
	boundingBox = bbox;

	for (size_t i = 0; i < sites.size(); ++i) {
		//sanitize sites by quantizing to integer multiple of epsilon
		sites[i].first.x = round(sites[i].first.x / EPSILON)*EPSILON;
		sites[i].first.y = round(sites[i].first.y / EPSILON)*EPSILON;

		siteEventQueue->push_back(&(sites[i]));
	}

	diagram = new Diagram();
	circleEventQueue = new CircleEventQueue();
	beachLine = new RBTree<BeachSection>();

	// Initialize site event queue
	std::sort(siteEventQueue->begin(), siteEventQueue->end(), pointComparator);

	// process queue
	std::pair<Point2,Color>* site = siteEventQueue->empty() ? nullptr : siteEventQueue->back();
	if (!siteEventQueue->empty()) siteEventQueue->pop_back();
	treeNode<CircleEvent>* circle;

	// main loop
	for (;;) {
		// figure out whether to handle a site or circle event
		// for this we find out if there is a site event and if it is
		// 'earlier' than the circle event
		circle = circleEventQueue->firstEvent;

		// add beach section
		if (site && (!circle || site->first.y < circle->data.y || (site->first.y == circle->data.y && site->first.x < circle->data.x))) {
			// first create cell for new site
			Cell* cell = diagram->createCell(*site);
			// then create a beachsection for that site
			addBeachSection(&cell->site);

			site = siteEventQueue->empty() ? nullptr : siteEventQueue->back();
			if (!siteEventQueue->empty()) siteEventQueue->pop_back();
		}

		// remove beach section
		else if (circle)
			removeBeachSection(circle->data.beachSection);

		// all done, quit
		else
			break;
	}

	// wrapping-up:
	//   connect dangling edges to bounding box
	//   cut edges as per bounding box
	//   discard edges completely outside bounding box
	//   discard edges which are point-like
	diagram->clipEdges(boundingBox);

	//   add missing edges in order to close open cells
	diagram->closeCells(boundingBox);

	diagram->finalize();

	delete circleEventQueue;
	circleEventQueue = nullptr;

	delete siteEventQueue;
	siteEventQueue = nullptr;

	delete beachLine;
	beachLine = nullptr;

	return diagram;
}

bool halfEdgesCW(HalfEdge* e1, HalfEdge* e2) {
	return e1->angle < e2->angle;
}

Diagram* VoronoiDiagramGenerator::relax() {
	std::vector<std::pair<Point2,Color>> sites;
	std::vector<Point2> verts;
	std::vector<Vector2> vectors;
	//replace each site with its cell's centroid:
	//    subdivide the cell into adjacent triangles
	//    find those triangles' centroids (by averaging corners) 
	//    and areas (by computing vector cross product magnitude)
	//    combine the triangles' centroids through weighted average
	//	  to get the whole cell's centroid
	for (Cell* c : diagram->cells) {
		size_t edgeCount = c->halfEdges.size();
		verts.resize(edgeCount);
		vectors.resize(edgeCount);

		for (size_t i = 0; i < edgeCount; ++i) {
			verts[i] = *c->halfEdges[i]->startPoint();
			vectors[i] = *c->halfEdges[i]->startPoint() - verts[0];
		}

		Point2 centroid(0.0, 0.0);
		double totalArea = 0.0;
		for (size_t i = 1; i < edgeCount-1; ++i) {
			double area = (vectors[i+1].x*vectors[i].y - vectors[i+1].y*vectors[i].x)/2;
			totalArea += area;
			centroid.x += area*(verts[0].x + verts[i].x + verts[i + 1].x) / 3;
			centroid.y += area*(verts[0].y + verts[i].y + verts[i + 1].y) / 3;
		}
		centroid.x /= totalArea;
		centroid.y /= totalArea;
		sites.push_back(std::pair<Point2,Color>(centroid,c->color));
	}

	//then recompute the diagram using the cells' centroids
	compute(sites, boundingBox);

	return diagram;
}