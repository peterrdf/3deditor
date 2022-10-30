#pragma once

#include "_material.h"

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

typedef _primitives _triangles;
typedef _primitives _polygons;
typedef _primitives _lines;
typedef _primitives _points;

class _face : public _triangles
{

private: // Members

	int64_t m_iIndex;

public: // Methods

	_face(int64_t iIndex, int64_t iStartIndex, int64_t iIndicesCount)
		: _triangles(iStartIndex, iIndicesCount)
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

