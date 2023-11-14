#include "stdafx.h"
#include "conceptMesh.h"
#include "_3DUtils.h"

#include <string>

// ------------------------------------------------------------------------------------------------
#define EPSILON 1E-6

// ------------------------------------------------------------------------------------------------
_octant::_octant(_octree* pTree, _octant_type type, double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax)
	: m_pTree(pTree)
	, m_type(type)
	, m_dXmin(dXmin)
	, m_dXmax(dXmax)
	, m_dYmin(dYmin)
	, m_dYmax(dYmax)
	, m_dZmin(dZmin)
	, m_dZmax(dZmax)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ _octant::~_octant()
{
}

// --------------------------------------------------------------------------------------------
/*virtual*/ _octree* _octant::getTree()
{
	return m_pTree;
}

// ------------------------------------------------------------------------------------------------
_octant_type _octant::getType() const
{
	return m_type;
}

// --------------------------------------------------------------------------------------------
double _octant::getXmin() const
{
	return m_dXmin;
}

// --------------------------------------------------------------------------------------------
double _octant::getXmax() const
{
	return m_dXmax;
}

// --------------------------------------------------------------------------------------------
double _octant::getYmin() const
{
	return m_dYmin;
}

// --------------------------------------------------------------------------------------------
double _octant::getYmax() const
{
	return m_dYmax;
}

// --------------------------------------------------------------------------------------------
double _octant::getZmin() const
{
	return m_dZmin;
}

// --------------------------------------------------------------------------------------------
double _octant::getZmax() const
{
	return m_dZmax;
}

// ------------------------------------------------------------------------------------------------
_octree_point::_octree_point(_octree* pTree, double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax, double dX, double dY, double dZ)
	: _octant(pTree, _octant_type::Point, dXmin, dXmax, dYmin, dYmax, dZmin, dZmax)
	, m_setNeighbors()
	, m_mapTrinagles()
	, m_dX(dX)
	, m_dY(dY)
	, m_dZ(dZ)
{
	getTree()->onPointCreated(this);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ _octree_point::~_octree_point()
{
	getTree()->onPointDeleted(this);
}

// ------------------------------------------------------------------------------------------------
double _octree_point::getX() const
{
	return m_dX;
}

// ------------------------------------------------------------------------------------------------
double _octree_point::getY() const
{
	return m_dY;
}

// ------------------------------------------------------------------------------------------------
double _octree_point::getZ() const
{
	return m_dZ;
}

// ------------------------------------------------------------------------------------------------
double _octree_point::distanceTo(const _octree_point* pPoint) const
{
	return sqrt(
		pow(m_dX - pPoint->m_dX, 2.) +
		pow(m_dY - pPoint->m_dY, 2.) +
		pow(m_dZ - pPoint->m_dZ, 2.));
}

// ------------------------------------------------------------------------------------------------
double _octree_point::distanceTo(const _vector3f& pPoint) const
{
	return sqrt(
		pow(m_dX - pPoint.getX(), 2.) +
		pow(m_dY - pPoint.getY(), 2.) +
		pow(m_dZ - pPoint.getZ(), 2.));
}

// ------------------------------------------------------------------------------------------------
set<_octree_point*>& _octree_point::neighbors()
{
	return m_setNeighbors;
}

// ------------------------------------------------------------------------------------------------
// https://en.wikipedia.org/wiki/Heron%27s_formula
double _octree_point::area(const _octree_point* pPoint2, const _octree_point* pPoint3) const
{
	auto a = distanceTo(pPoint2);
	auto b = pPoint2->distanceTo(pPoint3);
	auto c = pPoint3->distanceTo(this);

	// Semi-perimeter
	auto s = (a + b + c) / 2.;

	return sqrt(s * (s - a) * (s - b) * (s - c));
}

// --------------------------------------------------------------------------------------------
multimap<_octree_point*, _octree_point*>& _octree_point::triangles()
{
	return m_mapTrinagles;
}

// ------------------------------------------------------------------------------------------------
_octree_node::_octree_node(_octree* pTree, double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax)
	: _octant(pTree, _octant_type::Node, dXmin, dXmax, dYmin, dYmax, dZmin, dZmax)
	, m_vecOctants(8, nullptr)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ _octree_node::~_octree_node()
{
	for (size_t iOctant = 0; iOctant < m_vecOctants.size(); iOctant++)
	{
		delete m_vecOctants[iOctant];
	}
}

// ------------------------------------------------------------------------------------------------
const vector<_octant*>& _octree_node::getOctants() const
{
	return m_vecOctants;
}

// ------------------------------------------------------------------------------------------------
bool _octree_node::insertPoint(double dX, double dY, double dZ)
{
	/*
	* Validate
	*/

	if ((dX < m_dXmin) || (dX > m_dXmax))
	{
		// Invalid point!
		return false;
	}

	if ((dY < m_dYmin) || (dY > m_dYmax))
	{
		// Invalid point!
		return false;
	}

	if ((dZ < m_dZmin) || (dZ > m_dZmax))
	{
		// Invalid point!
		return false;
	}

	/*
	* Find the target octant
	*/

	_octant_position position = _octant_position::Unknown;

	const double X_MID = m_dXmin + ((m_dXmax - m_dXmin) / 2.);
	const double Y_MID = m_dYmin + ((m_dYmax - m_dYmin) / 2.);
	const double Z_MID = m_dZmin + ((m_dZmax - m_dZmin) / 2.);

	if ((dX >= m_dXmin) && (dX < X_MID))
	{
		// Left =>

		if ((dY >= m_dYmin) && (dY < Y_MID))
		{
			// Bottom  =>

			if ((dZ >= m_dZmin) && (dZ < Z_MID))
			{
				// Front
				position = _octant_position::FrontBottomLeft;
			}
			else
			{
				// Back
				position = _octant_position::BackBottomLeft;
			}
		} // if ((dY >= m_dYmin) && (dY < Y_MID))
		else
		{
			// Top =>

			if ((dZ >= m_dZmin) && (dZ < Z_MID))
			{
				// Front
				position = _octant_position::FrontTopLeft;
			}
			else
			{
				// Back
				position = _octant_position::BackTopLeft;
			}
		} // else if ((dY >= m_dYmin) && (dY < Y_MID))
	} // if ((dX >= m_dXmin) && (dX < X_MID))
	else
	{
		// Right =>

		if ((dY >= m_dYmin) && (dY < Y_MID))
		{
			// Bottom =>

			if ((dZ >= m_dZmin) && (dZ < Z_MID))
			{
				// Front
				position = _octant_position::FrontBottomRight;
			}
			else
			{
				// Back
				position = _octant_position::BackBottomRight;
			}
		} // if ((dY >= m_dYmin) && (dY < Y_MID))
		else
		{
			// Top =>

			if ((dZ >= m_dZmin) && (dZ < Z_MID))
			{
				// Front
				position = _octant_position::FrontTopRight;
			}
			else
			{
				// Back
				position = _octant_position::BackTopRight;
			}
		} // else if ((dY >= m_dYmin) && (dY < Y_MID))
	} // else if ((dX >= m_dXmin) && (dX < X_MID))

	if (position == _octant_position::Unknown)
	{
		assert(false); // Internal error!

		return false;
	}

	/*
	* Add a node
	*/
	if (m_vecOctants[(int)position] != nullptr)
	{
		if (m_vecOctants[(int)position]->getType() == _octant_type::Point)
		{
			/*
			* Add a node
			*/
			_octree_node* pNewNode = nullptr;
			switch (position)
			{
				case _octant_position::FrontBottomLeft:
				{
					pNewNode = new _octree_node(getTree(), m_dXmin, X_MID, m_dYmin, Y_MID, m_dZmin, Z_MID);
				}
				break;

				case _octant_position::FrontBottomRight:
				{
					pNewNode = new _octree_node(getTree(), X_MID, m_dXmax, m_dYmin, Y_MID, m_dZmin, Z_MID);
				}
				break;

				case _octant_position::FrontTopLeft:
				{
					pNewNode = new _octree_node(getTree(), m_dXmin, X_MID, Y_MID, m_dYmax, m_dZmin, Z_MID);
				}
				break;

				case _octant_position::FrontTopRight:
				{
					pNewNode = new _octree_node(getTree(), X_MID, m_dXmax, Y_MID, m_dYmax, m_dZmin, Z_MID);
				}
				break;

				case _octant_position::BackBottomLeft:
				{
					pNewNode = new _octree_node(getTree(), m_dXmin, X_MID, m_dYmin, Y_MID, Z_MID, m_dZmax);
				}
				break;

				case _octant_position::BackBottomRight:
				{
					pNewNode = new _octree_node(getTree(), X_MID, m_dXmax, m_dYmin, Y_MID, Z_MID, m_dZmax);
				}
				break;

				case _octant_position::BackTopLeft:
				{
					pNewNode = new _octree_node(getTree(), m_dXmin, X_MID, Y_MID, m_dYmax, Z_MID, m_dZmax);
				}
				break;

				case _octant_position::BackTopRight:
				{
					pNewNode = new _octree_node(getTree(), X_MID, m_dXmax, Y_MID, m_dYmax, Z_MID, m_dZmax);
				}
				break;

				default:
				{
					assert(false); // Internal error!
				}
				break;
			} // switch (position)

			if (pNewNode != nullptr)
			{
				auto pPoint = dynamic_cast<_octree_point*>(m_vecOctants[(int)position]);
				assert(pPoint != nullptr);

				pNewNode->insertPoint(pPoint->getX(), pPoint->getY(), pPoint->getZ());
				delete pPoint;

				pNewNode->insertPoint(dX, dY, dZ);

				m_vecOctants[(int)position] = pNewNode;
			}
		} // if (m_vecOctants[(int)position]->getType() == _octant_type::Point)
		else if (m_vecOctants[(int)position]->getType() == _octant_type::Node)
		{
			auto pNode = dynamic_cast<_octree_node*>(m_vecOctants[(int)position]);
			assert(pNode != nullptr);

			pNode->insertPoint(dX, dY, dZ);
		}
		else
		{
			assert(false); // Internal error!
		}
	} // if (m_vecOctants[(int)position] != nullptr)
	else
	{
		/*
		* Add a point
		*/
		_octree_point* pNewPoint = nullptr;
		switch (position)
		{
			case _octant_position::FrontBottomLeft:
			{
				pNewPoint = new _octree_point(getTree(), m_dXmin, X_MID, m_dYmin, Y_MID, m_dZmin, Z_MID, dX, dY, dZ);
			}
			break;

			case _octant_position::FrontBottomRight:
			{
				pNewPoint = new _octree_point(getTree(), X_MID, m_dXmax, m_dYmin, Y_MID, m_dZmin, Z_MID, dX, dY, dZ);
			}
			break;

			case _octant_position::FrontTopLeft:
			{
				pNewPoint = new _octree_point(getTree(), m_dXmin, X_MID, Y_MID, m_dYmax, m_dZmin, Z_MID, dX, dY, dZ);
			}
			break;

			case _octant_position::FrontTopRight:
			{
				pNewPoint = new _octree_point(getTree(), X_MID, m_dXmax, Y_MID, m_dYmax, m_dZmin, Z_MID, dX, dY, dZ);
			}
			break;

			case _octant_position::BackBottomLeft:
			{
				pNewPoint = new _octree_point(getTree(), m_dXmin, X_MID, m_dYmin, Y_MID, Z_MID, m_dZmax, dX, dY, dZ);
			}
			break;

			case _octant_position::BackBottomRight:
			{
				pNewPoint = new _octree_point(getTree(), X_MID, m_dXmax, m_dYmin, Y_MID, Z_MID, m_dZmax, dX, dY, dZ);
			}
			break;

			case _octant_position::BackTopLeft:
			{
				pNewPoint = new _octree_point(getTree(), m_dXmin, X_MID, Y_MID, m_dYmax, Z_MID, m_dZmax, dX, dY, dZ);
			}
			break;

			case _octant_position::BackTopRight:
			{
				pNewPoint = new _octree_point(getTree(), X_MID, m_dXmax, Y_MID, m_dYmax, Z_MID, m_dZmax, dX, dY, dZ);
			}
			break;

			default:
			{
				assert(false); // Internal error!
			}
			break;
		} // switch (position)

		m_vecOctants[(int)position] = pNewPoint;
	} // else if (m_vecOctants[(int)position] != nullptr)

	return true;
}

// ------------------------------------------------------------------------------------------------
void _octree_node::populateNeighbors(_octree_point* pPoint)
{
	if (pPoint == nullptr)
	{
		assert(false);

		return;
	}

	for (size_t iOctant = 0; iOctant < m_vecOctants.size(); iOctant++)
	{
		if (m_vecOctants[iOctant] == nullptr)
		{
			// Empty octant
			continue;
		}

		if (m_vecOctants[iOctant] == pPoint)
		{
			// The point
			continue;
		}

		// Prune - X
		if ((pPoint->getX() < getXmin()) &&
			(abs(pPoint->getX() - getXmin()) > getTree()->getSearchForNeighborsDistance()))
		{
			return;
		}

		// Prune - X
		if ((pPoint->getX() > getXmax()) &&
			(abs(pPoint->getX() - getXmax()) > getTree()->getSearchForNeighborsDistance()))
		{
			return;
		}

		// Prune - Y
		if ((pPoint->getY() < getYmin()) &&
			(abs(pPoint->getY() - getYmin()) > getTree()->getSearchForNeighborsDistance()))
		{
			return;
		}

		// Prune - Y
		if ((pPoint->getY() > getYmax()) &&
			(abs(pPoint->getY() - getYmax()) > getTree()->getSearchForNeighborsDistance()))
		{
			return;
		}

		// Prune - Z
		if ((pPoint->getZ() < getZmin()) &&
			(abs(pPoint->getZ() - getZmin()) > getTree()->getSearchForNeighborsDistance()))
		{
			return;
		}

		// Prune - Z
		if ((pPoint->getZ() > getZmax()) &&
			(abs(pPoint->getZ() - getZmax()) > getTree()->getSearchForNeighborsDistance()))
		{
			return;
		}

		switch (m_vecOctants[(int)iOctant]->getType())
		{
			case _octant_type::Node:
			{
				auto pNode = dynamic_cast<_octree_node*>(m_vecOctants[(int)iOctant]);
				assert(pNode != nullptr);

				pNode->populateNeighbors(pPoint);
			}
			break;

			case _octant_type::Point:
			{
				auto pPoint2 = dynamic_cast<_octree_point*>(m_vecOctants[(int)iOctant]);
				assert(pPoint2 != nullptr);

				assert(pPoint != pPoint2);

				double dDistance = pPoint->distanceTo(pPoint2);
				if (dDistance <= getTree()->getSearchForNeighborsDistance())
				{
					// It is neighbor
					if (pPoint->neighbors().find(pPoint2) == pPoint->neighbors().end())
					{
						pPoint->neighbors().insert(pPoint2);
						pPoint2->neighbors().insert(pPoint);
					} // if (pPoint->neighbors().find(pPoint2) == pPoint->neighbors().end())
				}
			}
			break;

			default:
			{
				assert(false); //Internal error!
			}
			break;
		} // switch (m_vecOctants[(int)iOctant]->getType())
	} // for (size_t iOctant = ...
}

// ------------------------------------------------------------------------------------------------
_octree::_octree(int64_t iModel, double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax)
	: _octree_node(nullptr, dXmin, dXmax, dYmin, dYmax, dZmin, dZmax)
	, m_iModel(iModel)
	, m_setPoints()
	, m_dOctantLength(DBL_MAX)
	, m_dSearchForNeighborsDistance(10.)
	, m_bIgnoreEvents(false)
{
	assert(m_iModel != 0);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ _octree::~_octree()
{
	m_bIgnoreEvents = true;
}

// ------------------------------------------------------------------------------------------------
int64_t _octree::getModel()
{
	return m_iModel;
}

// --------------------------------------------------------------------------------------------
/*virtual*/ _octree* _octree::getTree()
{
	return this;
}

// ------------------------------------------------------------------------------------------------
void _octree::onPointCreated(_octree_point* pPoint)
{
	if (m_bIgnoreEvents)
	{
		return;
	}

	if (pPoint == nullptr)
	{
		assert(false);

		return;
	}

	if (m_setPoints.find(pPoint) != m_setPoints.end())
	{
		assert(false);

		return;
	}

	m_setPoints.insert(pPoint);

	assert((pPoint->getXmax() - pPoint->getXmin()) - (pPoint->getYmax() - pPoint->getYmin()) <= EPSILON);
	assert((pPoint->getYmax() - pPoint->getYmin()) - (pPoint->getZmax() - pPoint->getZmin()) <= EPSILON);

	m_dOctantLength = min(m_dOctantLength, pPoint->getXmax() - pPoint->getXmin());
}

// ------------------------------------------------------------------------------------------------
void _octree::onPointDeleted(_octree_point* pPoint)
{
	if (m_bIgnoreEvents)
	{
		return;
	}

	if (pPoint == nullptr)
	{
		assert(false);

		return;
	}

	set<_octree_point*>::iterator itPoint = m_setPoints.find(pPoint);
	if (itPoint == m_setPoints.end())
	{
		assert(false);

		return;
	}

	m_setPoints.erase(itPoint);
}

// ------------------------------------------------------------------------------------------------
double _octree::getOctantLength()
{
	return m_dOctantLength;
}

// ------------------------------------------------------------------------------------------------
double _octree::getSearchForNeighborsDistance()
{
	return m_dSearchForNeighborsDistance;
}

// ------------------------------------------------------------------------------------------------
void _octree::dump()
{
	TRACE(L"\n*****************************************************");
	TRACE(L"\n*** Dimensions: %f, %f, %f, %f, %f, %f ***", m_dXmin, m_dXmax, m_dYmin, m_dYmax, m_dZmin, m_dZmax);
	TRACE(L"\n*** Points: %d ***", m_setPoints.size());
	TRACE(L"\n*** Octant length: %f ***", m_dOctantLength);
	TRACE(L"\n*****************************************************");
}

// ------------------------------------------------------------------------------------------------
void _octree::buildMesh()
{
	if (m_iModel == 0)
	{
		assert(false);

		return;
	}

	/*
	* Classes
	*/
	int64_t iCollectionClass = GetClassByName(m_iModel, "Collection");
	assert(iCollectionClass != 0);

	int64_t iPoint3DSetClass = GetClassByName(m_iModel, "Point3DSet");
	assert(iPoint3DSetClass != 0);

	int64_t iTriangleSetClass = GetClassByName(m_iModel, "TriangleSet");
	assert(iTriangleSetClass != 0);

	/*
	* Distance; 2% of the octants
	*/
	double dOctantsCount = ((m_dXmax - m_dXmin) / m_dOctantLength);
	m_dSearchForNeighborsDistance = (dOctantsCount * (dOctantsCount > 100. ? .02 : .2)) * m_dOctantLength;

	int64_t iPointsCount = 0;
	for (size_t iOctant = 0; iOctant < m_vecOctants.size(); iOctant++)
	{
		int64_t iCollectionInstance = CreateInstance(iCollectionClass, ("Octant " + to_string(iOctant)).c_str());
		int64_t iPoint3DSetInstance = CreateInstance(iPoint3DSetClass, "Points");

		if (m_vecOctants[iOctant] == nullptr)
		{
			// Empty octant
			continue;
		}

		set<_octree_point*> setPoints;
		switch (m_vecOctants[(int)iOctant]->getType())
		{
			case _octant_type::Node:
			{
				auto pNode = dynamic_cast<_octree_node*>(m_vecOctants[(int)iOctant]);
				assert(pNode != nullptr);

				preProcessing(pNode, setPoints);
			}
			break;

			case _octant_type::Point:
			{
				auto pPoint = dynamic_cast<_octree_point*>(m_vecOctants[(int)iOctant]);
				assert(pPoint != nullptr);

				setPoints.insert(pPoint);

				populateNeighbors(pPoint);
			}
			break;

			default:
			{
				assert(false); // Internal error!
			}
			break;
		} // switch (m_vecOctants[(int)iOctant]->getType())

		iPointsCount += setPoints.size();

		vector<double> vecPoints;
		set<_octree_point*>::iterator itPoint = setPoints.begin();
		for (; itPoint != setPoints.end(); itPoint++)
		{
			vecPoints.push_back((*itPoint)->getX());
			vecPoints.push_back((*itPoint)->getY());
			vecPoints.push_back((*itPoint)->getZ());
		}

		SetDatatypeProperty(iPoint3DSetInstance, GetPropertyByName(m_iModel, "coordinates"), vecPoints.data(), vecPoints.size());

		vector<int64_t> vecPoint3DSets { iPoint3DSetInstance };
		SetObjectProperty(iCollectionInstance, GetPropertyByName(m_iModel, "objects"), vecPoint3DSets.data(), vecPoint3DSets.size());
	} // for (size_t iOctant = ...	

	assert(iPointsCount == (int64_t)m_setPoints.size());

	/*
	* Build the triangles
	*/
	_vector3f vecBottomLeftCorner(m_dXmin, m_dYmin, m_dZmin);

	// Sorted by distance
	multimap<double, _octree_point*> mapSortedPoints;
	auto itPoint = m_setPoints.begin();
	for (; itPoint != m_setPoints.end(); itPoint++)
	{
		_octree_point* pPoint = *itPoint;

		mapSortedPoints.insert(pair<double, _octree_point*>(pPoint->distanceTo(vecBottomLeftCorner), pPoint));
	}

	vector<double> vecVertices;

	auto itSortedPoint = mapSortedPoints.begin();
	for (; itSortedPoint != mapSortedPoints.end(); itSortedPoint++)
	{
		_octree_point* pPoint = itSortedPoint->second;
		if (pPoint->neighbors().size() < 2)
		{
			assert(false);

			continue;
		}

		buildTriangles1Point(pPoint, vecVertices);
	}

	vector<int64_t> vecIndices;
	for (size_t iVertex = 0; iVertex < vecVertices.size() / 3; iVertex++)
	{
		vecIndices.push_back(iVertex);
	}

	int64_t iTriangleSetInstance = CreateInstance(iTriangleSetClass, "Triangles");
	SetDatatypeProperty(iTriangleSetInstance, GetPropertyByName(m_iModel, "vertices"), vecVertices.data(), vecVertices.size());
	SetDatatypeProperty(iTriangleSetInstance, GetPropertyByName(m_iModel, "indices"), vecIndices.data(), vecIndices.size());
}

// ------------------------------------------------------------------------------------------------
void _octree::preProcessing(_octree_node* pNode, set<_octree_point*>& setPoints)
{
	if (pNode == nullptr)
	{
		assert(false);

		return;
	}

	for (size_t iOctant = 0; iOctant < pNode->getOctants().size(); iOctant++)
	{
		if (pNode->getOctants()[iOctant] == nullptr)
		{
			// Empty octant
			continue;
		}

		switch (pNode->getOctants()[(int)iOctant]->getType())
		{
			case _octant_type::Node:
			{
				auto pChildNode = dynamic_cast<_octree_node*>(pNode->getOctants()[(int)iOctant]);
				assert(pChildNode != nullptr);

				preProcessing(pChildNode, setPoints);
			}
			break;

			case _octant_type::Point:
			{
				auto pPoint = dynamic_cast<_octree_point*>(pNode->getOctants()[(int)iOctant]);
				assert(pPoint != nullptr);

				setPoints.insert(pPoint);

				populateNeighbors(pPoint);
			}
			break;

			default:
			{
				assert(false); //Internal error!
			}
			break;
		} // switch (getOctants()[(int)iOctant]->getType())
	} // for (size_t iOctant = ...
}

// ------------------------------------------------------------------------------------------------
void _octree::buildTriangles1Point(_octree_point* pPoint1, vector<double>& vecVertices)
{
	if (pPoint1->neighbors().size() < 2)
	{
		assert(false);

		return;
	}

	// Sorted by distance
	multimap<double, _octree_point*> mapNeigbors;

	auto itNeighbor = pPoint1->neighbors().begin();
	for (; itNeighbor != pPoint1->neighbors().end(); itNeighbor++)
	{
		_octree_point* pPoint2 = *itNeighbor;
		assert(pPoint1 != pPoint2);

		mapNeigbors.insert(pair<double, _octree_point*>(pPoint1->distanceTo(pPoint2), pPoint2));
	}

	multimap<double, pair<_octree_point*, _octree_point*>> mapTriangles;

	auto itPoint2 = mapNeigbors.begin();
	for (; itPoint2 != mapNeigbors.end(); itPoint2++)
	{
		_octree_point* pPoint2 = itPoint2->second;

		auto itPoint3 = mapNeigbors.begin();
		for (; itPoint3 != mapNeigbors.end(); itPoint3++)
		{
			_octree_point* pPoint3 = itPoint3->second;

			if (pPoint2 == pPoint3)
			{
				continue;
			}

			assert((pPoint1 != pPoint2) && (pPoint1 != pPoint3) && (pPoint2 != pPoint3));

			if (triangleExists(pPoint1, pPoint2, pPoint3))
			{
				// The triangle is added already
				continue;
			}

			if (!validateTriangle(pPoint1, pPoint2, pPoint3))
			{
				// Invalid triangle
				continue;
			}

			_vector3f vecCenter;
			double R = calculateCircumsphere(pPoint1, pPoint2, pPoint3, vecCenter);

			mapTriangles.insert(pair<double, pair<_octree_point*, _octree_point*>>(R, pair<_octree_point*, _octree_point*>(pPoint2, pPoint3)));
		} // for (; itPoint != ...
	} // for (; itPoint2 != ...

	auto itTriangle = mapTriangles.begin();
	for (; itTriangle != mapTriangles.end(); itTriangle++)
	{
		_octree_point* pPoint2 = itTriangle->second.first;
		_octree_point* pPoint3 = itTriangle->second.second;

		assert((pPoint1 != pPoint2) && (pPoint1 != pPoint3) && (pPoint2 != pPoint3));

		if (!addTriangle(pPoint1, pPoint2, pPoint3, vecVertices))
		{
			continue;
		}

		buildTriangles2Points(pPoint1, pPoint2, true, vecVertices);
		buildTriangles2Points(pPoint1, pPoint3, true, vecVertices);
		buildTriangles2Points(pPoint2, pPoint3, true, vecVertices);

		return;
	} // for (; itTriangle != ...
}

// ------------------------------------------------------------------------------------------------
_octree_point* _octree::buildTriangles2Points(_octree_point* pPoint1, _octree_point* pPoint2, bool bBuildSideTriangles, vector<double>& vecVertices)
{
	// Sorted by area
	multimap<double, _octree_point*> mapNeigbors;

	/*
	* Point1
	*/
	{
		auto itNeighbor = pPoint1->neighbors().begin();
		for (; itNeighbor != pPoint1->neighbors().end(); itNeighbor++)
		{
			_octree_point* pPoint3 = *itNeighbor;

			if ((pPoint3 == pPoint1) || (pPoint3 == pPoint2))
			{
				continue;
			}

			if (triangleExists(pPoint1, pPoint2, pPoint3))
			{
				// The triangle is added already
				continue;
			}

			if (!validateTriangle(pPoint1, pPoint2, pPoint3))
			{
				// Invalid triangle
				continue;
			}

			_vector3f vecCenter;
			double R = calculateCircumsphere(pPoint1, pPoint2, pPoint3, vecCenter);

			mapNeigbors.insert(pair<double, _octree_point*>(R, pPoint3));

			//mapNeigbors.insert(pair<double, _octree_point*>(pPoint1->area(pPoint2, pPoint3), pPoint3));
		}
	}

	/*
	* Point2
	*/
	{
		auto itNeighbor = pPoint2->neighbors().begin();
		for (; itNeighbor != pPoint2->neighbors().end(); itNeighbor++)
		{
			_octree_point* pPoint3 = *itNeighbor;

			if ((pPoint3 == pPoint1) || (pPoint3 == pPoint2))
			{
				continue;
			}

			if (triangleExists(pPoint1, pPoint2, pPoint3))
			{
				// The triangle is added already
				continue;
			}

			if (!validateTriangle(pPoint1, pPoint2, pPoint3))
			{
				// Invalid triangle
				continue;
			}

			_vector3f vecCenter;
			double R = calculateCircumsphere(pPoint1, pPoint2, pPoint3, vecCenter);

			mapNeigbors.insert(pair<double, _octree_point*>(R, pPoint3));

			//mapNeigbors.insert(pair<double, _octree_point*>(pPoint1->area(pPoint2, pPoint3), pPoint3));
		}
	}

	auto itPoint = mapNeigbors.begin();
	for (; itPoint != mapNeigbors.end(); itPoint++)
	{
		_octree_point* pPoint3 = itPoint->second;

		assert((pPoint1 != pPoint2) && (pPoint1 != pPoint3) && (pPoint2 != pPoint3));

		if (!addTriangle(pPoint1, pPoint2, pPoint3, vecVertices))
		{
			continue;
		}

		if (bBuildSideTriangles)
		{
			/*auto pPoint4 = */buildTriangles2Points(pPoint1, pPoint2, false, vecVertices);
			/*auto pPoint5 = */buildTriangles2Points(pPoint1, pPoint3, false, vecVertices);
			buildTriangles2Points(pPoint2, pPoint3, false, vecVertices);

			//if ((pPoint4 != nullptr) && (pPoint5 != nullptr))
			//{
			//	// Try to add on the left/right side
			//	addTriangle(pPoint1, pPoint3, pPoint4, vecVertices);
			//	addTriangle(pPoint1, pPoint3, pPoint5, vecVertices);

			//	// Try to add on the left/right side
			//	addTriangle(pPoint2, pPoint3, pPoint4, vecVertices);
			//	addTriangle(pPoint2, pPoint3, pPoint5, vecVertices);
			//}
		}

		return pPoint3;
	} // for (; itPoint != ...

	return nullptr;
}

// ------------------------------------------------------------------------------------------------
bool _octree::triangleExists(_octree_point* pPoint1, _octree_point* pPoint2, _octree_point* pPoint3)
{
	if ((pPoint1 == nullptr) || (pPoint2 == nullptr) || (pPoint3 == nullptr))
	{
		assert(false);

		return false;
	}

	// Point 1
	{
		auto itTriangleLB = pPoint1->triangles().lower_bound(pPoint2);
		auto itTriangleUB = pPoint1->triangles().upper_bound(pPoint2);
		for (; itTriangleLB != itTriangleUB; itTriangleLB++)
		{
			if (itTriangleLB->second == pPoint3)
			{
				return true;
			}
		}

		itTriangleLB = pPoint1->triangles().lower_bound(pPoint3);
		itTriangleUB = pPoint1->triangles().upper_bound(pPoint3);
		for (; itTriangleLB != itTriangleUB; itTriangleLB++)
		{
			if (itTriangleLB->second == pPoint2)
			{
				return true;
			}
		}
	}

	// Point 2
	{
		auto itTriangleLB = pPoint2->triangles().lower_bound(pPoint1);
		auto itTriangleUB = pPoint2->triangles().upper_bound(pPoint1);
		for (; itTriangleLB != itTriangleUB; itTriangleLB++)
		{
			if (itTriangleLB->second == pPoint3)
			{
				return true;
			}
		}

		itTriangleLB = pPoint2->triangles().lower_bound(pPoint3);
		itTriangleUB = pPoint2->triangles().upper_bound(pPoint3);
		for (; itTriangleLB != itTriangleUB; itTriangleLB++)
		{
			if (itTriangleLB->second == pPoint1)
			{
				return true;
			}
		}
	}

	// Point 3
	{
		auto itTriangleLB = pPoint3->triangles().lower_bound(pPoint1);
		auto itTriangleUB = pPoint3->triangles().upper_bound(pPoint1);
		for (; itTriangleLB != itTriangleUB; itTriangleLB++)
		{
			if (itTriangleLB->second == pPoint2)
			{
				return true;
			}
		}

		itTriangleLB = pPoint3->triangles().lower_bound(pPoint2);
		itTriangleUB = pPoint3->triangles().upper_bound(pPoint2);
		for (; itTriangleLB != itTriangleUB; itTriangleLB++)
		{
			if (itTriangleLB->second == pPoint1)
			{
				return true;
			}
		}
	}

	return false;
}

// ------------------------------------------------------------------------------------------------
bool _octree::addTriangle(_octree_point* pPoint1, _octree_point* pPoint2, _octree_point* pPoint3, vector<double>& vecVertices)
{
	if ((pPoint1 == nullptr) || (pPoint2 == nullptr) || (pPoint3 == nullptr))
	{
		assert(false);

		return false;
	}

	if (triangleExists(pPoint1, pPoint2, pPoint3))
	{
		// The triangle is added already
		return false;
	}

	if (!validateTriangle(pPoint1, pPoint2, pPoint3))
	{
		// Invalid triangle
		return false;
	}

	pPoint1->triangles().insert(pair<_octree_point*, _octree_point*>(pPoint2, pPoint3));
	pPoint2->triangles().insert(pair<_octree_point*, _octree_point*>(pPoint1, pPoint3));
	pPoint3->triangles().insert(pair<_octree_point*, _octree_point*>(pPoint1, pPoint2));

	vecVertices.push_back(pPoint1->getX());
	vecVertices.push_back(pPoint1->getY());
	vecVertices.push_back(pPoint1->getZ());

	vecVertices.push_back(pPoint2->getX());
	vecVertices.push_back(pPoint2->getY());
	vecVertices.push_back(pPoint2->getZ());

	vecVertices.push_back(pPoint3->getX());
	vecVertices.push_back(pPoint3->getY());
	vecVertices.push_back(pPoint3->getZ());

	return true;
}

// ------------------------------------------------------------------------------------------------
bool _octree::validateTriangle(_octree_point* pPoint1, _octree_point* pPoint2, _octree_point* pPoint3) const
{
	if ((pPoint1 == nullptr) || (pPoint2 == nullptr) || (pPoint3 == nullptr))
	{
		assert(false);

		return false;
	}

	/**********************************************************************************************

					  +
					+ A +
				  +       +
				+     +      +
			c +   cpb + bps   + b
			+       ++ ++       +
		  +           +           +
		+             +             +
	  +               +               +
	+ B               + apb           C +
	++++++++++++++++++++++++++++++++++++++
					 a

	***********************************************************************************************/

	double a = pPoint1->distanceTo(pPoint2);
	double b = pPoint2->distanceTo(pPoint3);
	double c = pPoint3->distanceTo(pPoint1);

	/*
	* Lengths
	*/
	if ((a >= (b + c)) ||
		(b >= (a + c)) ||
		(c >= (a + b)))
	{
		// Invalid triangle
		return false;
	}

	/*
	* Angles
	*/
	double A = acos((pow(b, 2.) + pow(c, 2.) - pow(a, 2.)) / (2 * b * c)) * 180. / PI;
	double B = acos((pow(a, 2.) + pow(c, 2.) - pow(b, 2.)) / (2 * a * c)) * 180. / PI;
	double C = acos((pow(a, 2.) + pow(b, 2.) - pow(c, 2.)) / (2 * a * b)) * 180. / PI;

	assert(abs(180. - (A + B + C)) <= EPSILON);

	/*
	* ???
	*/

	/*const double MIN_ANGLE = 15.;
	const double MAX_ANGLE = 150.;*/

	const double MIN_ANGLE = 5.;
	const double MAX_ANGLE = 170.;

	if ((A < MIN_ANGLE || A > MAX_ANGLE) ||
		(B < MIN_ANGLE || B > MAX_ANGLE) ||
		(C < MIN_ANGLE || C > MAX_ANGLE))
	{
		return false;
	}

	_vector3f vecCenter;
	double R = calculateCircumsphere(pPoint1, pPoint2, pPoint3, vecCenter);

	if (neighborsInCircumsphere(pPoint1, pPoint2, pPoint3, vecCenter, R))
	{
		return false;
	}

	/*
	* Circumsphere - R
	*/
	//double R = (a / sin(A)) / 2.;
	//assert((circumsphereRadius - R) <= EPSILON);

	/*
	*  Perpendicular bisector <-> Circumsphere center (acc)
	*/
	//double acc = sqrt(pow(R, 2.) - pow(a / 2., 2.));

	/*
	*  Perpendicular bisector <-> Circumsphere center (bcc)
	*/
	//double bcc = sqrt(pow(R, 2.) - pow(b / 2., 2.));	

	return true;
}

// ------------------------------------------------------------------------------------------------
double _octree::calculateCircumsphere(const _octree_point* pPoint1, const _octree_point* pPoint2, const _octree_point* pPoint3, _vector3f& vecCenter) const
{
	if ((pPoint1 == nullptr) || (pPoint2 == nullptr) || (pPoint3 == nullptr))
	{
		assert(false);

		return -DBL_MAX;
	}

	// https://gamedev.stackexchange.com/questions/60630/how-do-i-find-the-circumcenter-of-a-triangle-in-3d
	// a, b, c are the 3 pts of the tri
	_vector3f a(pPoint1->getX(), pPoint1->getY(), pPoint1->getZ());
	_vector3f b(pPoint2->getX(), pPoint2->getY(), pPoint2->getZ());
	_vector3f c(pPoint3->getX(), pPoint3->getY(), pPoint3->getZ());

	_vector3f ac = c - a;
	_vector3f ab = b - a;
	_vector3f abXac = ab.cross(ac);

	// this is the vector from a TO the circumsphere center
	_vector3f toCircumsphereCenter = (abXac.cross(ab) * ac.length2() + ac.cross(abXac) * ab.length2()) / (2.f * abXac.length2());
	double circumsphereRadius = toCircumsphereCenter.length();

	// The 3 space coords of the circumsphere center then:
	vecCenter = a + toCircumsphereCenter; // now this is the actual 3space location

	return circumsphereRadius;
}

// ------------------------------------------------------------------------------------------------
bool _octree::inCircumsphere(_octree_point* pPoint, const _vector3f& vecCenter, double R) const
{
	if (pPoint == nullptr)
	{
		assert(false);

		return false;
	}

	const double TOLERANCE = 0.01;

	if ((R - (R * TOLERANCE)) - pPoint->distanceTo(vecCenter) > 0.)
	{
		return true;
	}

	return false;
}

// ------------------------------------------------------------------------------------------------
bool _octree::neighborsInCircumsphere(_octree_point* pPoint1, _octree_point* pPoint2, _octree_point* pPoint3, const _vector3f& vecCenter, double R) const
{
	/*
	* Point1
	*/
	{
		auto setNeighbours = pPoint1->neighbors();

		auto itNeighbor = setNeighbours.begin();
		for (; itNeighbor != setNeighbours.end(); itNeighbor++)
		{
			if ((*itNeighbor == pPoint1) || (*itNeighbor == pPoint2) || (*itNeighbor == pPoint3))
			{
				continue;
			}

			if (inCircumsphere(*itNeighbor, vecCenter, R))
			{
				return true;
			}
		}
	}

	/*
	* Point2
	*/
	{
		auto setNeighbours = pPoint2->neighbors();

		auto itNeighbor = setNeighbours.begin();
		for (; itNeighbor != setNeighbours.end(); itNeighbor++)
		{
			if ((*itNeighbor == pPoint1) || (*itNeighbor == pPoint2) || (*itNeighbor == pPoint3))
			{
				continue;
			}

			if (inCircumsphere(*itNeighbor, vecCenter, R))
			{
				return true;
			}
		}
	}

	/*
	* Point3
	*/
	{
		auto setNeighbours = pPoint3->neighbors();

		auto itNeighbor = setNeighbours.begin();
		for (; itNeighbor != setNeighbours.end(); itNeighbor++)
		{
			if ((*itNeighbor == pPoint1) || (*itNeighbor == pPoint2) || (*itNeighbor == pPoint3))
			{
				continue;
			}

			if (inCircumsphere(*itNeighbor, vecCenter, R))
			{
				return true;
			}
		}
	}

	return false;
}