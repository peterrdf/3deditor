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

// (255 * 255 * 255)[R] + (255 * 255)[G] + 255[B]
class _i64RGBCoder
{

public: // Methods

	static void Encode(int64_t i, float& fR, float& fG, float& fB)
	{
		static const float STEP = 1.f / 255.f;

		fR = 0.f;
		fG = 0.f;
		fB = 0.f;

		// R
		if (i >= (255 * 255))
		{
			int64_t iR = i / (255 * 255);
			fR = iR * STEP;

			i -= iR * (255 * 255);
		}

		// G
		if (i >= 255)
		{
			int64_t iG = i / 255;
			fG = iG * STEP;

			i -= iG * 255;
		}

		// B		
		fB = i * STEP;
	}

	static int64_t Decode(unsigned char R, unsigned char G, unsigned char B)
	{
		int64_t i = 0;

		// R
		i += R * (255 * 255);

		// G
		i += G * 255;

		// B
		i += B;

		return i;
	}
};