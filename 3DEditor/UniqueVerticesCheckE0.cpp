#include "stdafx.h"
#include "UniqueVerticesCheckE0.h"

#include <algorithm>

using namespace std;

#ifdef _USE_BOOST
#include <boost/chrono.hpp>

using namespace boost::chrono;
#endif // _USE_BOOST

// ------------------------------------------------------------------------------------------------
CUniqueVerticesCheckE0::CUniqueVerticesCheckE0()
	: m_mapXDistribution()
	, m_mapYDistribution()
	, m_mapZDistribution()
	, m_vecVerticesMapping()
	, m_vecVerticesConvertor()
{
}

// ------------------------------------------------------------------------------------------------
CUniqueVerticesCheckE0::~CUniqueVerticesCheckE0()
{
	Clear(m_mapXDistribution);
	Clear(m_mapYDistribution);
	Clear(m_mapZDistribution);
}

// ------------------------------------------------------------------------------------------------
int32_t CUniqueVerticesCheckE0::Check(CRDFInstance * pInstance, int32_t iVertexLength, wofstream & output)
{
	ASSERT(pInstance != nullptr);
	ASSERT(iVertexLength > 0);

	if (pInstance->GetVerticesCount() == 0)
	{
		return 0;
	}	

	/*if (pInstance->hasConceptualFaceWithTexture())
	{
		output << "****************************************************************************************************\n";
		output << "Instance: " << pInstance->getName() << "\n";
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

	/*
	* Mapping
	*/

	m_vecVerticesMapping.resize(pInstance->GetVerticesCount());
	fill(m_vecVerticesMapping.begin(), m_vecVerticesMapping.end(), -1);

	m_vecVerticesConvertor.resize(pInstance->GetVerticesCount());
	fill(m_vecVerticesConvertor.begin(), m_vecVerticesConvertor.end(), -1);

	output << "****************************************************************************************************\n";
	output << "Instance: " << pInstance->getName() << "\n";

	/*
	* X distribution
	*/

	Clear(m_mapXDistribution);
	for (int32_t iVertex = 0; iVertex < pInstance->GetVerticesCount(); iVertex++)
	{
		// X
		float fX = pInstance->GetVertices()[(iVertex * iVertexLength) + 0];

		map<float, vector<int32_t> *>::iterator itX = m_mapXDistribution.find(fX);
		if (itX == m_mapXDistribution.end())
		{
			vector<int32_t>* pIndices = new vector<int32_t>();
			pIndices->push_back(iVertex);

			m_mapXDistribution[fX] = pIndices;
		}
		else
		{
			itX->second->push_back(iVertex);
		}
	} // for (int32_t iVertex = ...

	if (m_mapXDistribution.size() == (size_t)pInstance->GetVerticesCount())
	{
#ifdef _USE_BOOST
		high_resolution_clock::time_point tpEnd = high_resolution_clock::now();
		duration<float> duration = tpEnd - tpStart;

		output << "0 duplicates were found.\n";
		output << "Vertices count: " << pInstance->GetVerticesCount() << "\n";
		output << "The check is done in " << duration << "." << "\n";
		output << "****************************************************************************************************\n";
#endif

		return 0;
	}

	/*
	* For each duplicated X value create Y distribution
	*/

	int32_t iDuplicatesCount = 0;

	map<float, vector<int32_t> *>::iterator itX = m_mapXDistribution.begin();
	for (; itX != m_mapXDistribution.end(); itX++)
	{
		if (itX->second->size() == 1)
		{
			continue;
		}

		Clear(m_mapYDistribution);
		for (size_t iIndex = 0; iIndex < itX->second->size(); iIndex++)
		{
			// Y
			float fY = pInstance->GetVertices()[(itX->second->at(iIndex) * iVertexLength) + 1];

			map<float, vector<int32_t> *>::iterator itY = m_mapYDistribution.find(fY);
			if (itY == m_mapYDistribution.end())
			{
				vector<int32_t>* pIndices = new vector<int32_t>();
				pIndices->push_back(itX->second->at(iIndex));

				m_mapYDistribution[fY] = pIndices;
			}
			else
			{
				itY->second->push_back(itX->second->at(iIndex));
			}
		} // for (size_t iIndex = ...

		/*
		* For each duplicated Y value create Z distribution
		*/

		map<float, vector<int32_t> *>::iterator itY = m_mapYDistribution.begin();
		for (; itY != m_mapYDistribution.end(); itY++)
		{
			if (itY->second->size() == 1)
			{
				continue;
			}

			Clear(m_mapZDistribution);
			for (size_t iIndex = 0; iIndex < itY->second->size(); iIndex++)
			{
				// Z
				float fZ = pInstance->GetVertices()[(itY->second->at(iIndex) * iVertexLength) + 2];

				map<float, vector<int32_t> *>::iterator itZ = m_mapZDistribution.find(fZ);
				if (itZ == m_mapZDistribution.end())
				{
					vector<int32_t>* pIndices = new vector<int32_t>();
					pIndices->push_back(itY->second->at(iIndex));

					m_mapZDistribution[fZ] = pIndices;
				}
				else
				{
					itZ->second->push_back(itY->second->at(iIndex));
				}
			} // for (size_t iIndex = ...

			/*
			* Print the duplicated vertices
			*/

			map<float, vector<int32_t> *>::iterator itZ = m_mapZDistribution.begin();
			for (; itZ != m_mapZDistribution.end(); itZ++)
			{
				if (itZ->second->size() == 1)
				{
					continue;
				}

				iDuplicatesCount++;

				sort(itZ->second->begin(), itZ->second->end());
				
				// TEST CODE
				// Comment 'if (pInstance->hasConceptualFaceWithTexture())'
				// TODO: use distribution map! 
				//if (pInstance->hasConceptualFaceWithTexture())
				//{
				//	// TODO: use distribution map
				//	vector<int32_t> vecIndices;
				//	vecIndices.push_back(itZ->second->at(0));

				//	for (size_t iVertex = 1; iVertex < itZ->second->size(); iVertex++)
				//	{
				//		if ((pInstance->GetVertices()[(itZ->second->at(0) * iVertexLength) + 6] == pInstance->GetVertices()[(itZ->second->at(iVertex) * iVertexLength) + 6]) &&
				//			pInstance->GetVertices()[(itZ->second->at(0) * iVertexLength) + 7] == pInstance->GetVertices()[(itZ->second->at(iVertex) * iVertexLength) + 7])
				//		{
				//			vecIndices.push_back(itZ->second->at(iVertex));
				//		}
				//	}

				//	if (vecIndices.size() == 1)
				//	{
				//		continue;
				//	}

				//	*(itZ->second) = vecIndices;
				//} // if (pInstance->hasConceptualFaceWithTexture())

				/*
				* Update the mapping
				*/

				ASSERT(m_vecVerticesMapping[itZ->second->at(0)] == -1);
				m_vecVerticesMapping[itZ->second->at(0)] = itZ->second->at(0);

				ASSERT(m_vecVerticesConvertor[itZ->second->at(0)] == -1);
				m_vecVerticesConvertor[itZ->second->at(0)] = itZ->second->at(0);

				for (size_t iIndex = 1; iIndex < itZ->second->size(); iIndex++)
				{
					ASSERT(m_vecVerticesMapping[itZ->second->at(iIndex)] == -1);
					m_vecVerticesMapping[itZ->second->at(iIndex)] = itZ->second->at(0);
				}

				output << "Duplicates:\n";
				for (size_t iIndex = 0; iIndex < itZ->second->size(); iIndex++)
				{
					// X
					float fX = pInstance->GetVertices()[(itZ->second->at(iIndex) * iVertexLength) + 0];

					// Y
					float fY = pInstance->GetVertices()[(itZ->second->at(iIndex) * iVertexLength) + 1];

					// Z
					float fZ = pInstance->GetVertices()[(itZ->second->at(iIndex) * iVertexLength) + 2];

					output << "Vertex: " << itZ->second->at(iIndex) << "(" << fX << ", " << fY << ", " << fZ << ")" << "\n";
				} // for (size_t iIndex = ...
			} // for (; itZ != ...
		} // for (; itY != ...
	} // for (; itX != ...

	/*
	* Clear
	*/
	Clear(m_mapXDistribution);
	Clear(m_mapYDistribution);
	Clear(m_mapZDistribution);

#ifdef _USE_BOOST
	high_resolution_clock::time_point tpEnd = high_resolution_clock::now();
	duration<float> duration = tpEnd - tpStart;

	output << "Vertices count: " << pInstance->GetVerticesCount() << "\n";
	output << iDuplicatesCount << " duplicates were found.\n";
	output << "The check is done in " << duration << "." << "\n";	
	output << "****************************************************************************************************\n";
#endif

	return iDuplicatesCount;
}

// --------------------------------------------------------------------------------------------
tuple<float *, int64_t, int32_t *> CUniqueVerticesCheckE0::RemoveDuplicates(CRDFInstance * pInstance, int32_t iVertexLength)
{
	ASSERT(pInstance != nullptr);
	ASSERT(pInstance->GetVerticesCount() > 0);
	ASSERT(pInstance->getIndicesCount() > 0);
	ASSERT(iVertexLength > 0);

	/*
	* Set up the unique vertices
	*/
	for (int32_t iVertex = 0; iVertex < (int32_t)m_vecVerticesMapping.size(); iVertex++)
	{
		if (m_vecVerticesMapping[iVertex] == -1)
		{
			m_vecVerticesMapping[iVertex] = iVertex;

			ASSERT(m_vecVerticesConvertor[iVertex] == -1);
			m_vecVerticesConvertor[iVertex] = iVertex;
		}
	}

	/*
	* Update the indices
	*/

	int32_t* pIndices = new int32_t[pInstance->getIndicesCount()];

	for (int32_t iIndex = 0; iIndex < pInstance->getIndicesCount(); iIndex++)
	{
		if (pInstance->getIndices()[iIndex] < 0)
		{
			pIndices[iIndex] = pInstance->getIndices()[iIndex];

			continue;
		}

		pIndices[iIndex] = m_vecVerticesMapping[pInstance->getIndices()[iIndex]];
	} // for (int32_t iIndex = ...	

	/*
	* Calculate the new indices
	*/ 
	int32_t iNewVertex = 0;
	for (int32_t iVertex = 0; iVertex < (int32_t)m_vecVerticesConvertor.size(); iVertex++)
	{
		if (m_vecVerticesConvertor[iVertex] == -1)
		{
			continue;
		}

		m_vecVerticesConvertor[iVertex] = iNewVertex++;
	}

	int32_t iVerticesCount = 0;
	for (size_t iIndex = 0; iIndex < m_vecVerticesConvertor.size(); iIndex++)
	{
		if (m_vecVerticesConvertor[iIndex] >= 0)
		{
			iVerticesCount++;
		}
	}

	float* pVertices = new float[iVerticesCount * iVertexLength];

	int32_t iVertex = 0;
	for (size_t iIndex = 0; iIndex < m_vecVerticesConvertor.size(); iIndex++)
	{
		if (m_vecVerticesConvertor[iIndex] == -1)
		{
			continue;
		}
		
		memcpy(pVertices + (iVertex * iVertexLength), pInstance->GetVertices() + (iIndex * iVertexLength), iVertexLength * sizeof(float));

		iVertex++;
	} // for (size_t iIndex = ...

	/*
	* Update the indices
	*/
	for (int32_t iIndex = 0; iIndex < pInstance->getIndicesCount(); iIndex++)
	{
		if (pIndices[iIndex] < 0)
		{
			continue;
		}

		ASSERT(m_vecVerticesConvertor[pIndices[iIndex]] != -1);
		pIndices[iIndex] = m_vecVerticesConvertor[pIndices[iIndex]];
	} // for (int32_t iIndex = ...

	return tuple<float *, int64_t, int32_t *>(pVertices, iVerticesCount, pIndices);
}

// ------------------------------------------------------------------------------------------------
void CUniqueVerticesCheckE0::Clear(map<float, vector<int32_t> *>& mapDistribution) const
{
	map<float, vector<int32_t> *>::iterator itValue = mapDistribution.begin();
	for (; itValue != mapDistribution.end(); itValue++)
	{
		delete itValue->second;
	}

	mapDistribution.clear();
}