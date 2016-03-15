#include "Geometry.h"
#include <math.h>

inline bool siteToLeft(Site* s1, Site* s2){
	if (s1->p[0] < s2->p[0] ||
		(s1->p[0] == s2->p[0] && s1->p[1] < s2->p[1]))
		return true;
	return false;
}

Point2 circumcenter(Point2 A, Point2 B, Point2 C){
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

	if (bmc1 == 0.0){
		double x = focus1[0];
		intersection1[0] = intersection2[0] = x;
		intersection1[1] = intersection2[1] = a2*x*x + b2*x + c2;
	}
	else if (bmc2 == 0.0){
		double x = focus2[0];
		intersection1[0] = intersection2[0] = focus2[0];
		intersection1[1] = intersection2[1] = a1*x*x + b1*x + c1;
	}
	else{
		intersection1[0] = (-b - sqrt(b*b - 4.0*a*c)) / (2.0*a);
		intersection2[0] = (-b + sqrt(b*b - 4.0*a*c)) / (2.0*a);

		intersection1[1] = a1*intersection1[0] * intersection1[0] + b1*intersection1[0] + c1;
		intersection2[1] = a2*intersection2[0] * intersection2[0] + b2*intersection2[0] + c2;
	}
}