#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "BezierCurve.h"

/**
* This class represents the concept RationalBezierCurve. (http://rdf.bg/RationalBezierCurve)
*/
class RationalBezierCurve : public BezierCurve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	RationalBezierCurve()
	{
	}

	virtual ~RationalBezierCurve()
	{
	}

	static const char* getClassName()
	{
		return "RationalBezierCurve";
	}

	/**
	* This datatype property is named weights. (Cardinality: [2, Inf])
	*/
	vector<double> getWeights()
	{
		auto property = Thing::getDataProperty<double>("weights");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named weights. (Cardinality: [2, Inf])
	*/
	void setWeights(const vector<double>& vecValues)
	{
		Thing::setDataProperty("weights", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named weights. (Cardinality: [2, Inf])
	*/
	__declspec(property(get = getWeights, put = setWeights)) vector<double> weights;

protected:

	virtual void postCreate()
	{
		BezierCurve::postCreate();
	}
};