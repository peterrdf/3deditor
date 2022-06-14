#include "stdafx.h"
#include "Octree.h"
#include "Generic.h"

#ifdef _USE_BOOST
#include <boost/chrono.hpp>

using namespace boost::chrono;
#endif // _USE_BOOST

// ------------------------------------------------------------------------------------------------
COctree::COctree(uint32_t iBinsCount/* = 1000*/)
	: m_iBinsCount(iBinsCount + 1/*for the last bin*/)
	, m_vecXBins()
	, m_vecXBinsIndices()
	, m_vecYBins()
	, m_vecYBinsIndices()
	, m_vecZBins()
	, m_vecZBinsIndices()
	, m_vecChildren()
{
	ASSERT(m_iBinsCount > 0);

	m_vecXBins.resize(m_iBinsCount, 0);
	m_vecXBinsIndices.resize(m_iBinsCount, nullptr);
	m_vecYBins.resize(m_iBinsCount, 0);
	m_vecYBinsIndices.resize(m_iBinsCount, nullptr);
	m_vecZBins.resize(m_iBinsCount, 0);
	m_vecZBinsIndices.resize(m_iBinsCount, nullptr);

	m_vecChildren.resize(8, nullptr);
}

// ------------------------------------------------------------------------------------------------
COctree::~COctree()
{
	for (size_t iBin = 0; iBin < m_vecXBinsIndices.size(); iBin++)
	{
		delete m_vecXBinsIndices[iBin];
	}

	m_vecXBinsIndices.clear();

	for (size_t iBin = 0; iBin < m_vecYBinsIndices.size(); iBin++)
	{
		delete m_vecYBinsIndices[iBin];
	}

	m_vecYBinsIndices.clear();

	for (size_t iBin = 0; iBin < m_vecZBinsIndices.size(); iBin++)
	{
		delete m_vecZBinsIndices[iBin];
	}

	m_vecZBinsIndices.clear();

	for (size_t iChild = 0; iChild < m_vecChildren.size(); iChild++)
	{
		delete m_vecChildren[iChild];
	} // for (size_t iChild = ...

	m_vecChildren.clear();
}

// ------------------------------------------------------------------------------------------------
void COctree::Build(CRDFInstance * pInstance)
{
	ASSERT(pInstance != nullptr);

	if (pInstance->getVerticesCount() == 0)
	{
		return;
	}

	if (wstring(pInstance->getName()) != wstring(L"BoundaryRepresentation"))
	{
		return;
	}

#ifdef _USE_BOOST
	high_resolution_clock::time_point tpStart = high_resolution_clock::now();
#endif

	fill(m_vecXBins.begin(), m_vecXBins.end(), 0);
	fill(m_vecYBins.begin(), m_vecYBins.end(), 0);
	fill(m_vecZBins.begin(), m_vecZBins.end(), 0);

	for (size_t iBin = 0; iBin < m_vecXBinsIndices.size(); iBin++)
	{
		delete m_vecXBinsIndices[iBin];
	}

	fill(m_vecXBinsIndices.begin(), m_vecXBinsIndices.end(), nullptr);

	for (size_t iBin = 0; iBin < m_vecYBinsIndices.size(); iBin++)
	{
		delete m_vecYBinsIndices[iBin];
	}

	fill(m_vecYBinsIndices.begin(), m_vecYBinsIndices.end(), nullptr);

	for (size_t iBin = 0; iBin < m_vecZBinsIndices.size(); iBin++)
	{
		delete m_vecZBinsIndices[iBin];
	}

	fill(m_vecZBinsIndices.begin(), m_vecZBinsIndices.end(), nullptr);	

	/*
	* Calculate X/Y/Z min/max
	*/

	float fXmin = FLT_MAX;
	float fYmin = FLT_MAX;
	float fZmin = FLT_MAX;

	float fXmax = FLT_MIN;
	float fYmax = FLT_MIN;
	float fZmax = FLT_MIN;
	
	for (int64_t iVertex = 0; iVertex < pInstance->getVerticesCount(); iVertex++)
	{
		fXmin = (float)fmin(fXmin, pInstance->getVertices()[(iVertex * VERTEX_LENGTH) + 0]);
		fYmin = (float)fmin(fYmin, pInstance->getVertices()[(iVertex * VERTEX_LENGTH) + 1]);
		fZmin = (float)fmin(fZmin, pInstance->getVertices()[(iVertex * VERTEX_LENGTH) + 2]);

		fXmax = (float)fmax(fXmax, pInstance->getVertices()[(iVertex * VERTEX_LENGTH) + 0]);
		fYmax = (float)fmax(fYmax, pInstance->getVertices()[(iVertex * VERTEX_LENGTH) + 1]);
		fZmax = (float)fmax(fZmax, pInstance->getVertices()[(iVertex * VERTEX_LENGTH) + 2]);
	} // for (int64_t iVertex = ...

	float fXDistance = fXmax != fXmin ? (fXmax - fXmin) : 1.f;
	float fYDistance = fYmax != fYmin ? (fYmax - fYmin) : 1.f;
	float fZDistance = fZmax != fZmin ? (fZmax - fZmin) : 1.f;

	/*
	* Build
	*/

	const float EPSILON = 1.f / (m_iBinsCount - 1);

	//for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
	//{
	//	for (int64_t iIndex = vecTriangles[iTriangle].first; iIndex < vecTriangles[iTriangle].first + vecTriangles[iTriangle].second; iIndex++)
	//	{
	//		// X/Y/Z
	//		float fX = pInstance->getVertices()[(pInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0];
	//		float fY = pInstance->getVertices()[(pInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1];
	//		float fZ = pInstance->getVertices()[(pInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2];

	//		// X => [0, 1]
	//		fX = (fX - fXmin) / fXDistance;
	//		ASSERT((fX >= 0.f) && (fX <= 1.f));

	//		// X => [0, 1]
	//		fY = (fY - fYmin) / fYDistance;
	//		ASSERT((fY >= 0.f) && (fY <= 1.f));

	//		// X => [0, 1]
	//		fZ = (fZ - fZmin) / fZDistance;
	//		ASSERT((fZ >= 0.f) && (fZ <= 1.f));

	//		// X bin
	//		uint32_t iXBin = (uint32_t)(fX / EPSILON);
	//		ASSERT((iXBin >= 0) && (iXBin <= m_iBinsCount));

	//		m_vecXBins[iXBin]++;

	//		// Y bin
	//		uint32_t iYBin = (uint32_t)(fY / EPSILON);
	//		ASSERT((iYBin >= 0) && (iYBin <= m_iBinsCount));

	//		m_vecYBins[iYBin]++;

	//		// Z bin
	//		uint32_t iZBin = (uint32_t)(fZ / EPSILON);
	//		ASSERT((iZBin >= 0) && (iZBin <= m_iBinsCount));

	//		m_vecZBins[iZBin]++;
	//	} // for (size_t iIndex = ...
	//} // for (size_t iTriangle = ...

	// ********************************************************************************************

	/*
	* Create X bins
	*/

	for (int64_t iVertex = 0; iVertex < pInstance->getVerticesCount(); iVertex++)
	{
		// X
		float fX = pInstance->getVertices()[(iVertex * VERTEX_LENGTH) + 0];

		// X => [0, 1]
		fX = (fX - fXmin) / fXDistance;
		ASSERT((fX >= 0.f) && (fX <= 1.f));

		// X bin
		uint32_t iXBin = (uint32_t)(fX / EPSILON);
		ASSERT((iXBin >= 0) && (iXBin <= m_iBinsCount));

		if (m_vecXBinsIndices[iXBin] == nullptr)
		{
			m_vecXBinsIndices[iXBin] = new vector<int64_t>();
		}

		m_vecXBinsIndices[iXBin]->push_back(iVertex);
	} // for (int64_t iVertex = ...

	/*
	* Check the non-empty X bins with more than one element and create Y bins
	*/

	for (size_t iBin = 0; iBin < m_vecXBinsIndices.size(); iBin++)
	{
		if ((m_vecXBinsIndices[iBin] != nullptr) && (m_vecXBinsIndices[iBin]->size() > 1))
		{
			for (size_t iVertex = 0; iVertex < m_vecXBinsIndices[iBin]->size(); iVertex++)
			{
				// Y					
				float fY = pInstance->getVertices()[(m_vecXBinsIndices[iBin]->at(iVertex) * VERTEX_LENGTH) + 1];

				// Y => [0, 1]
				fY = (fY - fYmin) / fYDistance;
				ASSERT((fY >= 0.f) && (fY <= 1.f));

				// Y bin
				uint32_t iYBin = (uint32_t)(fY / EPSILON);
				ASSERT((iYBin >= 0) && (iYBin <= m_iBinsCount));


				if (m_vecYBinsIndices[iYBin] == nullptr)
				{
					m_vecYBinsIndices[iYBin] = new vector<int64_t>();
				}

				m_vecYBinsIndices[iYBin]->push_back(m_vecXBinsIndices[iBin]->at(iVertex));
			} // for (size_t iVertex = ...
		} // if ((m_vecXBinsIndices[iBin] != nullptr) && ...
	} // for (size_t iBin = ...

	/*
	* Check the non-empty Y bins with more than one element and create Z bins
	*/

	for (size_t iBin = 0; iBin < m_vecYBinsIndices.size(); iBin++)
	{
		if ((m_vecYBinsIndices[iBin] != nullptr) && (m_vecYBinsIndices[iBin]->size() > 1))
		{
			for (size_t iVertex = 0; iVertex < m_vecYBinsIndices[iBin]->size(); iVertex++)
			{
				// Z					
				float fZ = pInstance->getVertices()[(m_vecYBinsIndices[iBin]->at(iVertex) * VERTEX_LENGTH) + 2];

				// Z => [0, 1]
				fZ = (fZ - fZmin) / fZDistance;
				ASSERT((fZ >= 0.f) && (fZ <= 1.f));

				// Z bin
				uint32_t iZBin = (uint32_t)(fZ / EPSILON);
				ASSERT((iZBin >= 0) && (iZBin <= m_iBinsCount));


				if (m_vecZBinsIndices[iZBin] == nullptr)
				{
					m_vecZBinsIndices[iZBin] = new vector<int64_t>();
				}

				m_vecZBinsIndices[iZBin]->push_back(m_vecYBinsIndices[iBin]->at(iVertex));
			} // for (size_t iVertex = ...
		} // if ((m_vecYBinsIndices[iBin] != nullptr) && ...
	} // for (size_t iBin = ...

	/*
	* Check the non-empty Z bins with more than one element
	*/

	LOG_INFO("*** Instance consistency: " << pInstance->getName() << ", Vertices count: " << pInstance->getVerticesCount());	

	//for (size_t iBin = 0; iBin < m_vecZBinsIndices.size(); iBin++)
	//{
	//	if ((m_vecZBinsIndices[iBin] != nullptr) && (m_vecZBinsIndices[iBin]->size() > 1))
	//	{
	//		LOG_INFO("*** Bin: " << (int64_t)iBin);

	//		for (size_t iVertex = 0; iVertex < m_vecZBinsIndices[iBin]->size(); iVertex++)
	//		{
	//			float fX = pInstance->getVertices()[(m_vecZBinsIndices[iBin]->at(iVertex) * VERTEX_LENGTH) + 0];
	//			float fY = pInstance->getVertices()[(m_vecZBinsIndices[iBin]->at(iVertex) * VERTEX_LENGTH) + 1];
	//			float fZ = pInstance->getVertices()[(m_vecZBinsIndices[iBin]->at(iVertex) * VERTEX_LENGTH) + 2];

	//			LOG_INFO("Vertex: " << m_vecZBinsIndices[iBin]->at(iVertex) << "(" << fX << ", " << fY << ", " << fZ << ")");
	//		} // for (size_t iVertex = ...			
	//	} // if ((m_vecZBinsIndices[iBin] != nullptr) && ...
	//} // for (size_t iBin = ...

#ifdef _USE_BOOST
	high_resolution_clock::time_point tpEnd = high_resolution_clock::now();
	duration<float> duration = tpEnd - tpStart;
	LOG_INFO("Consistency check is done in " << duration << ".");
#endif
}
