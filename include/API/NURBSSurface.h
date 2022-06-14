#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "BSplineSurface.h"

/**
* This class represents the concept NURBSSurface, also called a Non-uniform rational B-spline (non-uniform rational basis spline). (http://rdf.bg/NURBSSurface)
*/
class NURBSSurface : public BSplineSurface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	NURBSSurface()
	{
	}

	virtual ~NURBSSurface()
	{
	}

	static const char* getClassName()
	{
		return "NURBSSurface";
	}

	/**
	* This datatype property is named weights. (Cardinality: [4, Inf])
	*/
	vector<double> getWeights()
	{
		auto property = Thing::getDataProperty<double>("weights");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named weights. (Cardinality: [4, Inf])
	*/
	void setWeights(const vector<double>& vecValues)
	{
		Thing::setDataProperty("weights", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named weights. (Cardinality: [4, Inf])
	*/
	__declspec(property(get = getWeights, put = setWeights)) vector<double> weights;

protected:

	virtual void postCreate()
	{
		BSplineSurface::postCreate();
	}
};