#ifndef _DCEL_H_
#define _DCEL_H_

//TODO: get better point and vector classes
//TODO: change DCEL to class to provide constructors

//forward declarations
struct HalfEdge;
struct Site;

struct Point{
	double x, y;
};

struct Vertex{
	Point p;
	HalfEdge* leaving;
};

struct HalfEdge{
	Vertex* origin;
	HalfEdge* twin;
	HalfEdge* next;
	Site* site;
};

struct Site{
	Point p;
	HalfEdge* edge;
};

#endif