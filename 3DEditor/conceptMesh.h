#ifndef _CONCEPT_MESH_H_
#define _CONCEPT_MESH_H_

#include <vector>
#include <set>
#include <map>
#include <memory>

using namespace std;

// ------------------------------------------------------------------------------------------------
class _vector3f;
class _octree;

// ------------------------------------------------------------------------------------------------
enum class _octant_type
{
	Unknown = -1,
	Node,
	Point,
};

// ------------------------------------------------------------------------------------------------
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

// ------------------------------------------------------------------------------------------------
class  _octant
{

private: // Members

	// --------------------------------------------------------------------------------------------
	_octree* m_pTree;
	_octant_type m_type;

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

// ------------------------------------------------------------------------------------------------
class _octree_point : public _octant
{

private: // Members

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

// ------------------------------------------------------------------------------------------------
class _octree_node : public _octant
{

protected: // Members	

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

// ------------------------------------------------------------------------------------------------
class _octree : public _octree_node
{

private: // Members	

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

// ------------------------------------------------------------------------------------------------
// https://gist.github.com/zooty/f2222e0bc33e6845f068
class _vector3f {
private:
	double x;
	double y;
	double z;

public:
	_vector3f();
	_vector3f(double, double, double);

	double getX() const { return x; }
	double getY() const { return y; }
	double getZ() const { return z; }

	double length() {
		return sqrt((x * x) + (y * y) + (z * z));
	}

	double length2() {
		return pow(length(), 2.);
	}

	_vector3f normalized() {
		double mag = length();

		return _vector3f(x / mag, y / mag, z / mag);
	}

	_vector3f neg() {
		return _vector3f(-x, -y, -z);
	}

	double dot(_vector3f other) {
		return x * other.getX() + y * other.getY() + z * other.getZ();
	}

	_vector3f cross(_vector3f other) {
		double x_ = y * other.getZ() - z * other.getY();
		double y_ = z * other.getX() - x * other.getZ();
		double z_ = x * other.getY() - y * other.getX();

		return _vector3f(x_, y_, z_);
	}

	_vector3f operator + (const _vector3f& other) {
		return _vector3f(x + other.x, y + other.y, z + other.z);
	}

	_vector3f operator - (const _vector3f& other) {
		return _vector3f(x - other.x, y - other.y, z - other.z);
	}

	_vector3f operator * (const _vector3f& other) {
		return _vector3f(x * other.x, y * other.y, z * other.z);
	}

	_vector3f operator * (double number) {
		return _vector3f(x * number, y * number, z * number);
	}

	_vector3f operator / (const _vector3f& other) {
		return _vector3f(x / other.x, y / other.y, z / other.z);
	}

	_vector3f operator / (double number) {
		return _vector3f(x / number, y / number, z / number);
	}
};

#endif // _CONCEPT_MESH_H_