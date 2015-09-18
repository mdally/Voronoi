#include "BeachLine.h"

BeachLine::BeachLine(){

}

BeachLine::~BeachLine(){

}

bool BeachLine::empty(){
	return true;
}

void BeachLine::addArc(Site* s, bool siteAtMaxY){

}

beachLineNode* BeachLine::arcAbove(Site* s){
	return nullptr;
}

beachLineNode* BeachLine::predecessor(beachLineNode* n){
	return nullptr;
}

beachLineNode* BeachLine::successor(beachLineNode* n){
	return nullptr;
}

nodeTriplet BeachLine::leftTriplet(beachLineNode* n){
	nodeTriplet trash = { 0 };
	return trash;
}

nodeTriplet BeachLine::rightTriplet(beachLineNode* n){
	nodeTriplet trash = { 0 };
	return trash;
}