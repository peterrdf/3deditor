#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Matrix.h"

/**
* This class represents the concept MatrixMultiplication. (http://rdf.bg/MatrixMultiplication)
*/
class MatrixMultiplication : public Matrix
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	MatrixMultiplication()
	{
	}

	virtual ~MatrixMultiplication()
	{
	}

	static const char* getClassName()
	{
		return "MatrixMultiplication";
	}

	/**
	* This object property represents the relation firstMatrix. (Cardinality: [1, 1])
	*/
	Matrix* getFirstMatrix()
	{
		auto property = Thing::getThingProperty<Matrix>("firstMatrix");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation firstMatrix. (Cardinality: [1, 1])
	*/
	void setFirstMatrix(Matrix* pValue)
	{
		Thing::setThingProperty("firstMatrix", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation secondMatrix. (Cardinality: [1, 1])
	*/
	Matrix* getSecondMatrix()
	{
		auto property = Thing::getThingProperty<Matrix>("secondMatrix");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation secondMatrix. (Cardinality: [1, 1])
	*/
	void setSecondMatrix(Matrix* pValue)
	{
		Thing::setThingProperty("secondMatrix", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation firstMatrix. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getFirstMatrix, put = setFirstMatrix)) Matrix* firstMatrix;

	/**
	* This object property represents the relation secondMatrix. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getSecondMatrix, put = setSecondMatrix)) Matrix* secondMatrix;

protected:

	virtual void postCreate()
	{
		Matrix::postCreate();
	}
};