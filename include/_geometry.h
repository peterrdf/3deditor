#pragma once

#include "engine.h"

#include "glew.h"
#include "wglew.h"

#include "_3DUtils.h"
#include "_material.h"

#include <string>
#include <vector>
#include <map>
using namespace std;

#include <atlbase.h>

// ************************************************************************************************
class _primitives
{

private: // Members

	int64_t m_iStartIndex;
	int64_t m_iIndicesCount;

public: // Methods

	_primitives(int64_t iStartIndex, int64_t iIndicesCount)
		: m_iStartIndex(iStartIndex)
		, m_iIndicesCount(iIndicesCount)
	{}

	virtual ~_primitives()
	{}

	int64_t& startIndex() { return m_iStartIndex; }
	int64_t& indicesCount() { return m_iIndicesCount; }
};

// ************************************************************************************************
class _cohort
{

private: // Members

	vector<GLuint> m_vecIndices;

	GLuint m_iIBO;
	GLsizei m_iIBOOffset;

public: // Methods

	_cohort()
		: m_vecIndices()
		, m_iIBO(0)
		, m_iIBOOffset(0)
	{}

	virtual ~_cohort()
	{}	

	static unsigned int* merge(const vector<_cohort*>& vecCohorts, uint32_t& iIndicesCount)
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

	template<class T>
	static void clear(vector<T*>& vecCohorts)
	{
		for (size_t iCohort = 0; iCohort < vecCohorts.size(); iCohort++)
		{
			delete vecCohorts[iCohort];
		}

		vecCohorts.clear();
	}

	vector<GLuint>& indices() { return m_vecIndices; }
	GLuint& IBO() { return m_iIBO; }
	GLsizei& IBOOffset() { return m_iIBOOffset; }
};

// ************************************************************************************************
class _face : public _primitives
{

private: // Members

	int64_t m_iIndex;

public: // Methods

	_face(int64_t iIndex, int64_t iStartIndex, int64_t iIndicesCount)
		: _primitives(iStartIndex, iIndicesCount)
		, m_iIndex(iIndex)
	{}

	virtual ~_face()
	{}

	int64_t getIndex() const { return m_iIndex; }
};

// ************************************************************************************************
class _cohortWithMaterial : public _cohort
{

private: // Members

	vector<_face> m_vecFaces;
	_material m_material;

public: // Methods

	_cohortWithMaterial(const _material& material)
		: _cohort()
		, m_vecFaces()
		, m_material(material)
	{}

	virtual ~_cohortWithMaterial()
	{}

	vector<_face>& faces() { return m_vecFaces; }
	const _material* getMaterial() const { return &m_material; }
};

// ************************************************************************************************
typedef map<_material, vector<_face>, _materialsComparator> MATERIALS;

// ************************************************************************************************
class _geometry
{

protected: // Members

	// Metadata
	int64_t m_iID; // ID (1-based index)
	OwlInstance m_iInstance;
	wstring m_strName;
	wstring m_strUniqueName;
	bool m_bEnable;

	// Geometry
	_vertices_f* m_pVertexBuffer; // Vertices
	_indices_i32* m_pIndexBuffer; // Indices
	int64_t m_iConceptualFacesCount; // Conceptual faces	
	
	// BB/AABB
	_matrix* m_pmtxOriginalBBTransformation;
	_vector3d* m_pvecOriginalBBMin;
	_vector3d* m_pvecOriginalBBMax;
	_matrix* m_pmtxBBTransformation;
	_vector3d* m_pvecBBMin;
	_vector3d* m_pvecBBMax;
	_vector3d* m_pvecAABBMin;
	_vector3d* m_pvecAABBMax;

	// Primitives (Cache)
	vector<_primitives> m_vecTriangles;
	vector<_primitives> m_vecFacePolygons;
	vector<_primitives> m_vecConcFacePolygons;
	vector<_primitives> m_vecLines;
	vector<_primitives> m_vecPoints;

	// Cohorts (Cache)
	vector<_cohortWithMaterial*> m_vecConcFacesCohorts;
	vector<_cohort*> m_vecFacePolygonsCohorts;
	vector<_cohort*> m_vecConcFacePolygonsCohorts;
	vector<_cohortWithMaterial*> m_vecLinesCohorts;
	vector<_cohortWithMaterial*> m_vecPointsCohorts;

	// Vectors (Cache)
	vector<_cohort*> m_vecNormalVecsCohorts;
	vector<_cohort*> m_vecBiNormalVecsCohorts;
	vector<_cohort*> m_vecTangentVecsCohorts;

	// VBO (OpenGL)
	GLuint m_iVBO;
	GLsizei m_iVBOOffset;

public: // Methods

	_geometry(int64_t iID, OwlInstance iInstance, bool bEnable)
		: m_iID(iID)
		, m_iInstance(iInstance)
		, m_strName(L"NA")
		, m_strUniqueName(L"")
		, m_bEnable(bEnable)
		, m_pVertexBuffer(nullptr)
		, m_pIndexBuffer(nullptr)
		, m_iConceptualFacesCount(0)		
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
		, m_iVBO(0)
		, m_iVBOOffset(0)
	{}

	virtual ~_geometry()
	{
		clean();
	}

	void scale(float fScaleFactor)
	{
		if (getVerticesCount() == 0)
		{
			return;
		}

		const auto VERTEX_LENGTH = getVertexLength();

		for (int64_t iVertex = 0; iVertex < m_pVertexBuffer->size(); iVertex++)
		{
			m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 0] /= fScaleFactor;
			m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 1] /= fScaleFactor;
			m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 2] /= fScaleFactor;
		}
	}

	void translate(float fX, float fY, float fZ)
	{
		if (getVerticesCount() == 0)
		{
			return;
		}

		const auto iVertexLength = getVertexLength();

		/* Vertices */
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

	static void calculateBBMinMax(
		OwlInstance iInstance,
		double& dXmin, double& dXmax,
		double& dYmin, double& dYmax,
		double& dZmin, double& dZmax)
	{
		assert(iInstance != 0);

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

	void calculateMinMax(
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

	// Metadata
	int64_t getID() const { return m_iID; }
	OwlInstance getInstance() const { return m_iInstance; }
	OwlClass getClassInstance() const { return GetInstanceClass(m_iInstance); }
	virtual OwlModel getModel() const { return ::GetModel(m_iInstance); }
	bool isReferenced() const { return GetInstanceInverseReferencesByIterator(m_iInstance, 0) != 0; }
	bool getEnable() const { return m_bEnable; }
	virtual void setEnable(bool bEnable) { m_bEnable = bEnable; }
	const wchar_t* getName() const { return m_strName.c_str(); }
	const wchar_t* getUniqueName() const { return m_strUniqueName.c_str(); }

	// Geometry
	int32_t* getIndices() const { return m_pIndexBuffer != nullptr ? m_pIndexBuffer->data() : nullptr; }
	int64_t getIndicesCount() const { return m_pIndexBuffer != nullptr ? m_pIndexBuffer->size() : 0; }
	float* getVertices() const { return m_pVertexBuffer != nullptr ? m_pVertexBuffer->data() : nullptr; }
	int64_t getVerticesCount() const { return m_pVertexBuffer != nullptr ? m_pVertexBuffer->size() : 0; }
	uint32_t getVertexLength() const { return (uint32_t)SetFormat(getModel()) / sizeof(float); }
	int64_t getConceptualFacesCount() const { return m_iConceptualFacesCount; }
	bool hasGeometry() const { return (getVerticesCount() > 0) && (getIndicesCount() > 0); }

	// BB
	_vector3d* getOriginalBBMin() const { return m_pvecOriginalBBMin; }
	_vector3d* getOriginalBBMax() const { return m_pvecOriginalBBMax; }
	_matrix* getBBTransformation() const { return m_pmtxBBTransformation; }
	_vector3d* getBBMin() const { return m_pvecBBMin; }
	_vector3d* getBBMax() const { return m_pvecBBMax; }
	_vector3d* getAABBMin() const { return m_pvecAABBMin; }
	_vector3d* getAABBMax() const { return m_pvecAABBMax; }

	// Primitives
	const vector<_primitives>& getTriangles() const { return m_vecTriangles; }
	const vector<_primitives>& getLines() const { return m_vecLines; }
	const vector<_primitives>& getPoints() const { return m_vecPoints; }
	const vector<_primitives>& getFacePolygons() const { return m_vecFacePolygons; }
	const vector<_primitives>& getConcFacePolygons() const { return m_vecConcFacePolygons; }

	// Cohorts
	vector<_cohortWithMaterial*>& concFacesCohorts() { return m_vecConcFacesCohorts; }
	vector<_cohort*>& facePolygonsCohorts() { return m_vecFacePolygonsCohorts; }
	vector<_cohort*>& concFacePolygonsCohorts() { return m_vecConcFacePolygonsCohorts; }
	vector<_cohortWithMaterial*>& linesCohorts() { return m_vecLinesCohorts; }
	vector<_cohortWithMaterial*>& pointsCohorts() { return m_vecPointsCohorts; }

	// Vectors
	vector<_cohort*>& normalVecsCohorts() { return m_vecNormalVecsCohorts; }
	vector<_cohort*>& biNormalVecsCohorts() { return m_vecBiNormalVecsCohorts; }
	vector<_cohort*>& tangentVecsCohorts() { return m_vecTangentVecsCohorts; }

	// VBO (OpenGL)
	GLuint& VBO() { return m_iVBO; }
	GLsizei& VBOOffset() { return m_iVBOOffset; }

protected: // Methods

	void setSTEPFormatSettings()
	{
		uint64_t mask = 0;
		mask += FORMAT_SIZE_VERTEX_DOUBLE;
		mask += FORMAT_SIZE_INDEX_INT64;
		mask += FORMAT_VERTEX_NORMAL;
		mask += FORMAT_VERTEX_TEXTURE_UV;
		mask += FORMAT_EXPORT_TRIANGLES;
		mask += FORMAT_EXPORT_LINES;
		mask += FORMAT_EXPORT_POINTS;
		mask += FORMAT_EXPORT_CONCEPTUAL_FACE_POLYGONS;
		mask += FORMAT_EXPORT_POLYGONS_AS_TUPLES;

		uint64_t setting = 0;
		setting += FORMAT_VERTEX_NORMAL;
		setting += FORMAT_EXPORT_TRIANGLES;
		setting += FORMAT_EXPORT_LINES;
		setting += FORMAT_EXPORT_POINTS;
		setting += FORMAT_EXPORT_CONCEPTUAL_FACE_POLYGONS;
		setting += FORMAT_EXPORT_POLYGONS_AS_TUPLES;

		SetFormat(getModel(), setting, mask);
		SetBehavior(getModel(), 2048 + 4096, 2048 + 4096);
	}

	bool calculate(_vertices_f* pVertexBuffer, _indices_i32* pIndexBuffer)
	{
		assert(pVertexBuffer != nullptr);
		assert(pIndexBuffer != nullptr);
		
		int64_t iOwlInstance = calculateInstance(&pVertexBuffer->size(), &pIndexBuffer->size());
		if ((pVertexBuffer->size() == 0) || (pIndexBuffer->size() == 0))
		{
			return false;
		}

		pVertexBuffer->data() = new float[(uint32_t)pVertexBuffer->size() * (int64_t)pVertexBuffer->getVertexLength()];
		memset(pVertexBuffer->data(), 0, (uint32_t)pVertexBuffer->size() * (int64_t)pVertexBuffer->getVertexLength() * sizeof(float));

		UpdateInstanceVertexBuffer(iOwlInstance, pVertexBuffer->data());

		pIndexBuffer->data() = new int32_t[(uint32_t)pIndexBuffer->size()];
		memset(pIndexBuffer->data(), 0, (uint32_t)pIndexBuffer->size() * sizeof(int32_t));

		UpdateInstanceIndexBuffer(iOwlInstance, pIndexBuffer->data());

		return true;
	}

	virtual int64_t calculateInstance(int64_t* piVertexBufferSize, int64_t* piIndexBufferSize)
	{
		assert(piVertexBufferSize != nullptr);
		assert(piIndexBufferSize != nullptr);

		*piVertexBufferSize = *piIndexBufferSize = 0;

		CalculateInstance(m_iInstance, piVertexBufferSize, piIndexBufferSize, nullptr);

		return m_iInstance;
	}

	void addTriangles(int64_t iConceptualFaceIndex, int64_t iStartIndex, int64_t iIndicesCount, _material& material, MATERIALS& mapMaterials)
	{
		m_vecTriangles.push_back(_primitives(iStartIndex, iIndicesCount));

		addMaterial(iConceptualFaceIndex, iStartIndex, iIndicesCount, material, mapMaterials);
	}

	void addFacePolygons(int64_t iStartIndex, int64_t iIndicesCount)
	{
		m_vecFacePolygons.push_back(_primitives(iStartIndex, iIndicesCount));
	}

	void addConcFacePolygons(int64_t iStartIndex, int64_t iIndicesCount)
	{
		m_vecConcFacePolygons.push_back(_primitives(iStartIndex, iIndicesCount));
	}

	void addLines(int64_t iConceptualFaceIndex, int64_t iStartIndex, int64_t iIndicesCount, _material& material, MATERIALS& mapMaterials)
	{
		m_vecLines.push_back(_primitives(iStartIndex, iIndicesCount));

		addMaterial(iConceptualFaceIndex, iStartIndex, iIndicesCount, material, mapMaterials);
	}

	void addPoints(int64_t iConceptualFaceIndex, int64_t iStartIndex, int64_t iIndicesCount, _material& material, MATERIALS& mapMaterials)
	{
		m_vecPoints.push_back(_primitives(iStartIndex, iIndicesCount));

		addMaterial(iConceptualFaceIndex, iStartIndex, iIndicesCount, material, mapMaterials);
	}

	wstring getConcFaceTexture(ConceptualFace iConceptualFace)
	{
		wstring strTexture;

		OwlInstance iMaterialInstance = GetConceptualFaceMaterial(iConceptualFace);
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

	void addMaterial(int64_t iConceptualFaceIndex, int64_t iStartIndex, int64_t iIndicesCount, _material& material, MATERIALS& mapMaterials)
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

	void buildConcFacesCohorts(MATERIALS& mapMaterials, const GLsizei INDICES_COUNT_LIMIT)
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

	void buildFacePolygonsCohorts(const GLsizei INDICES_COUNT_LIMIT)
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

	void buildConcFacePolygonsCohorts(const GLsizei INDICES_COUNT_LIMIT)
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

	void buildLinesCohorts(MATERIALS& mapMaterials, const GLsizei INDICES_COUNT_LIMIT)
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

	void buildPointsCohorts(MATERIALS& mapMaterials, const GLsizei INDICES_COUNT_LIMIT)
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

	virtual void clean()
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
};

// ************************************************************************************************
struct _instancesComparator
{
	bool operator()(const _geometry* i1, const _geometry* i2) const
	{
		return wcscmp(i1->getName(), i2->getName()) < 0;
	}
};