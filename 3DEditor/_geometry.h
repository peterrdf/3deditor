#pragma once

#include "_material.h"

#include <map>
#include <vector>
using namespace std;

class _primitives
{

private: // Members

	int64_t m_iStartIndex;
	int64_t m_iIndicesCount;

public: // Methods

	_primitives(int64_t iStartIndex, int64_t iIndicesCount)
		: m_iStartIndex(iStartIndex)
		, m_iIndicesCount(iIndicesCount)
	{
	}

	virtual ~_primitives()
	{
	}

	int64_t& startIndex()
	{
		return m_iStartIndex;
	}

	int64_t& indicesCount()
	{
		return m_iIndicesCount;
	}
};

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
	{
	}

	virtual ~_cohort()
	{
	}

	vector<GLuint>& indices()
	{
		return m_vecIndices;
	}

	GLuint& ibo()
	{
		return m_iIBO;
	}

	GLsizei& iboOffset()
	{
		return m_iIBOOffset;
	}

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
};

class _face : public _primitives
{

private: // Members

	int64_t m_iIndex;

public: // Methods

	_face(int64_t iIndex, int64_t iStartIndex, int64_t iIndicesCount)
		: _primitives(iStartIndex, iIndicesCount)
		, m_iIndex(iIndex)
	{
	}

	virtual ~_face()
	{
	}

	int64_t getIndex() const
	{
		return m_iIndex;
	}
};

class _facesCohort : public _cohort
{

private: // Members

	vector<_face> m_vecFaces;
	_material m_material;

public: // Methods

	_facesCohort(const _material& material)
		: _cohort()
		, m_vecFaces()
		, m_material(material)
	{
	}

	virtual ~_facesCohort()
	{
	}

	vector<_face>& faces()
	{
		return m_vecFaces;
	}

	const _material* getMaterial() const
	{
		return &m_material;
	}
};

typedef map<_material, vector<_face>, _materialsComparator> MATERIALS;

struct _vector3d
{
	double x;
	double y;
	double z;
};

struct _matrix
{
	double _11, _12, _13;
	double _21, _22, _23;
	double _31, _32, _33;
	double _41, _42, _43;
};
