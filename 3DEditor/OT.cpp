#include "stdafx.h"
#include "OT.h"
#include "RDFController.h"
#include "Vector.h"

// ------------------------------------------------------------------------------------------------
#define PI 3.14159265
#define EPSILON 1E-6

// ------------------------------------------------------------------------------------------------
COctant::COctant(OctantType type, double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax)
	: m_type(type)
	, m_dXmin(dXmin)
	, m_dXmax(dXmax)
	, m_dYmin(dYmin)
	, m_dYmax(dYmax)
	, m_dZmin(dZmin)
	, m_dZmax(dZmax)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ COctant::~COctant()
{
}

// --------------------------------------------------------------------------------------------
double COctant::getXmin() const
{
	return m_dXmin;
}

// --------------------------------------------------------------------------------------------
double COctant::getXmax() const
{
	return m_dXmax;
}

// --------------------------------------------------------------------------------------------
double COctant::getYmin() const
{
	return m_dYmin;
}

// --------------------------------------------------------------------------------------------
double COctant::getYmax() const
{
	return m_dYmax;
}

// --------------------------------------------------------------------------------------------
double COctant::getZmin() const
{
	return m_dZmin;
}

// --------------------------------------------------------------------------------------------
double COctant::getZmax() const
{
	return m_dZmax;
}

// ------------------------------------------------------------------------------------------------
OctantType COctant::getType() const
{
	return m_type;
}

// ------------------------------------------------------------------------------------------------
COctreePoint::COctreePoint(double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax, double dX, double dY, double dZ)
	: COctant(OctantType::Point, dXmin, dXmax, dYmin, dYmax, dZmin, dZmax)
	, m_setNeighbors()
	, m_mapTrinagles()
	, m_dX(dX)
	, m_dY(dY)
	, m_dZ(dZ)
{
	COctree::onPointCreated(this);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ COctreePoint::~COctreePoint()
{
	COctree::onPointDeleted(this);
}

// ------------------------------------------------------------------------------------------------
double COctreePoint::getX() const
{
	return m_dX;
}

// ------------------------------------------------------------------------------------------------
double COctreePoint::getY() const
{
	return m_dY;
}

// ------------------------------------------------------------------------------------------------
double COctreePoint::getZ() const
{
	return m_dZ;
}

// ------------------------------------------------------------------------------------------------
double COctreePoint::distanceTo(const COctreePoint* pPoint) const
{
	return sqrt(
		pow(m_dX - pPoint->m_dX, 2.) +
		pow(m_dY - pPoint->m_dY, 2.) +
		pow(m_dZ - pPoint->m_dZ, 2.));
}

// ------------------------------------------------------------------------------------------------
double COctreePoint::distanceTo(const Vector3f& pPoint) const
{
	return sqrt(
		pow(m_dX - pPoint.getX(), 2.) +
		pow(m_dY - pPoint.getY(), 2.) +
		pow(m_dZ - pPoint.getZ(), 2.));
}

// ------------------------------------------------------------------------------------------------
set<COctreePoint*>& COctreePoint::neighbors()
{
	return m_setNeighbors;
}

// ------------------------------------------------------------------------------------------------
// https://en.wikipedia.org/wiki/Heron%27s_formula
double COctreePoint::area(const COctreePoint* pPoint2, const COctreePoint* pPoint3) const
{
	auto a = distanceTo(pPoint2);
	auto b = pPoint2->distanceTo(pPoint3);
	auto c = pPoint3->distanceTo(this);

	// Semi-perimeter
	auto s = (a + b + c) / 2.;

	return sqrt(s * (s - a) * (s - b) * (s - c));
}

// --------------------------------------------------------------------------------------------
multimap<COctreePoint*, COctreePoint*>& COctreePoint::triangles()
{
	return m_mapTrinagles;
}

// ------------------------------------------------------------------------------------------------
COctreeNode::COctreeNode(double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax)
	: COctant(OctantType::Node, dXmin, dXmax, dYmin, dYmax, dZmin, dZmax)
	, m_vecOctants(8, nullptr)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ COctreeNode::~COctreeNode()
{
	for (size_t iOctant = 0; iOctant < m_vecOctants.size(); iOctant++)
	{
		delete m_vecOctants[iOctant];
	}
}

// ------------------------------------------------------------------------------------------------
const vector<COctant*>& COctreeNode::getOctants() const
{
	return m_vecOctants;
}

// ------------------------------------------------------------------------------------------------
bool COctreeNode::insertPoint(double dX, double dY, double dZ)
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

	OctantPosition position = OctantPosition::Unknown;

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
				position = OctantPosition::FrontBottomLeft;
			}
			else
			{
				// Back
				position = OctantPosition::BackBottomLeft;
			}
		} // if ((dY >= m_dYmin) && (dY < Y_MID))
		else
		{
			// Top =>

			if ((dZ >= m_dZmin) && (dZ < Z_MID))
			{
				// Front
				position = OctantPosition::FrontTopLeft;
			}
			else
			{
				// Back
				position = OctantPosition::BackTopLeft;
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
				position = OctantPosition::FrontBottomRight;
			}
			else
			{
				// Back
				position = OctantPosition::BackBottomRight;
			}
		} // if ((dY >= m_dYmin) && (dY < Y_MID))
		else
		{
			// Top =>

			if ((dZ >= m_dZmin) && (dZ < Z_MID))
			{
				// Front
				position = OctantPosition::FrontTopRight;
			}
			else
			{
				// Back
				position = OctantPosition::BackTopRight;
			}
		} // else if ((dY >= m_dYmin) && (dY < Y_MID))
	} // else if ((dX >= m_dXmin) && (dX < X_MID))

	if (position == OctantPosition::Unknown)
	{
		ASSERT(FALSE); // Internal error!

		return false;
	}

	/*
	* Add a node
	*/
	if (m_vecOctants[(int)position] != nullptr)
	{
		if (m_vecOctants[(int)position]->getType() == OctantType::Point)
		{
			/*
			* Add a node
			*/
			COctreeNode* pNewNode = nullptr;
			switch (position)
			{
				case OctantPosition::FrontBottomLeft:
				{
					pNewNode = new COctreeNode(m_dXmin, X_MID, m_dYmin, Y_MID, m_dZmin, Z_MID);
				}
				break;

				case OctantPosition::FrontBottomRight:
				{
					pNewNode = new COctreeNode(X_MID, m_dXmax, m_dYmin, Y_MID, m_dZmin, Z_MID);
				}
				break;

				case OctantPosition::FrontTopLeft:
				{
					pNewNode = new COctreeNode(m_dXmin, X_MID, Y_MID, m_dYmax, m_dZmin, Z_MID);
				}
				break;

				case OctantPosition::FrontTopRight:
				{
					pNewNode = new COctreeNode(X_MID, m_dXmax, Y_MID, m_dYmax, m_dZmin, Z_MID);
				}
				break;

				case OctantPosition::BackBottomLeft:
				{
					pNewNode = new COctreeNode(m_dXmin, X_MID, m_dYmin, Y_MID, Z_MID, m_dZmax);
				}
				break;

				case OctantPosition::BackBottomRight:
				{
					pNewNode = new COctreeNode(X_MID, m_dXmax, m_dYmin, Y_MID, Z_MID, m_dZmax);
				}
				break;

				case OctantPosition::BackTopLeft:
				{
					pNewNode = new COctreeNode(m_dXmin, X_MID, Y_MID, m_dYmax, Z_MID, m_dZmax);
				}
				break;

				case OctantPosition::BackTopRight:
				{
					pNewNode = new COctreeNode(X_MID, m_dXmax, Y_MID, m_dYmax, Z_MID, m_dZmax);
				}
				break;

				default:
				{
					ASSERT(FALSE); // Internal error!
				}
				break;
			} // switch (position)

			if (pNewNode != nullptr)
			{
				auto pPoint = dynamic_cast<COctreePoint*>(m_vecOctants[(int)position]);
				ASSERT(pPoint != nullptr);

				pNewNode->insertPoint(pPoint->getX(), pPoint->getY(), pPoint->getZ());
				delete pPoint;

				pNewNode->insertPoint(dX, dY, dZ);

				m_vecOctants[(int)position] = pNewNode;
			}			
		} // if (m_vecOctants[(int)position]->getType() == OctantType::Point)
		else if (m_vecOctants[(int)position]->getType() == OctantType::Node)
		{
			auto pNode = dynamic_cast<COctreeNode*>(m_vecOctants[(int)position]);
			ASSERT(pNode != nullptr);

			pNode->insertPoint(dX, dY, dZ);
		}
		else
		{
			ASSERT(FALSE); // Internal error!
		}
	} // if (m_vecOctants[(int)position] != nullptr)
	else
	{
		/*
		* Add a point
		*/		
		COctreePoint* pNewPoint = nullptr;
		switch (position)
		{
			case OctantPosition::FrontBottomLeft:
			{
				pNewPoint = new COctreePoint(m_dXmin, X_MID, m_dYmin, Y_MID, m_dZmin, Z_MID, dX, dY, dZ);
			}
			break;

			case OctantPosition::FrontBottomRight:
			{
				pNewPoint = new COctreePoint(X_MID, m_dXmax, m_dYmin, Y_MID, m_dZmin, Z_MID, dX, dY, dZ);
			}
			break;

			case OctantPosition::FrontTopLeft:
			{
				pNewPoint = new COctreePoint(m_dXmin, X_MID, Y_MID, m_dYmax, m_dZmin, Z_MID, dX, dY, dZ);
			}
			break;

			case OctantPosition::FrontTopRight:
			{
				pNewPoint = new COctreePoint(X_MID, m_dXmax, Y_MID, m_dYmax, m_dZmin, Z_MID, dX, dY, dZ);
			}
			break;

			case OctantPosition::BackBottomLeft:
			{
				pNewPoint = new COctreePoint(m_dXmin, X_MID, m_dYmin, Y_MID, Z_MID, m_dZmax, dX, dY, dZ);
			}
			break;

			case OctantPosition::BackBottomRight:
			{
				pNewPoint = new COctreePoint(X_MID, m_dXmax, m_dYmin, Y_MID, Z_MID, m_dZmax, dX, dY, dZ);
			}
			break;

			case OctantPosition::BackTopLeft:
			{
				pNewPoint = new COctreePoint(m_dXmin, X_MID, Y_MID, m_dYmax, Z_MID, m_dZmax, dX, dY, dZ);
			}
			break;

			case OctantPosition::BackTopRight:
			{
				pNewPoint = new COctreePoint(X_MID, m_dXmax, Y_MID, m_dYmax, Z_MID, m_dZmax, dX, dY, dZ);
			}
			break;

			default:
			{
				ASSERT(FALSE); // Internal error!
			}
			break;
		} // switch (position)

		m_vecOctants[(int)position] = pNewPoint;
	} // else if (m_vecOctants[(int)position] != nullptr)

	return true;
}

// ------------------------------------------------------------------------------------------------
void COctreeNode::findNeighbors(COctreePoint* pPoint)
{
	if (pPoint == nullptr)
	{
		ASSERT(FALSE);

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
			(abs(pPoint->getX() - getXmin()) > COctree::getSearchForNeighborsDistance()))
		{
			return;
		}		

		// Prune - X
		if ((pPoint->getX() > getXmax()) &&
			(abs(pPoint->getX() - getXmax()) > COctree::getSearchForNeighborsDistance()))
		{
			return;
		}

		// Prune - Y
		if ((pPoint->getY() < getYmin()) &&
			(abs(pPoint->getY() - getYmin()) > COctree::getSearchForNeighborsDistance()))
		{
			return;
		}

		// Prune - Y
		if ((pPoint->getY() > getYmax()) &&
			(abs(pPoint->getY() - getYmax()) > COctree::getSearchForNeighborsDistance()))
		{
			return;
		}

		// Prune - Z
		if ((pPoint->getZ() < getZmin()) &&
			(abs(pPoint->getZ() - getZmin()) > COctree::getSearchForNeighborsDistance()))
		{
			return;
		}

		// Prune - Z
		if ((pPoint->getZ() > getZmax()) &&
			(abs(pPoint->getZ() - getZmax()) > COctree::getSearchForNeighborsDistance()))
		{
			return;
		}

		switch (m_vecOctants[(int)iOctant]->getType())
		{
			case OctantType::Node:
			{
				auto pNode = dynamic_cast<COctreeNode*>(m_vecOctants[(int)iOctant]);
				ASSERT(pNode != nullptr);

				pNode->findNeighbors(pPoint);
			}
			break;

			case OctantType::Point:
			{
				auto pPoint2 = dynamic_cast<COctreePoint*>(m_vecOctants[(int)iOctant]);
				ASSERT(pPoint2 != nullptr);

				ASSERT(pPoint != pPoint2);

				double dDistance = pPoint->distanceTo(pPoint2);				
				if (dDistance <= COctree::getSearchForNeighborsDistance())
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
				ASSERT(FALSE); //Internal error!
			}
			break;
		} // switch (m_vecOctants[(int)iOctant]->getType())
	} // for (size_t iOctant = ...
}

// ------------------------------------------------------------------------------------------------
/*static*/ CRDFController* COctree::s_pController = nullptr;

// ------------------------------------------------------------------------------------------------
/*static*/ int64_t COctree::s_iModel = 0;

// ------------------------------------------------------------------------------------------------
/*static*/ set<COctreePoint*> COctree::s_setPoints = set<COctreePoint*>();

// ------------------------------------------------------------------------------------------------
/*static*/ double COctree::s_dOctantLength = DBL_MAX;

// ------------------------------------------------------------------------------------------------
/*static*/ double COctree::s_dSearchForNeighborsDistance = 10.;

// ------------------------------------------------------------------------------------------------
/*static*/ bool COctree::s_bStepByStepMode = false;

// ------------------------------------------------------------------------------------------------
COctree::COctree(int64_t iModel, double dXmin, double dXmax, double dYmin, double dYmax, double dZmin, double dZmax)
	: COctreeNode(dXmin, dXmax, dYmin, dYmax, dZmin, dZmax)
{
	s_iModel = iModel;

	s_setPoints.clear();

	s_dOctantLength = DBL_MAX;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ COctree::~COctree()
{
}

// ------------------------------------------------------------------------------------------------
/*static*/ int64_t COctree::getModel()
{
	return s_iModel;
}

// ------------------------------------------------------------------------------------------------
/*static*/ void COctree::onPointCreated(COctreePoint* pPoint)
{
	if (pPoint == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	if (s_setPoints.find(pPoint) != s_setPoints.end())
	{
		ASSERT(FALSE);

		return;
	}

	s_setPoints.insert(pPoint);	

	ASSERT((pPoint->getXmax() - pPoint->getXmin()) - (pPoint->getYmax() - pPoint->getYmin()) <= EPSILON);
	ASSERT((pPoint->getYmax() - pPoint->getYmin()) - (pPoint->getZmax() - pPoint->getZmin()) <= EPSILON);

	s_dOctantLength = min(s_dOctantLength, pPoint->getXmax() - pPoint->getXmin());
}

// ------------------------------------------------------------------------------------------------
/*static*/ void COctree::onPointDeleted(COctreePoint* pPoint)
{
	if (pPoint == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	set<COctreePoint*>::iterator itPoint = s_setPoints.find(pPoint);
	if (itPoint == s_setPoints.end())
	{
		ASSERT(FALSE);

		return;
	}

	s_setPoints.erase(itPoint);
}

// ------------------------------------------------------------------------------------------------
/*static*/ double COctree::getOctantLength()
{
	return s_dOctantLength;
}

// ------------------------------------------------------------------------------------------------
/*static*/ double COctree::getSearchForNeighborsDistance()
{
	return s_dSearchForNeighborsDistance;
}

// ------------------------------------------------------------------------------------------------
void COctree::dump()
{
	TRACE(L"\n*****************************************************");	
	TRACE(L"\n*** Dimensions: %f, %f, %f, %f, %f, %f ***", m_dXmin, m_dXmax, m_dYmin, m_dYmax, m_dZmin, m_dZmax);
	TRACE(L"\n*** Points: %d ***", s_setPoints.size());
	TRACE(L"\n*** Octant length: %f ***", s_dOctantLength);
	TRACE(L"\n*****************************************************");
}

// ------------------------------------------------------------------------------------------------
void COctree::buildMesh()
{
	if (s_iModel == 0)
	{
		ASSERT(FALSE);

		return;
	}

	unique_ptr<Model> pModel(new Model(s_iModel));

#ifdef _DEBUG_OCTREE 
	s_bStepByStepMode = true;
#endif

	/*
	* ???
	*/

	/*
	* Distance; 2% of the octants
	*/
	double dOctantsCount = ((m_dXmax - m_dXmin) / s_dOctantLength);
	s_dSearchForNeighborsDistance = (dOctantsCount * (dOctantsCount > 100. ? .02 : .2)) * s_dOctantLength;

	int64_t iPointsCount = 0;
	for (size_t iOctant = 0; iOctant < m_vecOctants.size(); iOctant++)
	{
		auto pOctantCollection = pModel->create<Collection>();
		pOctantCollection->setNameW((L"Octant " + to_wstring(iOctant)).c_str());

		auto pPoint3DSet = pModel->create<Point3DSet>();
		pPoint3DSet->setNameW(L"Points");		

		if (m_vecOctants[iOctant] == nullptr)
		{
			// Empty octant
			continue;
		}
		
		set<COctreePoint*> setPoints;
		switch (m_vecOctants[(int)iOctant]->getType())
		{
			case OctantType::Node:
			{
				auto pNode = dynamic_cast<COctreeNode*>(m_vecOctants[(int)iOctant]);
				ASSERT(pNode != nullptr);

				buildMesh(pNode, setPoints);
			}
			break;

			case OctantType::Point:
			{
				auto pPoint = dynamic_cast<COctreePoint*>(m_vecOctants[(int)iOctant]);
				ASSERT(pPoint != nullptr);

				setPoints.insert(pPoint);

				findNeighbors(pPoint);
			}
			break;

			default:
			{
				ASSERT(FALSE); //Internal error!
			}
			break;
		} // switch (m_vecOctants[(int)iOctant]->getType())

		iPointsCount += setPoints.size();

		vector<double> vecPoints;
		set<COctreePoint*>::iterator itPoint = setPoints.begin();
		for (; itPoint != setPoints.end(); itPoint++)
		{
			vecPoints.push_back((*itPoint)->getX());
			vecPoints.push_back((*itPoint)->getY());
			vecPoints.push_back((*itPoint)->getZ());
		}		

		pPoint3DSet->coordinates = vecPoints;		

		pOctantCollection->objects = vector<GeometricItem*> { pPoint3DSet };

#ifdef _DEBUG_OCTREE
		COctree::s_pController->OnOctreeInstanceCreated(nullptr, pOctantCollection);
#endif // _DEBUG_OCTREE
	} // for (size_t iOctant = ...	

	ASSERT(iPointsCount == (int64_t)s_setPoints.size());

	auto pTriangleSet = pModel->create<TriangleSet>();
	pTriangleSet->setNameW(L"Triangles");

	vector<double> vecVertices;

	/*
	* Build the triangles
	*/
	
	// Sorted by distance
	multimap<double, COctreePoint*> mapSortedPoints;

	Vector3f vecBottomLeftCorener(m_dXmin, m_dYmin, m_dZmin);

	auto itPoint = s_setPoints.begin();
	for (; itPoint != s_setPoints.end(); itPoint++)
	{
		COctreePoint* pPoint = *itPoint;

		mapSortedPoints.insert(pair<double, COctreePoint*>(pPoint->distanceTo(vecBottomLeftCorener), pPoint));
	}

	auto itSortedPoint = mapSortedPoints.begin();
	for (; itSortedPoint != mapSortedPoints.end(); itSortedPoint++)
	{
		COctreePoint* pPoint = itSortedPoint->second;
		if (pPoint->neighbors().size() < 2)
		{
			ASSERT(FALSE);

			continue;
		}

		buildTriangles1Point(pPoint, vecVertices);
	}

	vector<int64_t> vecIndices;
	for (size_t iVertex = 0; iVertex < vecVertices.size() / 3; iVertex++)
	{
		vecIndices.push_back(iVertex);
	}

	pTriangleSet->vertices = vecVertices;
	pTriangleSet->indices = vecIndices;
}

// ------------------------------------------------------------------------------------------------
void COctree::buildMesh(COctreeNode* pNode, set<COctreePoint*>& setPoints)
{
	if (pNode == nullptr)
	{
		ASSERT(FALSE);

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
			case OctantType::Node:
			{
				auto pChildNode = dynamic_cast<COctreeNode*>(pNode->getOctants()[(int)iOctant]);
				ASSERT(pChildNode != nullptr);

				buildMesh(pChildNode, setPoints);
			}
			break;

			case OctantType::Point:
			{
				auto pPoint = dynamic_cast<COctreePoint*>(pNode->getOctants()[(int)iOctant]);
				ASSERT(pPoint != nullptr);

				setPoints.insert(pPoint);			

				findNeighbors(pPoint);
			}
			break;

			default:
			{
				ASSERT(FALSE); //Internal error!
			}
			break;
		} // switch (getOctants()[(int)iOctant]->getType())
	} // for (size_t iOctant = ...
}

// ------------------------------------------------------------------------------------------------
void COctree::buildTriangles1Point(COctreePoint* pPoint1, vector<double>& vecVertices)
{
	if (pPoint1->neighbors().size() < 2)
	{
		ASSERT(FALSE);

		return;
	}

	// Sorted by distance
	multimap<double, COctreePoint*> mapNeigbors;

	auto itNeighbor = pPoint1->neighbors().begin();
	for (; itNeighbor != pPoint1->neighbors().end(); itNeighbor++)
	{
		COctreePoint* pPoint2 = *itNeighbor;
		ASSERT(pPoint1 != pPoint2);

		mapNeigbors.insert(pair<double, COctreePoint*>(pPoint1->distanceTo(pPoint2), pPoint2));
	}
	
	multimap<double, pair<COctreePoint*, COctreePoint*>> mapTriangles;

	auto itPoint2 = mapNeigbors.begin();
	for (; itPoint2 != mapNeigbors.end(); itPoint2++)
	{
		COctreePoint* pPoint2 = itPoint2->second;

		auto itPoint3 = mapNeigbors.begin();
		for (; itPoint3 != mapNeigbors.end(); itPoint3++)
		{
			COctreePoint* pPoint3 = itPoint3->second;

			if (pPoint2 == pPoint3)
			{
				continue;
			}

			ASSERT((pPoint1 != pPoint2) && (pPoint1 != pPoint3) && (pPoint2 != pPoint3));

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

			Vector3f vecCenter;
			double R = calculateCircumsphere(pPoint1, pPoint2, pPoint3, vecCenter);

			mapTriangles.insert(pair<double, pair<COctreePoint*, COctreePoint*>>(R, pair<COctreePoint*, COctreePoint*>(pPoint2, pPoint3)));
		} // for (; itPoint != ...
	} // for (; itPoint2 != ...

	auto itTriangle = mapTriangles.begin();
	for (; itTriangle != mapTriangles.end(); itTriangle++)
	{
		COctreePoint* pPoint2 = itTriangle->second.first;
		COctreePoint* pPoint3 = itTriangle->second.second;

		ASSERT((pPoint1 != pPoint2) && (pPoint1 != pPoint3) && (pPoint2 != pPoint3));

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
COctreePoint* COctree::buildTriangles2Points(COctreePoint* pPoint1, COctreePoint* pPoint2, bool bBuildSideTriangles, vector<double>& vecVertices)
{
	// Sorted by area
	multimap<double, COctreePoint*> mapNeigbors;

	/*
	* Point1
	*/
	{
		auto itNeighbor = pPoint1->neighbors().begin();
		for (; itNeighbor != pPoint1->neighbors().end(); itNeighbor++)
		{
			COctreePoint* pPoint3 = *itNeighbor;

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

			Vector3f vecCenter;
			double R = calculateCircumsphere(pPoint1, pPoint2, pPoint3, vecCenter);

			mapNeigbors.insert(pair<double, COctreePoint*>(R, pPoint3));
			
			//mapNeigbors.insert(pair<double, COctreePoint*>(pPoint1->area(pPoint2, pPoint3), pPoint3));
		}
	}

	/*
	* Point2
	*/
	{
		auto itNeighbor = pPoint2->neighbors().begin();
		for (; itNeighbor != pPoint2->neighbors().end(); itNeighbor++)
		{
			COctreePoint* pPoint3 = *itNeighbor;

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

			Vector3f vecCenter;
			double R = calculateCircumsphere(pPoint1, pPoint2, pPoint3, vecCenter);

			mapNeigbors.insert(pair<double, COctreePoint*>(R, pPoint3));

			//mapNeigbors.insert(pair<double, COctreePoint*>(pPoint1->area(pPoint2, pPoint3), pPoint3));
		}
	}	

	auto itPoint = mapNeigbors.begin();
	for (; itPoint != mapNeigbors.end(); itPoint++)
	{
		COctreePoint* pPoint3 = itPoint->second;

		ASSERT((pPoint1 != pPoint2) && (pPoint1 != pPoint3) && (pPoint2 != pPoint3));				

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
bool COctree::triangleExists(COctreePoint* pPoint1, COctreePoint* pPoint2, COctreePoint* pPoint3)
{
	if ((pPoint1 == nullptr) || (pPoint2 == nullptr) || (pPoint3 == nullptr))
	{
		ASSERT(FALSE);

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
bool COctree::addTriangle(COctreePoint* pPoint1, COctreePoint* pPoint2, COctreePoint* pPoint3, vector<double>& vecVertices)
{
	if ((pPoint1 == nullptr) || (pPoint2 == nullptr) || (pPoint3 == nullptr))
	{
		ASSERT(FALSE);

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

	pPoint1->triangles().insert(pair<COctreePoint*, COctreePoint*>(pPoint2, pPoint3));
	pPoint2->triangles().insert(pair<COctreePoint*, COctreePoint*>(pPoint1, pPoint3));
	pPoint3->triangles().insert(pair<COctreePoint*, COctreePoint*>(pPoint1, pPoint2));

#ifdef _DEBUG_OCTREE
	auto pCollection = s_pModel->create<Collection>();

	auto pTriangleSet = s_pModel->create<TriangleSet>();
	pTriangleSet->vertices = vector<double>
	{
		pPoint1->getX(),
		pPoint1->getY(),
		pPoint1->getZ(),
		pPoint2->getX(),
		pPoint2->getY(),
		pPoint2->getZ(),
		pPoint3->getX(),
		pPoint3->getY(),
		pPoint3->getZ()
	};

	pTriangleSet->indices = vector<int64_t>
	{
		0, 1, 2
	};

	auto pAmbient = s_pModel->create<ColorComponent>();
	pAmbient->R = 0.;
	pAmbient->G = 0.;
	pAmbient->B = 1.;
	pAmbient->W = 0.1;

	auto pColor = s_pModel->create<Color>();
	pColor->ambient = pAmbient;

	auto pMaterial = s_pModel->create<Material>();
	pMaterial->color = pColor;

	pTriangleSet->material = pMaterial;

	Vector3f vecCenter;
	double R = calculateCircumsphere(pPoint1, pPoint2, pPoint3, vecCenter);

	auto pLine3D1 = s_pModel->create<Line3D>();
	pLine3D1->points = vector<double>{ vecCenter.getX(), vecCenter.getY(), vecCenter.getZ(), pPoint1->getX(), pPoint1->getY(), pPoint1->getZ() };

	auto pLine3D2 = s_pModel->create<Line3D>();
	pLine3D2->points = vector<double>{ vecCenter.getX(), vecCenter.getY(), vecCenter.getZ(), pPoint2->getX(), pPoint2->getY(), pPoint2->getZ() };

	auto pLine3D3 = s_pModel->create<Line3D>();
	pLine3D3->points = vector<double>{ vecCenter.getX(), vecCenter.getY(), vecCenter.getZ(), pPoint3->getX(), pPoint3->getY(), pPoint3->getZ() };

	auto pMatrix = s_pModel->create<Matrix>();
	pMatrix->_41 = vecCenter.getX();
	pMatrix->_42 = vecCenter.getY();
	pMatrix->_43 = vecCenter.getZ();

	/*auto pTransformation = s_pModel->create<Transformation>();
	pTransformation->matrix = pMatrix;

	auto pSphere = s_pModel->create<Sphere>();
	pSphere->radius = R;
	pSphere->segmentationParts = 36;
	pSphere->material = pMaterial;

	pTransformation->object = pSphere;*/

	pCollection->objects = vector<GeometricItem*>{ pTriangleSet, pLine3D1, pLine3D2, pLine3D3/*, pTransformation*/ };

	COctree::s_pController->OnOctreeInstanceCreated(nullptr, pCollection);
#else
	vecVertices.push_back(pPoint1->getX());
	vecVertices.push_back(pPoint1->getY());
	vecVertices.push_back(pPoint1->getZ());

	vecVertices.push_back(pPoint2->getX());
	vecVertices.push_back(pPoint2->getY());
	vecVertices.push_back(pPoint2->getZ());

	vecVertices.push_back(pPoint3->getX());
	vecVertices.push_back(pPoint3->getY());
	vecVertices.push_back(pPoint3->getZ());
#endif // _DEBUG_OCTREE			

	return true;
}

// ------------------------------------------------------------------------------------------------
bool COctree::validateTriangle(COctreePoint* pPoint1, COctreePoint* pPoint2, COctreePoint* pPoint3) const
{
	if ((pPoint1 == nullptr) || (pPoint2 == nullptr) || (pPoint3 == nullptr))
	{
		ASSERT(FALSE);

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
	
	ASSERT(abs(180. - (A + B + C)) <= EPSILON);

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

	Vector3f vecCenter;
	double R = calculateCircumsphere(pPoint1, pPoint2, pPoint3, vecCenter);

	if (neighborsInCircumsphere(pPoint1, pPoint2, pPoint3, vecCenter, R))
	{
		return false;
	}

	/*
	* Circumsphere - R 
	*/
	//double R = (a / sin(A)) / 2.;
	//ASSERT((circumsphereRadius - R) <= EPSILON);

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
double COctree::calculateCircumsphere(const COctreePoint* pPoint1, const COctreePoint* pPoint2, const COctreePoint* pPoint3, Vector3f& vecCenter) const
{
	if ((pPoint1 == nullptr) || (pPoint2 == nullptr) || (pPoint3 == nullptr))
	{
		ASSERT(FALSE);

		return -DBL_MAX;
	}

	// https://gamedev.stackexchange.com/questions/60630/how-do-i-find-the-circumcenter-of-a-triangle-in-3d
	// a, b, c are the 3 pts of the tri
	Vector3f a(pPoint1->getX(), pPoint1->getY(), pPoint1->getZ());
	Vector3f b(pPoint2->getX(), pPoint2->getY(), pPoint2->getZ());
	Vector3f c(pPoint3->getX(), pPoint3->getY(), pPoint3->getZ());

	Vector3f ac = c - a;
	Vector3f ab = b - a;
	Vector3f abXac = ab.cross(ac);

	// this is the vector from a TO the circumsphere center
	Vector3f toCircumsphereCenter = (abXac.cross(ab) * ac.length2() + ac.cross(abXac) * ab.length2()) / (2.f * abXac.length2());
	double circumsphereRadius = toCircumsphereCenter.length();	

	// The 3 space coords of the circumsphere center then:
	vecCenter = a + toCircumsphereCenter; // now this is the actual 3space location

	return circumsphereRadius;
}

// ------------------------------------------------------------------------------------------------
bool COctree::inCircumsphere(COctreePoint* pPoint, const Vector3f& vecCenter, double R) const
{
	if (pPoint == nullptr)
	{
		ASSERT(FALSE);

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
bool COctree::neighborsInCircumsphere(COctreePoint* pPoint1, COctreePoint* pPoint2, COctreePoint* pPoint3, const Vector3f& vecCenter, double R) const
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
