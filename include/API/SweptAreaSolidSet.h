#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"
#include "Curve.h"

/**
* This class represents the concept SweptAreaSolidSet. (http://rdf.bg/SweptAreaSolidSet)
*/
class SweptAreaSolidSet : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	SweptAreaSolidSet()
	{
	}

	virtual ~SweptAreaSolidSet()
	{
	}

	static const char* getClassName()
	{
		return "SweptAreaSolidSet";
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
	* This object property represents the relation path. (Cardinality: [1, 1])
	*/
	Curve* getPath()
	{
		auto property = Thing::getThingProperty<Curve>("path");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation path. (Cardinality: [1, 1])
	*/
	void setPath(Curve* pValue)
	{
		Thing::setThingProperty("path", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation sweptAreaSet. (Cardinality: [1, Inf])
	*/
	vector<Curve*> getSweptAreaSet()
	{
		auto property = Thing::getThingProperty<Curve>("sweptAreaSet");
		return property;
	}

	/**
	* This object property represents the relation sweptAreaSet. (Cardinality: [1, Inf])
	*/
	void setSweptAreaSet(const vector<Curve*>& vecValues)
	{
		Thing::setThingProperty("sweptAreaSet", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named fraction. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getFraction, put = setFraction)) double fraction;

	/**
	* This object property represents the relation path. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getPath, put = setPath)) Curve* path;

	/**
	* This object property represents the relation sweptAreaSet. (Cardinality: [1, Inf])
	*/
	__declspec(property(get = getSweptAreaSet, put = setSweptAreaSet)) vector<Curve*> sweptAreaSet;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();
	}
};