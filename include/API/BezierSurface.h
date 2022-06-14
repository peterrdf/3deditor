#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "SplineSurface.h"

/**
* This class represents the concept BezierSurface. (http://rdf.bg/BezierSurface)
*/
class BezierSurface : public SplineSurface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	BezierSurface()
	{
	}

	virtual ~BezierSurface()
	{
	}

	static const char* getClassName()
	{
		return "BezierSurface";
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
	* This datatype property is named setting. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSetting, put = setSetting)) int64_t setting;

protected:

	virtual void postCreate()
	{
		SplineSurface::postCreate();
	}
};