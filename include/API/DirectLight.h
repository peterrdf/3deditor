#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Light.h"

/**
* This class represents the abstract concept DirectLight. (http://rdf.bg/DirectLight)
*/
class DirectLight : public Light
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	DirectLight()
	{
	}

	virtual ~DirectLight()
	{
	}

	static const char* getClassName()
	{
		return "DirectLight";
	}

	/**
	* This datatype property is named attenuation. (Cardinality: [0, 1])
	*/
	double getAttenuation()
	{
		auto property = Thing::getDataProperty<double>("attenuation");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named attenuation. (Cardinality: [0, 1])
	*/
	void setAttenuation(double value)
	{
		Thing::setDataProperty("attenuation", &value, 1);
	}

	/**
	* This datatype property is named range. (Cardinality: [0, 1])
	*/
	double getRange()
	{
		auto property = Thing::getDataProperty<double>("range");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named range. (Cardinality: [0, 1])
	*/
	void setRange(double value)
	{
		Thing::setDataProperty("range", &value, 1);
	}

	/**
	* This datatype property is named attenuation. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getAttenuation, put = setAttenuation)) double attenuation;

	/**
	* This datatype property is named range. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getRange, put = setRange)) double range;

protected:

	virtual void postCreate()
	{
		Light::postCreate();
	}
};