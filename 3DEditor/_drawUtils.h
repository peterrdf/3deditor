#pragma once

#include "_geometry.h"

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
	{
	}

	virtual ~_buffer()
	{
		delete[] m_pData;
	}

	T*& data()
	{
		return m_pData;
	}

	int64_t& size()
	{
		return m_iSize;
	}
};

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
	{
	}

	int64_t& vertexLength()
	{
		return m_iVertexLength;
	}
};

typedef _vertexBuffer<float> _vertices_f;
typedef _vertexBuffer<double> _vertices_d;

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
	{
	}
};


typedef _indexBuffer<int32_t> _indices_i32;
typedef _indexBuffer<int64_t> _indices_i64;