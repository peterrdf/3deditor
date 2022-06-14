#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Face.h"
#include "Curve.h"

/**
* This class represents the concept Face2D. (http://rdf.bg/Face2D)
*/
class Face2D : public Face
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Face2D()
	{
	}

	virtual ~Face2D()
	{
	}

	static const char* getClassName()
	{
		return "Face2D";
	}

	/**
	* This object property represents the relation innerPolygons. (Cardinality: [0, Inf])
	*/
	vector<Curve*> getInnerPolygons()
	{
		auto property = Thing::getThingProperty<Curve>("innerPolygons");
		return property;
	}

	/**
	* This object property represents the relation innerPolygons. (Cardinality: [0, Inf])
	*/
	void setInnerPolygons(const vector<Curve*>& vecValues)
	{
		Thing::setThingProperty("innerPolygons", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This object property represents the relation outerPolygon. (Cardinality: [1, 1])
	*/
	Curve* getOuterPolygon()
	{
		auto property = Thing::getThingProperty<Curve>("outerPolygon");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation outerPolygon. (Cardinality: [1, 1])
	*/
	void setOuterPolygon(Curve* pValue)
	{
		Thing::setThingProperty("outerPolygon", (const _Thing**)&pValue, 1);
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
	* This object property represents the relation innerPolygons. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getInnerPolygons, put = setInnerPolygons)) vector<Curve*> innerPolygons;

	/**
	* This object property represents the relation outerPolygon. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getOuterPolygon, put = setOuterPolygon)) Curve* outerPolygon;

	/**
	* This datatype property is named setting. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSetting, put = setSetting)) int64_t setting;

protected:

	virtual void postCreate()
	{
		Face::postCreate();
	}
};