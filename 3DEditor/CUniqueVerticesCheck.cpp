#include "stdafx.h"
#include "CUniqueVerticesCheck.h"
#include "Generic.h"
#include <math.h>

#ifdef _USE_BOOST
#include <boost/chrono.hpp>

using namespace boost::chrono;
#endif // _USE_BOOST

// ------------------------------------------------------------------------------------------------
CUniqueVerticesCheck::CUniqueVerticesCheck(uint32_t iBinsCount/* = 10000*/)
	: m_iBinsCount(iBinsCount + 1/*for the last bin*/)
	, m_vecXBinsIndices()
	, m_vecYBinsIndices()
	, m_vecZBinsIndices()
{
	ASSERT(m_iBinsCount > 0);
	
	m_vecXBinsIndices.resize(m_iBinsCount, nullptr);
	m_vecYBinsIndices.resize(m_iBinsCount, nullptr);
	m_vecZBinsIndices.resize(m_iBinsCount, nullptr);
}

// ------------------------------------------------------------------------------------------------
CUniqueVerticesCheck::~CUniqueVerticesCheck()
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
}

// ------------------------------------------------------------------------------------------------
int32_t CUniqueVerticesCheck::Check(CRDFInstance * pInstance, wofstream & output)
{
	ASSERT(pInstance != nullptr);

	if (pInstance->getVerticesCount() == 0)
	{
		return 0;
	}

	/*if (pInstance->hasConceptualFaceWithTexture())
	{
		output << "****************************************************************************************************\n";
		output << "Instance: " << pInstance->GetName() << "\n";
		output << "Skipped - found conceptual face with a texture.\n";
		output << "****************************************************************************************************\n";

		return 0;
	}*/

	/*
	* Start
	*/

#ifdef _USE_BOOST
	high_resolution_clock::time_point tpStart = high_resolution_clock::now();	
#endif

	output << "****************************************************************************************************\n";
	output << "Instance: " << pInstance->getName() << "\n";

	/*
	* Clean
	*/

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
		fXmin = fmin(fXmin, pInstance->getVertices()[(iVertex * VERTEX_LENGTH) + 0]);
		fYmin = fmin(fYmin, pInstance->getVertices()[(iVertex * VERTEX_LENGTH) + 1]);
		fZmin = fmin(fZmin, pInstance->getVertices()[(iVertex * VERTEX_LENGTH) + 2]);

		fXmax = fmax(fXmax, pInstance->getVertices()[(iVertex * VERTEX_LENGTH) + 0]);
		fYmax = fmax(fYmax, pInstance->getVertices()[(iVertex * VERTEX_LENGTH) + 1]);
		fZmax = fmax(fZmax, pInstance->getVertices()[(iVertex * VERTEX_LENGTH) + 2]);
	} // for (int64_t iVertex = ...

	float fXDistance = fXmax != fXmin ? (fXmax - fXmin) : 1.f;
	float fYDistance = fYmax != fYmin ? (fYmax - fYmin) : 1.f;
	float fZDistance = fZmax != fZmin ? (fZmax - fZmin) : 1.f;

	const float BIN_SIZE = 1.f / (m_iBinsCount - 1);

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
		uint32_t iXBin = (uint32_t)(fX / BIN_SIZE);
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

	int32_t iDuplicatesCount = false;

	for (size_t iXBin = 0; iXBin < m_vecXBinsIndices.size(); iXBin++)
	{
		if ((m_vecXBinsIndices[iXBin] != nullptr) && (m_vecXBinsIndices[iXBin]->size() > 1))
		{
			for (size_t iBin = 0; iBin < m_vecYBinsIndices.size(); iBin++)
			{
				delete m_vecYBinsIndices[iBin];
			}

			fill(m_vecYBinsIndices.begin(), m_vecYBinsIndices.end(), nullptr);

			for (size_t iVertex = 0; iVertex < m_vecXBinsIndices[iXBin]->size(); iVertex++)
			{
				// Y					
				float fY = pInstance->getVertices()[(m_vecXBinsIndices[iXBin]->at(iVertex) * VERTEX_LENGTH) + 1];

				// Y => [0, 1]
				fY = (fY - fYmin) / fYDistance;
				ASSERT((fY >= 0.f) && (fY <= 1.f));

				// Y bin
				uint32_t iYBin = (uint32_t)(fY / BIN_SIZE);
				ASSERT((iYBin >= 0) && (iYBin <= m_iBinsCount));

				if (m_vecYBinsIndices[iYBin] == nullptr)
				{
					m_vecYBinsIndices[iYBin] = new vector<int64_t>();
				}

				m_vecYBinsIndices[iYBin]->push_back(m_vecXBinsIndices[iXBin]->at(iVertex));
			} // for (size_t iVertex = ...

			/*
			* Check the non-empty Y bins with more than one element and create Z bins
			*/

			for (size_t iYBin = 0; iYBin < m_vecYBinsIndices.size(); iYBin++)
			{
				if ((m_vecYBinsIndices[iYBin] != nullptr) && (m_vecYBinsIndices[iYBin]->size() > 1))
				{
					for (size_t iBin = 0; iBin < m_vecZBinsIndices.size(); iBin++)
					{
						delete m_vecZBinsIndices[iBin];
					}

					fill(m_vecZBinsIndices.begin(), m_vecZBinsIndices.end(), nullptr);

					for (size_t iVertex = 0; iVertex < m_vecYBinsIndices[iYBin]->size(); iVertex++)
					{
						// Z					
						float fZ = pInstance->getVertices()[(m_vecYBinsIndices[iYBin]->at(iVertex) * VERTEX_LENGTH) + 2];

						// Z => [0, 1]
						fZ = (fZ - fZmin) / fZDistance;
						ASSERT((fZ >= 0.f) && (fZ <= 1.f));

						// Z bin
						uint32_t iZBin = (uint32_t)(fZ / BIN_SIZE);
						ASSERT((iZBin >= 0) && (iZBin <= m_iBinsCount));

						if (m_vecZBinsIndices[iZBin] == nullptr)
						{
							m_vecZBinsIndices[iZBin] = new vector<int64_t>();
						}

						m_vecZBinsIndices[iZBin]->push_back(m_vecYBinsIndices[iYBin]->at(iVertex));
					} // for (size_t iVertex = ...

					/*
					* Check the non-empty Z bins with more than one element
					*/
					
					for (size_t iZBin = 0; iZBin < m_vecZBinsIndices.size(); iZBin++)
					{
						if ((m_vecZBinsIndices[iZBin] != nullptr) && (m_vecZBinsIndices[iZBin]->size() > 1))
						{
							iDuplicatesCount++;

							output << "Duplicates:\n";
							for (size_t iVertex = 0; iVertex < m_vecZBinsIndices[iZBin]->size(); iVertex++)
							{
								float fX = pInstance->getVertices()[(m_vecZBinsIndices[iZBin]->at(iVertex) * VERTEX_LENGTH) + 0];
								float fY = pInstance->getVertices()[(m_vecZBinsIndices[iZBin]->at(iVertex) * VERTEX_LENGTH) + 1];
								float fZ = pInstance->getVertices()[(m_vecZBinsIndices[iZBin]->at(iVertex) * VERTEX_LENGTH) + 2];

								output << "Vertex: " << m_vecZBinsIndices[iZBin]->at(iVertex) << "(" << fX << ", " << fY << ", " << fZ << ")" << "\n";
							} // for (size_t iVertex = ...			
						} // if ((m_vecZBinsIndices[iZBin] != nullptr) && ...
					} // for (size_t iZBin = ...
				} // if ((m_vecYBinsIndices[iYBin] != nullptr) && ...
			} // for (size_t iYBin = ...
		} // if ((m_vecXBinsIndices[iXBin] != nullptr) && ...
	} // for (size_t iXBin = ...

	if (iDuplicatesCount == 0)
	{
		output << "No duplicates were found.";
	}
	
#ifdef _USE_BOOST
	high_resolution_clock::time_point tpEnd = high_resolution_clock::now();
	duration<float> duration = tpEnd - tpStart;

	output << "Vertices count: " << pInstance->GetVerticesCount() << "\n";
	output << "The check is done in " << duration << "." << "\n";
	output << iDuplicatesCount << " duplicates were found.\n";
	output << "****************************************************************************************************\n";
#endif

	return iDuplicatesCount;
}