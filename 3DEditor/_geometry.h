#pragma once

#include "glew.h"
#include "wglew.h"

#include "_material.h"

#include <map>
#include <vector>
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
