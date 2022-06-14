#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "_Model.h"

/**
* C++ Wrapper
*/
template<class V>
class _VertexBuffer
{
	/**
	* Thing will populate the buffer
	*/
	friend class Thing;

public: // Members

	/**
	* Vertices
	*/
	V* m_pVertices;

	/**
	* Count
	*/
	int64_t m_iVerticesCount;

	/**
	* Length
	*/
	int64_t m_iVertexLength;

public: // Methods

	/**
	* ctor
	*/
	_VertexBuffer()
		: m_pVertices(nullptr)
		, m_iVerticesCount(0)
		, m_iVertexLength(0)
	{
		static_assert(
			is_same<V, float>::value ||
			is_same<V, double>::value,
			"V must be float or double type.");
	}

	/**
	* dtor
	*/
	virtual ~_VertexBuffer()
	{
		delete[] m_pVertices;
	}

	/**
	* Getter
	*/
	V* getVertices() const
	{
		return m_pVertices;
	}

	/**
	* Getter
	*/
	int64_t getVerticesCount() const
	{
		return m_iVerticesCount;
	}

	/**
	* Getter
	*/
	int64_t getVertexLength() const
	{
		return m_iVertexLength;
	}
};

/**
* C++ Wrapper
*/
template<class I>
class _IndexBuffer
{
	/**
	* Thing will populate the buffer
	*/
	friend class Thing;

public: // Members

	/**
	* Indices
	*/
	I* m_pIndices;

	/**
	* Count
	*/
	int64_t m_iIndicesCount;

public: // Methods

	/**
	* ctor
	*/
	_IndexBuffer()
		: m_pIndices(nullptr)
		, m_iIndicesCount(0)
	{
		static_assert(
			is_same<I, int32_t>::value ||
			is_same<I, int64_t>::value,
			"I must be int32_t or int64_t type.");
	}

	/**
	* dtor
	*/
	virtual ~_IndexBuffer()
	{
		delete[] m_pIndices;
	}

	/**
	* Getter
	*/
	I* getIndices() const
	{
		return m_pIndices;
	}

	/**
	* Getter
	*/
	int64_t getIndicesCount() const
	{
		return m_iIndicesCount;
	}
};

/**
* int32_t
*/
typedef _IndexBuffer<int32_t> IndexBuffer;

/**
* Double
*/
typedef _IndexBuffer<int64_t> DoubleIndexBuffer;

/**
* Float
*/
typedef _VertexBuffer<float> VertexBuffer;

/**
* Double
*/
typedef _VertexBuffer<double> DoubleVertexBuffer;

/**
* C++ Wrapper
*/
class Thing : public _Thing
{
	/**
	* Model
	*/
	friend class _Model;	

private: // Members

	/**
	* Model
	*/
	_Model* m_pModel;

	/**
	* Instance
	*/
	int64_t m_iInstance;

	/**
	* Parent
	*/
	Thing* m_pParent;

public: // Methods

	/**
	* ctor
	*/
	Thing()
		: m_pModel(NULL)
		, m_iInstance(0)
		, m_pParent(NULL)
	{
	}

	/**
	* ctor
	*/
	Thing(int64_t iInstance)
		: m_pModel(NULL)
		, m_iInstance(iInstance)
		, m_pParent(NULL)
	{
		assert(m_iInstance != 0);
	}

	/**
	* dtor
	*/
	virtual ~Thing()
	{
		delete m_pParent;
	}

	/**
	* Getter
	*/
	virtual int64_t getInstance() const
	{
		return m_iInstance;
	}

	/**
	* Getter
	*/
	virtual _Model* getModel() const
	{
		return m_pModel;
	}

	template<typename T>
	T* as()
	{
		static_assert(is_base_of<_Thing, T>::value, "T must derive from _Thing.");

		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		delete m_pParent;

		m_pParent = new T();
		m_pParent->m_pModel = m_pModel;
		m_pParent->m_iInstance = m_iInstance;

		return dynamic_cast<T*>(m_pParent);
	}

	/**
	* Getter
	*/
	char* getName() const
	{
		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		char* szName = nullptr;
		GetNameOfInstance(m_iInstance, &szName);

		return szName;
	}

	/**
	* Setter
	*/
	__int64 setName(const char* szName)
	{
		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		return SetNameOfInstance(m_iInstance, szName);
	}

	/**
	* Getter
	*/
	wchar_t* getNameW() const
	{
		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		wchar_t* szName = nullptr;
		GetNameOfInstanceW(m_iInstance, &szName);

		return szName;
	}

	/**
	* Setter
	*/
	__int64 setNameW(const wchar_t* szName)
	{
		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		return SetNameOfInstanceW(m_iInstance, szName);
	}

	/**
	* Getter
	*/
	int64_t getClassInstance() const
	{
		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		int64_t iClassInstance = GetInstanceClass(m_iInstance);
		ASSERT(iClassInstance != 0);

		return iClassInstance;
	}

	/**
	* Getter
	*/
	wchar_t* getNameOfClassW() const
	{
		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		wchar_t* szName = nullptr;
		GetNameOfClassW(getClassInstance(), &szName);

		return szName;
	}

	/**
	* Getter
	*/
	int64_t getInstanceInverseReferencesByIterator()
	{
		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		return GetInstanceInverseReferencesByIterator(m_iInstance, 0);
	}

	/**
	* Geometry
	*/
	template<class V, class I>
	bool PopulateIndexVertexBuffers(_VertexBuffer<V>* pVertexBuffer, _IndexBuffer<I>* pIndexBuffer)
	{
		static_assert(
			is_same<V, float>::value ||
			is_same<V, double>::value,
			"V must be float or double type.");

		static_assert(
			is_same<I, int32_t>::value ||
			is_same<I, int64_t>::value,
			"I must be int32_t or int64_t type.");

		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		/**
		* Check the Format - Vertices
		*/
		if (m_pModel->getFormat()->vertexArrayElementsDoublePrecision)
		{
			if (!is_same<V, double>::value)
			{
				assert(false);

				return false;
			}
		}
		else
		{
			if (!is_same<V, float>::value)
			{
				assert(false);

				return false;
			}
		}

		/**
		* Check the Format - Indices
		*/
		if (m_pModel->getFormat()->indexArrayElementsInt64)
		{
			if (!is_same<I, int64_t>::value)
			{
				assert(false);

				return false;
			}
		}
		else
		{
			if (!is_same<I, int32_t>::value)
			{
				assert(false);

				return false;
			}
		}
		
		/**
		* Calculate
		*/
		CalculateInstance(m_iInstance, &pVertexBuffer->m_iVerticesCount, &pIndexBuffer->m_iIndicesCount, NULL);

		/**
		* Retrieve the buffers
		*/
		if ((pVertexBuffer->m_iVerticesCount > 0) && (pIndexBuffer->m_iIndicesCount))
		{
			/**
			* Retrieves the vertices
			*/
			pVertexBuffer->m_iVertexLength = SetFormat(m_pModel->getInstance(), 0, 0) / sizeof(V);
			
			pVertexBuffer->m_pVertices = new V[pVertexBuffer->m_iVerticesCount * pVertexBuffer->m_iVertexLength];
			memset(pVertexBuffer->m_pVertices, 0, pVertexBuffer->m_iVerticesCount * pVertexBuffer->m_iVertexLength * sizeof(V));

			UpdateInstanceVertexBuffer(m_iInstance, pVertexBuffer->m_pVertices);

			/**
			* Retrieves the indices
			*/
			pIndexBuffer->m_pIndices = new I[pIndexBuffer->m_iIndicesCount];
			memset(pIndexBuffer->m_pIndices, 0, pIndexBuffer->m_iIndicesCount * sizeof(I));

			UpdateInstanceIndexBuffer(m_iInstance, pIndexBuffer->m_pIndices);

			return true;
		} // if ((pVertexBuffer->m_iVerticesCount > 0) && ...

		return false;
	}

	/**
	* Setter - Data Property
	*/
	template<typename T>
	void setDataProperty(const char* szProperty, T* pValues, int64_t iCount)
	{
		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		m_pModel->setDataProperty(m_iInstance, szProperty, pValues, iCount);
	}

	/**
	* Getter - Data Property
	*/
	template<typename T>
	pair<T*, int64_t> getDataProperty(const char* szProperty)
	{
		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		auto prProperty = m_pModel->getDataProperty<T>(m_iInstance, szProperty);

		return pair<T*, int64_t>((T*)prProperty.first, prProperty.second);
	}

	/**
	* Setter - Thing Property
	*/
	template <typename T>
	void setThingProperty(const char* szProperty, const T** pValues, int64_t iCount)
	{
		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		m_pModel->setThingProperty(m_iInstance, szProperty, pValues, iCount);
	}

	/**
	* Getter - Thing Property
	*/
	template<typename T>
	vector<T*> getThingProperty(const char* szProperty)
	{
		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		m_pModel->getThingProperty<T>(m_iInstance, szProperty);		
	}

	/**
	* Factory - Property
	*/
	template<typename T>
	T* createProperty(const char* szName)
	{
		assert(m_pModel != NULL);
		assert(m_iInstance != 0);

		return getModel()->createProperty<T>(getInstance(), szName);
	}

protected: // Methods

	/**
	* Add extra initialization here
	*/
	virtual void postCreate()
	{
	}
};

