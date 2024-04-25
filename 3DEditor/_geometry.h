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

	static unsigned int* merge(const vector<_cohort*>& vecCohorts, int64_t& iIndicesCount)
	{
		iIndicesCount = 0;
		for (size_t iCohort = 0; iCohort < vecCohorts.size(); iCohort++)
		{
			iIndicesCount += vecCohorts[iCohort]->indices().size();
		}

		unsigned int* pIndices = new unsigned int[iIndicesCount];

		int64_t iOffset = 0;
		for (size_t iCohort = 0; iCohort < vecCohorts.size(); iCohort++)
		{
			if (vecCohorts[iCohort]->indices().size() == 0)
			{
				continue;
			}

			memcpy((unsigned int*)pIndices + iOffset, vecCohorts[iCohort]->indices().data(),
				vecCohorts[iCohort]->indices().size() * sizeof(unsigned int));

			iOffset += vecCohorts[iCohort]->indices().size();
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
	GLuint& ibo() { return m_iIBO; }
	GLsizei& iboOffset() { return m_iIBOOffset; }
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
template<class T>
class _buffer
{

private: // Members

	T* m_pData;
	int64_t m_iSize;

public: // Methods

	_buffer()
		: m_pData(nullptr)
		, m_iSize(0)
	{}

	virtual ~_buffer()
	{
		delete[] m_pData;
	}

	T*& data() { return m_pData; }
	int64_t& size() { return m_iSize; }
};

// ************************************************************************************************
template<class V>
class _vertexBuffer : public _buffer<V>
{

private: // Members

	int64_t m_iVertexLength;

public: // Methods

	_vertexBuffer()
		: m_iVertexLength(0)
	{
		static_assert(
			is_same<V, float>::value ||
			is_same<V, double>::value,
			"V must be float or double type.");
	}

	virtual ~_vertexBuffer()
	{}

	int64_t& vertexLength() { return m_iVertexLength; }
};

// ************************************************************************************************
typedef _vertexBuffer<float> _vertices_f;
typedef _vertexBuffer<double> _vertices_d;

// ************************************************************************************************
template<class I>
class _indexBuffer : public _buffer<I>
{

public: // Methods

	_indexBuffer()
	{
		static_assert(
			is_same<I, int32_t>::value ||
			is_same<I, int64_t>::value,
			"I must be int32_t or int64_t type.");
	}

	virtual ~_indexBuffer()
	{}
};

// ************************************************************************************************
typedef _indexBuffer<int32_t> _indices_i32;
typedef _indexBuffer<int64_t> _indices_i64;

// ************************************************************************************************
class _instance
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
	bool m_bNeedsRefresh; // The data (geometry) is out of date

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

public: // Methods

	_instance(int64_t iID, OwlInstance iInstance, bool bEnable)
		: m_iID(iID)
		, m_iInstance(iInstance)
		, m_strName(L"NA")
		, m_strUniqueName(L"")
		, m_bEnable(bEnable)
		, m_pVertexBuffer(nullptr)
		, m_pIndexBuffer(nullptr)
		, m_iConceptualFacesCount(0)
		, m_bNeedsRefresh(false)
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
	{}

	virtual ~_instance()
	{
		clean();
	}

	void scale(float fScaleFactor)
	{
		if (getVerticesCount() == 0)
		{
			return;
		}

		const auto iVertexLength = getVertexLength();

		/* Vertices */
		for (int64_t iVertex = 0; iVertex < m_pVertexBuffer->size(); iVertex++)
		{
			m_pVertexBuffer->data()[(iVertex * iVertexLength) + 0] = m_pVertexBuffer->data()[(iVertex * iVertexLength) + 0] / fScaleFactor;
			m_pVertexBuffer->data()[(iVertex * iVertexLength) + 1] = m_pVertexBuffer->data()[(iVertex * iVertexLength) + 1] / fScaleFactor;
			m_pVertexBuffer->data()[(iVertex * iVertexLength) + 2] = m_pVertexBuffer->data()[(iVertex * iVertexLength) + 2] / fScaleFactor;
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
	OwlModel getModel() const { return ::GetModel(m_iInstance); }
	bool isReferenced() const { return GetInstanceInverseReferencesByIterator(m_iInstance, 0); }
	bool getEnable() const { return m_bEnable; }
	virtual void setEnable(bool bEnable) { m_bEnable = bEnable; }
	const wchar_t* getName() const { return m_strName.c_str(); }
	const wchar_t* getUniqueName() const { return m_strUniqueName.c_str(); }

	// Geometry
	int32_t* getIndices() const { return m_pIndexBuffer != nullptr ? m_pIndexBuffer->data() : nullptr; }
	int64_t getIndicesCount() const { return m_pIndexBuffer != nullptr ? m_pIndexBuffer->size() : 0; }
	float* getVertices() const { return m_pVertexBuffer != nullptr ? m_pVertexBuffer->data() : nullptr; }
	int64_t getVerticesCount() const { return m_pVertexBuffer != nullptr ? m_pVertexBuffer->size() : 0; }
	uint64_t getVertexLength() const { return SetFormat(getModel()) / sizeof(float); }
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

protected: // Methods

	void clean()
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
