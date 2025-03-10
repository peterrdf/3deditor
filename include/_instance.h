#pragma once

#include "_geometry.h"

// ************************************************************************************************
class _instance
{

private: // Members

	int64_t m_iID;
	_geometry* m_pGeometry;
	_matrix4x4* m_pTransformationMatrix;
	bool m_bEnable;

public: // Methods

	_instance(int64_t iID, _geometry* pGeometry, _matrix4x3* pTransformationMatrix)
		: m_iID(iID)
		, m_pGeometry(pGeometry)
		, m_pTransformationMatrix(nullptr)
		, m_bEnable(true)
	{
		assert(m_iID > 0);
		assert(m_pGeometry != nullptr);

		setTransformationMatrix(pTransformationMatrix);
	}

	virtual ~_instance()
	{
		delete m_pTransformationMatrix;
	}

	void scale(float fScaleFactor)
	{
		m_pTransformationMatrix->_41 /= fScaleFactor;
		m_pTransformationMatrix->_42 /= fScaleFactor;
		m_pTransformationMatrix->_43 /= fScaleFactor;
	}

	void translate(float fX, float fY, float fZ)
	{
		m_pTransformationMatrix->_41 += fX;
		m_pTransformationMatrix->_42 += fY;
		m_pTransformationMatrix->_43 += fZ;
	}

public: // Properties

	int64_t getID() const { return m_iID; }
	virtual const wchar_t* getName() const { return m_pGeometry->getName(); }
	virtual const wchar_t* getUniqueName() const { return m_pGeometry->getUniqueName(); }
	_geometry* getGeometry() const { return m_pGeometry; }
	template<typename T>
	T* getGeometryAs() const { return dynamic_cast<T*>(getGeometry()); }
	virtual _instance* getOwner() const { return nullptr; }
	bool getEnable() const { return getOwner() != nullptr ? getOwner()->getEnable() : m_bEnable; }
	virtual void setEnable(bool bEnable) { m_bEnable = bEnable; }
	_matrix4x4* getTransformationMatrix() const { return m_pTransformationMatrix; }
	void setTransformationMatrix(_matrix4x3* pTransformationMatrix) 
	{ 
		delete m_pTransformationMatrix;

		m_pTransformationMatrix = new _matrix4x4();
		_matrix4x4Identity(m_pTransformationMatrix);

		if (pTransformationMatrix != nullptr)
		{
			m_pTransformationMatrix->_11 = pTransformationMatrix->_11;
			m_pTransformationMatrix->_12 = pTransformationMatrix->_12;
			m_pTransformationMatrix->_13 = pTransformationMatrix->_13;
			m_pTransformationMatrix->_21 = pTransformationMatrix->_21;
			m_pTransformationMatrix->_22 = pTransformationMatrix->_22;
			m_pTransformationMatrix->_23 = pTransformationMatrix->_23;
			m_pTransformationMatrix->_31 = pTransformationMatrix->_31;
			m_pTransformationMatrix->_32 = pTransformationMatrix->_32;
			m_pTransformationMatrix->_33 = pTransformationMatrix->_33;
			m_pTransformationMatrix->_41 = pTransformationMatrix->_41;
			m_pTransformationMatrix->_42 = pTransformationMatrix->_42;
			m_pTransformationMatrix->_43 = pTransformationMatrix->_43;
		}
	}

	OwlModel getOwlModel() { return getGeometry()->getOwlModel(); }
	OwlInstance getOwlInstance() const { return getGeometry()->getOwlInstance(); }
	bool hasGeometry() const { return getGeometry()->hasGeometry(); }
};

struct _instancesComparator
{
	bool operator()(const _instance* i1, const _instance* i2) const
	{
		return wcscmp(i1->getName(), i2->getName()) < 0;
	}
};
