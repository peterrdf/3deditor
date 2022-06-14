#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Thing.h"

#include <string>

using namespace std;

/**
* C++ Wrapper
*/
class Property 
	: public Thing
	, public _Property
{
	/**
	* Model
	*/
	friend class _Model;

private: // Members

	/**
	* Name
	*/
	string m_strName;

public: // Methods

	/**
	* ctor
	*/
	Property()
		: m_strName("")
	{
	}

	/**
	* ctor
	*/
	Property(const char* szName)
		: m_strName(szName)
	{	
	}

	/**
	* dtor
	*/
	virtual ~Property()
	{
	}

	/**
	* _Property
	*/
	virtual const char* getName() const
	{
		return m_strName.c_str();
	}

protected: // Methods

};

/**
* C++ bit set
*/
template<typename T>
class BitsetDataProperty : public Property
{

private: // Members

public: // Methods

	/**
	* ctor
	*/
	BitsetDataProperty()
	{
		static_assert(is_integral<T>::value, "T must be integral type.");
	}

	/**
	* dtor
	*/
	virtual ~BitsetDataProperty()
	{
	}

	/**
	* Setter
	*/
	void setBit(T mask)
	{
		T value = getValue();

		value |= mask;

		Thing::setDataProperty(getName(), &value, 1);
	}

	/**
	* Setter
	*/
	void clearBit(T mask)
	{
		T value = getValue();

		value &= ~mask;

		Thing::setDataProperty(getName(), &value, 1);
	}

	/**
	* Setter
	*/
	void setBit(T mask, bool value)
	{
		if (value)
		{
			setBit(mask);
		}
		else
		{
			clearBit(mask);
		}
	}

	/**
	* Getter
	*/
	bool getBit(T mask)
	{
		T value = getValue();

		return (value & mask) == mask;
	}

	/**
	* Getter
	*/
	T getValue()
	{
		auto property = Thing::getDataProperty<T>("consistencyCheck");
		if (property.second == 1) 
		{ 
			return property.first[0]; 
		}

		return (T)0;
	}
};