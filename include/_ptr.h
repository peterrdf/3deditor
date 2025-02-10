#pragma once

#include <vector>
using namespace std;

// ************************************************************************************************
template<typename Tout>
class _ptr
{

private: // Members

	Tout* m_pTout;

public: // Methods

	template<typename Tin>
	_ptr(Tin* pTin, bool bValidate = true)
		: m_pTout(nullptr)
	{
		if (bValidate)
		{
			assert(pTin != nullptr);
		}		

		m_pTout = dynamic_cast<Tout*>(pTin);

		if (bValidate)
		{
			assert(m_pTout != nullptr);
		}
	}

	virtual ~_ptr()
	{
	}

	Tout* p()
	{
		return m_pTout;
	}

	operator bool() const 
	{ 
		return m_pTout != nullptr;
	}

	operator Tout* ()
	{
		return m_pTout;
	}

	Tout* operator -> ()
	{
		return m_pTout;
	}
};

// ************************************************************************************************
template<typename T>
class _vector_sequential_iterator
{

private: // Members

	const vector<T*>& m_vecData;
	int64_t m_iIndex;

public: // Methods

	_vector_sequential_iterator(const vector<T*>& vecData)
		: m_vecData(vecData)
		, m_iIndex(-1)
	{
		assert(!m_vecData.empty());
	}

	virtual ~_vector_sequential_iterator()
	{
	}		

	void reset()
	{
		m_iIndex = -1;
	}

	T* getNextItem()
	{
		if (++m_iIndex >= (int64_t)m_vecData.size())
		{
			m_iIndex = -1;

			return nullptr;
		}

		return m_vecData[(unsigned int)m_iIndex];
	}

public: // Properties

	const vector<T*>& data() { return m_vecData; }
	size_t size() const { return m_vecData.size(); }
	bool empty() const { return m_vecData().empty(); }
};