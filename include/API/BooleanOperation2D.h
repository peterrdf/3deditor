#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Face.h"
#include "GeometricItem.h"

/**
* This class represents the concept BooleanOperation2D. (http://rdf.bg/BooleanOperation2D)
*/
class BooleanOperation2D : public Face
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

	BooleanOperation2D()
	{
	}

	virtual ~BooleanOperation2D()
	{
	}

	static const char* getClassName()
	{
		return "BooleanOperation2D";
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
	* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getType, put = setType)) BooleanOperation_type type;

protected:

	virtual void postCreate()
	{
		Face::postCreate();
	}
};