#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "SplineCurve.h"

/**
* This class represents the concept BSplineCurve, also called a Basis Spline Curve. (http://rdf.bg/BSplineCurve)
*/
class BSplineCurve : public SplineCurve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	BSplineCurve()
	{
	}

	virtual ~BSplineCurve()
	{
	}

	static const char* getClassName()
	{
		return "BSplineCurve";
	}

	/**
	* This datatype property is named knotMultiplicities. (Cardinality: [0, Inf])
	*/
	vector<int64_t> getKnotMultiplicities()
	{
		auto property = Thing::getDataProperty<int64_t>("knotMultiplicities");
		vector<int64_t> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named knotMultiplicities. (Cardinality: [0, Inf])
	*/
	void setKnotMultiplicities(const vector<int64_t>& vecValues)
	{
		Thing::setDataProperty("knotMultiplicities", (const int64_t*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named knots. (Cardinality: [2, Inf])
	*/
	vector<double> getKnots()
	{
		auto property = Thing::getDataProperty<double>("knots");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named knots. (Cardinality: [2, Inf])
	*/
	void setKnots(const vector<double>& vecValues)
	{
		Thing::setDataProperty("knots", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named setting. (Cardinality: [0, 1])
	*/
	int64_t getSetting()
	{
		auto property = Thing::getDataProperty<int64_t>("setting");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named setting. (Cardinality: [0, 1])
	*/
	void setSetting(int64_t value)
	{
		Thing::setDataProperty("setting", &value, 1);
	}

	/**
	* This datatype property is named knotMultiplicities. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getKnotMultiplicities, put = setKnotMultiplicities)) vector<int64_t> knotMultiplicities;

	/**
	* This datatype property is named knots. (Cardinality: [2, Inf])
	*/
	__declspec(property(get = getKnots, put = setKnots)) vector<double> knots;

	/**
	* This datatype property is named setting. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSetting, put = setSetting)) int64_t setting;

protected:

	virtual void postCreate()
	{
		SplineCurve::postCreate();
	}
};