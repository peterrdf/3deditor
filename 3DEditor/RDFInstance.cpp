#include "stdafx.h"
#include "RDFInstance.h"
#include "Generic.h"

#include <memory>

// ************************************************************************************************
CRDFInstance::CRDFInstance(int64_t iID, OwlInstance iInstance, bool bEnable)
	: _instance(iID, iInstance, bEnable)	
	, m_pOriginalVertexBuffer(nullptr)	
	, m_iVBO(0)
	, m_iVBOOffset(0)
{
	UpdateName();

	Calculate();
}

/*virtual*/ CRDFInstance::~CRDFInstance()
{
	Clean();
}

/*virtual*/ void CRDFInstance::setEnable(bool bEnable) /*override*/
{
	_instance::setEnable(bEnable);

	if (m_bEnable && m_bNeedsRefresh)
	{
		m_bNeedsRefresh = false;

		Recalculate();
	}
}

void CRDFInstance::UpdateName()
{
	OwlClass iClassInstance = GetInstanceClass(m_iInstance);
	ASSERT(iClassInstance != 0);

	wchar_t* szClassName = nullptr;
	GetNameOfClassW(iClassInstance, &szClassName);

	wchar_t* szName = nullptr;
	GetNameOfInstanceW(m_iInstance, &szName);

	if (szName == nullptr)
	{
		RdfProperty iTagProperty = GetPropertyByName(getModel(), "tag");
		if (iTagProperty != 0)
		{
			SetCharacterSerialization(getModel(), 0, 0, false);

			int64_t iCard = 0;
			wchar_t** szValue = nullptr;
			GetDatatypeProperty(m_iInstance, iTagProperty, (void**)&szValue, &iCard);

			if (iCard == 1)
			{
				szName = szValue[0];
			}

			SetCharacterSerialization(getModel(), 0, 0, true);
		}
	} // if (szName == nullptr)

	wchar_t szUniqueName[512];

	if (szName != nullptr)
	{
		m_strName = szName;
		swprintf(szUniqueName, 512, L"%s (%s)", szName, szClassName);
	}
	else
	{
		m_strName = szClassName;
		swprintf(szUniqueName, 512, L"#%lld (%s)", m_iInstance, szClassName);
	}

	m_strUniqueName = szUniqueName;
}

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

void CRDFInstance::ResetVertexBuffers()
{
	if (getVerticesCount() == 0)
	{
		return;
	}

	// Vertices
	ASSERT(m_pVertexBuffer != nullptr);
	ASSERT(m_pOriginalVertexBuffer != nullptr);
	ASSERT(m_pVertexBuffer->size() == m_pOriginalVertexBuffer->size());
	ASSERT(m_pVertexBuffer->vertexLength() == m_pOriginalVertexBuffer->vertexLength());
	memcpy(m_pVertexBuffer->data(), m_pOriginalVertexBuffer->data(), m_pOriginalVertexBuffer->size() * m_pOriginalVertexBuffer->vertexLength() * sizeof(float));

	// Bounding box
	memcpy(m_pmtxBBTransformation, m_pmtxOriginalBBTransformation, sizeof(_matrix));
	memcpy(m_pvecBBMin, m_pvecOriginalBBMin, sizeof(_vector3d));
	memcpy(m_pvecBBMax, m_pvecOriginalBBMax, sizeof(_vector3d));
}

void CRDFInstance::CalculateMinMax(
	float& fXmin, float& fXmax, 
	float& fYmin, float& fYmax, 
	float& fZmin, float& fZmax)
{
	if (getVerticesCount() == 0)
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

/*static*/ void CRDFInstance::CalculateBBMinMax(
	OwlInstance iInstance,
	double& dXmin, double& dXmax,
	double& dYmin, double& dYmax,
	double& dZmin, double& dZmax)
{
	ASSERT(iInstance != 0);

	_vector3d vecOriginalBBMin;
	_vector3d vecOriginalBBMax;
	if (GetInstanceGeometryClass(iInstance) &&
		GetBoundingBox(
		iInstance,
		(double*)&vecOriginalBBMin,
		(double*)&vecOriginalBBMax))
	{
		dXmin = fmin(dXmin, vecOriginalBBMin.x);
		dYmin = fmin(dYmin, vecOriginalBBMin.y);
		dZmin = fmin(dZmin, vecOriginalBBMin.z);

		dXmax = fmax(dXmax, vecOriginalBBMax.x);
		dYmax = fmax(dYmax, vecOriginalBBMax.y);
		dZmax = fmax(dZmax, vecOriginalBBMax.z);
	}
}

void CRDFInstance::Calculate()
{
	ASSERT(m_pOriginalVertexBuffer == nullptr);
	m_pOriginalVertexBuffer = new _vertices_f();

	ASSERT(m_pVertexBuffer == nullptr);

	ASSERT(m_pIndexBuffer == nullptr);
	m_pIndexBuffer = new _indices_i32();

	/* BBs/AABBs */

	ASSERT(m_pmtxOriginalBBTransformation == nullptr);
	m_pmtxOriginalBBTransformation = new _matrix();

	ASSERT(m_pvecOriginalBBMin == nullptr);
	m_pvecOriginalBBMin = new _vector3d();

	ASSERT(m_pvecOriginalBBMax == nullptr);
	m_pvecOriginalBBMax = new _vector3d();

	ASSERT(m_pmtxBBTransformation == nullptr);
	m_pmtxBBTransformation = new _matrix();

	ASSERT(m_pvecBBMin == nullptr);
	m_pvecBBMin = new _vector3d();

	ASSERT(m_pvecBBMax == nullptr);
	m_pvecBBMax = new _vector3d();

	ASSERT(m_pvecAABBMin == nullptr);
	m_pvecAABBMin = new _vector3d();

	ASSERT(m_pvecAABBMax == nullptr);
	m_pvecAABBMax = new _vector3d();

	GetBoundingBox(
		m_iInstance,
		(double*)m_pmtxOriginalBBTransformation,
		(double*)m_pvecOriginalBBMin,
		(double*)m_pvecOriginalBBMax);

	memcpy(m_pmtxBBTransformation, m_pmtxOriginalBBTransformation, sizeof(_matrix));
	memcpy(m_pvecBBMin, m_pvecOriginalBBMin, sizeof(_vector3d));
	memcpy(m_pvecBBMax, m_pvecOriginalBBMax, sizeof(_vector3d));

	if (!GetBoundingBox(
		m_iInstance,
		(double*)m_pvecAABBMin,
		(double*)m_pvecAABBMax))
	{
		return;
	}

	// Calculate
	CalculateInstance(m_iInstance, &m_pOriginalVertexBuffer->size(), &m_pIndexBuffer->size(), nullptr);

	// Retrieve the buffers
	if ((m_pOriginalVertexBuffer->size() == 0) || (m_pIndexBuffer->size() == 0))
	{
		return;
	}

	// Retrieves the vertices
	m_pOriginalVertexBuffer->vertexLength() = SetFormat(getModel(), 0, 0) / sizeof(float);

	m_pOriginalVertexBuffer->data() = new float[m_pOriginalVertexBuffer->size() * m_pOriginalVertexBuffer->vertexLength()];
	memset(m_pOriginalVertexBuffer->data(), 0, m_pOriginalVertexBuffer->size() * m_pOriginalVertexBuffer->vertexLength() * sizeof(float));

	UpdateInstanceVertexBuffer(m_iInstance, m_pOriginalVertexBuffer->data());

	// Retrieves the indices
	m_pIndexBuffer->data() = new int32_t[m_pIndexBuffer->size()];
	memset(m_pIndexBuffer->data(), 0, m_pIndexBuffer->size() * sizeof(int32_t));

	UpdateInstanceIndexBuffer(m_iInstance, m_pIndexBuffer->data());

	m_pVertexBuffer = new _vertices_f();
	m_pVertexBuffer->size() = m_pOriginalVertexBuffer->size();
	m_pVertexBuffer->vertexLength() = m_pOriginalVertexBuffer->vertexLength();
	m_pVertexBuffer->data() = new float[m_pOriginalVertexBuffer->size() * m_pOriginalVertexBuffer->vertexLength()];
	memcpy(m_pVertexBuffer->data(), m_pOriginalVertexBuffer->data(), m_pOriginalVertexBuffer->size() * m_pOriginalVertexBuffer->vertexLength() * sizeof(float));

	MATERIALS mapMaterial2ConcFaces; // MATERIAL : FACE INDEX, START INDEX, INIDCES COUNT, etc.
	MATERIALS mapMaterial2ConcFaceLines; // MATERIAL : FACE INDEX, START INDEX, INIDCES COUNT, etc.
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
				GetPropertyByName(getModel(), "textures"),
				&piInstances,
				&iCard);

			if (iCard == 1)
			{
				iCard = 0;
				char** szValue = nullptr;
				GetDatatypeProperty(
					piInstances[0],
					GetPropertyByName(getModel(), "name"),
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

			float fColor = *(m_pVertexBuffer->data() + iIndexValue + 8);
			unsigned int iAmbientColor = *(reinterpret_cast<unsigned int*>(&fColor));
			float fTransparency = (float)(iAmbientColor & (255)) / (float)255;

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 9);
			unsigned int iDiffuseColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 10);
			unsigned int iEmissiveColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 11);
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
				mapMaterial2ConcFaces[material] = vector<_face>{ _face(iConceptualFace, iStartIndexTriangles, iIndicesCountTriangles) };
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
			int32_t iIndexValue = *(m_pIndexBuffer->data() + iStartIndexLines);
			iIndexValue *= VERTEX_LENGTH;

			float fColor = *(m_pVertexBuffer->data() + iIndexValue + 8);
			unsigned int iAmbientColor = *(reinterpret_cast<unsigned int*>(&fColor));
			float fTransparency = (float)(iAmbientColor & (255)) / (float)255;

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 9);
			unsigned int iDiffuseColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 10);
			unsigned int iEmissiveColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 11);
			unsigned int iSpecularColor = *(reinterpret_cast<unsigned int*>(&fColor));

			// Material
			_material material(
				iAmbientColor,
				iDiffuseColor,
				iEmissiveColor,
				iSpecularColor,
				fTransparency,
				!strTexture.empty() ? strTexture.c_str() : nullptr);

			auto itMaterial2ConcFaceLines = mapMaterial2ConcFaceLines.find(material);
			if (itMaterial2ConcFaceLines == mapMaterial2ConcFaceLines.end())
			{
				mapMaterial2ConcFaceLines[material] = vector<_face>{ _face(iConceptualFace, iStartIndexLines, iIndicesCountLines) };
			}
			else
			{
				itMaterial2ConcFaceLines->second.push_back(_face(iConceptualFace, iStartIndexLines, iIndicesCountLines));
			}

			m_vecLines.push_back(_primitives(iStartIndexLines, iIndicesCountLines));
		}

		if (iIndicesCountPoints > 0)
		{
			int32_t iIndexValue = *(m_pIndexBuffer->data() + iStartIndexPoints);
			iIndexValue *= VERTEX_LENGTH;

			float fColor = *(m_pVertexBuffer->data() + iIndexValue + 8);
			unsigned int iAmbientColor = *(reinterpret_cast<unsigned int*>(&fColor));
			float fTransparency = (float)(iAmbientColor & (255)) / (float)255;

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 9);
			unsigned int iDiffuseColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 10);
			unsigned int iEmissiveColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 11);
			unsigned int iSpecularColor = *(reinterpret_cast<unsigned int*>(&fColor));

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

	// Group the faces
	auto itMaterial2ConcFaces = mapMaterial2ConcFaces.begin();
	for (; itMaterial2ConcFaces != mapMaterial2ConcFaces.end(); itMaterial2ConcFaces++)
	{
		_cohortWithMaterial* pCohort = nullptr;

		for (size_t iConcFace = 0; iConcFace < itMaterial2ConcFaces->second.size(); iConcFace++)
		{
			_face& concFace = itMaterial2ConcFaces->second[iConcFace];

			int64_t iStartIndex = concFace.startIndex();
			int64_t iIndicesCount = concFace.indicesCount();

			// Split the conceptual face - isolated case
			if (iIndicesCount > _oglUtils::getIndicesCountLimit())
			{
				while (iIndicesCount > _oglUtils::getIndicesCountLimit())
				{
					auto pNewCohort = new _cohortWithMaterial(itMaterial2ConcFaces->first);
					for (int64_t iIndex = iStartIndex;
						iIndex < iStartIndex + _oglUtils::getIndicesCountLimit();
						iIndex++)
					{
						pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
					}

					concFacesCohorts().push_back(pNewCohort);

					// Update Conceptual face start index
					concFace.startIndex() = 0;

					// Conceptual faces
					pNewCohort->faces().push_back(concFace);

					iIndicesCount -= _oglUtils::getIndicesCountLimit();
					iStartIndex += _oglUtils::getIndicesCountLimit();
				}

				if (iIndicesCount > 0)
				{
					auto pNewCohort = new _cohortWithMaterial(itMaterial2ConcFaces->first);
					for (int64_t iIndex = iStartIndex;
						iIndex < iStartIndex + iIndicesCount;
						iIndex++)
					{
						pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
					}

					concFacesCohorts().push_back(pNewCohort);

					// Update Conceptual face start index
					concFace.startIndex() = 0;

					// Conceptual faces
					pNewCohort->faces().push_back(concFace);
				}

				continue;
			} // if (iIndicesCount > _oglUtils::GetIndicesCountLimit())	

			// Create material
			if (pCohort == nullptr)
			{
				pCohort = new _cohortWithMaterial(itMaterial2ConcFaces->first);

				concFacesCohorts().push_back(pCohort);
			}

			// Check the limit
			if (pCohort->indices().size() + iIndicesCount > _oglUtils::getIndicesCountLimit())
			{
				pCohort = new _cohortWithMaterial(itMaterial2ConcFaces->first);

				concFacesCohorts().push_back(pCohort);
			}

			// Update Conceptual face start index
			concFace.startIndex() = pCohort->indices().size();

			// Add the indices
			for (int64_t iIndex = iStartIndex;
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
		// Use the last cohort (if any)
		_cohort* pCohort = facePolygonsCohorts().empty() ? nullptr : facePolygonsCohorts()[facePolygonsCohorts().size() - 1];

		// Create the cohort
		if (pCohort == nullptr)
		{
			pCohort = new _cohort();
			facePolygonsCohorts().push_back(pCohort);
		}

		for (size_t iFace = 0; iFace < m_vecFacePolygons.size(); iFace++)
		{
			int64_t iStartIndex = m_vecFacePolygons[iFace].startIndex();
			int64_t iIndicesCount = m_vecFacePolygons[iFace].indicesCount();

			// Split the conceptual face - isolated case
			if (iIndicesCount > _oglUtils::getIndicesCountLimit() / 2)
			{
				while (iIndicesCount > _oglUtils::getIndicesCountLimit() / 2)
				{
					pCohort = new _cohort();
					facePolygonsCohorts().push_back(pCohort);

					for (int64_t iIndex = iStartIndex;
						iIndex < iStartIndex + _oglUtils::getIndicesCountLimit() / 2;
						iIndex += 2)
					{
						pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
						pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
					}

					iIndicesCount -= _oglUtils::getIndicesCountLimit() / 2;
					iStartIndex += _oglUtils::getIndicesCountLimit() / 2;
				} // while (iIndicesCount > _oglUtils::GetIndicesCountLimit() / 2)

				if (iIndicesCount > 0)
				{
					pCohort = new _cohort();
					facePolygonsCohorts().push_back(pCohort);

					for (int64_t iIndex = iStartIndex;
						iIndex < iStartIndex + iIndicesCount;
						iIndex += 2)
					{
						pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
						pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
					}
				}

				continue;
			} // if (iIndicesCount > _oglUtils::GetIndicesCountLimit() / 2)

			// Check the limit
			if ((pCohort->indices().size() + (iIndicesCount * 2)) > _oglUtils::getIndicesCountLimit())
			{
				pCohort = new _cohort();
				facePolygonsCohorts().push_back(pCohort);
			}

			for (int64_t iIndex = iStartIndex;
				iIndex < iStartIndex + iIndicesCount;
				iIndex += 2)
			{
				pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
				pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
			}			
		} // for (size_t iFace = ...
	} // if (!m_vecFacePolygons.empty())

	// Group the polygons
	if (!m_vecConcFacePolygons.empty())
	{
		// Use the last cohort (if any)
		_cohort* pCohort = concFacePolygonsCohorts().empty() ? nullptr : concFacePolygonsCohorts()[concFacePolygonsCohorts().size() - 1];

		// Create the cohort
		if (pCohort == nullptr)
		{
			pCohort = new _cohort();
			concFacePolygonsCohorts().push_back(pCohort);
		}

		for (size_t iFace = 0; iFace < m_vecConcFacePolygons.size(); iFace++)
		{
			int64_t iStartIndex = m_vecConcFacePolygons[iFace].startIndex();
			int64_t iIndicesCount = m_vecConcFacePolygons[iFace].indicesCount();

			// Split the conceptual face - isolated case
			if (iIndicesCount > _oglUtils::getIndicesCountLimit() / 2)
			{
				while (iIndicesCount > _oglUtils::getIndicesCountLimit() / 2)
				{
					pCohort = new _cohort();
					concFacePolygonsCohorts().push_back(pCohort);

					for (int64_t iIndex = iStartIndex;
						iIndex < iStartIndex + _oglUtils::getIndicesCountLimit() / 2;
						iIndex += 2)
					{
						pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
						pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
					}

					iIndicesCount -= _oglUtils::getIndicesCountLimit() / 2;
					iStartIndex += _oglUtils::getIndicesCountLimit() / 2;
				} // while (iIndicesCount > _oglUtils::GetIndicesCountLimit() / 2)

				if (iIndicesCount > 0)
				{
					pCohort = new _cohort();
					concFacePolygonsCohorts().push_back(pCohort);

					for (int64_t iIndex = iStartIndex;
						iIndex < iStartIndex + iIndicesCount;
						iIndex += 2)
					{
						pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
						pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
					}
				}

				continue;
			} // if (iIndicesCount > _oglUtils::GetIndicesCountLimit() / 2)

			// Check the limit
			if ((pCohort->indices().size() + (iIndicesCount * 2)) > _oglUtils::getIndicesCountLimit())
			{
				pCohort = new _cohort();
				concFacePolygonsCohorts().push_back(pCohort);
			}

			for (int64_t iIndex = iStartIndex;
				iIndex < iStartIndex + iIndicesCount;
				iIndex += 2)
			{
				pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
				pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
			}
		} // for (size_t iFace = ...
	} // if (!m_vecConcFacePolygons.empty())

	// Group the lines
	auto itMaterial2ConcFaceLines = mapMaterial2ConcFaceLines.begin();
	for (; itMaterial2ConcFaceLines != mapMaterial2ConcFaceLines.end(); itMaterial2ConcFaceLines++)
	{
		_cohortWithMaterial* pCohort = nullptr;

		for (size_t iConcFace = 0; iConcFace < itMaterial2ConcFaceLines->second.size(); iConcFace++)
		{
			_face& concFace = itMaterial2ConcFaceLines->second[iConcFace];

			int64_t iStartIndex = concFace.startIndex();
			int64_t iIndicesCount = concFace.indicesCount();

			// Split the conceptual face - isolated case
			if (iIndicesCount > _oglUtils::getIndicesCountLimit())
			{
				while (iIndicesCount > _oglUtils::getIndicesCountLimit())
				{
					auto pNewCohort = new _cohortWithMaterial(itMaterial2ConcFaceLines->first);
					for (int64_t iIndex = iStartIndex;
						iIndex < iStartIndex + _oglUtils::getIndicesCountLimit();
						iIndex++)
					{
						pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
					}

					linesCohorts().push_back(pNewCohort);

					// Update Conceptual face start index
					concFace.startIndex() = 0;

					// Conceptual faces
					pNewCohort->faces().push_back(concFace);

					iIndicesCount -= _oglUtils::getIndicesCountLimit();
					iStartIndex += _oglUtils::getIndicesCountLimit();
				}

				if (iIndicesCount > 0)
				{
					auto pNewCohort = new _cohortWithMaterial(itMaterial2ConcFaceLines->first);
					for (int64_t iIndex = iStartIndex;
						iIndex < iStartIndex + iIndicesCount;
						iIndex++)
					{
						pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
					}

					linesCohorts().push_back(pNewCohort);

					// Update Conceptual face start index
					concFace.startIndex() = 0;

					// Conceptual faces
					pNewCohort->faces().push_back(concFace);
				}

				continue;
			} // if (iIndicesCountTriangles > _oglUtils::GetIndicesCountLimit())	

			// Create material
			if (pCohort == nullptr)
			{
				pCohort = new _cohortWithMaterial(itMaterial2ConcFaceLines->first);

				linesCohorts().push_back(pCohort);
			}

			// Check the limit
			if (pCohort->indices().size() + iIndicesCount > _oglUtils::getIndicesCountLimit())
			{
				pCohort = new _cohortWithMaterial(itMaterial2ConcFaceLines->first);

				linesCohorts().push_back(pCohort);
			}

			// Update Conceptual face start index
			concFace.startIndex() = pCohort->indices().size();

			// Add the indices
			for (int64_t iIndex = iStartIndex;
				iIndex < iStartIndex + iIndicesCount;
				iIndex++)
			{
				pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
			}

			// Conceptual faces
			pCohort->faces().push_back(concFace);
		} // for (size_t iConcFace = ...
	} // for (; itMaterial2ConceptualFaces != ...

	// Group the points
	auto itMaterial2ConcFacePoints = mapMaterial2ConcFacePoints.begin();
	for (; itMaterial2ConcFacePoints != mapMaterial2ConcFacePoints.end(); itMaterial2ConcFacePoints++)
	{
		_cohortWithMaterial* pCohort = nullptr;

		for (size_t iConcFace = 0; iConcFace < itMaterial2ConcFacePoints->second.size(); iConcFace++)
		{
			_face& concFace = itMaterial2ConcFacePoints->second[iConcFace];

			int64_t iStartIndex = concFace.startIndex();
			int64_t iIndicesCount = concFace.indicesCount();

			// Split the conceptual face - isolated case
			if (iIndicesCount > _oglUtils::getIndicesCountLimit())
			{
				while (iIndicesCount > _oglUtils::getIndicesCountLimit())
				{
					auto pNewCohort = new _cohortWithMaterial(itMaterial2ConcFacePoints->first);
					for (int64_t iIndex = iStartIndex;
						iIndex < iStartIndex + _oglUtils::getIndicesCountLimit();
						iIndex++)
					{
						pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
					}

					pointsCohorts().push_back(pNewCohort);

					// Update Conceptual face start index
					concFace.startIndex() = 0;

					// Conceptual faces
					pNewCohort->faces().push_back(concFace);

					iIndicesCount -= _oglUtils::getIndicesCountLimit();
					iStartIndex += _oglUtils::getIndicesCountLimit();
				}

				if (iIndicesCount > 0)
				{
					auto pNewCohort = new _cohortWithMaterial(itMaterial2ConcFacePoints->first);
					for (int64_t iIndex = iStartIndex;
						iIndex < iStartIndex + iIndicesCount;
						iIndex++)
					{
						pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
					}

					pointsCohorts().push_back(pNewCohort);

					// Update Conceptual face start index
					concFace.startIndex() = 0;

					// Conceptual faces
					pNewCohort->faces().push_back(concFace);
				}

				continue;
			} // if (iIndicesCountTriangles > _oglUtils::GetIndicesCountLimit())	

			// Create material
			if (pCohort == nullptr)
			{
				pCohort = new _cohortWithMaterial(itMaterial2ConcFacePoints->first);

				pointsCohorts().push_back(pCohort);
			}

			// Check the limit
			if (pCohort->indices().size() + iIndicesCount > _oglUtils::getIndicesCountLimit())
			{
				pCohort = new _cohortWithMaterial(itMaterial2ConcFacePoints->first);

				pointsCohorts().push_back(pCohort);
			}

			// Update Conceptual face start index
			concFace.startIndex() = pCohort->indices().size();

			// Add the indices
			for (int64_t iIndex = iStartIndex;
				iIndex < iStartIndex + iIndicesCount;
				iIndex++)
			{
				pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
			}

			// Conceptual faces
			pCohort->faces().push_back(concFace);
		} // for (size_t iConcFace = ...
	} // for (; itMaterial2ConceptualFaces != ...
}

void CRDFInstance::Clean()
{
	_instance::clean();

	delete m_pOriginalVertexBuffer;
	m_pOriginalVertexBuffer = nullptr;
}
