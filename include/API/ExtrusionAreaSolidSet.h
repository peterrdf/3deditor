#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"
#include "Curve.h"

/**
* This class represents the concept ExtrusionAreaSolidSet. (http://rdf.bg/ExtrusionAreaSolidSet)
*/
class ExtrusionAreaSolidSet : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	ExtrusionAreaSolidSet()
	{
	}

	virtual ~ExtrusionAreaSolidSet()
	{
	}

	static const char* getClassName()
	{
		return "ExtrusionAreaSolidSet";
	}

	/**
	* This object property represents the relation extrusionAreaSet. (Cardinality: [1, Inf])
	*/
	vector<Curve*> getExtrusionAreaSet()
	{
		auto property = Thing::getThingProperty<Curve>("extrusionAreaSet");
		return property;
	}

	/**
	* This object property represents the relation extrusionAreaSet. (Cardinality: [1, Inf])
	*/
	void setExtrusionAreaSet(const vector<Curve*>& vecValues)
	{
		Thing::setThingProperty("extrusionAreaSet", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named extrusionDirection. (Cardinality: [0, 3])
	*/
	vector<double> getExtrusionDirection()
	{
		auto property = Thing::getDataProperty<double>("extrusionDirection");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named extrusionDirection. (Cardinality: [0, 3])
	*/
	void setExtrusionDirection(const vector<double>& vecValues)
	{
		Thing::setDataProperty("extrusionDirection", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named extrusionLength. (Cardinality: [1, 1])
	*/
	double getExtrusionLength()
	{
		auto property = Thing::getDataProperty<double>("extrusionLength");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named extrusionLength. (Cardinality: [1, 1])
	*/
	void setExtrusionLength(double value)
	{
		Thing::setDataProperty("extrusionLength", &value, 1);
	}

	/**
	* This datatype property is named fraction. (Cardinality: [0, 1])
	*/
	double getFraction()
	{
		auto property = Thing::getDataProperty<double>("fraction");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named fraction. (Cardinality: [0, 1])
	*/
	void setFraction(double value)
	{
		Thing::setDataProperty("fraction", &value, 1);
	}

	/**
	* This object property represents the relation extrusionAreaSet. (Cardinality: [1, Inf])
	*/
	__declspec(property(get = getExtrusionAreaSet, put = setExtrusionAreaSet)) vector<Curve*> extrusionAreaSet;

	/**
	* This datatype property is named extrusionDirection. (Cardinality: [0, 3])
	*/
	__declspec(property(get = getExtrusionDirection, put = setExtrusionDirection)) vector<double> extrusionDirection;

	/**
	* This datatype property is named extrusionLength. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getExtrusionLength, put = setExtrusionLength)) double extrusionLength;

	/**
	* This datatype property is named fraction. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getFraction, put = setFraction)) double fraction;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();
	}
};