#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Matrix.h"

/**
* This class represents the concept InverseMatrix. (http://rdf.bg/InverseMatrix)
*/
class InverseMatrix : public Matrix
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	InverseMatrix()
	{
	}

	virtual ~InverseMatrix()
	{
	}

	static const char* getClassName()
	{
		return "InverseMatrix";
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
	* This object property represents the relation matrix. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getMatrix, put = setMatrix)) Matrix* matrix;

protected:

	virtual void postCreate()
	{
		Matrix::postCreate();
	}
};