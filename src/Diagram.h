#ifndef _DIAGRAM_H_
#define _DIAGRAM_H_

#include "MemoryPool.h"
#include "Edge.h"
#include "Cell.h"
#include <set>

struct BoundingBox;
class Diagram {
public:
	std::vector<Cell*> cells;
	std::vector<Edge*> edges;
	std::vector<Point2*> vertices;

	void printDiagram();
private:
	friend class VoronoiDiagramGenerator;

	std::set<Cell*> tmpCells;
	std::set<Edge*> tmpEdges;
	std::set<Point2*> tmpVertices;

	MemoryPool<Cell> cellPool;
	MemoryPool<Edge> edgePool;
	MemoryPool<HalfEdge> halfEdgePool;
	MemoryPool<Point2> vertexPool;

	Point2* createVertex(double x, double y);
	Cell* createCell(Point2 site);
	Edge* createEdge(Site* lSite, Site* rSite, Point2* vertA, Point2* vertB);
	Edge* createBorderEdge(Site* lSite, Point2* vertA, Point2* vertB);
	HalfEdge* createHalfEdge(Edge* edge, Site* lSite, Site* rSite);

	bool connectEdge(Edge* edge, BoundingBox bbox);
	bool clipEdge(Edge* edge, BoundingBox bbox);
	void clipEdges(BoundingBox bbox);
	void closeCells(BoundingBox bbox);
	void finalize();
};

#endif