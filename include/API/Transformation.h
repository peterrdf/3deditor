#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "GeometricItem.h"
#include "Matrix.h"

/**
* This class represents the concept Transformation. (http://rdf.bg/Transformation)
*/
class Transformation : public GeometricItem
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Transformation()
	{
	}

	virtual ~Transformation()
	{
	}

	static const char* getClassName()
	{
		return "Transformation";
	}

	/**
	* This object property represents the relation matrix. (Cardinality: [0, 1])
	*/
	Matrix* getMatrix()
	{
		auto property = Thing::getThingProperty<Matrix>("matrix");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation matrix. (Cardinality: [0, 1])
	*/
	void setMatrix(Matrix* pValue)
	{
		Thing::setThingProperty("matrix", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation object. (Cardinality: [0, 1])
	*/
	GeometricItem* getObject()
	{
		auto property = Thing::getThingProperty<GeometricItem>("object");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation object. (Cardinality: [0, 1])
	*/
	void setObject(GeometricItem* pValue)
	{
		Thing::setThingProperty("object", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation matrix. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getMatrix, put = setMatrix)) Matrix* matrix;

	/**
	* This object property represents the relation object. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getObject, put = setObject)) GeometricItem* object;

protected:

	virtual void postCreate()
	{
		GeometricItem::postCreate();
	}
};