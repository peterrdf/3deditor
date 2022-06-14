#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Environment.h"
#include "Matrix.h"

/**
* This class represents the concept View. (http://rdf.bg/View)
*/
class View : public Environment
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	View()
	{
	}

	virtual ~View()
	{
	}

	static const char* getClassName()
	{
		return "View";
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
	* This object property represents the relation matrix. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getMatrix, put = setMatrix)) Matrix* matrix;

protected:

	virtual void postCreate()
	{
		Environment::postCreate();
	}
};