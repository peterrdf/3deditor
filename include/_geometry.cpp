#include "_host.h"
#include "_geometry.h"
#include "_instance.h"
#include "_oglUtils.h"

// ************************************************************************************************
/*static*/ unsigned int* _cohort::merge(const vector<_cohort*>& vecCohorts, uint32_t& iIndicesCount)
{
	iIndicesCount = 0;
	for (size_t iCohort = 0; iCohort < vecCohorts.size(); iCohort++)
	{
		iIndicesCount += (uint32_t)vecCohorts[iCohort]->indices().size();
	}

	unsigned int* pIndices = new unsigned int[iIndicesCount];

	uint32_t iOffset = 0;
	for (size_t iCohort = 0; iCohort < vecCohorts.size(); iCohort++)
	{
		if (vecCohorts[iCohort]->indices().size() == 0)
		{
			continue;
		}

		memcpy((unsigned int*)pIndices + iOffset, vecCohorts[iCohort]->indices().data(),
			vecCohorts[iCohort]->indices().size() * sizeof(unsigned int));

		iOffset += (uint32_t)vecCohorts[iCohort]->indices().size();
	}

	return pIndices;
}

// ************************************************************************************************
_geometry::_geometry(OwlInstance owlInstance)
	: m_owlInstance(owlInstance)
	, m_strName(L"NA")
	, m_strUniqueName(L"")
	, m_pVertexBuffer(nullptr)
	, m_pIndexBuffer(nullptr)
	, m_iConceptualFacesCount(0)
	, m_bShow(true)
	, m_pmtxOriginalBBTransformation(nullptr)
	, m_pvecOriginalBBMin(nullptr)
	, m_pvecOriginalBBMax(nullptr)
	, m_pmtxBBTransformation(nullptr)
	, m_pvecBBMin(nullptr)
	, m_pvecBBMax(nullptr)
	, m_pvecAABBMin(nullptr)
	, m_pvecAABBMax(nullptr)
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
	, m_vecInstances()
	, m_iVBO(0)
	, m_iVBOOffset(0)
{
}

/*virtual*/ _geometry::~_geometry()
{
	clean();
}

/*static*/ void _geometry::calculateBBMinMax(
	OwlInstance iInstance,
	double& dXmin, double& dXmax,
	double& dYmin, double& dYmax,
	double& dZmin, double& dZmax)
{
	assert(iInstance != 0);

	_vector3d vecBBMin;
	_vector3d vecBBMax;
	if (GetInstanceGeometryClass(iInstance) &&
		GetBoundingBox(
			iInstance,
			(double*)&vecBBMin,
			(double*)&vecBBMax))
	{
		dXmin = fmin(dXmin, vecBBMin.x);
		dYmin = fmin(dYmin, vecBBMin.y);
		dZmin = fmin(dZmin, vecBBMin.z);

		dXmax = fmax(dXmax, vecBBMax.x);
		dYmax = fmax(dYmax, vecBBMax.y);
		dZmax = fmax(dZmax, vecBBMax.z);
	}
}

void _geometry::calculateMinMax(
	float& fXmin, float& fXmax,
	float& fYmin, float& fYmax,
	float& fZmin, float& fZmax)
{
	if (getVerticesCount() == 0)
	{
		return;
	}

	const auto iVertexLength = getVertexLength();

	// Triangles
	if (!m_vecTriangles.empty())
	{
		for (size_t iTriangle = 0; iTriangle < m_vecTriangles.size(); iTriangle++)
		{
			for (int64_t iIndex = m_vecTriangles[iTriangle].startIndex();
				iIndex < m_vecTriangles[iTriangle].startIndex() + m_vecTriangles[iTriangle].indicesCount();
				iIndex++)
			{
				fXmin = (float)fmin(fXmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 0]);
				fXmax = (float)fmax(fXmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 0]);
				fYmin = (float)fmin(fYmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 1]);
				fYmax = (float)fmax(fYmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 1]);
				fZmin = (float)fmin(fZmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 2]);
				fZmax = (float)fmax(fZmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 2]);
			} // for (size_t iIndex = ...
		} // for (size_t iTriangle = ...
	} // if (!m_vecTriangles.empty())

	// Faces polygons
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

				fXmin = (float)fmin(fXmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 0]);
				fXmax = (float)fmax(fXmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 0]);
				fYmin = (float)fmin(fYmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 1]);
				fYmax = (float)fmax(fYmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 1]);
				fZmin = (float)fmin(fZmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 2]);
				fZmax = (float)fmax(fZmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 2]);
			} // for (size_t iIndex = ...
		} // for (size_t iPolygon = ...
	} // if (!m_vecFacePolygons.empty())

	// Conceptual faces polygons
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

				fXmin = (float)fmin(fXmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 0]);
				fXmax = (float)fmax(fXmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 0]);
				fYmin = (float)fmin(fYmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 1]);
				fYmax = (float)fmax(fYmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 1]);
				fZmin = (float)fmin(fZmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 2]);
				fZmax = (float)fmax(fZmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 2]);
			} // for (size_t iIndex = ...
		} // for (size_t iPolygon = ...
	} // if (!m_vecConcFacePolygons.empty())

	// Lines
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

				fXmin = (float)fmin(fXmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 0]);
				fXmax = (float)fmax(fXmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 0]);
				fYmin = (float)fmin(fYmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 1]);
				fYmax = (float)fmax(fYmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 1]);
				fZmin = (float)fmin(fZmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 2]);
				fZmax = (float)fmax(fZmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 2]);
			} // for (size_t iIndex = ...
		} // for (size_t iPolygon = ...
	} // if (!m_vecLines.empty())

	// Points
	if (!m_vecPoints.empty())
	{
		for (size_t iPolygon = 0; iPolygon < m_vecPoints.size(); iPolygon++)
		{
			for (int64_t iIndex = m_vecPoints[iPolygon].startIndex();
				iIndex < m_vecPoints[iPolygon].startIndex() + m_vecPoints[iPolygon].indicesCount();
				iIndex++)
			{
				fXmin = (float)fmin(fXmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 0]);
				fXmax = (float)fmax(fXmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 0]);
				fYmin = (float)fmin(fYmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 1]);
				fYmax = (float)fmax(fYmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 1]);
				fZmin = (float)fmin(fZmin, getVertices()[(getIndices()[iIndex] * iVertexLength) + 2]);
				fZmax = (float)fmax(fZmax, getVertices()[(getIndices()[iIndex] * iVertexLength) + 2]);
			} // for (size_t iIndex = ...
		} // for (size_t iPolygon = ...
	} // if (!m_vecPoints.empty())
}

void _geometry::calculateMinMaxTransform(
	const _matrix4x4* pTransformationMatrix,
	float& fXmin, float& fXmax,
	float& fYmin, float& fYmax,
	float& fZmin, float& fZmax)
{
	if (!hasGeometry())
	{
		return;
	}

	const uint32_t VERTEX_LENGTH = getVertexLength();

	// Triangles
	if (!m_vecTriangles.empty())
	{
		for (size_t iTriangle = 0; iTriangle < m_vecTriangles.size(); iTriangle++)
		{
			for (int64_t iIndex = m_vecTriangles[iTriangle].startIndex();
				iIndex < m_vecTriangles[iTriangle].startIndex() + m_vecTriangles[iTriangle].indicesCount();
				iIndex++)
			{
				_vector3 vecPoint =
				{
					m_pVertexBuffer->data()[(m_pIndexBuffer->data()[iIndex] * VERTEX_LENGTH) + 0],
					m_pVertexBuffer->data()[(m_pIndexBuffer->data()[iIndex] * VERTEX_LENGTH) + 1],
					m_pVertexBuffer->data()[(m_pIndexBuffer->data()[iIndex] * VERTEX_LENGTH) + 2]
				};

				if (pTransformationMatrix != nullptr)
				{
					_transform(&vecPoint, pTransformationMatrix, &vecPoint);
				}

				fXmin = (float)fmin(fXmin, vecPoint.x);
				fXmax = (float)fmax(fXmax, vecPoint.x);
				fYmin = (float)fmin(fYmin, vecPoint.y);
				fYmax = (float)fmax(fYmax, vecPoint.y);
				fZmin = (float)fmin(fZmin, vecPoint.z);
				fZmax = (float)fmax(fZmax, vecPoint.z);
			} // for (size_t iIndex = ...
		} // for (size_t iTriangle = ...
	} // if (!m_vecTriangles.empty())	

	// Conceptual faces polygons
	if (!m_vecConcFacePolygons.empty())
	{
		for (size_t iPolygon = 0; iPolygon < m_vecConcFacePolygons.size(); iPolygon++)
		{
			for (int64_t iIndex = m_vecConcFacePolygons[iPolygon].startIndex();
				iIndex < m_vecConcFacePolygons[iPolygon].startIndex() + m_vecConcFacePolygons[iPolygon].indicesCount();
				iIndex++)
			{
				if ((m_pIndexBuffer->data()[iIndex] == -1) || (m_pIndexBuffer->data()[iIndex] == -2))
				{
					continue;
				}

				_vector3 vecPoint =
				{
					m_pVertexBuffer->data()[(m_pIndexBuffer->data()[iIndex] * VERTEX_LENGTH) + 0],
					m_pVertexBuffer->data()[(m_pIndexBuffer->data()[iIndex] * VERTEX_LENGTH) + 1],
					m_pVertexBuffer->data()[(m_pIndexBuffer->data()[iIndex] * VERTEX_LENGTH) + 2]
				};

				if (pTransformationMatrix != nullptr)
				{
					_transform(&vecPoint, pTransformationMatrix, &vecPoint);
				}

				fXmin = (float)fmin(fXmin, vecPoint.x);
				fXmax = (float)fmax(fXmax, vecPoint.x);
				fYmin = (float)fmin(fYmin, vecPoint.y);
				fYmax = (float)fmax(fYmax, vecPoint.y);
				fZmin = (float)fmin(fZmin, vecPoint.z);
				fZmax = (float)fmax(fZmax, vecPoint.z);
			} // for (size_t iIndex = ...
		} // for (size_t iPolygon = ...
	} // if (!m_vecConcFacePolygons.empty())

	// Lines
	if (!m_vecLines.empty())
	{
		for (size_t iPolygon = 0; iPolygon < m_vecLines.size(); iPolygon++)
		{
			for (int64_t iIndex = m_vecLines[iPolygon].startIndex();
				iIndex < m_vecLines[iPolygon].startIndex() + m_vecLines[iPolygon].indicesCount();
				iIndex++)
			{
				if (m_pIndexBuffer->data()[iIndex] == -1)
				{
					continue;
				}

				_vector3 vecPoint =
				{
					m_pVertexBuffer->data()[(m_pIndexBuffer->data()[iIndex] * VERTEX_LENGTH) + 0],
					m_pVertexBuffer->data()[(m_pIndexBuffer->data()[iIndex] * VERTEX_LENGTH) + 1],
					m_pVertexBuffer->data()[(m_pIndexBuffer->data()[iIndex] * VERTEX_LENGTH) + 2]
				};

				if (pTransformationMatrix != nullptr)
				{
					_transform(&vecPoint, pTransformationMatrix, &vecPoint);
				}

				fXmin = (float)fmin(fXmin, vecPoint.x);
				fXmax = (float)fmax(fXmax, vecPoint.x);
				fYmin = (float)fmin(fYmin, vecPoint.y);
				fYmax = (float)fmax(fYmax, vecPoint.y);
				fZmin = (float)fmin(fZmin, vecPoint.z);
				fZmax = (float)fmax(fZmax, vecPoint.z);
			} // for (size_t iIndex = ...
		} // for (size_t iPolygon = ...
	} // if (!m_vecLines.empty())

	// Points
	if (!m_vecPoints.empty())
	{
		for (size_t iPolygon = 0; iPolygon < m_vecPoints.size(); iPolygon++)
		{
			for (int64_t iIndex = m_vecPoints[iPolygon].startIndex();
				iIndex < m_vecPoints[iPolygon].startIndex() + m_vecPoints[iPolygon].indicesCount();
				iIndex++)
			{
				_vector3 vecPoint =
				{
					m_pVertexBuffer->data()[(m_pIndexBuffer->data()[iIndex] * VERTEX_LENGTH) + 0],
					m_pVertexBuffer->data()[(m_pIndexBuffer->data()[iIndex] * VERTEX_LENGTH) + 1],
					m_pVertexBuffer->data()[(m_pIndexBuffer->data()[iIndex] * VERTEX_LENGTH) + 2]
				};

				if (pTransformationMatrix != nullptr)
				{
					_transform(&vecPoint, pTransformationMatrix, &vecPoint);
				}

				fXmin = (float)fmin(fXmin, vecPoint.x);
				fXmax = (float)fmax(fXmax, vecPoint.x);
				fYmin = (float)fmin(fYmin, vecPoint.y);
				fYmax = (float)fmax(fYmax, vecPoint.y);
				fZmin = (float)fmin(fZmin, vecPoint.z);
				fZmax = (float)fmax(fZmax, vecPoint.z);
			} // for (size_t iIndex = ...
		} // for (size_t iPolygon = ...
	} // if (!m_vecPoints.empty())
}

void _geometry::calculateMinMaxTransform(
	_model* pModel,
	_instance* pInstance,
	float& fXmin, float& fXmax,
	float& fYmin, float& fYmax,
	float& fZmin, float& fZmax)
{
	assert(pModel != nullptr);
	assert(pInstance != nullptr);

	_vector3d vecVertexBufferOffset;
	GetVertexBufferOffset(pModel->getOwlModel(), (double*)&vecVertexBufferOffset);

	float dScaleFactor = (float)pModel->getOriginalBoundingSphereDiameter() / 2.f;
	float fXTranslation = (float)vecVertexBufferOffset.x / dScaleFactor;
	float fYTranslation = (float)vecVertexBufferOffset.y / dScaleFactor;
	float fZTranslation = (float)vecVertexBufferOffset.z / dScaleFactor;

	glm::mat4 matInstanceTransformation = glm::make_mat4((GLdouble*)pInstance->getTransformationMatrix());
	glm::mat4 matFinalTransformation = glm::identity<glm::mat4>();
	matFinalTransformation = glm::translate(matFinalTransformation, glm::vec3(fXTranslation, fYTranslation, fZTranslation));
	matFinalTransformation = matFinalTransformation * matInstanceTransformation;
	matFinalTransformation = glm::translate(matFinalTransformation, glm::vec3(-fXTranslation, -fYTranslation, -fZTranslation));

	double arTransformation[16] = { 0.0 };
	const float* pRawData = (const float*)glm::value_ptr(matFinalTransformation);
	for (int i = 0; i < 16; ++i)
		arTransformation[i] = pRawData[i];

	calculateMinMaxTransform(
		(const _matrix4x4*)arTransformation,
		fXmin, fXmax,
		fYmin, fYmax,
		fZmin, fZmax);
}

void _geometry::scale(float fScaleFactor)
{
	if (getVerticesCount() == 0)
	{
		return;
	}

	const auto VERTEX_LENGTH = getVertexLength();

	// Geometry
	for (int64_t iVertex = 0; iVertex < m_pVertexBuffer->size(); iVertex++)
	{
		m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 0] /= fScaleFactor;
		m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 1] /= fScaleFactor;
		m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 2] /= fScaleFactor;
	}

	// Instances
	for (size_t iInstance = 0; iInstance < m_vecInstances.size(); iInstance++)
	{
		m_vecInstances[iInstance]->scale(fScaleFactor);
	}
}

void _geometry::translate(float fX, float fY, float fZ)
{
	if (getVerticesCount() == 0)
	{
		return;
	}

	const auto iVertexLength = getVertexLength();

	// Geometry
	for (int64_t iVertex = 0; iVertex < m_pVertexBuffer->size(); iVertex++)
	{
		m_pVertexBuffer->data()[(iVertex * iVertexLength) + 0] += fX;
		m_pVertexBuffer->data()[(iVertex * iVertexLength) + 1] += fY;
		m_pVertexBuffer->data()[(iVertex * iVertexLength) + 2] += fZ;
	}

	/* BB - Min */
	m_pvecBBMin->x += fX;
	m_pvecBBMin->y += fY;
	m_pvecBBMin->z += fZ;

	/* BB - Max */
	m_pvecBBMax->x += fX;
	m_pvecBBMax->y += fY;
	m_pvecBBMax->z += fZ;
}

void _geometry::addInstance(_instance* pInstance)
{
	assert(pInstance != 0);

	m_vecInstances.push_back(pInstance);
}

/*virtual*/ void _geometry::enableInstances(bool bEnable)
{
	for (auto pInstance : m_vecInstances)
	{
		pInstance->setEnable(bEnable);
	}
}

long _geometry::getEnabledInstancesCount() const
{
	long lCount = 0;
	for (auto pInstance : m_vecInstances)
	{
		lCount += pInstance->getEnable() ? 1 : 0;
	}

	return lCount;
}

bool  _geometry::calculateInstance(_vertices_f* pVertexBuffer, _indices_i32* pIndexBuffer)
{
	assert(pVertexBuffer != nullptr);
	assert(pIndexBuffer != nullptr);

	if (getOwlInstance() == 0)
	{
		return false;
	}

	CalculateInstance(getOwlInstance(), &pVertexBuffer->size(), &pIndexBuffer->size(), nullptr);
	if ((pVertexBuffer->size() == 0) || (pIndexBuffer->size() == 0))
	{
		return false;
	}

	pVertexBuffer->data() = new float[(uint32_t)pVertexBuffer->size() * (int64_t)pVertexBuffer->getVertexLength()];
	memset(pVertexBuffer->data(), 0, (uint32_t)pVertexBuffer->size() * (int64_t)pVertexBuffer->getVertexLength() * sizeof(float));

	UpdateInstanceVertexBuffer(getOwlInstance(), pVertexBuffer->data());

	pIndexBuffer->data() = new int32_t[(uint32_t)pIndexBuffer->size()];
	memset(pIndexBuffer->data(), 0, (uint32_t)pIndexBuffer->size() * sizeof(int32_t));

	UpdateInstanceIndexBuffer(getOwlInstance(), pIndexBuffer->data());

	return true;
}

/*virtual*/ void _geometry::calculateCore()
{
	ASSERT(m_pVertexBuffer == nullptr);
	m_pVertexBuffer = new _vertices_f(getVertexLength());

	ASSERT(m_pIndexBuffer == nullptr);
	m_pIndexBuffer = new _indices_i32();

	assert(m_pmtxOriginalBBTransformation == nullptr);
	m_pmtxOriginalBBTransformation = new _matrix();

	assert(m_pvecOriginalBBMin == nullptr);
	m_pvecOriginalBBMin = new _vector3d();

	assert(m_pvecOriginalBBMax == nullptr);
	m_pvecOriginalBBMax = new _vector3d();

	assert(m_pmtxBBTransformation == nullptr);
	m_pmtxBBTransformation = new _matrix();

	assert(m_pvecBBMin == nullptr);
	m_pvecBBMin = new _vector3d();

	assert(m_pvecBBMax == nullptr);
	m_pvecBBMax = new _vector3d();

	assert(m_pvecAABBMin == nullptr);
	m_pvecAABBMin = new _vector3d();

	assert(m_pvecAABBMax == nullptr);
	m_pvecAABBMax = new _vector3d();

	GetBoundingBox(
		getOwlInstance(),
		(double*)m_pmtxOriginalBBTransformation,
		(double*)m_pvecOriginalBBMin,
		(double*)m_pvecOriginalBBMax);

	memcpy(m_pmtxBBTransformation, m_pmtxOriginalBBTransformation, sizeof(_matrix));
	memcpy(m_pvecBBMin, m_pvecOriginalBBMin, sizeof(_vector3d));
	memcpy(m_pvecBBMax, m_pvecOriginalBBMax, sizeof(_vector3d));

	if (!GetBoundingBox(
		getOwlInstance(),
		(double*)m_pvecAABBMin,
		(double*)m_pvecAABBMax))
	{
		return;
	}

	if (!calculateInstance(m_pVertexBuffer, m_pIndexBuffer))
	{
		return;
	}

	MATERIALS mapMaterial2ConcFaces;
	MATERIALS mapMaterial2ConcFaceLines;
	MATERIALS mapMaterial2ConcFacePoints;

	m_iConceptualFacesCount = GetConceptualFaceCnt(getOwlInstance());
	for (int64_t iConceptualFaceIndex = 0; iConceptualFaceIndex < m_iConceptualFacesCount; iConceptualFaceIndex++)
	{
		int64_t iStartIndexTriangles = 0;
		int64_t iIndicesCountTriangles = 0;
		int64_t iStartIndexLines = 0;
		int64_t iIndicesCountLines = 0;
		int64_t iStartIndexPoints = 0;
		int64_t iIndicesCountPoints = 0;
		int64_t iStartIndexConceptualFacePolygons = 0;
		int64_t iIndicesCountConceptualFacePolygons = 0;
		ConceptualFace iConceptualFace = GetConceptualFace(
			getOwlInstance(),
			iConceptualFaceIndex,
			&iStartIndexTriangles, &iIndicesCountTriangles,
			&iStartIndexLines, &iIndicesCountLines,
			&iStartIndexPoints, &iIndicesCountPoints,
			0, 0,
			&iStartIndexConceptualFacePolygons, &iIndicesCountConceptualFacePolygons);

		//
		// Material
		// 

		uint32_t iAmbientColor = getDefaultColor();
		uint32_t iDiffuseColor = getDefaultColor();
		uint32_t iEmissiveColor = getDefaultColor();
		uint32_t iSpecularColor = getDefaultColor();
		float fTransparency = 1.f;

		OwlInstance iMaterialInstance = GetConceptualFaceMaterial(iConceptualFace);
		if (iMaterialInstance != 0)
		{
			iAmbientColor = GetMaterialColorAmbient(iMaterialInstance);
			iDiffuseColor = GetMaterialColorDiffuse(iMaterialInstance);
			iEmissiveColor = GetMaterialColorEmissive(iMaterialInstance);
			iSpecularColor = GetMaterialColorSpecular(iMaterialInstance);
			fTransparency = getTransparency(iMaterialInstance);
		}

		_material material(
			iAmbientColor,
			iDiffuseColor,
			iEmissiveColor,
			iSpecularColor,
			fTransparency,
			nullptr);

		if (iIndicesCountTriangles > 0)
		{
			addTriangles(iConceptualFaceIndex, iStartIndexTriangles, iIndicesCountTriangles, material, mapMaterial2ConcFaces);
		}

		if (iIndicesCountConceptualFacePolygons > 0)
		{
			addConcFacePolygons(iStartIndexConceptualFacePolygons, iIndicesCountConceptualFacePolygons);
		}

		if (iIndicesCountLines > 0)
		{
			addLines(iConceptualFaceIndex, iStartIndexLines, iIndicesCountLines, material, mapMaterial2ConcFaceLines);
		}

		if (iIndicesCountPoints > 0)
		{
			addPoints(iConceptualFaceIndex, iStartIndexPoints, iIndicesCountPoints, material, mapMaterial2ConcFacePoints);
		}
	} // for (int64_t iConceptualFaceIndex = ...

	// Build the cohorts
	buildConcFacesCohorts(mapMaterial2ConcFaces, _oglUtils::getIndicesCountLimit());
	buildConcFacePolygonsCohorts(_oglUtils::getIndicesCountLimit());
	buildLinesCohorts(mapMaterial2ConcFaceLines, _oglUtils::getIndicesCountLimit());
	buildPointsCohorts(mapMaterial2ConcFacePoints, _oglUtils::getIndicesCountLimit());
}

void _geometry::addMaterial(int64_t iConceptualFaceIndex, int64_t iStartIndex, int64_t iIndicesCount, _material& material, MATERIALS& mapMaterials)
{
	auto itMaterial = mapMaterials.find(material);
	if (itMaterial == mapMaterials.end())
	{
		mapMaterials[material] = vector<_face>{ _face(iConceptualFaceIndex, iStartIndex, iIndicesCount) };
	}
	else
	{
		itMaterial->second.push_back(_face(iConceptualFaceIndex, iStartIndex, iIndicesCount));
	}
}

wstring _geometry::getConcFaceTexture(ConceptualFace iConceptualFace)
{
	wstring strTexture;

	OwlInstance iMaterialInstance = GetConceptualFaceMaterial(iConceptualFace);
	if (iMaterialInstance != 0)
	{
		int64_t* piInstances = nullptr;
		int64_t iCard = 0;
		GetObjectProperty(
			iMaterialInstance,
			GetPropertyByName(getOwlModel(), "textures"),
			&piInstances,
			&iCard);

		if (iCard == 1)
		{
			iCard = 0;
			char** szValue = nullptr;
			GetDatatypeProperty(
				piInstances[0],
				GetPropertyByName(getOwlModel(), "name"),
				(void**)&szValue,
				&iCard);

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

	return strTexture;
}

float _geometry::getTransparency(OwlInstance owlMaterialInstance)
{
	assert(owlMaterialInstance != 0);

	//
	// color
	//

	OwlInstance* powlInstances = nullptr;
	int64_t iCard = 0;
	GetObjectProperty(
		owlMaterialInstance,
		GetPropertyByName(getOwlModel(), "color"),
		&powlInstances,
		&iCard);
	
	// color
	if (iCard == 1)
	{
		OwlInstance owlColorInstance = powlInstances[0];
		assert(owlColorInstance != 0);

		//
		// color/transparency
		//

		iCard = 0;
		double* pdValue = nullptr;
		GetDatatypeProperty(
			owlColorInstance,
			GetPropertyByName(getOwlModel(), "transparency"),
			(void**)&pdValue,
			&iCard);

		if (iCard == 1)
		{
			return (float)pdValue[0];
		}

		//
		// color/ambient/W
		//

		powlInstances = nullptr;
		iCard = 0;
		GetObjectProperty(
			owlColorInstance,
			GetPropertyByName(getOwlModel(), "ambient"),
			&powlInstances,
			&iCard);

		// ambient
		if (iCard == 1)
		{
			OwlInstance owlColorComponentInstance = powlInstances[0];
			assert(owlColorComponentInstance != 0);

			iCard = 0;
			pdValue = nullptr;
			GetDatatypeProperty(
				owlColorComponentInstance,
				GetPropertyByName(getOwlModel(), "W"),
				(void**)&pdValue,
				&iCard);

			if (iCard == 1)
			{
				return (float)pdValue[0];
			}
		} // ambient
	} // color

	return 1.f;
}

void _geometry::buildConcFacesCohorts(MATERIALS& mapMaterials, const GLsizei INDICES_COUNT_LIMIT)
{
	auto itMaterial = mapMaterials.begin();
	for (; itMaterial != mapMaterials.end(); itMaterial++)
	{
		_cohortWithMaterial* pCohort = nullptr;

		for (size_t iConcFace = 0; iConcFace < itMaterial->second.size(); iConcFace++)
		{
			_face& concFace = itMaterial->second[iConcFace];

			int64_t iStartIndex = concFace.startIndex();
			int64_t iIndicesCount = concFace.indicesCount();

			// Split the conceptual face - isolated case
			if (iIndicesCount > INDICES_COUNT_LIMIT)
			{
				while (iIndicesCount > INDICES_COUNT_LIMIT)
				{
					auto pNewCohort = new _cohortWithMaterial(itMaterial->first);
					for (int64_t iIndex = iStartIndex;
						iIndex < iStartIndex + INDICES_COUNT_LIMIT;
						iIndex++)
					{
						pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
					}

					concFacesCohorts().push_back(pNewCohort);

					// Update Conceptual face start index
					concFace.startIndex() = 0;

					// Conceptual faces
					pNewCohort->faces().push_back(concFace);

					iIndicesCount -= INDICES_COUNT_LIMIT;
					iStartIndex += INDICES_COUNT_LIMIT;
				}

				if (iIndicesCount > 0)
				{
					auto pNewCohort = new _cohortWithMaterial(itMaterial->first);
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
			} // if (iIndicesCount > INDICES_COUNT_LIMIT)	

			// Create cohort
			if (pCohort == nullptr)
			{
				pCohort = new _cohortWithMaterial(itMaterial->first);

				concFacesCohorts().push_back(pCohort);
			}

			// Check the limit
			if (pCohort->indices().size() + iIndicesCount > INDICES_COUNT_LIMIT)
			{
				pCohort = new _cohortWithMaterial(itMaterial->first);

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
	} // for (; itMaterial != ...
}

void _geometry::buildFacePolygonsCohorts(const GLsizei INDICES_COUNT_LIMIT)
{
	if (m_vecFacePolygons.empty())
	{
		return;
	}

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
		if (iIndicesCount > INDICES_COUNT_LIMIT / 2)
		{
			while (iIndicesCount > INDICES_COUNT_LIMIT / 2)
			{
				pCohort = new _cohort();
				facePolygonsCohorts().push_back(pCohort);

				for (int64_t iIndex = iStartIndex;
					iIndex < iStartIndex + INDICES_COUNT_LIMIT / 2;
					iIndex += 2)
				{
					pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
					pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
				}

				iIndicesCount -= INDICES_COUNT_LIMIT / 2;
				iStartIndex += INDICES_COUNT_LIMIT / 2;
			} // while (iIndicesCount > INDICES_COUNT_LIMIT / 2)

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
		} // if (iIndicesCount > INDICES_COUNT_LIMIT / 2)

		// Check the limit
		if ((pCohort->indices().size() + (iIndicesCount * 2)) > INDICES_COUNT_LIMIT)
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
}

void _geometry::buildConcFacePolygonsCohorts(const GLsizei INDICES_COUNT_LIMIT)
{
	if (m_vecConcFacePolygons.empty())
	{
		return;
	}

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
		if (iIndicesCount > INDICES_COUNT_LIMIT / 2)
		{
			while (iIndicesCount > INDICES_COUNT_LIMIT / 2)
			{
				pCohort = new _cohort();
				concFacePolygonsCohorts().push_back(pCohort);

				for (int64_t iIndex = iStartIndex;
					iIndex < iStartIndex + INDICES_COUNT_LIMIT / 2;
					iIndex += 2)
				{
					pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
					pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
				}

				iIndicesCount -= INDICES_COUNT_LIMIT / 2;
				iStartIndex += INDICES_COUNT_LIMIT / 2;
			} // while (iIndicesCount > INDICES_COUNT_LIMIT / 2)

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
		} // if (iIndicesCount > INDICES_COUNT_LIMIT / 2)

		// Check the limit
		if ((pCohort->indices().size() + (iIndicesCount * 2)) > INDICES_COUNT_LIMIT)
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
}

void _geometry::buildLinesCohorts(MATERIALS& mapMaterials, const GLsizei INDICES_COUNT_LIMIT)
{
	auto itMaterial = mapMaterials.begin();
	for (; itMaterial != mapMaterials.end(); itMaterial++)
	{
		_cohortWithMaterial* pCohort = nullptr;

		for (size_t iConcFace = 0; iConcFace < itMaterial->second.size(); iConcFace++)
		{
			_face& concFace = itMaterial->second[iConcFace];

			int64_t iStartIndex = concFace.startIndex();
			int64_t iIndicesCount = concFace.indicesCount();

			// Split the conceptual face - isolated case
			if (iIndicesCount > INDICES_COUNT_LIMIT)
			{
				while (iIndicesCount > INDICES_COUNT_LIMIT)
				{
					auto pNewCohort = new _cohortWithMaterial(itMaterial->first);
					for (int64_t iIndex = iStartIndex;
						iIndex < iStartIndex + INDICES_COUNT_LIMIT;
						iIndex++)
					{
						pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
					}

					linesCohorts().push_back(pNewCohort);

					// Update Conceptual face start index
					concFace.startIndex() = 0;

					// Conceptual faces
					pNewCohort->faces().push_back(concFace);

					iIndicesCount -= INDICES_COUNT_LIMIT;
					iStartIndex += INDICES_COUNT_LIMIT;
				}

				if (iIndicesCount > 0)
				{
					auto pNewCohort = new _cohortWithMaterial(itMaterial->first);
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
			} // if (iIndicesCountTriangles > INDICES_COUNT_LIMIT)	

			// Create cohort
			if (pCohort == nullptr)
			{
				pCohort = new _cohortWithMaterial(itMaterial->first);

				linesCohorts().push_back(pCohort);
			}

			// Check the limit
			if (pCohort->indices().size() + iIndicesCount > INDICES_COUNT_LIMIT)
			{
				pCohort = new _cohortWithMaterial(itMaterial->first);

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
	} // for (; itMaterial != ...
}

void _geometry::buildPointsCohorts(MATERIALS& mapMaterials, const GLsizei INDICES_COUNT_LIMIT)
{
	auto itMaterial = mapMaterials.begin();
	for (; itMaterial != mapMaterials.end(); itMaterial++)
	{
		_cohortWithMaterial* pCohort = nullptr;

		for (size_t iConcFace = 0; iConcFace < itMaterial->second.size(); iConcFace++)
		{
			_face& concFace = itMaterial->second[iConcFace];

			int64_t iStartIndex = concFace.startIndex();
			int64_t iIndicesCount = concFace.indicesCount();

			// Split the conceptual face - isolated case
			if (iIndicesCount > INDICES_COUNT_LIMIT)
			{
				while (iIndicesCount > INDICES_COUNT_LIMIT)
				{
					auto pNewCohort = new _cohortWithMaterial(itMaterial->first);
					for (int64_t iIndex = iStartIndex;
						iIndex < iStartIndex + INDICES_COUNT_LIMIT;
						iIndex++)
					{
						pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
					}

					pointsCohorts().push_back(pNewCohort);

					// Update Conceptual face start index
					concFace.startIndex() = 0;

					// Conceptual faces
					pNewCohort->faces().push_back(concFace);

					iIndicesCount -= INDICES_COUNT_LIMIT;
					iStartIndex += INDICES_COUNT_LIMIT;
				}

				if (iIndicesCount > 0)
				{
					auto pNewCohort = new _cohortWithMaterial(itMaterial->first);
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
			} // if (iIndicesCountTriangles > INDICES_COUNT_LIMIT)	

			// Create cohort
			if (pCohort == nullptr)
			{
				pCohort = new _cohortWithMaterial(itMaterial->first);

				pointsCohorts().push_back(pCohort);
			}

			// Check the limit
			if (pCohort->indices().size() + iIndicesCount > INDICES_COUNT_LIMIT)
			{
				pCohort = new _cohortWithMaterial(itMaterial->first);

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

			// Add Conceptual faces
			pCohort->faces().push_back(concFace);
		} // for (size_t iConcFace = ...
	} // for (; itMaterial != ...
}

/*virtual*/ void _geometry::clean()
{
	delete m_pVertexBuffer;
	m_pVertexBuffer = nullptr;

	delete m_pIndexBuffer;
	m_pIndexBuffer = nullptr;

	delete m_pmtxOriginalBBTransformation;
	m_pmtxOriginalBBTransformation = nullptr;

	delete m_pvecOriginalBBMin;
	m_pvecOriginalBBMin = nullptr;

	delete m_pvecOriginalBBMax;
	m_pvecOriginalBBMax = nullptr;

	delete m_pmtxBBTransformation;
	m_pmtxBBTransformation = nullptr;

	delete m_pvecBBMin;
	m_pvecBBMin = nullptr;

	delete m_pvecBBMax;
	m_pvecBBMax = nullptr;

	delete m_pvecAABBMin;
	m_pvecAABBMin = nullptr;

	delete m_pvecAABBMax;
	m_pvecAABBMax = nullptr;

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

/*virtual*/ void _geometry::cleanCachedGeometry()
{
	m_owlInstance = 0;
}