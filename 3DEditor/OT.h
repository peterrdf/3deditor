#pragma once

#include <vector>
#include <set>
#include <map>
#include <memory>

using namespace std;

// ------------------------------------------------------------------------------------------------
class Vector3f;

// ------------------------------------------------------------------------------------------------
class CRDFController;

// ------------------------------------------------------------------------------------------------
enum class OctantType
{
	Unknown = -1,
	Node,
	Point,	
};

/*
Front
XYZ
010(2)-110(6)
000(0)-100(4)

Back
XYZ
011(3)-111(7)
001(1)-101(5)
*/
// ------------------------------------------------------------------------------------------------
enum class OctantPosition
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

// ------------------------------------------------------------------------------------------------
class COctant
{

private: // Members

	// --------------------------------------------------------------------------------------------
	OctantType m_type;


protected:  // Members

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
	COctant(OctantType type, double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~COctant();

	// --------------------------------------------------------------------------------------------
	double getXmin() const;
	double getXmax() const;
	double getYmin() const;
	double getYmax() const;
	double getZmin() const;
	double getZmax() const;

	// --------------------------------------------------------------------------------------------
	OctantType getType() const;
};

// ------------------------------------------------------------------------------------------------
class COctreePoint : public COctant
{

private: // Members

	// --------------------------------------------------------------------------------------------
	set<COctreePoint*> m_setNeighbors;

	// --------------------------------------------------------------------------------------------
	multimap<COctreePoint*, COctreePoint*> m_mapTrinagles;

	// --------------------------------------------------------------------------------------------
	double m_dX;
	double m_dY;
	double m_dZ;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	COctreePoint(double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax, double dX, double dY, double dZ);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~COctreePoint();

	// --------------------------------------------------------------------------------------------
	double getX() const;

	// --------------------------------------------------------------------------------------------
	double getY() const;

	// --------------------------------------------------------------------------------------------
	double getZ() const;

	// --------------------------------------------------------------------------------------------
	double distanceTo(const COctreePoint* pPoint) const;

	// --------------------------------------------------------------------------------------------
	double distanceTo(const Vector3f& pPoint) const;

	// --------------------------------------------------------------------------------------------
	// https://en.wikipedia.org/wiki/Heron%27s_formula
	double area(const COctreePoint* pPoint2, const COctreePoint* pPoint3) const;

	// --------------------------------------------------------------------------------------------
	set<COctreePoint*>& neighbors();

	// --------------------------------------------------------------------------------------------
	multimap<COctreePoint*, COctreePoint*>& triangles();
};

// ------------------------------------------------------------------------------------------------
class COctreeNode : public COctant
{

protected: // Members	

	// --------------------------------------------------------------------------------------------
	vector<COctant*> m_vecOctants;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	COctreeNode(double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~COctreeNode();	

	// --------------------------------------------------------------------------------------------
	const vector<COctant*>& getOctants() const;

	// --------------------------------------------------------------------------------------------
	bool insertPoint(double dX, double dY, double dZ);

	// --------------------------------------------------------------------------------------------
	void findNeighbors(COctreePoint* pPoint);
};

// ------------------------------------------------------------------------------------------------
class COctree : public COctreeNode
{

	// --------------------------------------------------------------------------------------------
	friend class CRDFController;

private: // Members	

	// --------------------------------------------------------------------------------------------
	static CRDFController* s_pController;

	// --------------------------------------------------------------------------------------------
	static int64_t s_iModel;

	// --------------------------------------------------------------------------------------------
	static set<COctreePoint*> s_setPoints;

	// --------------------------------------------------------------------------------------------
	static double s_dOctantLength;

	// --------------------------------------------------------------------------------------------
	static double s_dSearchForNeighborsDistance;
		
public: // Members	

	// --------------------------------------------------------------------------------------------
	static bool s_bStepByStepMode; 

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	COctree(int64_t iModel, double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~COctree();

	// --------------------------------------------------------------------------------------------
	static int64_t getModel();

	// --------------------------------------------------------------------------------------------
	static void onPointCreated(COctreePoint* pPoint);

	// --------------------------------------------------------------------------------------------
	static void onPointDeleted(COctreePoint* pPoint);

	// --------------------------------------------------------------------------------------------
	static double getOctantLength();

	// --------------------------------------------------------------------------------------------
	static double getSearchForNeighborsDistance();

	// --------------------------------------------------------------------------------------------
	void dump();

	// --------------------------------------------------------------------------------------------
	void buildMesh();

protected: // Methods

	// --------------------------------------------------------------------------------------------
	void buildMesh(COctreeNode* pNode, set<COctreePoint*>& setPoints);

	// --------------------------------------------------------------------------------------------
	void buildTriangles1Point(COctreePoint* pPoint1, vector<double>& vecVertices);	

	// --------------------------------------------------------------------------------------------
	COctreePoint* buildTriangles2Points(COctreePoint* pPoint1, COctreePoint* pPoint2, bool bBuildSideTriangles, vector<double>& vecVertices);

	// --------------------------------------------------------------------------------------------
	bool triangleExists(COctreePoint* pPoint1, COctreePoint* pPoint2, COctreePoint* pPoint3);

	// --------------------------------------------------------------------------------------------
	bool addTriangle(COctreePoint* pPoint1, COctreePoint* pPoint2, COctreePoint* pPoint3, vector<double>& vecVertices);

	// --------------------------------------------------------------------------------------------
	bool validateTriangle(COctreePoint* pPoint1, COctreePoint* pPoint2, COctreePoint* pPoint3) const;

	// --------------------------------------------------------------------------------------------
	double calculateCircumsphere(const COctreePoint* pPoint1, const COctreePoint* pPoint2, const COctreePoint* pPoint3, Vector3f& vecCenter) const;

	// --------------------------------------------------------------------------------------------
	bool inCircumsphere(COctreePoint* pPoint, const Vector3f& vecCenter, double R) const;

	// --------------------------------------------------------------------------------------------
	bool neighborsInCircumsphere(COctreePoint* pPoint1, COctreePoint* pPoint2, COctreePoint* pPoint3, const Vector3f& vecCenter, double R) const;
};

