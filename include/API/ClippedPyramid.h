#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"

/**
* This class represents the concept ClippedPyramid. (http://rdf.bg/ClippedPyramid)
*/
class ClippedPyramid : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	ClippedPyramid()
	{
	}

	virtual ~ClippedPyramid()
	{
	}

	static const char* getClassName()
	{
		return "ClippedPyramid";
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [12, 12])
	*/
	vector<double> getCoordinates()
	{
		auto property = Thing::getDataProperty<double>("coordinates");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [12, 12])
	*/
	void setCoordinates(const vector<double>& vecValues)
	{
		Thing::setDataProperty("coordinates", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [12, 12])
	*/
	__declspec(property(get = getCoordinates, put = setCoordinates)) vector<double> coordinates;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();
	}
};