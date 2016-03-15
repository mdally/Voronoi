#include "Voronoi.h"
#include "Geometry.h"
#include <iostream>
using std::cout;
using std::endl;
#include <string>

BeachLine::BeachLine(){
	root = nullptr;
}

BeachLine::~BeachLine(){
	if (root) destroy(root);
}

beachLineNode* BeachLine::arcAbove(Site* s){
	beachLineNode* arc = root;

	while (arc->s2 != nullptr){
		Point2* p1 = &(arc->s1->p);
		Point2* p2 = &(arc->s2->p);

		Point2* compare;
		Point2 intersect1, intersect2;

		if ((*p1)[1] == (*p2)[1]){
			intersect1[0] = ((*p1)[0] + (*p2)[0]) / 2.0;
			intersect1[1] = ((*p1)[1] + (*p2)[1]) / 2.0;  //should just be (*p1)[1]??
			compare = &intersect1;
		}
		else{
			bool lowerFirst;

			if ((*p1)[1] < (*p2)[1]){
				lowerFirst = true;
			}
			else{
				lowerFirst = false;
			}

			//get the 2 intersections for this breakpoint. Pick either left of right based on stuff above
			//move to either left or right child depending on where new site is in relation to breakpoint
			findParabolaIntersections(*p1, *p2, s->p[1], intersect1, intersect2);

			Point2* left;
			Point2* right;

			if (intersect1[0] < intersect2[0]){
				left = &intersect1;
				right = &intersect2;
			}
			else{
				left = &intersect2;
				right = &intersect1;
			}

			if (lowerFirst){
				compare = right;
			}
			else{
				compare = left;
			}
		}
		
		if (s->p[0] == (*compare)[0]) {
			return arc;
		}
		if (s->p[0] > (*compare)[0]){
			arc = arc->right;
		}
		else{
			arc = arc->left;
		}
	}

	return arc;
}

beachLineNode* BeachLine::min(beachLineNode* n){
	if (n == nullptr) return nullptr;

	while (n->left != nullptr){
		n = n->left;
	}

	return n;
}

beachLineNode* BeachLine::max(beachLineNode* n){
	if (n == nullptr) return nullptr;

	while (n->right != nullptr){
		n = n->right;
	}

	return n;
}

beachLineNode* BeachLine::predecessor(beachLineNode* n){
	if (n == nullptr) return nullptr;

	if (n->left != nullptr){
		return max(n->left);
	}
	beachLineNode* predecessor = n->parent;
	while (predecessor != nullptr && n == predecessor->left){
		n = predecessor;
		predecessor = predecessor->parent;
	}
	return predecessor;
}

beachLineNode* BeachLine::successor(beachLineNode* n){
	if (n == nullptr) return nullptr;

	if (n->right != nullptr){
		return min(n->right);
	}
	beachLineNode* successor = n->parent;
	while (successor != nullptr && n == successor->right){
		n = successor;
		successor = successor->parent;
	}
	return successor;
}

beachLineNode* BeachLine::prevArc(beachLineNode* n){
	return predecessor(predecessor(n));
}

beachLineNode* BeachLine::nextArc(beachLineNode* n){
	return successor(successor(n));
}

nodeTriplet BeachLine::leftTriplet(beachLineNode* n){
	nodeTriplet triplet = { 0 };
	triplet.n1 = n;
	triplet.n2 = nextArc(n);
	triplet.n3 = nextArc(triplet.n2);

	return triplet;
}

nodeTriplet BeachLine::rightTriplet(beachLineNode* n){
	nodeTriplet triplet = { 0 };
	triplet.n3 = n;
	triplet.n2 = prevArc(n);
	triplet.n1 = prevArc(triplet.n2);

	return triplet;
}

void BeachLine::destroy(beachLineNode* n){
	if (n == nullptr) return;
	if (n->left) destroy(n->left);
	if (n->right) destroy(n->right);
	delete n;
}

void BeachLine::printLine(){
	beachLineNode* n = min(root);

	while(n){
		printNode(n);
		cout << endl;
		n = successor(n);
	}
	cout << endl << endl;
}

void BeachLine::printNode(beachLineNode* n){
	cout << "<";
	cout << "(" << n->s1->p[0] << "," << n->s1->p[1] << ")";
	if (n->s2){
		cout << " - (" << n->s2->p[0] << "," << n->s2->p[1] << ")";
	}
	cout << ">";
}