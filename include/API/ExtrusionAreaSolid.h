#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"
#include "Curve.h"

/**
* This class represents the concept ExtrusionAreaSolid. (http://rdf.bg/ExtrusionAreaSolid)
*/
class ExtrusionAreaSolid : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	ExtrusionAreaSolid()
	{
	}

	virtual ~ExtrusionAreaSolid()
	{
	}

	static const char* getClassName()
	{
		return "ExtrusionAreaSolid";
	}

	/**
	* This object property represents the relation extrusionArea. (Cardinality: [1, 1])
	*/
	Curve* getExtrusionArea()
	{
		auto property = Thing::getThingProperty<Curve>("extrusionArea");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation extrusionArea. (Cardinality: [1, 1])
	*/
	void setExtrusionArea(Curve* pValue)
	{
		Thing::setThingProperty("extrusionArea", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation extrusionAreaOpenings. (Cardinality: [0, Inf])
	*/
	vector<Curve*> getExtrusionAreaOpenings()
	{
		auto property = Thing::getThingProperty<Curve>("extrusionAreaOpenings");
		return property;
	}

	/**
	* This object property represents the relation extrusionAreaOpenings. (Cardinality: [0, Inf])
	*/
	void setExtrusionAreaOpenings(const vector<Curve*>& vecValues)
	{
		Thing::setThingProperty("extrusionAreaOpenings", (const _Thing**)vecValues.data(), vecValues.size());
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
	* This object property represents the relation extrusionArea. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getExtrusionArea, put = setExtrusionArea)) Curve* extrusionArea;

	/**
	* This object property represents the relation extrusionAreaOpenings. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getExtrusionAreaOpenings, put = setExtrusionAreaOpenings)) vector<Curve*> extrusionAreaOpenings;

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