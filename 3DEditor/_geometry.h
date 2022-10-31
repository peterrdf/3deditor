#pragma once

#include "_material.h"

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

	int64_t getStartIndex() const
	{
		return m_iStartIndex;
	}

	int64_t getIndicesCount() const
	{
		return m_iIndicesCount;
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
};
