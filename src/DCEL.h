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
};

struct HalfEdge{
	Vertex* origin;
	HalfEdge* twin;
	HalfEdge* next;
	Site* site;
};

struct Site{
	Point2 p;
	HalfEdge* edge;
};

#endif