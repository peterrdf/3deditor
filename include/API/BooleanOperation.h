#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"
#include "GeometricItem.h"

/**
* This class represents the concept BooleanOperation. (http://rdf.bg/BooleanOperation)
*/
class BooleanOperation : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

	enum BooleanOperation_type {
		_UNION = 0, // The union of firstObject and secondObject.
		_DIFFERENCE = 1, // The difference of firstObject and secondObject.
		_DIFFERENCE_INVERSE = 2, // The difference of secondObject and firstObject.
		_INTERSECTION = 3, // The intersection of secondObject and firstObject.
	};

public:

	BooleanOperation()
	{
	}

	virtual ~BooleanOperation()
	{
	}

	static const char* getClassName()
	{
		return "BooleanOperation";
	}

	/**
	* This object property represents the relation firstObject. (Cardinality: [0, 1])
	*/
	GeometricItem* getFirstObject()
	{
		auto property = Thing::getThingProperty<GeometricItem>("firstObject");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation firstObject. (Cardinality: [0, 1])
	*/
	void setFirstObject(GeometricItem* pValue)
	{
		Thing::setThingProperty("firstObject", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation secondObject. (Cardinality: [0, 1])
	*/
	GeometricItem* getSecondObject()
	{
		auto property = Thing::getThingProperty<GeometricItem>("secondObject");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation secondObject. (Cardinality: [0, 1])
	*/
	void setSecondObject(GeometricItem* pValue)
	{
		Thing::setThingProperty("secondObject", (const _Thing**)&pValue, 1);
	}

	/**
	* This datatype property is named setting. (Cardinality: [0, 1])
	*/
	int64_t getSetting()
	{
		auto property = Thing::getDataProperty<int64_t>("setting");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named setting. (Cardinality: [0, 1])
	*/
	void setSetting(int64_t value)
	{
		Thing::setDataProperty("setting", &value, 1);
	}

	/**
	* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
	*/
	BooleanOperation_type getType()
	{
		auto property = Thing::getDataProperty<BooleanOperation_type>("type");
		if (property.second == 1) { return property.first[0]; }

		return (BooleanOperation_type)0;
	}

	/**
	* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
	*/
	void setType(BooleanOperation_type value)
	{
		Thing::setDataProperty("type", &value, 1);
	}

	/**
	* This object property represents the relation firstObject. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getFirstObject, put = setFirstObject)) GeometricItem* firstObject;

	/**
	* This object property represents the relation secondObject. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSecondObject, put = setSecondObject)) GeometricItem* secondObject;

	/**
	* This datatype property is named setting. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSetting, put = setSetting)) int64_t setting;

	/**
	* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getType, put = setType)) BooleanOperation_type type;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();
	}
};