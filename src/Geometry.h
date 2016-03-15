#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "Point2.h"
#include "DCEL.h"

inline bool siteToLeft(Site* s1, Site* s2);

Point2 circumcenter(Point2 A, Point2 B, Point2 C);

void findParabolaIntersections(Point2& focus1, Point2& focus2, double directrixY, 
	Point2& intersection1, Point2& intersection2);

#endif