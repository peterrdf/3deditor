#ifndef _CONCEPT_MESH_H_
#define _CONCEPT_MESH_H_

#include "_3DUtils.h"

#include <vector>
#include <set>
#include <map>
#include <memory>
using namespace std;

class _vector3f;
class _octree;

enum class _octant_type
{
	Unknown = -1,
	Node,
	Point,
};

enum class _octant_position
{
	Unknown = -1,
	FrontBottomLeft = 0,
	FrontBottomRight = 1,
	FrontTopLeft = 2,
	FrontTopRight = 3,
	BackBottomLeft = 4,
	BackBottomRight = 5,
	BackTopLeft = 6,
	BackTopRight = 7,
};

class  _octant
{

private: // Fields

	// --------------------------------------------------------------------------------------------
	_octree* m_pTree;
	_octant_type m_type;

protected:  // Fields

	// --------------------------------------------------------------------------------------------
	double m_dXmin;
	double m_dXmax;
	double m_dYmin;
	double m_dYmax;
	double m_dZmin;
	double m_dZmax;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	_octant(_octree* pTree, _octant_type type, double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~_octant();

	// --------------------------------------------------------------------------------------------
	virtual _octree* getTree();

	// --------------------------------------------------------------------------------------------
	_octant_type getType() const;

	// --------------------------------------------------------------------------------------------
	double getXmin() const;
	double getXmax() const;
	double getYmin() const;
	double getYmax() const;
	double getZmin() const;
	double getZmax() const;
};

class _octree_point : public _octant
{

private: // Fields

	// --------------------------------------------------------------------------------------------
	set<_octree_point*> m_setNeighbors;

	// --------------------------------------------------------------------------------------------
	multimap<_octree_point*, _octree_point*> m_mapTrinagles;

	// --------------------------------------------------------------------------------------------
	double m_dX;
	double m_dY;
	double m_dZ;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	_octree_point(_octree* pTree, double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax, double dX, double dY, double dZ);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~_octree_point();

	// --------------------------------------------------------------------------------------------
	double getX() const;

	// --------------------------------------------------------------------------------------------
	double getY() const;

	// --------------------------------------------------------------------------------------------
	double getZ() const;

	// --------------------------------------------------------------------------------------------
	double distanceTo(const _octree_point* pPoint) const;

	// --------------------------------------------------------------------------------------------
	double distanceTo(const _vector3f& pPoint) const;

	// --------------------------------------------------------------------------------------------
	// https://en.wikipedia.org/wiki/Heron%27s_formula
	double area(const _octree_point* pPoint2, const _octree_point* pPoint3) const;

	// --------------------------------------------------------------------------------------------
	set<_octree_point*>& neighbors();

	// --------------------------------------------------------------------------------------------
	multimap<_octree_point*, _octree_point*>& triangles();
};

class _octree_node : public _octant
{

protected: // Fields	

	// --------------------------------------------------------------------------------------------
	vector<_octant*> m_vecOctants;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	_octree_node(_octree* pTree, double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~_octree_node();

	// --------------------------------------------------------------------------------------------
	const vector<_octant*>& getOctants() const;

	// --------------------------------------------------------------------------------------------
	bool insertPoint(double dX, double dY, double dZ);

	// --------------------------------------------------------------------------------------------
	void populateNeighbors(_octree_point* pPoint);
};

class _octree : public _octree_node
{

private: // Fields	

	// --------------------------------------------------------------------------------------------
	int64_t m_iModel;	

	// --------------------------------------------------------------------------------------------
	set<_octree_point*> m_setPoints;

	// --------------------------------------------------------------------------------------------
	double m_dOctantLength;

	// --------------------------------------------------------------------------------------------
	double m_dSearchForNeighborsDistance;

	// --------------------------------------------------------------------------------------------
	bool m_bIgnoreEvents;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	_octree(int64_t iModel, double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~_octree();

	// --------------------------------------------------------------------------------------------
	virtual _octree* getTree();

	// --------------------------------------------------------------------------------------------
	int64_t getModel();

	// --------------------------------------------------------------------------------------------
	void onPointCreated(_octree_point* pPoint);

	// --------------------------------------------------------------------------------------------
	void onPointDeleted(_octree_point* pPoint);

	// --------------------------------------------------------------------------------------------
	double getOctantLength();

	// --------------------------------------------------------------------------------------------
	double getSearchForNeighborsDistance();

	// --------------------------------------------------------------------------------------------
	void dump();

	// --------------------------------------------------------------------------------------------
	void buildMesh();

protected: // Methods

	// --------------------------------------------------------------------------------------------
	void preProcessing(_octree_node* pNode, set<_octree_point*>& setPoints);

	// --------------------------------------------------------------------------------------------
	void buildTriangles1Point(_octree_point* pPoint1, vector<double>& vecVertices);

	// --------------------------------------------------------------------------------------------
	_octree_point* buildTriangles2Points(_octree_point* pPoint1, _octree_point* pPoint2, bool bBuildSideTriangles, vector<double>& vecVertices);

	// --------------------------------------------------------------------------------------------
	bool triangleExists(_octree_point* pPoint1, _octree_point* pPoint2, _octree_point* pPoint3);

	// --------------------------------------------------------------------------------------------
	bool addTriangle(_octree_point* pPoint1, _octree_point* pPoint2, _octree_point* pPoint3, vector<double>& vecVertices);

	// --------------------------------------------------------------------------------------------
	bool validateTriangle(_octree_point* pPoint1, _octree_point* pPoint2, _octree_point* pPoint3) const;

	// --------------------------------------------------------------------------------------------
	double calculateCircumsphere(const _octree_point* pPoint1, const _octree_point* pPoint2, const _octree_point* pPoint3, _vector3f& vecCenter) const;

	// --------------------------------------------------------------------------------------------
	bool inCircumsphere(_octree_point* pPoint, const _vector3f& vecCenter, double R) const;

	// --------------------------------------------------------------------------------------------
	bool neighborsInCircumsphere(_octree_point* pPoint1, _octree_point* pPoint2, _octree_point* pPoint3, const _vector3f& vecCenter, double R) const;
};

#endif // _CONCEPT_MESH_H_