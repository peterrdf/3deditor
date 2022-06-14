#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Face.h"
#include "Curve.h"

/**
* This class represents the concept Face2DSet. (http://rdf.bg/Face2DSet)
*/
class Face2DSet : public Face
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Face2DSet()
	{
	}

	virtual ~Face2DSet()
	{
	}

	static const char* getClassName()
	{
		return "Face2DSet";
	}

	/**
	* This object property represents the relation polygons. (Cardinality: [0, Inf])
	*/
	vector<Curve*> getPolygons()
	{
		auto property = Thing::getThingProperty<Curve>("polygons");
		return property;
	}

	/**
	* This object property represents the relation polygons. (Cardinality: [0, Inf])
	*/
	void setPolygons(const vector<Curve*>& vecValues)
	{
		Thing::setThingProperty("polygons", (const _Thing**)vecValues.data(), vecValues.size());
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
	* This object property represents the relation polygons. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getPolygons, put = setPolygons)) vector<Curve*> polygons;

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