#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "GeometricItem.h"
#include "Matrix.h"

/**
* This class represents the concept Repetition. (http://rdf.bg/Repetition)
*/
class Repetition : public GeometricItem
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Repetition()
	{
	}

	virtual ~Repetition()
	{
	}

	static const char* getClassName()
	{
		return "Repetition";
	}

	/**
	* This datatype property is named count. (Cardinality: [1, 1])
	*/
	int64_t getCount()
	{
		auto property = Thing::getDataProperty<int64_t>("count");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named count. (Cardinality: [1, 1])
	*/
	void setCount(int64_t value)
	{
		Thing::setDataProperty("count", &value, 1);
	}

	/**
	* This object property represents the relation matrix. (Cardinality: [1, 1])
	*/
	Matrix* getMatrix()
	{
		auto property = Thing::getThingProperty<Matrix>("matrix");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation matrix. (Cardinality: [1, 1])
	*/
	void setMatrix(Matrix* pValue)
	{
		Thing::setThingProperty("matrix", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation object. (Cardinality: [1, 1])
	*/
	GeometricItem* getObject()
	{
		auto property = Thing::getThingProperty<GeometricItem>("object");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation object. (Cardinality: [1, 1])
	*/
	void setObject(GeometricItem* pValue)
	{
		Thing::setThingProperty("object", (const _Thing**)&pValue, 1);
	}

	/**
	* This datatype property is named count. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getCount, put = setCount)) int64_t count;

	/**
	* This object property represents the relation matrix. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getMatrix, put = setMatrix)) Matrix* matrix;

	/**
	* This object property represents the relation object. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getObject, put = setObject)) GeometricItem* object;

protected:

	virtual void postCreate()
	{
		GeometricItem::postCreate();
	}
};