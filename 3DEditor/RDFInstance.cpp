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
	, m_pVertices(nullptr)	
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
	, m_pvecOriginalBoundingBoxMin(nullptr)
	, m_pvecOriginalBoundingBoxMax(nullptr)
	, m_pmtxOriginalBoundingBoxTransformation(nullptr)
	, m_pvecAABBBoundingBoxMin(nullptr)
	, m_pvecAABBBoundingBoxMax(nullptr)
	, m_pvecBoundingBoxMin(nullptr)
	, m_pvecBoundingBoxMax(nullptr)
	, m_pmtxBoundingBoxTransformation(nullptr)
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

	if (szName != nullptr)
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
#else
    m_strName = wxString(szName).wchar_str();

    wchar_t szUniqueName[200];
	swprintf(szUniqueName, 200, L"%lld (%ls)", m_pInstance->GetInstance(), wxString(szClassName).wc_str());

	m_strUniqueName = szUniqueName;
#endif // _LINUX

	Calculate();
}

// ------------------------------------------------------------------------------------------------
CRDFInstance::~CRDFInstance()
{
	Clean();	
	
	delete m_pvecOriginalBoundingBoxMin;
	delete m_pvecOriginalBoundingBoxMax;
	delete m_pmtxOriginalBoundingBoxTransformation;

	delete m_pvecAABBBoundingBoxMin;
	delete m_pvecAABBBoundingBoxMax;
	
	delete m_pvecBoundingBoxMin;
	delete m_pvecBoundingBoxMax;
	delete m_pmtxBoundingBoxTransformation;
}

// ------------------------------------------------------------------------------------------------
void CRDFInstance::Recalculate(bool bForceUpdate)
{	
	if (!bForceUpdate && !m_bEnable)
	{
		// Reloading on demand
		m_bNeedsRefresh = true;

		return;
	}

	Clean();

	Calculate();
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFInstance::GetID() const
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
int64_t CRDFInstance::GetInstance() const
{
	return m_iInstance;
}

// ------------------------------------------------------------------------------------------------
int64_t CRDFInstance::GetClassInstance() const
{
	int64_t iClassInstance = GetInstanceClass(m_iInstance);
	ASSERT(iClassInstance != 0);

	return iClassInstance;
}

// ------------------------------------------------------------------------------------------------
const wchar_t* CRDFInstance::GetName() const
{
	return m_strName.c_str();
}

// ------------------------------------------------------------------------------------------------
const wchar_t* CRDFInstance::GetUniqueName() const
{
	return m_strUniqueName.c_str();
}

// ------------------------------------------------------------------------------------------------
bool CRDFInstance::IsReferenced() const
{
	return GetInstanceInverseReferencesByIterator(m_iInstance, 0);
}

// ------------------------------------------------------------------------------------------------
bool CRDFInstance::HasGeometry() const
{
	return (m_pOriginalVertexBuffer->size() > 0) && (m_pIndexBuffer->size() > 0);
}

// ------------------------------------------------------------------------------------------------
int32_t * CRDFInstance::GetIndices() const
{
	return m_pIndexBuffer->data();
}

int64_t CRDFInstance::GetIndicesCount() const
{
	return m_pIndexBuffer->size();
}

float * CRDFInstance::GetVertices() const
{
	return m_pVertices;
}

float* CRDFInstance::GetOriginalVertices() const
{
	return m_pOriginalVertexBuffer != nullptr ? m_pOriginalVertexBuffer->data() : nullptr;
}

int64_t CRDFInstance::GetVerticesCount() const
{
	return m_pOriginalVertexBuffer->size();
}

int64_t CRDFInstance::GetVertexLength() const 
{ 
	return VERTEX_LENGTH; 
}

int64_t CRDFInstance::GetConceptualFacesCount() const
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

vector<_facesCohort*>& CRDFInstance::pointsCohorts()
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
	return m_pvecBoundingBoxMin;
}

// ------------------------------------------------------------------------------------------------
_vector3d* CRDFInstance::getBoundingBoxMax() const
{
	return m_pvecBoundingBoxMax;
}

// ------------------------------------------------------------------------------------------------
_matrix * CRDFInstance::getBoundingBoxTransformation() const
{
	return m_pmtxBoundingBoxTransformation;
}

// ------------------------------------------------------------------------------------------------
_vector3d* CRDFInstance::getOriginalBoundingBoxMin() const
{
	return m_pvecOriginalBoundingBoxMin;
}

// ------------------------------------------------------------------------------------------------
_vector3d* CRDFInstance::getOriginalBoundingBoxMax() const
{
	return m_pvecOriginalBoundingBoxMax;
}

// ------------------------------------------------------------------------------------------------
_vector3d* CRDFInstance::getAABBBoundingBoxMin() const
{
	return m_pvecAABBBoundingBoxMin;
}

// ------------------------------------------------------------------------------------------------
_vector3d* CRDFInstance::getAABBBoundingBoxMax() const
{
	return m_pvecAABBBoundingBoxMax;
}

// ------------------------------------------------------------------------------------------------
void CRDFInstance::setEnable(bool bEnable)
{
	m_bEnable = bEnable;

	if (m_bEnable && m_bNeedsRefresh)
	{
		m_bNeedsRefresh = false;

		Recalculate(false);
	}
}

// ------------------------------------------------------------------------------------------------
bool CRDFInstance::getEnable() const
{
	return m_bEnable;
}

// ------------------------------------------------------------------------------------------------
void CRDFInstance::ResetScaleAndCenter()
{
	if (GetVerticesCount() == 0)
	{
		return;
	}
	
	// Vertices
	delete[] m_pVertices;
	m_pVertices = new float[m_pOriginalVertexBuffer->size() * VERTEX_LENGTH];
	memcpy(m_pVertices, m_pOriginalVertexBuffer->data(), m_pOriginalVertexBuffer->size() * m_pOriginalVertexBuffer->vertexLength() * sizeof(float));

	// Bounding box
	memcpy(m_pvecBoundingBoxMin, m_pvecOriginalBoundingBoxMin, sizeof(_vector3d));
	memcpy(m_pvecBoundingBoxMax, m_pvecOriginalBoundingBoxMax, sizeof(_vector3d));
	memcpy(m_pmtxBoundingBoxTransformation, m_pmtxOriginalBoundingBoxTransformation, sizeof(_matrix));

	// Apply the Transformation
	_transform(m_pvecBoundingBoxMin, m_pmtxBoundingBoxTransformation, m_pvecBoundingBoxMin);
	_transform(m_pvecBoundingBoxMax, m_pmtxBoundingBoxTransformation, m_pvecBoundingBoxMax);
}

// ------------------------------------------------------------------------------------------------
void CRDFInstance::CalculateMinMax(
	float& fXmin, float& fXmax, 
	float& fYmin, float& fYmax, 
	float& fZmin, float& fZmax)
{
	if (GetVerticesCount() == 0)
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
				fXmin = (float)fmin(fXmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH)]);
				fXmax = (float)fmax(fXmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH)]);
				fYmin = (float)fmin(fYmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fYmax = (float)fmax(fYmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fZmin = (float)fmin(fZmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);
				fZmax = (float)fmax(fZmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);
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
				if ((GetIndices()[iIndex] == -1) || (GetIndices()[iIndex] == -2))
				{
					continue;
				}

				fXmin = (float)fmin(fXmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH)]);
				fXmax = (float)fmax(fXmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH)]);
				fYmin = (float)fmin(fYmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fYmax = (float)fmax(fYmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fZmin = (float)fmin(fZmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);
				fZmax = (float)fmax(fZmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);
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
				if ((GetIndices()[iIndex] == -1) || (GetIndices()[iIndex] == -2))
				{
					continue;
				}

				fXmin = (float)fmin(fXmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH)]);
				fXmax = (float)fmax(fXmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH)]);
				fYmin = (float)fmin(fYmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fYmax = (float)fmax(fYmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fZmin = (float)fmin(fZmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);
				fZmax = (float)fmax(fZmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);
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
				if (GetIndices()[iIndex] == -1)
				{
					continue;
				}

				fXmin = (float)fmin(fXmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH)]);
				fXmax = (float)fmax(fXmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH)]);
				fYmin = (float)fmin(fYmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fYmax = (float)fmax(fYmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fZmin = (float)fmin(fZmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);
				fZmax = (float)fmax(fZmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);
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
				fXmin = (float)fmin(fXmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH)]);
				fXmax = (float)fmax(fXmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH)]);
				fYmin = (float)fmin(fYmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fYmax = (float)fmax(fYmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				fZmin = (float)fmin(fZmin, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);
				fZmax = (float)fmax(fZmax, GetVertices()[(GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);
			} // for (size_t iIndex = ...
		} // for (size_t iPolygon = ...
	} // if (!m_vecPoints.empty())
}

// ------------------------------------------------------------------------------------------------
void CRDFInstance::ScaleAndCenter(
	float fXmin, float fXmax, 
	float fYmin, float fYmax, 
	float fZmin, float fZmax, 
	float fResoltuion,
	bool bScale)
{
	if (GetVerticesCount() == 0)
	{
		return;
	}	

	/** Vertices */	

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
		if (bScale)
		{
			m_pVertices[(iVertex * VERTEX_LENGTH)] = m_pVertices[(iVertex * VERTEX_LENGTH)] / (fResoltuion / 2.0f);
			m_pVertices[(iVertex * VERTEX_LENGTH) + 1] = m_pVertices[(iVertex * VERTEX_LENGTH) + 1] / (fResoltuion / 2.0f);
			m_pVertices[(iVertex * VERTEX_LENGTH) + 2] = m_pVertices[(iVertex * VERTEX_LENGTH) + 2] / (fResoltuion / 2.0f);
		}		
	}

	/** Bounding box - Min */	

	// [0.0 -> X/Y/Zmin + X/Y/Zmax]
	m_pvecBoundingBoxMin->x = m_pvecBoundingBoxMin->x - fXmin;
	m_pvecBoundingBoxMin->y = m_pvecBoundingBoxMin->y - fYmin;
	m_pvecBoundingBoxMin->z = m_pvecBoundingBoxMin->z - fZmin;

	// center
	m_pvecBoundingBoxMin->x = m_pvecBoundingBoxMin->x - ((fXmax - fXmin) / 2.0f);
	m_pvecBoundingBoxMin->y = m_pvecBoundingBoxMin->y - ((fYmax - fYmin) / 2.0f);
	m_pvecBoundingBoxMin->z = m_pvecBoundingBoxMin->z - ((fZmax - fZmin) / 2.0f);

	// [-1.0 -> 1.0]
	if (bScale)
	{
		m_pvecBoundingBoxMin->x = m_pvecBoundingBoxMin->x / (fResoltuion / 2.0f);
		m_pvecBoundingBoxMin->y = m_pvecBoundingBoxMin->y / (fResoltuion / 2.0f);
		m_pvecBoundingBoxMin->z = m_pvecBoundingBoxMin->z / (fResoltuion / 2.0f);
	}	

	/**
	* Bounding box - Max
	*/

	// [0.0 -> X/Y/Zmin + X/Y/Zmax]
	m_pvecBoundingBoxMax->x = m_pvecBoundingBoxMax->x - fXmin;
	m_pvecBoundingBoxMax->y = m_pvecBoundingBoxMax->y - fYmin;
	m_pvecBoundingBoxMax->z = m_pvecBoundingBoxMax->z - fZmin;

	// center
	m_pvecBoundingBoxMax->x = m_pvecBoundingBoxMax->x - ((fXmax - fXmin) / 2.0f);
	m_pvecBoundingBoxMax->y = m_pvecBoundingBoxMax->y - ((fYmax - fYmin) / 2.0f);
	m_pvecBoundingBoxMax->z = m_pvecBoundingBoxMax->z - ((fZmax - fZmin) / 2.0f);

	// [-1.0 -> 1.0]
	if (bScale)
	{
		m_pvecBoundingBoxMax->x = m_pvecBoundingBoxMax->x / (fResoltuion / 2.0f);
		m_pvecBoundingBoxMax->y = m_pvecBoundingBoxMax->y / (fResoltuion / 2.0f);
		m_pvecBoundingBoxMax->z = m_pvecBoundingBoxMax->z / (fResoltuion / 2.0f);
	}	

	//bbref!!!
	///**
	//* Bounding box - Transformation
	//*/

	//// [-1.0 -> 1.0]
	//if (bScale)
	//{
	//	m_pmtxBoundingBoxTransformation->_41 /= (fResoltuion / 2.0f);
	//	m_pmtxBoundingBoxTransformation->_42 /= (fResoltuion / 2.0f);
	//	m_pmtxBoundingBoxTransformation->_43 /= (fResoltuion / 2.0f);
	//}	
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
	m_pVertices = nullptr;

	delete m_pIndexBuffer;
	m_pIndexBuffer = new _indices_i32();

	/**
	* Calculate
	*/
	CalculateInstance(m_iInstance, &m_pOriginalVertexBuffer->size(), &m_pIndexBuffer->size(), nullptr);

	/**
	* Retrieve the buffers
	*/
	if ((m_pOriginalVertexBuffer->size() == 0) || (m_pIndexBuffer->size() == 0))
	{
		return;
	}

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

	m_pVertices = new float[m_pOriginalVertexBuffer->size() * VERTEX_LENGTH];
	memcpy(m_pVertices, m_pOriginalVertexBuffer->data(), m_pOriginalVertexBuffer->size() * m_pOriginalVertexBuffer->vertexLength() * sizeof(float));	
	
	MATERIALS mapMaterial2ConcFaces; // MATERIAL : FACE INDEX, START INDEX, INIDCES COUNT, etc.
	MATERIALS mapMaterial2ConcFacePoints; // MATERIAL : FACE INDEX, START INDEX, INIDCES COUNT, etc.

	m_iConceptualFacesCount = GetConceptualFaceCnt(m_iInstance);
	for (int64_t iConceptualFace = 0; iConceptualFace < m_iConceptualFacesCount; iConceptualFace++)
	{
		int64_t iStartIndexTriangles = 0;
		int64_t iIndicesCountTriangles = 0;
		int64_t iStartIndexLines = 0;
		int64_t iIndicesCountLines = 0;
		int64_t iStartIndexPoints = 0;
		int64_t iIndicesCountPoints = 0;
		int64_t iStartIndexFacePolygons = 0;
		int64_t iIndicesCountFacePolygons = 0;
		int64_t iStartIndexConceptualFacePolygons = 0;
		int64_t iIndicesCountConceptualFacePolygons = 0;

		ConceptualFace conceptualFace = GetConceptualFace(m_iInstance, iConceptualFace,
			&iStartIndexTriangles, &iIndicesCountTriangles,
			&iStartIndexLines, &iIndicesCountLines,
			&iStartIndexPoints, &iIndicesCountPoints,
			&iStartIndexFacePolygons, &iIndicesCountFacePolygons,
			&iStartIndexConceptualFacePolygons, &iIndicesCountConceptualFacePolygons);

		// Check for Texture
		wstring strTexture;
		OwlInstance iMaterialInstance = GetConceptualFaceMaterial(conceptualFace);
		if (iMaterialInstance != 0)
		{
			int64_t* piInstances = nullptr;
			int64_t iCard = 0;
			GetObjectProperty(
				iMaterialInstance, 
				GetPropertyByName(GetModel(), "textures"),
				&piInstances, 
				&iCard);

			if (iCard == 1)
			{
				iCard = 0;
				char** szValue = nullptr;
				GetDatatypeProperty(
					piInstances[0],
					GetPropertyByName(GetModel(), "name"),
					(void**)&szValue, 
					&iCard);

				ASSERT(iCard >= 0);

				if (iCard == 1)
				{
					strTexture = CA2W(szValue[0]);
				}

				if (strTexture.empty())
				{
					strTexture = L"default";
				}
			} // if (iCard == 1)
		} // if (iMaterialInstance != 0)
		
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
			* Material
			*/
			_material material(
				iAmbientColor, 
				iDiffuseColor, 
				iEmissiveColor, 
				iSpecularColor, 
				fTransparency,
				!strTexture.empty() ? strTexture.c_str() : nullptr);

			MATERIALS::iterator itMaterial2ConcFaces = mapMaterial2ConcFaces.find(material);
			if (itMaterial2ConcFaces == mapMaterial2ConcFaces.end())
			{
				mapMaterial2ConcFaces[material] = vector<_face> { _face(iConceptualFace, iStartIndexTriangles, iIndicesCountTriangles) };
			}
			else
			{
				itMaterial2ConcFaces->second.push_back(_face(iConceptualFace, iStartIndexTriangles, iIndicesCountTriangles));
			}

			m_vecTriangles.push_back(_primitives(iStartIndexTriangles, iIndicesCountTriangles));
		} // if (iIndicesCountTriangles > 0)

		if (iIndicesCountFacePolygons > 0)
		{
			m_vecFacePolygons.push_back(_primitives(iStartIndexFacePolygons, iIndicesCountFacePolygons));
		}

		if (iIndicesCountConceptualFacePolygons > 0)
		{
			m_vecConcFacePolygons.push_back(_primitives(iStartIndexConceptualFacePolygons, iIndicesCountConceptualFacePolygons));
		}

		if (iIndicesCountLines > 0)
		{
			m_vecLines.push_back(_primitives(iStartIndexLines, iIndicesCountLines));
		}

		if (iIndicesCountPoints > 0)
		{
			int32_t iIndexValue = *(m_pIndexBuffer->data() + iStartIndexPoints);
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
			* Material
			*/
			_material material(
				iAmbientColor,
				iDiffuseColor,
				iEmissiveColor,
				iSpecularColor,
				fTransparency,
				!strTexture.empty() ? strTexture.c_str() : nullptr);

			auto itMaterial2ConcFacePoints = mapMaterial2ConcFacePoints.find(material);
			if (itMaterial2ConcFacePoints == mapMaterial2ConcFacePoints.end())
			{
				mapMaterial2ConcFacePoints[material] = vector<_face>{ _face(iConceptualFace, iStartIndexPoints, iIndicesCountPoints) };
			}
			else
			{
				itMaterial2ConcFacePoints->second.push_back(_face(iConceptualFace, iStartIndexPoints, iIndicesCountPoints));
			}

			m_vecPoints.push_back(_primitives(iStartIndexPoints, iIndicesCountPoints));
		} // if (iIndicesCountPoints > 0)		
	} // for (int64_t iConceptualFace = ...

	/*
	* Group the faces
	*/
	auto itMaterial2ConcFaces = mapMaterial2ConcFaces.begin();
	for (; itMaterial2ConcFaces != mapMaterial2ConcFaces.end(); itMaterial2ConcFaces++)
	{
		_facesCohort* pCohort = nullptr;

		for (size_t iConcFace = 0; iConcFace < itMaterial2ConcFaces->second.size(); iConcFace++)
		{
			_face& concFace = itMaterial2ConcFaces->second[iConcFace];

			int_t iStartIndex = concFace.startIndex();
			int_t iIndicesCount = concFace.indicesCount();

			/*
			* Split the conceptual face - isolated case
			*/
			if (iIndicesCount > _oglUtils::getIndicesCountLimit())
			{
				while (iIndicesCount > _oglUtils::getIndicesCountLimit())
				{
					auto pNewCohort = new _facesCohort(itMaterial2ConcFaces->first);
					for (int_t iIndex = iStartIndex;
						iIndex < iStartIndex + _oglUtils::getIndicesCountLimit();
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

					iIndicesCount -= _oglUtils::getIndicesCountLimit();
					iStartIndex += _oglUtils::getIndicesCountLimit();
				}

				if (iIndicesCount > 0)
				{
					auto pNewCohort = new _facesCohort(itMaterial2ConcFaces->first);
					for (int_t iIndex = iStartIndex;
						iIndex < iStartIndex + iIndicesCount;
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
			} // if (iIndicesCount > _oglUtils::GetIndicesCountLimit())	

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
			if (pCohort->indices().size() + iIndicesCount > _oglUtils::getIndicesCountLimit())
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
			for (int_t iIndex = iStartIndex; 
				iIndex < iStartIndex + iIndicesCount;
				iIndex++)
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
	if (!m_vecFacePolygons.empty())
	{
		/*
		* Use the last cohort (if any)
		*/
		_cohort* pCohort = facePolygonsCohorts().empty() ? nullptr : facePolygonsCohorts()[facePolygonsCohorts().size() - 1];

		/*
		* Create the cohort
		*/
		if (pCohort == nullptr)
		{
			pCohort = new _cohort();
			facePolygonsCohorts().push_back(pCohort);
		}

		for (size_t iFace = 0; iFace < m_vecFacePolygons.size(); iFace++)
		{
			int_t iStartIndex = m_vecFacePolygons[iFace].startIndex();
			int_t iIndicesCount = m_vecFacePolygons[iFace].indicesCount();

			/*
			* Split the conceptual face - isolated case
			*/
			if (iIndicesCount > _oglUtils::getIndicesCountLimit() / 2)
			{
				while (iIndicesCount > _oglUtils::getIndicesCountLimit() / 2)
				{
					pCohort = new _cohort();
					facePolygonsCohorts().push_back(pCohort);

					int_t iPreviousIndex = -1;
					for (int_t iIndex = iStartIndex;
						iIndex < iStartIndex + _oglUtils::getIndicesCountLimit() / 2;
						iIndex++)
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

					iIndicesCount -= _oglUtils::getIndicesCountLimit() / 2;
					iStartIndex += _oglUtils::getIndicesCountLimit() / 2;
				} // while (iIndicesCount > _oglUtils::GetIndicesCountLimit() / 2)

				if (iIndicesCount > 0)
				{
					pCohort = new _cohort();
					facePolygonsCohorts().push_back(pCohort);

					int_t iPreviousIndex = -1;
					for (int_t iIndex = iStartIndex;
						iIndex < iStartIndex + iIndicesCount;
						iIndex++)
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
			} // if (iIndicesCount > _oglUtils::GetIndicesCountLimit() / 2)

			/*
			* Check the limit
			*/
			if ((pCohort->indices().size() + (iIndicesCount * 2)) > _oglUtils::getIndicesCountLimit())
			{
				pCohort = new _cohort();
				facePolygonsCohorts().push_back(pCohort);
			}

			int_t iPreviousIndex = -1;
			for (int_t iIndex = iStartIndex;
				iIndex < iStartIndex + iIndicesCount;
				iIndex++)
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
			ASSERT(facePolygonsCohorts()[iCohort]->indices().size() <= _oglUtils::getIndicesCountLimit());
		}
#endif
	} // if (!m_vecFacePolygons.empty())

	/*
	* Group the polygons
	*/
	if (!m_vecConcFacePolygons.empty())
	{
		/*
		* Use the last cohort (if any)
		*/
		_cohort* pCohort = concFacePolygonsCohorts().empty() ? nullptr : concFacePolygonsCohorts()[concFacePolygonsCohorts().size() - 1];

		/*
		* Create the cohort
		*/
		if (pCohort == nullptr)
		{
			pCohort = new _cohort();
			concFacePolygonsCohorts().push_back(pCohort);
		}

		for (size_t iFace = 0; iFace < m_vecConcFacePolygons.size(); iFace++)
		{
			int_t iStartIndex = m_vecConcFacePolygons[iFace].startIndex();
			int_t iIndicesCount = m_vecConcFacePolygons[iFace].indicesCount();

			/*
			* Split the conceptual face - isolated case
			*/
			if (iIndicesCount > _oglUtils::getIndicesCountLimit() / 2)
			{
				while (iIndicesCount > _oglUtils::getIndicesCountLimit() / 2)
				{
					pCohort = new _cohort();
					concFacePolygonsCohorts().push_back(pCohort);

					int_t iPreviousIndex = -1;
					for (int_t iIndex = iStartIndex;
						iIndex < iStartIndex + _oglUtils::getIndicesCountLimit() / 2;
						iIndex++)
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

					iIndicesCount -= _oglUtils::getIndicesCountLimit() / 2;
					iStartIndex += _oglUtils::getIndicesCountLimit() / 2;
				} // while (iIndicesCount > _oglUtils::GetIndicesCountLimit() / 2)

				if (iIndicesCount > 0)
				{
					pCohort = new _cohort();
					concFacePolygonsCohorts().push_back(pCohort);

					int_t iPreviousIndex = -1;
					for (int_t iIndex = iStartIndex;
						iIndex < iStartIndex + iIndicesCount;
						iIndex++)
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
			} // if (iIndicesCount > _oglUtils::GetIndicesCountLimit() / 2)

			/*
			* Check the limit
			*/
			if ((pCohort->indices().size() + (iIndicesCount * 2)) > _oglUtils::getIndicesCountLimit())
			{
				pCohort = new _cohort();
				concFacePolygonsCohorts().push_back(pCohort);
			}

			int_t iPreviousIndex = -1;
			for (int_t iIndex = iStartIndex;
				iIndex < iStartIndex + iIndicesCount;
				iIndex++)
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
		for (size_t iCohort = 0; iCohort < concFacePolygonsCohorts().size(); iCohort++)
		{
			ASSERT(concFacePolygonsCohorts()[iCohort]->indices().size() <= _oglUtils::getIndicesCountLimit());
		}
#endif
	} // if (!m_vecConcFacePolygons.empty())

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
		if (pCohort == nullptr)
		{
			pCohort = new _cohort();
			linesCohorts().push_back(pCohort);
		}

		for (size_t iFace = 0; iFace < m_vecLines.size(); iFace++)
		{
			int_t iStartIndex = m_vecLines[iFace].startIndex();
			int_t iIndicesCount = m_vecLines[iFace].indicesCount();

			/*
			* Check the limit
			*/
			if (pCohort->indices().size() + iIndicesCount > _oglUtils::getIndicesCountLimit())
			{
				pCohort = new _cohort();
				linesCohorts().push_back(pCohort);
			}

			for (int_t iIndex = iStartIndex; 
				iIndex < iStartIndex + iIndicesCount;
				iIndex++)
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
			ASSERT(linesCohorts()[iCohort]->indices().size() <= _oglUtils::getIndicesCountLimit());
		}
#endif
	} // if (!m_vecLines.empty())		

	/*
	* Group the points
	*/
	auto itMaterial2ConcFacePoints = mapMaterial2ConcFacePoints.begin();
	for (; itMaterial2ConcFacePoints != mapMaterial2ConcFacePoints.end(); itMaterial2ConcFacePoints++)
	{
		_facesCohort* pCohort = nullptr;

		for (size_t iConcFace = 0; iConcFace < itMaterial2ConcFacePoints->second.size(); iConcFace++)
		{
			_face& concFace = itMaterial2ConcFacePoints->second[iConcFace];

			int_t iStartIndex = concFace.startIndex();
			int_t iIndicesCount = concFace.indicesCount();

			/*
			* Split the conceptual face - isolated case
			*/
			if (iIndicesCount > _oglUtils::getIndicesCountLimit())
			{
				while (iIndicesCount > _oglUtils::getIndicesCountLimit())
				{
					auto pNewCohort = new _facesCohort(itMaterial2ConcFacePoints->first);
					for (int_t iIndex = iStartIndex;
						iIndex < iStartIndex + _oglUtils::getIndicesCountLimit();
						iIndex++)
					{
						pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
					}

					pointsCohorts().push_back(pNewCohort);

					/*
					* Update Conceptual face start index
					*/
					concFace.startIndex() = 0;

					// Conceptual faces
					pNewCohort->faces().push_back(concFace);

					iIndicesCount -= _oglUtils::getIndicesCountLimit();
					iStartIndex += _oglUtils::getIndicesCountLimit();
				}

				if (iIndicesCount > 0)
				{
					auto pNewCohort = new _facesCohort(itMaterial2ConcFacePoints->first);
					for (int_t iIndex = iStartIndex;
						iIndex < iStartIndex + iIndicesCount;
						iIndex++)
					{
						pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
					}

					pointsCohorts().push_back(pNewCohort);

					/*
					* Update Conceptual face start index
					*/
					concFace.startIndex() = 0;

					// Conceptual faces
					pNewCohort->faces().push_back(concFace);
				}

				continue;
			} // if (iIndicesCountTriangles > _oglUtils::GetIndicesCountLimit())	

			/*
			* Create material
			*/
			if (pCohort == nullptr)
			{
				pCohort = new _facesCohort(itMaterial2ConcFacePoints->first);

				pointsCohorts().push_back(pCohort);
			}

			/*
			* Check the limit
			*/
			if (pCohort->indices().size() + iIndicesCount > _oglUtils::getIndicesCountLimit())
			{
				pCohort = new _facesCohort(itMaterial2ConcFacePoints->first);

				pointsCohorts().push_back(pCohort);
			}

			/*
			* Update Conceptual face start index
			*/
			concFace.startIndex() = pCohort->indices().size();

			/*
			* Add the indices
			*/
			for (int_t iIndex = iStartIndex; 
				iIndex < iStartIndex + iIndicesCount;
				iIndex++)
			{
				pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
			}

			// Conceptual faces
			pCohort->faces().push_back(concFace);
		} // for (size_t iConcFace = ...
	} // for (; itMaterial2ConceptualFaces != ...
		
	/** Bounding box */	
	if (m_pmtxOriginalBoundingBoxTransformation == nullptr)
	{
		ASSERT(m_pmtxBoundingBoxTransformation == nullptr);
		ASSERT((m_pvecOriginalBoundingBoxMin == nullptr) && (m_pvecOriginalBoundingBoxMax == nullptr));
		ASSERT((m_pvecAABBBoundingBoxMin == nullptr) && (m_pvecAABBBoundingBoxMax == nullptr));

		m_pmtxOriginalBoundingBoxTransformation = new _matrix();
		m_pvecOriginalBoundingBoxMin = new _vector3d();
		m_pvecOriginalBoundingBoxMax = new _vector3d();		

		//bbref!!!
		/*SetBoundingBoxReference(
			m_iInstance, (double*)
			m_pmtxOriginalBoundingBoxTransformation, (double*)
			m_pvecOriginalBoundingBoxMin, (double*)
			m_pvecOriginalBoundingBoxMax);	*/

		// Object BB
		GetBoundingBox(
			m_iInstance,
			(double*)m_pmtxOriginalBoundingBoxTransformation,
			(double*)m_pvecOriginalBoundingBoxMin,
			(double*)m_pvecOriginalBoundingBoxMax);

		// Copy BB //bbref!!!
		m_pmtxBoundingBoxTransformation = new _matrix();
		m_pvecBoundingBoxMin = new _vector3d();
		m_pvecBoundingBoxMax = new _vector3d();

		memcpy(m_pmtxBoundingBoxTransformation, m_pmtxOriginalBoundingBoxTransformation, sizeof(_matrix));
		memcpy(m_pvecBoundingBoxMin, m_pvecOriginalBoundingBoxMin, sizeof(_vector3d));
		memcpy(m_pvecBoundingBoxMax, m_pvecOriginalBoundingBoxMax, sizeof(_vector3d));

		// Apply the Transformation
		_transform(m_pvecBoundingBoxMin, m_pmtxBoundingBoxTransformation, m_pvecBoundingBoxMin);
		_transform(m_pvecBoundingBoxMax, m_pmtxBoundingBoxTransformation, m_pvecBoundingBoxMax);

		// AABB
		m_pvecAABBBoundingBoxMin = new _vector3d();
		m_pvecAABBBoundingBoxMax = new _vector3d();
		GetBoundingBox(
			m_iInstance,
			(double*)m_pvecAABBBoundingBoxMin,
			(double*)m_pvecAABBBoundingBoxMax);
	} // if (m_pmtxOriginalBoundingBoxTransformation == nullptr)
}

// ------------------------------------------------------------------------------------------------
void CRDFInstance::Clean()
{
	delete m_pOriginalVertexBuffer;
	m_pOriginalVertexBuffer = nullptr;

	delete[] m_pVertices;
	m_pVertices = nullptr;

	delete m_pIndexBuffer;
	m_pIndexBuffer = nullptr;	

	m_vecTriangles.clear();
	m_vecFacePolygons.clear();
	m_vecConcFacePolygons.clear();
	m_vecLines.clear();
	m_vecPoints.clear();	

	_cohort::clear(m_vecConcFacesCohorts);
	_cohort::clear(m_vecFacePolygonsCohorts);
	_cohort::clear(m_vecConcFacePolygonsCohorts);
	_cohort::clear(m_vecLinesCohorts);
	_cohort::clear(m_vecPointsCohorts);		
	_cohort::clear(m_vecNormalVecsCohorts);
	_cohort::clear(m_vecBiNormalVecsCohorts);
	_cohort::clear(m_vecTangentVecsCohorts);
}
