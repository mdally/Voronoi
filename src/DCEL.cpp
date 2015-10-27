#include "DCEL.h"

Vertex::Vertex(){
	leaving = nullptr;
}

Vertex::Vertex(Point2 _p){
	p = _p;
	leaving = nullptr;
}

Vertex::Vertex(double _x, double _y){
	p = Point2(_x, _y);
	leaving = nullptr;
}

Vertex::Vertex(Point2 _p, HalfEdge* _e){
	p = _p;
	leaving = _e;
}

Vertex::Vertex(double _x, double _y, HalfEdge* _e){
	p = Point2(_x, _y);
	leaving = _e;
}

HalfEdge::HalfEdge(){
	origin = nullptr;
	twin = nullptr;
	next = nullptr;
	site = nullptr;
}

Site::Site(){
	p = Point2(0, 0);
	edge = nullptr;
}

Site::Site(Point2 _p){
	p = _p;
	edge = nullptr;
}

Site::Site(double _x, double _y){
	p = Point2(_x, _y);
	edge = nullptr;
}

Site::Site(Point2 _p, HalfEdge* _e){
	p = _p;
	edge = _e;
}

Site::Site(double _x, double _y, HalfEdge* _e){
	p = Point2(_x, _y);
	edge = _e;
}