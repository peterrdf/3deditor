#include "stdafx.h"
#include "RDFInstance.h"
#include "Generic.h"

#include <memory>

// ------------------------------------------------------------------------------------------------
CRDFInstance::CRDFInstance(int64_t iID, int64_t iInstance)
	: m_iID(iID)
	, m_iInstance(iInstance)
	, m_strName(L"NA")
	, m_strUniqueName(L"")
	, m_pOriginalVertexBuffer(new _vertices_f())
	, m_pVertices(NULL)	
	, m_pIndexBuffer(new _indices_i32())
	, m_iConceptualFacesCount(0)
	, m_vecTriangles()
	, m_vecFacePolygons()
	, m_vecConcFacePolygons()
	, m_vecLines()
	, m_vecPoints()	
	, m_vecConcFacesCohorts()
	, m_vecFacePolygonsCohorts()
	, m_vecConcFacePolygonsCohorts()
	, m_vecLinesCohorts()
	, m_vecPointsCohorts()
	, m_vecNormalVecsCohorts()
	, m_vecBiNormalVecsCohorts()
	, m_vecTangentVecsCohorts()
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
	return (m_pOriginalVertexBuffer->size() > 0) && (m_pIndexBuffer->size() > 0);
}

// ------------------------------------------------------------------------------------------------
int32_t * CRDFInstance::getIndices() const
{
	return m_pIndexBuffer->data();
}

int64_t CRDFInstance::getIndicesCount() const
{
	return m_pIndexBuffer->size();
}

float * CRDFInstance::getVertices() const
{
	return m_pVertices;
}

float* CRDFInstance::getOriginalVertices() const
{
	return m_pOriginalVertexBuffer != nullptr ? m_pOriginalVertexBuffer->data() : nullptr;
}

int64_t CRDFInstance::getVerticesCount() const
{
	return m_pOriginalVertexBuffer->size();
}

int64_t CRDFInstance::getVertexLength() const 
{ 
	return VERTEX_LENGTH; 
}

int64_t CRDFInstance::getConceptualFacesCount() const
{
	return m_iConceptualFacesCount;
}

const vector<_primitives>& CRDFInstance::getTriangles() const
{
	return m_vecTriangles;
}

const vector<_primitives>& CRDFInstance::getLines() const
{
	return m_vecLines;
}

const vector<_primitives>& CRDFInstance::getPoints() const
{
	return m_vecPoints;
}

const vector<_primitives>& CRDFInstance::getFacePolygons() const
{
	return m_vecFacePolygons;
}

const vector<_primitives>& CRDFInstance::getConcFacePolygons() const
{
	return m_vecConcFacePolygons;
}

vector<_facesCohort*>& CRDFInstance::concFacesCohorts()
{
	return m_vecConcFacesCohorts;
}

vector<_cohort*>& CRDFInstance::facePolygonsCohorts()
{
	return m_vecFacePolygonsCohorts;
}

vector<_cohort*>& CRDFInstance::concFacePolygonsCohorts()
{
	return m_vecConcFacePolygonsCohorts;
}

vector<_cohort*>& CRDFInstance::linesCohorts()
{
	return m_vecLinesCohorts;
}

vector<_cohort*>& CRDFInstance::pointsCohorts()
{
	return m_vecPointsCohorts;
}

// ------------------------------------------------------------------------------------------------
vector<_cohort*>& CRDFInstance::normalVecsCohorts()
{
	return m_vecNormalVecsCohorts;
}

// ------------------------------------------------------------------------------------------------
vector<_cohort*>& CRDFInstance::biNormalVecsCohorts()
{
	return m_vecBiNormalVecsCohorts;
}

// ------------------------------------------------------------------------------------------------
vector<_cohort*>& CRDFInstance::tangentVecsCohorts()
{
	return m_vecTangentVecsCohorts;
}

// ------------------------------------------------------------------------------------------------
_vector3d * CRDFInstance::getBoundingBoxMin() const
{
	return m_vecBoundingBoxMin;
}

// ------------------------------------------------------------------------------------------------
_vector3d* CRDFInstance::getBoundingBoxMax() const
{
	return m_vecBoundingBoxMax;
}

// ------------------------------------------------------------------------------------------------
_matrix * CRDFInstance::getBoundingBoxTransformation() const
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
	if (m_pOriginalVertexBuffer->size() == 0)
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
			for (int64_t iIndex = m_vecTriangles[iTriangle].startIndex(); 
				iIndex < m_vecTriangles[iTriangle].startIndex() + m_vecTriangles[iTriangle].indicesCount();
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
	if (!m_vecFacePolygons.empty())
	{
		for (size_t iPolygon = 0; iPolygon < m_vecFacePolygons.size(); iPolygon++)
		{
			for (int64_t iIndex = m_vecFacePolygons[iPolygon].startIndex(); 
				iIndex < m_vecFacePolygons[iPolygon].startIndex() + m_vecFacePolygons[iPolygon].indicesCount();
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
	} // if (!m_vecFacePolygons.empty())

	/*
	* Conceptual faces polygons
	*/
	if (!m_vecConcFacePolygons.empty())
	{
		for (size_t iPolygon = 0; iPolygon < m_vecConcFacePolygons.size(); iPolygon++)
		{
			for (int64_t iIndex = m_vecConcFacePolygons[iPolygon].startIndex(); 
				iIndex < m_vecConcFacePolygons[iPolygon].startIndex() + m_vecConcFacePolygons[iPolygon].indicesCount(); 
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
	} // if (!m_vecConcFacePolygons.empty())

	/*
	* Lines
	*/
	if (!m_vecLines.empty())
	{
		for (size_t iPolygon = 0; iPolygon < m_vecLines.size(); iPolygon++)
		{
			for (int64_t iIndex = m_vecLines[iPolygon].startIndex();
				iIndex < m_vecLines[iPolygon].startIndex() + m_vecLines[iPolygon].indicesCount();
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
			for (int64_t iIndex = m_vecPoints[iPolygon].startIndex(); 
				iIndex < m_vecPoints[iPolygon].startIndex() + m_vecPoints[iPolygon].indicesCount();
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
	if (m_pOriginalVertexBuffer->size() == 0)
	{
		return;
	}

	delete[] m_pVertices;
	m_pVertices = new float[m_pOriginalVertexBuffer->size() * VERTEX_LENGTH];
	memcpy(m_pVertices, m_pOriginalVertexBuffer->data(), m_pOriginalVertexBuffer->size() * m_pOriginalVertexBuffer->vertexLength() * sizeof(float));

	/**
	* Vertices
	*/
	for (int_t iVertex = 0; iVertex < m_pOriginalVertexBuffer->size(); iVertex++)
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
	ASSERT(m_pOriginalVertexBuffer->size() > 0);

	/*
	* Faces Vertex Buffer
	*/
	float* pFacesVertices = new float[m_pOriginalVertexBuffer->size() * GEOMETRY_VBO_VERTEX_LENGTH];
	memset(pFacesVertices, 0, m_pOriginalVertexBuffer->size() * GEOMETRY_VBO_VERTEX_LENGTH * sizeof(float));

	for (int64_t iVertex = 0; iVertex < m_pOriginalVertexBuffer->size(); iVertex++)
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
	m_pOriginalVertexBuffer = new _vertices_f();

	delete[] m_pVertices;
	m_pVertices = NULL;

	delete m_pIndexBuffer;
	m_pIndexBuffer = new _indices_i32();

	/**
	* Calculate
	*/
	CalculateInstance(m_iInstance, &m_pOriginalVertexBuffer->size(), &m_pIndexBuffer->size(), NULL);

	/**
	* Retrieve the buffers
	*/
	if ((m_pOriginalVertexBuffer->size() > 0) && (m_pIndexBuffer->size()))
	{
		/**
		* Retrieves the vertices
		*/
		m_pOriginalVertexBuffer->vertexLength() = SetFormat(GetModel(), 0, 0) / sizeof(float);

		m_pOriginalVertexBuffer->data() = new float[m_pOriginalVertexBuffer->size() * m_pOriginalVertexBuffer->vertexLength()];
		memset(m_pOriginalVertexBuffer->data(), 0, m_pOriginalVertexBuffer->size() * m_pOriginalVertexBuffer->vertexLength() * sizeof(float));

		UpdateInstanceVertexBuffer(m_iInstance, m_pOriginalVertexBuffer->data());

		/**
		* Retrieves the indices
		*/
		m_pIndexBuffer->data() = new int32_t[m_pIndexBuffer->size()];
		memset(m_pIndexBuffer->data(), 0, m_pIndexBuffer->size() * sizeof(int32_t));

		UpdateInstanceIndexBuffer(m_iInstance, m_pIndexBuffer->data());
	} // if ((m_pOriginalVertexBuffer->m_iVerticesCount > 0) && ...	

	m_pVertices = new float[m_pOriginalVertexBuffer->size() * VERTEX_LENGTH];
	memcpy(m_pVertices, m_pOriginalVertexBuffer->data(), m_pOriginalVertexBuffer->size() * m_pOriginalVertexBuffer->vertexLength() * sizeof(float));
	
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
			int32_t iIndexValue = *(m_pIndexBuffer->data() + iStartIndexTriangles);
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
			_material material(
				iAmbientColor, 
				iDiffuseColor, 
				iEmissiveColor, 
				iSpecularColor, 
				fTransparency,
				bHasTexture ? L"default" : nullptr);

			MATERIALS::iterator itMaterial2ConceptualFaces = mapMaterial2ConceptualFaces.find(material);
			if (itMaterial2ConceptualFaces == mapMaterial2ConceptualFaces.end())
			{
				mapMaterial2ConceptualFaces[material] = vector<_face> { _face(iConceptualFace, iStartIndexTriangles, iIndicesCountTriangles) };
			}
			else
			{
				itMaterial2ConceptualFaces->second.push_back(_face(iConceptualFace, iStartIndexTriangles, iIndicesCountTriangles));
			}
		}

		if (iIndicesCountTriangles > 0)
		{
			m_vecTriangles.push_back(_primitives(iStartIndexTriangles, iIndicesCountTriangles));
		}

		if (iIndicesCountPolygonFaces > 0)
		{
			m_vecFacePolygons.push_back(_primitives(iStartIndexFacesPolygons, iIndicesCountPolygonFaces));
		}

		if (iConceptualFacePolygonsIndicesCount > 0)
		{
			m_vecConcFacePolygons.push_back(_primitives(iStartIndexConceptualFacePolygons, iConceptualFacePolygonsIndicesCount));
		}

		if (iLinesIndicesCount > 0)
		{
			m_vecLines.push_back(_primitives(iStartIndexLines, iLinesIndicesCount));
		}

		if (iPointsIndicesCount > 0)
		{
			m_vecPoints.push_back(_primitives(iStartIndexPoints, iPointsIndicesCount));
		}		
	} // for (int64_t iConceptualFace = ...

	/*
	* Group the faces
	*/
	MATERIALS::iterator itMaterial2ConcFaces = mapMaterial2ConceptualFaces.begin();
	for (; itMaterial2ConcFaces != mapMaterial2ConceptualFaces.end(); itMaterial2ConcFaces++)
	{
		_facesCohort* pCohort = nullptr;

		for (size_t iConcFace = 0; iConcFace < itMaterial2ConcFaces->second.size(); iConcFace++)
		{
			_face& concFace = itMaterial2ConcFaces->second[iConcFace];

			int_t iStartIndexTriangles = concFace.startIndex();
			int_t iIndicesCountTriangles = concFace.indicesCount();

			/*
			* Split the conceptual face - isolated case
			*/
			if (iIndicesCountTriangles > _openGLUtils::getIndicesCountLimit())
			{
				while (iIndicesCountTriangles > _openGLUtils::getIndicesCountLimit())
				{
					auto pNewCohort = new _facesCohort(itMaterial2ConcFaces->first);
					for (int_t iIndex = iStartIndexTriangles; 
						iIndex < iStartIndexTriangles + _openGLUtils::getIndicesCountLimit(); 
						iIndex++)
					{
						pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
					}

					concFacesCohorts().push_back(pNewCohort);

					/*
					* Update Conceptual face start index
					*/
					concFace.startIndex() = 0;

					// Conceptual faces
					pNewCohort->faces().push_back(concFace);

					iIndicesCountTriangles -= _openGLUtils::getIndicesCountLimit();
					iStartIndexTriangles += _openGLUtils::getIndicesCountLimit();
				}

				if (iIndicesCountTriangles > 0)
				{
					auto pNewCohort = new _facesCohort(itMaterial2ConcFaces->first);
					for (int_t iIndex = iStartIndexTriangles; 
						iIndex < iStartIndexTriangles + iIndicesCountTriangles; 
						iIndex++)
					{
						pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
					}

					concFacesCohorts().push_back(pNewCohort);

					/*
					* Update Conceptual face start index
					*/
					concFace.startIndex() = 0;

					// Conceptual faces
					pNewCohort->faces().push_back(concFace);
				}

				continue;
			} // if (iIndicesCountTriangles > _openGLUtils::GetIndicesCountLimit())	

			/*
			* Create material
			*/
			if (pCohort == nullptr)
			{
				pCohort = new _facesCohort(itMaterial2ConcFaces->first);

				concFacesCohorts().push_back(pCohort);
			}

			/*
			* Check the limit
			*/
			if (pCohort->indices().size() + iIndicesCountTriangles > _openGLUtils::getIndicesCountLimit())
			{
				pCohort = new _facesCohort(itMaterial2ConcFaces->first);

				concFacesCohorts().push_back(pCohort);
			}

			/*
			* Update Conceptual face start index
			*/
			concFace.startIndex() = pCohort->indices().size();

			/*
			* Add the indices
			*/
			for (int_t iIndex = iStartIndexTriangles; iIndex < iStartIndexTriangles + iIndicesCountTriangles; iIndex++)
			{
				pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
			}

			// Conceptual faces
			pCohort->faces().push_back(concFace);
		} // for (size_t iConcFace = ...
	} // for (; itMaterial2ConceptualFaces != ...

	/*
	* Group the polygons
	*/
	if (!m_vecConcFacePolygons.empty())
	{
		/*
		* Use the last cohort (if any)
		*/
		_cohort* pCohort = concFacePolygonsCohorts().empty() ? NULL : concFacePolygonsCohorts()[concFacePolygonsCohorts().size() - 1];

		/*
		* Create the cohort
		*/
		if (pCohort == NULL)
		{
			pCohort = new _cohort();
			concFacePolygonsCohorts().push_back(pCohort);
		}

		for (size_t iFace = 0; iFace < m_vecConcFacePolygons.size(); iFace++)
		{
			int_t iStartIndexFacesPolygons = m_vecConcFacePolygons[iFace].startIndex();
			int_t iIndicesFacesPolygonsCount = m_vecConcFacePolygons[iFace].indicesCount();

			/*
			* Split the conceptual face - isolated case
			*/
			if (iIndicesFacesPolygonsCount > _openGLUtils::getIndicesCountLimit() / 2)
			{
				while (iIndicesFacesPolygonsCount > _openGLUtils::getIndicesCountLimit() / 2)
				{
					pCohort = new _cohort();
					concFacePolygonsCohorts().push_back(pCohort);

					int_t iPreviousIndex = -1;
					for (int_t iIndex = iStartIndexFacesPolygons; iIndex < iStartIndexFacesPolygons + _openGLUtils::getIndicesCountLimit() / 2; iIndex++)
					{
						if (m_pIndexBuffer->data()[iIndex] < 0)
						{
							iPreviousIndex = -1;

							continue;
						}

						if (iPreviousIndex != -1)
						{
							pCohort->indices().push_back(m_pIndexBuffer->data()[iPreviousIndex]);
							pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
						} // if (iPreviousIndex != -1)

						iPreviousIndex = iIndex;
					} // for (int_t iIndex = ...

					iIndicesFacesPolygonsCount -= _openGLUtils::getIndicesCountLimit() / 2;
					iStartIndexFacesPolygons += _openGLUtils::getIndicesCountLimit() / 2;
				} // while (iIndicesFacesPolygonsCount > _openGLUtils::GetIndicesCountLimit() / 2)

				if (iIndicesFacesPolygonsCount > 0)
				{
					pCohort = new _cohort();
					concFacePolygonsCohorts().push_back(pCohort);

					int_t iPreviousIndex = -1;
					for (int_t iIndex = iStartIndexFacesPolygons; iIndex < iStartIndexFacesPolygons + iIndicesFacesPolygonsCount; iIndex++)
					{
						if (m_pIndexBuffer->data()[iIndex] < 0)
						{
							iPreviousIndex = -1;

							continue;
						}

						if (iPreviousIndex != -1)
						{
							pCohort->indices().push_back(m_pIndexBuffer->data()[iPreviousIndex]);
							pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
						} // if (iPreviousIndex != -1)

						iPreviousIndex = iIndex;
					} // for (int_t iIndex = ...
				}

				continue;
			} // if (iIndicesFacesPolygonsCount > _openGLUtils::GetIndicesCountLimit() / 2)

			/*
			* Check the limit
			*/
			if ((pCohort->indices().size() + (iIndicesFacesPolygonsCount * 2)) > _openGLUtils::getIndicesCountLimit())
			{
				pCohort = new _cohort();
				concFacePolygonsCohorts().push_back(pCohort);
			}

			int_t iPreviousIndex = -1;
			for (int_t iIndex = iStartIndexFacesPolygons; iIndex < iStartIndexFacesPolygons + iIndicesFacesPolygonsCount; iIndex++)
			{
				if (m_pIndexBuffer->data()[iIndex] < 0)
				{
					iPreviousIndex = -1;

					continue;
				}

				if (iPreviousIndex != -1)
				{
					pCohort->indices().push_back(m_pIndexBuffer->data()[iPreviousIndex]);
					pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
				} // if (iPreviousIndex != -1)

				iPreviousIndex = iIndex;
			} // for (int_t iIndex = ...
		} // for (size_t iFace = ...

#ifdef _DEBUG
		for (size_t iCohort = 0; iCohort < concFacesCohorts().size(); iCohort++)
		{
			ASSERT(concFacesCohorts()[iCohort]->indices().size() <= _openGLUtils::getIndicesCountLimit());
		}
#endif
	} // if (!m_vecConcFacePolygons.empty())

	/*
	* Group the polygons
	*/
	if (!m_vecFacePolygons.empty())
	{
		/*
		* Use the last cohort (if any)
		*/
		_cohort* pCohort = facePolygonsCohorts().empty() ? NULL : facePolygonsCohorts()[facePolygonsCohorts().size() - 1];

		/*
		* Create the cohort
		*/
		if (pCohort == NULL)
		{
			pCohort = new _cohort();
			facePolygonsCohorts().push_back(pCohort);
		}

		for (size_t iFace = 0; iFace < m_vecFacePolygons.size(); iFace++)
		{
			int_t iStartIndexFacesPolygons = m_vecFacePolygons[iFace].startIndex();
			int_t iIndicesFacesPolygonsCount = m_vecFacePolygons[iFace].indicesCount();

			/*
			* Split the conceptual face - isolated case
			*/
			if (iIndicesFacesPolygonsCount > _openGLUtils::getIndicesCountLimit() / 2)
			{
				while (iIndicesFacesPolygonsCount > _openGLUtils::getIndicesCountLimit() / 2)
				{
					pCohort = new _cohort();
					facePolygonsCohorts().push_back(pCohort);

					int_t iPreviousIndex = -1;
					for (int_t iIndex = iStartIndexFacesPolygons; iIndex < iStartIndexFacesPolygons + _openGLUtils::getIndicesCountLimit() / 2; iIndex++)
					{
						if (m_pIndexBuffer->data()[iIndex] < 0)
						{
							iPreviousIndex = -1;

							continue;
						}

						if (iPreviousIndex != -1)
						{
							pCohort->indices().push_back(m_pIndexBuffer->data()[iPreviousIndex]);
							pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
						} // if (iPreviousIndex != -1)

						iPreviousIndex = iIndex;
					} // for (int_t iIndex = ...

					iIndicesFacesPolygonsCount -= _openGLUtils::getIndicesCountLimit() / 2;
					iStartIndexFacesPolygons += _openGLUtils::getIndicesCountLimit() / 2;
				} // while (iIndicesFacesPolygonsCount > _openGLUtils::GetIndicesCountLimit() / 2)

				if (iIndicesFacesPolygonsCount > 0)
				{
					pCohort = new _cohort();
					facePolygonsCohorts().push_back(pCohort);

					int_t iPreviousIndex = -1;
					for (int_t iIndex = iStartIndexFacesPolygons; iIndex < iStartIndexFacesPolygons + iIndicesFacesPolygonsCount; iIndex++)
					{
						if (m_pIndexBuffer->data()[iIndex] < 0)
						{
							iPreviousIndex = -1;

							continue;
						}

						if (iPreviousIndex != -1)
						{
							pCohort->indices().push_back(m_pIndexBuffer->data()[iPreviousIndex]);
							pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
						} // if (iPreviousIndex != -1)

						iPreviousIndex = iIndex;
					} // for (int_t iIndex = ...
				}

				continue;
			} // if (iIndicesFacesPolygonsCount > _openGLUtils::GetIndicesCountLimit() / 2)

			/*
			* Check the limit
			*/
			if ((pCohort->indices().size() + (iIndicesFacesPolygonsCount * 2)) > _openGLUtils::getIndicesCountLimit())
			{
				pCohort = new _cohort();
				facePolygonsCohorts().push_back(pCohort);
			}

			int_t iPreviousIndex = -1;
			for (int_t iIndex = iStartIndexFacesPolygons; iIndex < iStartIndexFacesPolygons + iIndicesFacesPolygonsCount; iIndex++)
			{
				if (m_pIndexBuffer->data()[iIndex] < 0)
				{
					iPreviousIndex = -1;

					continue;
				}

				if (iPreviousIndex != -1)
				{
					pCohort->indices().push_back(m_pIndexBuffer->data()[iPreviousIndex]);
					pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
				} // if (iPreviousIndex != -1)

				iPreviousIndex = iIndex;
			} // for (int_t iIndex = ...				
		} // for (size_t iFace = ...

#ifdef _DEBUG
		for (size_t iCohort = 0; iCohort < facePolygonsCohorts().size(); iCohort++)
		{
			ASSERT(facePolygonsCohorts()[iCohort]->indices().size() <= _openGLUtils::getIndicesCountLimit());
		}
#endif
	} // if (!m_vecFacePolygons.empty())

	/*
	* Group the lines
	*/
	if (!m_vecLines.empty())
	{
		/*
		* Use the last cohort (if any)
		*/
		auto pCohort = linesCohorts().empty() ? nullptr : linesCohorts()[linesCohorts().size() - 1];

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
			int_t iStartIndexLines = m_vecLines[iFace].startIndex();
			int_t iIndicesLinesCount = m_vecLines[iFace].indicesCount();

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
				if (m_pIndexBuffer->data()[iIndex] < 0)
				{
					continue;
				}

				pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
			} // for (int_t iIndex = ...
		} // for (size_t iFace = ...

#ifdef _DEBUG
		for (size_t iCohort = 0; iCohort < linesCohorts().size(); iCohort++)
		{
			ASSERT(linesCohorts()[iCohort]->indices().size() <= _openGLUtils::getIndicesCountLimit());
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
			int_t iStartIndexPoints = m_vecPoints[iFace].startIndex();
			int_t iIndicesPointsCount = m_vecPoints[iFace].indicesCount();

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
				pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
			}
		} // for (size_t iFace = ...

#ifdef _DEBUG
		for (size_t iCohort = 0; iCohort < pointsCohorts().size(); iCohort++)
		{
			ASSERT(pointsCohorts()[iCohort]->indices().size() <= _openGLUtils::getIndicesCountLimit());
		}
#endif
	} // if (!m_vecPoints.empty())			

	/*
	* Bounding box
	*/
	if (m_mtxBoundingBoxTransformation == NULL)
	{
		m_mtxBoundingBoxTransformation = new _matrix();
		m_vecBoundingBoxMin = new _vector3d();
		m_vecBoundingBoxMax = new _vector3d();

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
	m_vecFacePolygons.clear();
	m_vecConcFacePolygons.clear();

	_cohort::clear(m_vecConcFacesCohorts);
	_cohort::clear(m_vecFacePolygonsCohorts);
	_cohort::clear(m_vecConcFacePolygonsCohorts);
	_cohort::clear(m_vecLinesCohorts);
	_cohort::clear(m_vecPointsCohorts);		
	_cohort::clear(m_vecNormalVecsCohorts);
	_cohort::clear(m_vecBiNormalVecsCohorts);
	_cohort::clear(m_vecTangentVecsCohorts);
}
