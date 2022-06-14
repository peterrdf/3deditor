#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "SplineSurface.h"

/**
* This class represents the concept BSplineSurface. (http://rdf.bg/BSplineSurface)
*/
class BSplineSurface : public SplineSurface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	BSplineSurface()
	{
	}

	virtual ~BSplineSurface()
	{
	}

	static const char* getClassName()
	{
		return "BSplineSurface";
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
	* This datatype property is named uKnotMultiplicities. (Cardinality: [0, Inf])
	*/
	vector<int64_t> getUKnotMultiplicities()
	{
		auto property = Thing::getDataProperty<int64_t>("uKnotMultiplicities");
		vector<int64_t> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named uKnotMultiplicities. (Cardinality: [0, Inf])
	*/
	void setUKnotMultiplicities(const vector<int64_t>& vecValues)
	{
		Thing::setDataProperty("uKnotMultiplicities", (const int64_t*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named uKnots. (Cardinality: [2, Inf])
	*/
	vector<double> getUKnots()
	{
		auto property = Thing::getDataProperty<double>("uKnots");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named uKnots. (Cardinality: [2, Inf])
	*/
	void setUKnots(const vector<double>& vecValues)
	{
		Thing::setDataProperty("uKnots", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named vKnotMultiplicities. (Cardinality: [0, Inf])
	*/
	vector<int64_t> getVKnotMultiplicities()
	{
		auto property = Thing::getDataProperty<int64_t>("vKnotMultiplicities");
		vector<int64_t> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named vKnotMultiplicities. (Cardinality: [0, Inf])
	*/
	void setVKnotMultiplicities(const vector<int64_t>& vecValues)
	{
		Thing::setDataProperty("vKnotMultiplicities", (const int64_t*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named vKnots. (Cardinality: [2, Inf])
	*/
	vector<double> getVKnots()
	{
		auto property = Thing::getDataProperty<double>("vKnots");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named vKnots. (Cardinality: [2, Inf])
	*/
	void setVKnots(const vector<double>& vecValues)
	{
		Thing::setDataProperty("vKnots", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named setting. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSetting, put = setSetting)) int64_t setting;

	/**
	* This datatype property is named uKnotMultiplicities. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getUKnotMultiplicities, put = setUKnotMultiplicities)) vector<int64_t> uKnotMultiplicities;

	/**
	* This datatype property is named uKnots. (Cardinality: [2, Inf])
	*/
	__declspec(property(get = getUKnots, put = setUKnots)) vector<double> uKnots;

	/**
	* This datatype property is named vKnotMultiplicities. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getVKnotMultiplicities, put = setVKnotMultiplicities)) vector<int64_t> vKnotMultiplicities;

	/**
	* This datatype property is named vKnots. (Cardinality: [2, Inf])
	*/
	__declspec(property(get = getVKnots, put = setVKnots)) vector<double> vKnots;

protected:

	virtual void postCreate()
	{
		SplineSurface::postCreate();
	}
};