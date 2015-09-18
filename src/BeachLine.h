#ifndef _BEACH_LINE_H_
#define _BEACH_LINE_H_

//forward declarations
struct Site;
struct event;

struct beachLineNode{
	Site* s1;
	Site* s2;

	beachLineNode* parent;
	beachLineNode* left;
	beachLineNode* right;

	event* circleEvent;
};

struct nodeTriplet{
	beachLineNode* n1;
	beachLineNode* n2;
	beachLineNode* n3;
};

class BeachLine{
public:
	beachLineNode* root;

	BeachLine();
	~BeachLine();

	bool empty();

	void addArc(Site* s, bool siteAtMaxY);

	beachLineNode* arcAbove(Site* s);
	beachLineNode* predecessor(beachLineNode* n);
	beachLineNode* successor(beachLineNode* n);

	nodeTriplet leftTriplet(beachLineNode* n);
	nodeTriplet rightTriplet(beachLineNode* n);
private:
};

#endif