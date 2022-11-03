#include "stdafx.h"
#include "RDFInstance.h"
#include "Generic.h"
#include "_openGLUtils.h"

#include <memory>

// ------------------------------------------------------------------------------------------------
CRDFInstance::CRDFInstance(int64_t iID, int64_t iInstance)
	: m_iID(iID)
	, m_iInstance(iInstance)
	, m_strName(L"NA")
	, m_strUniqueName(L"")
	, m_pVertices(NULL)
	, m_pOriginalVertexBuffer(new VertexBuffer())
	, m_pIndexBuffer(new IndexBuffer())
	, m_iConceptualFacesCount(0)
	, m_vecTriangles()
	, m_vecLines()
	, m_vecPoints()
	, m_vecFacesPolygons()
	, m_vecConceptualFacesPolygons()
	, m_vecMaterials()
	, m_vecLinesCohorts()
	, m_vecPointsCohorts()	
	, m_vecConceptualFacesCohorts()
	, m_vecFacesCohorts()
	, m_vecNormalVectorsCohorts()
	, m_vecBiNormalVectorsCohorts()
	, m_vecTangentVectorsCohorts()
	, m_vecBoundingBoxMin(NULL)
	, m_vecBoundingBoxMax(NULL)
	, m_mtxBoundingBoxTransformation(NULL)
	, m_bEnable(true)
	, m_bNeedsRefresh(false)
	, m_iVBO(0)
	, m_iVBOOffset(0)
{	
	ASSERT(m_iInstance != 0);

	int64_t iClassInstance = GetInstanceClass(m_iInstance);
	ASSERT(iClassInstance != 0);

	wchar_t* szClassName = nullptr;
	GetNameOfClassW(iClassInstance, &szClassName);

	wchar_t* szName = nullptr;
	GetNameOfInstanceW(m_iInstance, &szName);

#ifndef _LINUX	

	wchar_t szUniqueName[200];

	if (szName != NULL)
	{
		m_strName = szName;

		swprintf(szUniqueName, 200, L"%s (%s)", szName, szClassName);
	}
	else
	{
		m_strName = szClassName;

		swprintf(szUniqueName, 200, L"#%lld (%s)", m_iInstance, szClassName);
	}	

	m_strUniqueName = szUniqueName;

//	LOG_DEBUG("*** INSTANCE " << m_strName.c_str());
#else
    m_strName = wxString(szName).wchar_str();

    wchar_t szUniqueName[200];
	swprintf(szUniqueName, 200, L"%lld (%ls)", m_pInstance->getInstance(), wxString(szClassName).wc_str());

	m_strUniqueName = szUniqueName;
#endif // _LINUX

	Calculate();
}

// ------------------------------------------------------------------------------------------------
CRDFInstance::~CRDFInstance()
{
	Clean();

	delete m_mtxBoundingBoxTransformation;
	m_mtxBoundingBoxTransformation = NULL;
	delete m_vecBoundingBoxMin;
	m_vecBoundingBoxMin = NULL;
	delete m_vecBoundingBoxMax;
	m_vecBoundingBoxMax = NULL;
}

// ------------------------------------------------------------------------------------------------
void CRDFInstance::Recalculate()
{	
	if (!m_bEnable)
	{
		// Reloading on demand
		m_bNeedsRefresh = true;

		return;
	}

	Clean();

	Calculate();
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFInstance::getID() const
{
	return m_iID;
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFInstance::GetModel() const
{
	int64_t iModel = ::GetModel(m_iInstance);
	ASSERT(iModel != 0);

	return iModel;
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFInstance::getInstance() const
{
	return m_iInstance;
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFInstance::getClassInstance() const
{
	int64_t iClassInstance = GetInstanceClass(m_iInstance);
	ASSERT(iClassInstance != 0);

	return iClassInstance;
}

// ------------------------------------------------------------------------------------------------
const wchar_t * CRDFInstance::getName() const
{
	return m_strName.c_str();
}

// ------------------------------------------------------------------------------------------------
const wchar_t * CRDFInstance::getUniqueName() const
{
	return m_strUniqueName.c_str();
}

// ------------------------------------------------------------------------------------------------
bool CRDFInstance::isReferenced() const
{
	return GetInstanceInverseReferencesByIterator(m_iInstance, 0);
}

// ------------------------------------------------------------------------------------------------
bool CRDFInstance::hasGeometry() const
{
	return (m_pOriginalVertexBuffer->getVerticesCount() > 0) && (m_pIndexBuffer->getIndicesCount() > 0);
}

// ------------------------------------------------------------------------------------------------
int32_t * CRDFInstance::getIndices() const
{
	return m_pIndexBuffer->getIndices();
}

int64_t CRDFInstance::getIndicesCount() const
{
	return m_pIndexBuffer->getIndicesCount();
}

// ------------------------------------------------------------------------------------------------
float * CRDFInstance::getVertices() const
{
	return m_pVertices;
}

// ------------------------------------------------------------------------------------------------
float* CRDFInstance::getOriginalVertices() const
{
	return m_pOriginalVertexBuffer != NULL ? m_pOriginalVertexBuffer->getVertices() : NULL;
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFInstance::getVerticesCount() const
{
	return m_pOriginalVertexBuffer->getVerticesCount();
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFInstance::getConceptualFacesCount() const
{
	return m_iConceptualFacesCount;
}

// ------------------------------------------------------------------------------------------------
const vector<_primitives> & CRDFInstance::getTriangles() const
{
	return m_vecTriangles;
}

const vector<_primitives> & CRDFInstance::getLines() const
{
	return m_vecLines;
}

const vector<_primitives> & CRDFInstance::getPoints() const
{
	return m_vecPoints;
}

const vector<_primitives> & CRDFInstance::getFacesPolygons() const
{
	return m_vecFacesPolygons;
}

const vector<_primitives> & CRDFInstance::getConceptualFacesPolygons() const
{
	return m_vecConceptualFacesPolygons;
}

// ------------------------------------------------------------------------------------------------
vector<CRDFGeometryWithMaterial*>& CRDFInstance::conceptualFacesMaterials()
{
	return m_vecMaterials;
}

// ------------------------------------------------------------------------------------------------
vector<_cohort*>& CRDFInstance::linesCohorts()
{
	return m_vecLinesCohorts;
}

// ------------------------------------------------------------------------------------------------
vector<_cohort*>& CRDFInstance::pointsCohorts()
{
	return m_vecPointsCohorts;
}

// ------------------------------------------------------------------------------------------------
vector<_cohort*>& CRDFInstance::conceptualFacesCohorts()
{
	return m_vecConceptualFacesCohorts;
}

// ------------------------------------------------------------------------------------------------
vector<_cohort*>& CRDFInstance::facesCohorts()
{
	return m_vecFacesCohorts;
}

// ------------------------------------------------------------------------------------------------
vector<_cohort*>& CRDFInstance::normalVectorsCohorts()
{
	return m_vecNormalVectorsCohorts;
}

// ------------------------------------------------------------------------------------------------
vector<_cohort*>& CRDFInstance::biNormalVectorsCohorts()
{
	return m_vecBiNormalVectorsCohorts;
}

// ------------------------------------------------------------------------------------------------
vector<_cohort*>& CRDFInstance::tangentVectorsCohorts()
{
	return m_vecTangentVectorsCohorts;
}

// ------------------------------------------------------------------------------------------------
VECTOR3 * CRDFInstance::getBoundingBoxMin() const
{
	return m_vecBoundingBoxMin;
}

// ------------------------------------------------------------------------------------------------
VECTOR3 * CRDFInstance::getBoundingBoxMax() const
{
	return m_vecBoundingBoxMax;
}

// ------------------------------------------------------------------------------------------------
MATRIX * CRDFInstance::getBoundingBoxTransformation() const
{
	return m_mtxBoundingBoxTransformation;
}

// ------------------------------------------------------------------------------------------------
void CRDFInstance::setEnable(bool bEnable)
{
	m_bEnable = bEnable;

	if (m_bEnable && m_bNeedsRefresh)
	{
		m_bNeedsRefresh = false;

		Recalculate();
	}
}

// ------------------------------------------------------------------------------------------------
bool CRDFInstance::getEnable() const
{
	return m_bEnable;
}

// ------------------------------------------------------------------------------------------------
void CRDFInstance::CalculateMinMax(float & fXmin, float & fXmax, float & fYmin, float & fYmax, float & fZmin, float & fZmax)
{
	if (m_pOriginalVertexBuffer->getVerticesCount() == 0)
	{
		return;
	}

	/*
	* Triangles
	*/
	if (!m_vecTriangles.empty())
	{
		for (size_t iTriangle = 0; iTriangle < m_vecTriangles.size(); iTriangle++)
		{
			for (int64_t iIndex = m_vecTriangles[iTriangle].getStartIndex(); 
				iIndex < m_vecTriangles[iTriangle].getStartIndex() + m_vecTriangles[iTriangle].getIndicesCount();
				iIndex++)
			{
				fXmin = (float)fmin(fXmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH)]);
				fXmax = (float)fmax(fXmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH)]);
				fYmin = (float)fmin(fYmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fYmax = (float)fmax(fYmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fZmin = (float)fmin(fZmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
				fZmax = (float)fmax(fZmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
			} // for (size_t iIndex = ...
		} // for (size_t iTriangle = ...
	} // if (!m_vecTriangles.empty())

	/*
	* Faces polygons
	*/
	if (!m_vecFacesPolygons.empty())
	{
		for (size_t iPolygon = 0; iPolygon < m_vecFacesPolygons.size(); iPolygon++)
		{
			for (int64_t iIndex = m_vecFacesPolygons[iPolygon].getStartIndex(); 
				iIndex < m_vecFacesPolygons[iPolygon].getStartIndex() + m_vecFacesPolygons[iPolygon].getIndicesCount();
				iIndex++)
			{
				if ((getIndices()[iIndex] == -1) || (getIndices()[iIndex] == -2))
				{
					continue;
				}

				fXmin = (float)fmin(fXmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH)]);
				fXmax = (float)fmax(fXmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH)]);
				fYmin = (float)fmin(fYmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fYmax = (float)fmax(fYmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fZmin = (float)fmin(fZmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
				fZmax = (float)fmax(fZmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
			} // for (size_t iIndex = ...
		} // for (size_t iPolygon = ...
	} // if (!m_vecFacesPolygons.empty())

	/*
	* Conceptual faces polygons
	*/
	if (!m_vecConceptualFacesPolygons.empty())
	{
		for (size_t iPolygon = 0; iPolygon < m_vecConceptualFacesPolygons.size(); iPolygon++)
		{
			for (int64_t iIndex = m_vecConceptualFacesPolygons[iPolygon].getStartIndex(); 
				iIndex < m_vecConceptualFacesPolygons[iPolygon].getStartIndex() + m_vecConceptualFacesPolygons[iPolygon].getIndicesCount(); 
				iIndex++)
			{
				if ((getIndices()[iIndex] == -1) || (getIndices()[iIndex] == -2))
				{
					continue;
				}

				fXmin = (float)fmin(fXmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH)]);
				fXmax = (float)fmax(fXmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH)]);
				fYmin = (float)fmin(fYmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fYmax = (float)fmax(fYmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fZmin = (float)fmin(fZmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
				fZmax = (float)fmax(fZmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
			} // for (size_t iIndex = ...
		} // for (size_t iPolygon = ...
	} // if (!m_vecConceptualFacesPolygons.empty())

	/*
	* Lines
	*/
	if (!m_vecLines.empty())
	{
		for (size_t iPolygon = 0; iPolygon < m_vecLines.size(); iPolygon++)
		{
			for (int64_t iIndex = m_vecLines[iPolygon].getStartIndex();
				iIndex < m_vecLines[iPolygon].getStartIndex() + m_vecLines[iPolygon].getIndicesCount();
				iIndex++)
			{
				if (getIndices()[iIndex] == -1)
				{
					continue;
				}

				fXmin = (float)fmin(fXmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH)]);
				fXmax = (float)fmax(fXmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH)]);
				fYmin = (float)fmin(fYmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fYmax = (float)fmax(fYmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fZmin = (float)fmin(fZmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
				fZmax = (float)fmax(fZmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
			} // for (size_t iIndex = ...
		} // for (size_t iPolygon = ...
	} // if (!m_vecLines.empty())

	/*
	* Points
	*/
	if (!m_vecPoints.empty())
	{
		for (size_t iPolygon = 0; iPolygon < m_vecPoints.size(); iPolygon++)
		{
			for (int64_t iIndex = m_vecPoints[iPolygon].getStartIndex(); 
				iIndex < m_vecPoints[iPolygon].getStartIndex() + m_vecPoints[iPolygon].getIndicesCount();
				iIndex++)
			{
				fXmin = (float)fmin(fXmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH)]);
				fXmax = (float)fmax(fXmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH)]);
				fYmin = (float)fmin(fYmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fYmax = (float)fmax(fYmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fZmin = (float)fmin(fZmin, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
				fZmax = (float)fmax(fZmax, getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
			} // for (size_t iIndex = ...
		} // for (size_t iPolygon = ...
	} // if (!m_vecPoints.empty())
}

// ------------------------------------------------------------------------------------------------
void CRDFInstance::ScaleAndCenter(float fXmin, float fXmax, float fYmin, float fYmax, float fZmin, float fZmax, float fResoltuion)
{
	if (m_pOriginalVertexBuffer->getVerticesCount() == 0)
	{
		return;
	}

	delete[] m_pVertices;
	m_pVertices = new float[m_pOriginalVertexBuffer->getVerticesCount() * VERTEX_LENGTH];
	memcpy(m_pVertices, m_pOriginalVertexBuffer->getVertices(), m_pOriginalVertexBuffer->getVerticesCount() * m_pOriginalVertexBuffer->getVertexLength() * sizeof(float));

	/**
	* Vertices
	*/
	for (int_t iVertex = 0; iVertex < m_pOriginalVertexBuffer->getVerticesCount(); iVertex++)
	{
		// [0.0 -> X/Y/Zmin + X/Y/Zmax]
		m_pVertices[(iVertex * VERTEX_LENGTH)] = m_pVertices[(iVertex * VERTEX_LENGTH)] - fXmin;
		m_pVertices[(iVertex * VERTEX_LENGTH) + 1] = m_pVertices[(iVertex * VERTEX_LENGTH) + 1] - fYmin;
		m_pVertices[(iVertex * VERTEX_LENGTH) + 2] = m_pVertices[(iVertex * VERTEX_LENGTH) + 2] - fZmin;

		// center
		m_pVertices[(iVertex * VERTEX_LENGTH)] = m_pVertices[(iVertex * VERTEX_LENGTH)] - ((fXmax - fXmin) / 2.0f);
		m_pVertices[(iVertex * VERTEX_LENGTH) + 1] = m_pVertices[(iVertex * VERTEX_LENGTH) + 1] - ((fYmax - fYmin) / 2.0f);
		m_pVertices[(iVertex * VERTEX_LENGTH) + 2] = m_pVertices[(iVertex * VERTEX_LENGTH) + 2] - ((fZmax - fZmin) / 2.0f);

		// [-1.0 -> 1.0]
		m_pVertices[(iVertex * VERTEX_LENGTH)] = m_pVertices[(iVertex * VERTEX_LENGTH)] / (fResoltuion / 2.0f);
		m_pVertices[(iVertex * VERTEX_LENGTH) + 1] = m_pVertices[(iVertex * VERTEX_LENGTH) + 1] / (fResoltuion / 2.0f);
		m_pVertices[(iVertex * VERTEX_LENGTH) + 2] = m_pVertices[(iVertex * VERTEX_LENGTH) + 2] / (fResoltuion / 2.0f);
	}

	/**
	* Bounding box - Min
	*/

	// [0.0 -> X/Y/Zmin + X/Y/Zmax]
	m_vecBoundingBoxMin->x = m_vecBoundingBoxMin->x - fXmin;
	m_vecBoundingBoxMin->y = m_vecBoundingBoxMin->y - fYmin;
	m_vecBoundingBoxMin->z = m_vecBoundingBoxMin->z - fZmin;

	// center
	m_vecBoundingBoxMin->x = m_vecBoundingBoxMin->x - ((fXmax - fXmin) / 2.0f);
	m_vecBoundingBoxMin->y = m_vecBoundingBoxMin->y - ((fYmax - fYmin) / 2.0f);
	m_vecBoundingBoxMin->z = m_vecBoundingBoxMin->z - ((fZmax - fZmin) / 2.0f);

	// [-1.0 -> 1.0]
	m_vecBoundingBoxMin->x = m_vecBoundingBoxMin->x / (fResoltuion / 2.0f);
	m_vecBoundingBoxMin->y = m_vecBoundingBoxMin->y / (fResoltuion / 2.0f);
	m_vecBoundingBoxMin->z = m_vecBoundingBoxMin->z / (fResoltuion / 2.0f);

	/**
	* Bounding box - Max
	*/

	// [0.0 -> X/Y/Zmin + X/Y/Zmax]
	m_vecBoundingBoxMax->x = m_vecBoundingBoxMax->x - fXmin;
	m_vecBoundingBoxMax->y = m_vecBoundingBoxMax->y - fYmin;
	m_vecBoundingBoxMax->z = m_vecBoundingBoxMax->z - fZmin;

	// center
	m_vecBoundingBoxMax->x = m_vecBoundingBoxMax->x - ((fXmax - fXmin) / 2.0f);
	m_vecBoundingBoxMax->y = m_vecBoundingBoxMax->y - ((fYmax - fYmin) / 2.0f);
	m_vecBoundingBoxMax->z = m_vecBoundingBoxMax->z - ((fZmax - fZmin) / 2.0f);

	// [-1.0 -> 1.0]
	m_vecBoundingBoxMax->x = m_vecBoundingBoxMax->x / (fResoltuion / 2.0f);
	m_vecBoundingBoxMax->y = m_vecBoundingBoxMax->y / (fResoltuion / 2.0f);
	m_vecBoundingBoxMax->z = m_vecBoundingBoxMax->z / (fResoltuion / 2.0f);

	/**
	* Bounding box - Transformation
	*/

	// [-1.0 -> 1.0]
	m_mtxBoundingBoxTransformation->_41 /= (fResoltuion / 2.0f);
	m_mtxBoundingBoxTransformation->_42 /= (fResoltuion / 2.0f);
	m_mtxBoundingBoxTransformation->_43 /= (fResoltuion / 2.0f);
}

// --------------------------------------------------------------------------------------------
float* CRDFInstance::BuildFacesVertices()
{
	ASSERT(m_pOriginalVertexBuffer->getVerticesCount() > 0);

	/*
	* Faces Vertex Buffer
	*/
	float* pFacesVertices = new float[m_pOriginalVertexBuffer->getVerticesCount() * GEOMETRY_VBO_VERTEX_LENGTH];
	memset(pFacesVertices, 0, m_pOriginalVertexBuffer->getVerticesCount() * GEOMETRY_VBO_VERTEX_LENGTH * sizeof(float));

	for (int64_t iVertex = 0; iVertex < m_pOriginalVertexBuffer->getVerticesCount(); iVertex++)
	{
		pFacesVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 0] = m_pVertices[(iVertex * VERTEX_LENGTH) + 0];
		pFacesVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 1] = m_pVertices[(iVertex * VERTEX_LENGTH) + 1];
		pFacesVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 2] = m_pVertices[(iVertex * VERTEX_LENGTH) + 2];

		pFacesVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 3] = m_pVertices[(iVertex * VERTEX_LENGTH) + 3];
		pFacesVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 4] = m_pVertices[(iVertex * VERTEX_LENGTH) + 4];
		pFacesVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 5] = m_pVertices[(iVertex * VERTEX_LENGTH) + 5];

		pFacesVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 6] = m_pVertices[(iVertex * VERTEX_LENGTH) + 6];
		pFacesVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 7] = m_pVertices[(iVertex * VERTEX_LENGTH) + 7];
	} // for (size_t iIndex = ...		

	return pFacesVertices;
}

//float* CRDFInstance::BuildGeometryVertices(float fXmin, float fXmax, float fYmin, float fYmax, float fZmin, float fZmax, float fResoltuion)
//{
//	ASSERT(m_iVerticesCount > 0);
//
//	/*
//	* Vertex Buffer
//	*/
//	float* pVertices = new float[m_iVerticesCount * GEOMETRY_VBO_VERTEX_LENGTH];
//	memset(pVertices, 0, m_iVerticesCount * GEOMETRY_VBO_VERTEX_LENGTH * sizeof(float));
//
//	for (int64_t iVertex = 0; iVertex < m_iVerticesCount; iVertex++)
//	{
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 0] = m_pOriginalVertices[(iVertex * VERTEX_LENGTH) + 0];
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 1] = m_pOriginalVertices[(iVertex * VERTEX_LENGTH) + 1];
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 2] = m_pOriginalVertices[(iVertex * VERTEX_LENGTH) + 2];
//
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 3] = m_pOriginalVertices[(iVertex * VERTEX_LENGTH) + 3];
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 4] = m_pOriginalVertices[(iVertex * VERTEX_LENGTH) + 4];
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 5] = m_pOriginalVertices[(iVertex * VERTEX_LENGTH) + 5];
//
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 6] = m_pOriginalVertices[(iVertex * VERTEX_LENGTH) + 6];
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 7] = m_pOriginalVertices[(iVertex * VERTEX_LENGTH) + 7];
//	} // for (size_t iIndex = ...		
//
//	for (int_t iVertex = 0; iVertex < m_iVerticesCount; iVertex++)
//	{
//		// [0.0 -> X/Y/Zmin + X/Y/Zmax]
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH)] = pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH)] - fXmin;
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 1] = pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 1] - fYmin;
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 2] = pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 2] - fZmin;
//
//		// center
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH)] = pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH)] - ((fXmax - fXmin) / 2.0f);
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 1] = pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 1] - ((fYmax - fYmin) / 2.0f);
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 2] = pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 2] - ((fZmax - fZmin) / 2.0f);
//
//		// [-1.0 -> 1.0]
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH)] = pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH)] / (fResoltuion / 2.0f);
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 1] = pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 1] / (fResoltuion / 2.0f);
//		pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 2] = pVertices[(iVertex * GEOMETRY_VBO_VERTEX_LENGTH) + 2] / (fResoltuion / 2.0f);
//	}
//
//	return pVertices;
//}

// ------------------------------------------------------------------------------------------------
void CRDFInstance::UpdateVertices(float * /*pVertices*/, int64_t /*iVerticesCount*/, int32_t * /*pIndices*/)
{
	ASSERT(FALSE); // TODO
	/*ASSERT(pVertices != nullptr);
	ASSERT(iVerticesCount > 0);
	ASSERT(pIndices != nullptr);

	delete[] m_pVertices;
	m_pVertices = pVertices;

	m_iVerticesCount = iVerticesCount;

	delete[] m_pOriginalVertices;
	m_pOriginalVertices = NULL;

	if (m_iVerticesCount > 0)
	{
		m_pOriginalVertices = new float[m_iVerticesCount * VERTEX_LENGTH];
		memcpy(m_pOriginalVertices, m_pVertices, m_iVerticesCount * VERTEX_LENGTH * sizeof(float));
	}	

	delete[] m_pIndices;
	m_pIndices = pIndices;*/
}

// ------------------------------------------------------------------------------------------------
GLuint& CRDFInstance::VBO()
{
	return m_iVBO;
}

// ------------------------------------------------------------------------------------------------
GLsizei& CRDFInstance::VBOOffset()
{
	return m_iVBOOffset;
}

// ------------------------------------------------------------------------------------------------
void CRDFInstance::Calculate()
{
	delete m_pOriginalVertexBuffer;
	m_pOriginalVertexBuffer = new VertexBuffer();

	delete[] m_pVertices;
	m_pVertices = NULL;

	delete m_pIndexBuffer;
	m_pIndexBuffer = new IndexBuffer();

	/**
	* Calculate
	*/
	CalculateInstance(m_iInstance, &m_pOriginalVertexBuffer->m_iVerticesCount, &m_pIndexBuffer->m_iIndicesCount, NULL);

	/**
	* Retrieve the buffers
	*/
	if ((m_pOriginalVertexBuffer->m_iVerticesCount > 0) && (m_pIndexBuffer->m_iIndicesCount))
	{
		/**
		* Retrieves the vertices
		*/
		m_pOriginalVertexBuffer->m_iVertexLength = SetFormat(GetModel(), 0, 0) / sizeof(float);

		m_pOriginalVertexBuffer->m_pVertices = new float[m_pOriginalVertexBuffer->m_iVerticesCount * m_pOriginalVertexBuffer->m_iVertexLength];
		memset(m_pOriginalVertexBuffer->m_pVertices, 0, m_pOriginalVertexBuffer->m_iVerticesCount * m_pOriginalVertexBuffer->m_iVertexLength * sizeof(float));

		UpdateInstanceVertexBuffer(m_iInstance, m_pOriginalVertexBuffer->m_pVertices);

		/**
		* Retrieves the indices
		*/
		m_pIndexBuffer->m_pIndices = new int32_t[m_pIndexBuffer->m_iIndicesCount];
		memset(m_pIndexBuffer->m_pIndices, 0, m_pIndexBuffer->m_iIndicesCount * sizeof(int32_t));

		UpdateInstanceIndexBuffer(m_iInstance, m_pIndexBuffer->m_pIndices);
	} // if ((m_pOriginalVertexBuffer->m_iVerticesCount > 0) && ...	

	m_pVertices = new float[m_pOriginalVertexBuffer->getVerticesCount() * VERTEX_LENGTH];
	memcpy(m_pVertices, m_pOriginalVertexBuffer->getVertices(), m_pOriginalVertexBuffer->getVerticesCount() * m_pOriginalVertexBuffer->getVertexLength() * sizeof(float));
	
	// MATERIAL : FACE INDEX, START INDEX, INIDCES COUNT, etc.
	MATERIALS mapMaterial2ConceptualFaces;

	m_iConceptualFacesCount = GetConceptualFaceCnt(m_iInstance);
	for (int64_t iConceptualFace = 0; iConceptualFace < m_iConceptualFacesCount; iConceptualFace++)
	{
		int64_t iStartIndexTriangles = 0;
		int64_t iIndicesCountTriangles = 0;
		int64_t iStartIndexLines = 0;
		int64_t iLinesIndicesCount = 0;
		int64_t iStartIndexPoints = 0;
		int64_t iPointsIndicesCount = 0;
		int64_t iStartIndexFacesPolygons = 0;
		int64_t iIndicesCountPolygonFaces = 0;
		int64_t iStartIndexConceptualFacePolygons = 0;
		int64_t iConceptualFacePolygonsIndicesCount = 0;

		GetConceptualFace(m_iInstance, iConceptualFace,
			&iStartIndexTriangles, &iIndicesCountTriangles,
			&iStartIndexLines, &iLinesIndicesCount,
			&iStartIndexPoints, &iPointsIndicesCount,
			&iStartIndexFacesPolygons, &iIndicesCountPolygonFaces,
			&iStartIndexConceptualFacePolygons, &iConceptualFacePolygonsIndicesCount);

		bool bHasTexture = false;
		if (iIndicesCountTriangles > 0)
		{
			/*
			* Material
			*/
			int32_t iIndexValue = *(m_pIndexBuffer->getIndices() + iStartIndexTriangles);
			iIndexValue *= VERTEX_LENGTH;

			float fColor = *(m_pVertices + iIndexValue + 8);
			unsigned int iAmbientColor = *(reinterpret_cast<unsigned int*>(&fColor));
			float fTransparency = (float)(iAmbientColor & (255)) / (float)255;

			fColor = *(m_pVertices + iIndexValue + 9);
			unsigned int iDiffuseColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertices + iIndexValue + 10);
			unsigned int iEmissiveColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertices + iIndexValue + 11);
			unsigned int iSpecularColor = *(reinterpret_cast<unsigned int*>(&fColor));

			/*
			* Check for a texture
			*/
			for (int64_t iIndex = iStartIndexTriangles; iIndex < iStartIndexTriangles + iIndicesCountTriangles; iIndex++)
			{
				if ((getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 6] != 0.f) || (getVertices()[(getIndices()[iIndex] * VERTEX_LENGTH) + 7] != 0.f))
				{
					bHasTexture = true;

					break;
				}
			} // for (size_t iIndex = ...

			/*
			* Material
			*/
			CRDFGeometryWithMaterial* pGeometryWithMaterial = new CRDFGeometryWithMaterial(
				iAmbientColor, iDiffuseColor, iEmissiveColor, iSpecularColor, fTransparency,
				bHasTexture ? L"default" : NULL);

			MATERIALS::iterator itMaterial2ConceptualFaces = mapMaterial2ConceptualFaces.find(*pGeometryWithMaterial);
			if (itMaterial2ConceptualFaces == mapMaterial2ConceptualFaces.end())
			{
				vector<CConceptualFace> vecConceptualFaces;

				CConceptualFace conceptualFace;
				conceptualFace.index() = iConceptualFace;
				conceptualFace.trianglesStartIndex() = iStartIndexTriangles;
				conceptualFace.trianglesIndicesCount() = iIndicesCountTriangles;

				vecConceptualFaces.push_back(conceptualFace);

				mapMaterial2ConceptualFaces[*pGeometryWithMaterial] = vecConceptualFaces;
			}
			else
			{
				CConceptualFace conceptualFace;
				conceptualFace.index() = iConceptualFace;
				conceptualFace.trianglesStartIndex() = iStartIndexTriangles;
				conceptualFace.trianglesIndicesCount() = iIndicesCountTriangles;

				itMaterial2ConceptualFaces->second.push_back(conceptualFace);
			}

			delete pGeometryWithMaterial;
		}

		if (iIndicesCountTriangles > 0)
		{
			m_vecTriangles.push_back(_primitives(iStartIndexTriangles, iIndicesCountTriangles));
		}

		if (iLinesIndicesCount > 0)
		{
			m_vecLines.push_back(_primitives(iStartIndexLines, iLinesIndicesCount));
		}

		if (iPointsIndicesCount > 0)
		{
			m_vecPoints.push_back(_primitives(iStartIndexPoints, iPointsIndicesCount));
		}

		if (iIndicesCountPolygonFaces > 0)
		{
			m_vecFacesPolygons.push_back(_primitives(iStartIndexFacesPolygons, iIndicesCountPolygonFaces));
		}

		if (iConceptualFacePolygonsIndicesCount > 0)
		{
			m_vecConceptualFacesPolygons.push_back(_primitives(iStartIndexConceptualFacePolygons, iConceptualFacePolygonsIndicesCount));
		}
	} // for (int64_t iConceptualFace = ...

	/*
	* Group the triangles
	*/
	MATERIALS::iterator itMaterial2ConceptualFaces = mapMaterial2ConceptualFaces.begin();
	for (; itMaterial2ConceptualFaces != mapMaterial2ConceptualFaces.end(); itMaterial2ConceptualFaces++)
	{
		CRDFGeometryWithMaterial* pGeometryWithMaterial = NULL;

		for (size_t iConceptualFace = 0; iConceptualFace < itMaterial2ConceptualFaces->second.size(); iConceptualFace++)
		{
			CConceptualFace& conceptualFace = itMaterial2ConceptualFaces->second[iConceptualFace];

			int_t iStartIndexTriangles = conceptualFace.trianglesStartIndex();
			int_t iIndicesCountTriangles = conceptualFace.trianglesIndicesCount();

			/*
			* Split the conceptual face - isolated case
			*/
			if (iIndicesCountTriangles > _openGLUtils::getIndicesCountLimit())
			{
				while (iIndicesCountTriangles > _openGLUtils::getIndicesCountLimit())
				{
					// INDICES
					CRDFGeometryWithMaterial* pNewMaterial = new CRDFGeometryWithMaterial(itMaterial2ConceptualFaces->first);
					for (int_t iIndex = iStartIndexTriangles; iIndex < iStartIndexTriangles + _openGLUtils::getIndicesCountLimit(); iIndex++)
					{
						pNewMaterial->addIndex(m_pIndexBuffer->getIndices()[iIndex]);
					}

					conceptualFacesMaterials().push_back(pNewMaterial);

					/*
					* Update Conceptual face start index
					*/
					conceptualFace.trianglesStartIndex() = 0;

					// Conceptual faces
					pNewMaterial->conceptualFaces().push_back(conceptualFace);

					iIndicesCountTriangles -= _openGLUtils::getIndicesCountLimit();
					iStartIndexTriangles += _openGLUtils::getIndicesCountLimit();
				}

				if (iIndicesCountTriangles > 0)
				{
					// INDICES
					CRDFGeometryWithMaterial* pNewMaterial = new CRDFGeometryWithMaterial(itMaterial2ConceptualFaces->first);
					for (int_t iIndex = iStartIndexTriangles; iIndex < iStartIndexTriangles + iIndicesCountTriangles; iIndex++)
					{
						pNewMaterial->addIndex(m_pIndexBuffer->getIndices()[iIndex]);
					}

					conceptualFacesMaterials().push_back(pNewMaterial);

					/*
					* Update Conceptual face start index
					*/
					conceptualFace.trianglesStartIndex() = 0;

					// Conceptual faces
					pNewMaterial->conceptualFaces().push_back(conceptualFace);
				}

				continue;
			} // if (iIndicesCountTriangles > _openGLUtils::GetIndicesCountLimit())	

			/*
			* Create material
			*/
			if (pGeometryWithMaterial == NULL)
			{
				pGeometryWithMaterial = new CRDFGeometryWithMaterial(itMaterial2ConceptualFaces->first);

				conceptualFacesMaterials().push_back(pGeometryWithMaterial);
			}

			/*
			* Check the limit
			*/
			if (pGeometryWithMaterial->getIndicesCount() + iIndicesCountTriangles > _openGLUtils::getIndicesCountLimit())
			{
				pGeometryWithMaterial = new CRDFGeometryWithMaterial(itMaterial2ConceptualFaces->first);

				conceptualFacesMaterials().push_back(pGeometryWithMaterial);
			}

			/*
			* Update Conceptual face start index
			*/
			conceptualFace.trianglesStartIndex() = pGeometryWithMaterial->getIndicesCount();

			/*
			* Add the indices
			*/
			for (int_t iIndex = iStartIndexTriangles; iIndex < iStartIndexTriangles + iIndicesCountTriangles; iIndex++)
			{
				pGeometryWithMaterial->addIndex(m_pIndexBuffer->getIndices()[iIndex]);
			}

			// Conceptual faces
			pGeometryWithMaterial->conceptualFaces().push_back(conceptualFace);
		} // for (size_t iConceptualFace = ...				
	} // for (; itMaterial2ConceptualFaces != ...

	/*
	* Group the polygons
	*/
	if (!m_vecConceptualFacesPolygons.empty())
	{
		/*
		* Use the last cohort (if any)
		*/
		_cohort* pWireframesCohort = conceptualFacesCohorts().empty() ? NULL : conceptualFacesCohorts()[conceptualFacesCohorts().size() - 1];

		/*
		* Create the cohort
		*/
		if (pWireframesCohort == NULL)
		{
			pWireframesCohort = new _cohort();
			conceptualFacesCohorts().push_back(pWireframesCohort);
		}

		for (size_t iFace = 0; iFace < m_vecConceptualFacesPolygons.size(); iFace++)
		{
			int_t iStartIndexFacesPolygons = m_vecConceptualFacesPolygons[iFace].getStartIndex();
			int_t iIndicesFacesPolygonsCount = m_vecConceptualFacesPolygons[iFace].getIndicesCount();

			/*
			* Split the conceptual face - isolated case
			*/
			if (iIndicesFacesPolygonsCount > _openGLUtils::getIndicesCountLimit() / 2)
			{
				while (iIndicesFacesPolygonsCount > _openGLUtils::getIndicesCountLimit() / 2)
				{
					pWireframesCohort = new _cohort();
					conceptualFacesCohorts().push_back(pWireframesCohort);

					int_t iPreviousIndex = -1;
					for (int_t iIndex = iStartIndexFacesPolygons; iIndex < iStartIndexFacesPolygons + _openGLUtils::getIndicesCountLimit() / 2; iIndex++)
					{
						if (m_pIndexBuffer->getIndices()[iIndex] < 0)
						{
							iPreviousIndex = -1;

							continue;
						}

						if (iPreviousIndex != -1)
						{
							pWireframesCohort->indices().push_back(m_pIndexBuffer->getIndices()[iPreviousIndex]);
							pWireframesCohort->indices().push_back(m_pIndexBuffer->getIndices()[iIndex]);
						} // if (iPreviousIndex != -1)

						iPreviousIndex = iIndex;
					} // for (int_t iIndex = ...

					iIndicesFacesPolygonsCount -= _openGLUtils::getIndicesCountLimit() / 2;
					iStartIndexFacesPolygons += _openGLUtils::getIndicesCountLimit() / 2;
				} // while (iIndicesFacesPolygonsCount > _openGLUtils::GetIndicesCountLimit() / 2)

				if (iIndicesFacesPolygonsCount > 0)
				{
					pWireframesCohort = new _cohort();
					conceptualFacesCohorts().push_back(pWireframesCohort);

					int_t iPreviousIndex = -1;
					for (int_t iIndex = iStartIndexFacesPolygons; iIndex < iStartIndexFacesPolygons + iIndicesFacesPolygonsCount; iIndex++)
					{
						if (m_pIndexBuffer->getIndices()[iIndex] < 0)
						{
							iPreviousIndex = -1;

							continue;
						}

						if (iPreviousIndex != -1)
						{
							pWireframesCohort->indices().push_back(m_pIndexBuffer->getIndices()[iPreviousIndex]);
							pWireframesCohort->indices().push_back(m_pIndexBuffer->getIndices()[iIndex]);
						} // if (iPreviousIndex != -1)

						iPreviousIndex = iIndex;
					} // for (int_t iIndex = ...
				}

				continue;
			} // if (iIndicesFacesPolygonsCount > _openGLUtils::GetIndicesCountLimit() / 2)

			/*
			* Check the limit
			*/
			if ((pWireframesCohort->indices().size() + (iIndicesFacesPolygonsCount * 2)) > _openGLUtils::getIndicesCountLimit())
			{
				pWireframesCohort = new _cohort();
				conceptualFacesCohorts().push_back(pWireframesCohort);
			}

			int_t iPreviousIndex = -1;
			for (int_t iIndex = iStartIndexFacesPolygons; iIndex < iStartIndexFacesPolygons + iIndicesFacesPolygonsCount; iIndex++)
			{
				if (m_pIndexBuffer->getIndices()[iIndex] < 0)
				{
					iPreviousIndex = -1;

					continue;
				}

				if (iPreviousIndex != -1)
				{
					pWireframesCohort->indices().push_back(m_pIndexBuffer->getIndices()[iPreviousIndex]);
					pWireframesCohort->indices().push_back(m_pIndexBuffer->getIndices()[iIndex]);
				} // if (iPreviousIndex != -1)

				iPreviousIndex = iIndex;
			} // for (int_t iIndex = ...
		} // for (size_t iFace = ...

#ifdef _DEBUG
		for (size_t iCohort = 0; iCohort < conceptualFacesCohorts().size(); iCohort++)
		{
			ASSERT(conceptualFacesCohorts()[iCohort]->indices().size() <= _openGLUtils::GetIndicesCountLimit());
		}
#endif
	} // if (!m_vecConceptualFacesPolygons.empty())

	/*
	* Group the polygons
	*/
	if (!m_vecFacesPolygons.empty())
	{
		/*
		* Use the last cohort (if any)
		*/
		_cohort* pWireframesCohort = facesCohorts().empty() ? NULL : facesCohorts()[facesCohorts().size() - 1];

		/*
		* Create the cohort
		*/
		if (pWireframesCohort == NULL)
		{
			pWireframesCohort = new _cohort();
			facesCohorts().push_back(pWireframesCohort);
		}

		for (size_t iFace = 0; iFace < m_vecFacesPolygons.size(); iFace++)
		{
			int_t iStartIndexFacesPolygons = m_vecFacesPolygons[iFace].getStartIndex();
			int_t iIndicesFacesPolygonsCount = m_vecFacesPolygons[iFace].getIndicesCount();

			/*
			* Split the conceptual face - isolated case
			*/
			if (iIndicesFacesPolygonsCount > _openGLUtils::getIndicesCountLimit() / 2)
			{
				while (iIndicesFacesPolygonsCount > _openGLUtils::getIndicesCountLimit() / 2)
				{
					pWireframesCohort = new _cohort();
					facesCohorts().push_back(pWireframesCohort);

					int_t iPreviousIndex = -1;
					for (int_t iIndex = iStartIndexFacesPolygons; iIndex < iStartIndexFacesPolygons + _openGLUtils::getIndicesCountLimit() / 2; iIndex++)
					{
						if (m_pIndexBuffer->getIndices()[iIndex] < 0)
						{
							iPreviousIndex = -1;

							continue;
						}

						if (iPreviousIndex != -1)
						{
							pWireframesCohort->indices().push_back(m_pIndexBuffer->getIndices()[iPreviousIndex]);
							pWireframesCohort->indices().push_back(m_pIndexBuffer->getIndices()[iIndex]);
						} // if (iPreviousIndex != -1)

						iPreviousIndex = iIndex;
					} // for (int_t iIndex = ...

					iIndicesFacesPolygonsCount -= _openGLUtils::getIndicesCountLimit() / 2;
					iStartIndexFacesPolygons += _openGLUtils::getIndicesCountLimit() / 2;
				} // while (iIndicesFacesPolygonsCount > _openGLUtils::GetIndicesCountLimit() / 2)

				if (iIndicesFacesPolygonsCount > 0)
				{
					pWireframesCohort = new _cohort();
					facesCohorts().push_back(pWireframesCohort);

					int_t iPreviousIndex = -1;
					for (int_t iIndex = iStartIndexFacesPolygons; iIndex < iStartIndexFacesPolygons + iIndicesFacesPolygonsCount; iIndex++)
					{
						if (m_pIndexBuffer->getIndices()[iIndex] < 0)
						{
							iPreviousIndex = -1;

							continue;
						}

						if (iPreviousIndex != -1)
						{
							pWireframesCohort->indices().push_back(m_pIndexBuffer->getIndices()[iPreviousIndex]);
							pWireframesCohort->indices().push_back(m_pIndexBuffer->getIndices()[iIndex]);
						} // if (iPreviousIndex != -1)

						iPreviousIndex = iIndex;
					} // for (int_t iIndex = ...
				}

				continue;
			} // if (iIndicesFacesPolygonsCount > _openGLUtils::GetIndicesCountLimit() / 2)

			/*
			* Check the limit
			*/
			if ((pWireframesCohort->indices().size() + (iIndicesFacesPolygonsCount * 2)) > _openGLUtils::getIndicesCountLimit())
			{
				pWireframesCohort = new _cohort();
				facesCohorts().push_back(pWireframesCohort);
			}

			int_t iPreviousIndex = -1;
			for (int_t iIndex = iStartIndexFacesPolygons; iIndex < iStartIndexFacesPolygons + iIndicesFacesPolygonsCount; iIndex++)
			{
				if (m_pIndexBuffer->getIndices()[iIndex] < 0)
				{
					iPreviousIndex = -1;

					continue;
				}

				if (iPreviousIndex != -1)
				{
					pWireframesCohort->indices().push_back(m_pIndexBuffer->getIndices()[iPreviousIndex]);
					pWireframesCohort->indices().push_back(m_pIndexBuffer->getIndices()[iIndex]);
				} // if (iPreviousIndex != -1)

				iPreviousIndex = iIndex;
			} // for (int_t iIndex = ...				
		} // for (size_t iFace = ...

#ifdef _DEBUG
		for (size_t iCohort = 0; iCohort < facesCohorts().size(); iCohort++)
		{
			ASSERT(facesCohorts()[iCohort]->indices().size() <= _openGLUtils::GetIndicesCountLimit());
		}
#endif
	} // if (!m_vecFacesPolygons.empty())

	/*
	* Group the lines
	*/
	if (!m_vecLines.empty())
	{
		/*
		* Use the last cohort (if any)
		*/
		auto* pCohort = linesCohorts().empty() ? nullptr : linesCohorts()[linesCohorts().size() - 1];

		/*
		* Create the cohort
		*/
		if (pCohort == NULL)
		{
			pCohort = new _cohort();
			linesCohorts().push_back(pCohort);
		}

		for (size_t iFace = 0; iFace < m_vecLines.size(); iFace++)
		{
			int_t iStartIndexLines = m_vecLines[iFace].getStartIndex();
			int_t iIndicesLinesCount = m_vecLines[iFace].getIndicesCount();

			/*
			* Check the limit
			*/
			if (pCohort->indices().size() + iIndicesLinesCount > _openGLUtils::getIndicesCountLimit())
			{
				pCohort = new _cohort();
				linesCohorts().push_back(pCohort);
			}

			for (int_t iIndex = iStartIndexLines; iIndex < iStartIndexLines + iIndicesLinesCount; iIndex++)
			{
				if (m_pIndexBuffer->getIndices()[iIndex] < 0)
				{
					continue;
				}

				pCohort->indices().push_back(m_pIndexBuffer->getIndices()[iIndex]);
			} // for (int_t iIndex = ...
		} // for (size_t iFace = ...

#ifdef _DEBUG
		for (size_t iCohort = 0; iCohort < linesCohorts().size(); iCohort++)
		{
			ASSERT(linesCohorts()[iCohort]->indices().size() <= _openGLUtils::GetIndicesCountLimit());
		}
#endif
	} // if (!m_vecLines.empty())		

	/*
	* Group the points
	*/
	if (!m_vecPoints.empty())
	{
		/*
		* Use the last cohort (if any)
		*/
		auto pCohort = pointsCohorts().empty() ? NULL : pointsCohorts()[pointsCohorts().size() - 1];

		/*
		* Create the cohort
		*/
		if (pCohort == NULL)
		{
			pCohort = new _cohort();
			pointsCohorts().push_back(pCohort);
		}

		for (size_t iFace = 0; iFace < m_vecPoints.size(); iFace++)
		{
			int_t iStartIndexPoints = m_vecPoints[iFace].getStartIndex();
			int_t iIndicesPointsCount = m_vecPoints[iFace].getIndicesCount();

			/*
			* Check the limit
			*/
			if (pCohort->indices().size() + iIndicesPointsCount > _openGLUtils::getIndicesCountLimit())
			{
				pCohort = new _cohort();
				pointsCohorts().push_back(pCohort);
			}

			for (int_t iIndex = iStartIndexPoints; iIndex < iStartIndexPoints + iIndicesPointsCount; iIndex++)
			{
				pCohort->indices().push_back(m_pIndexBuffer->getIndices()[iIndex]);
			}
		} // for (size_t iFace = ...

#ifdef _DEBUG
		for (size_t iCohort = 0; iCohort < pointsCohorts().size(); iCohort++)
		{
			ASSERT(pointsCohorts()[iCohort]->indices().size() <= _openGLUtils::GetIndicesCountLimit());
		}
#endif
	} // if (!m_vecPoints.empty())			

	/*
	* Bounding box
	*/
	if (m_mtxBoundingBoxTransformation == NULL)
	{
		m_mtxBoundingBoxTransformation = new MATRIX();
		m_vecBoundingBoxMin = new VECTOR3();
		m_vecBoundingBoxMax = new VECTOR3();

		SetBoundingBoxReference(m_iInstance, (double*)m_mtxBoundingBoxTransformation, (double*)m_vecBoundingBoxMin, (double*)m_vecBoundingBoxMax);
	}
}

// ------------------------------------------------------------------------------------------------
void CRDFInstance::Clean()
{
	delete m_pOriginalVertexBuffer;
	m_pOriginalVertexBuffer = NULL;

	delete[] m_pVertices;
	m_pVertices = NULL;

	delete m_pIndexBuffer;
	m_pIndexBuffer = NULL;	

	m_vecTriangles.clear();
	m_vecLines.clear();
	m_vecPoints.clear();
	m_vecFacesPolygons.clear();
	m_vecConceptualFacesPolygons.clear();	

	for (size_t iMaterial = 0; iMaterial < m_vecMaterials.size(); iMaterial++)
	{
		delete m_vecMaterials[iMaterial];
	}
	m_vecMaterials.clear();

	for (size_t iLinesCohort = 0; iLinesCohort < m_vecLinesCohorts.size(); iLinesCohort++)
	{
		delete m_vecLinesCohorts[iLinesCohort];
	}
	m_vecLinesCohorts.clear();

	for (size_t iPointsCohort = 0; iPointsCohort < m_vecPointsCohorts.size(); iPointsCohort++)
	{
		delete m_vecPointsCohorts[iPointsCohort];
	}
	m_vecPointsCohorts.clear();	

	for (size_t iWireframesCohort = 0; iWireframesCohort < m_vecConceptualFacesCohorts.size(); iWireframesCohort++)
	{
		delete m_vecConceptualFacesCohorts[iWireframesCohort];
	}
	m_vecConceptualFacesCohorts.clear();

	for (size_t iWireframesCohort = 0; iWireframesCohort < m_vecFacesCohorts.size(); iWireframesCohort++)
	{
		delete m_vecFacesCohorts[iWireframesCohort];
	}
	m_vecFacesCohorts.clear();

	for (size_t iLinesCohort = 0; iLinesCohort < m_vecNormalVectorsCohorts.size(); iLinesCohort++)
	{
		delete m_vecNormalVectorsCohorts[iLinesCohort];
	}
	m_vecNormalVectorsCohorts.clear();

	for (size_t iLinesCohort = 0; iLinesCohort < m_vecBiNormalVectorsCohorts.size(); iLinesCohort++)
	{
		delete m_vecBiNormalVectorsCohorts[iLinesCohort];
	}
	m_vecBiNormalVectorsCohorts.clear();

	for (size_t iLinesCohort = 0; iLinesCohort < m_vecTangentVectorsCohorts.size(); iLinesCohort++)
	{
		delete m_vecTangentVectorsCohorts[iLinesCohort];
	}
	m_vecTangentVectorsCohorts.clear();
}
