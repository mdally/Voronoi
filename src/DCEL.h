#ifndef _DCEL_H_
#define _DCEL_H_

#include "Point2.h"
#include "Vector2.h"

//TODO: change DCEL to class to provide constructors

//forward declarations
struct HalfEdge;
struct Site;

struct Vertex{
	Point2 p;
	HalfEdge* leaving;

	Vertex();
	Vertex(Point2 _p);
	Vertex(double _x, double _y);
	Vertex(Point2 _p, HalfEdge* _e);
	Vertex(double _x, double _y, HalfEdge* _e);
};

struct HalfEdge{
	Vertex* origin;
	HalfEdge* twin;
	HalfEdge* next;
	Site* site;

	HalfEdge();
};

struct Site{
	Point2 p;
	HalfEdge* edge;

	Site();
	Site(Point2 _p);
	Site(double _x, double _y);
	Site(Point2 _p, HalfEdge* _e);
	Site(double _x, double _y, HalfEdge* _e);
};

#endif
