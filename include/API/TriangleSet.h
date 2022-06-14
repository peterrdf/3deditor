#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Surface.h"

/**
* This class represents the concept TriangleSet. (http://rdf.bg/TriangleSet)
*/
class TriangleSet : public Surface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	TriangleSet()
	{
	}

	virtual ~TriangleSet()
	{
	}

	static const char* getClassName()
	{
		return "TriangleSet";
	}

	/**
	* This datatype property is named indices. (Cardinality: [0, Inf])
	*/
	vector<int64_t> getIndices()
	{
		auto property = Thing::getDataProperty<int64_t>("indices");
		vector<int64_t> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named indices. (Cardinality: [0, Inf])
	*/
	void setIndices(const vector<int64_t>& vecValues)
	{
		Thing::setDataProperty("indices", (const int64_t*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named vertices. (Cardinality: [3, Inf])
	*/
	vector<double> getVertices()
	{
		auto property = Thing::getDataProperty<double>("vertices");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named vertices. (Cardinality: [3, Inf])
	*/
	void setVertices(const vector<double>& vecValues)
	{
		Thing::setDataProperty("vertices", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named indices. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getIndices, put = setIndices)) vector<int64_t> indices;

	/**
	* This datatype property is named vertices. (Cardinality: [3, Inf])
	*/
	__declspec(property(get = getVertices, put = setVertices)) vector<double> vertices;

protected:

	virtual void postCreate()
	{
		Surface::postCreate();
	}
};