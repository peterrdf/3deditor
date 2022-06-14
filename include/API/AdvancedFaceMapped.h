#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "AdvancedFace.h"
#include "Curve.h"

/**
* This class represents the concept AdvancedFaceMapped. (http://rdf.bg/AdvancedFaceMapped)
*/
class AdvancedFaceMapped : public AdvancedFace
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	AdvancedFaceMapped()
	{
	}

	virtual ~AdvancedFaceMapped()
	{
	}

	static const char* getClassName()
	{
		return "AdvancedFaceMapped";
	}

	/**
	* This datatype property is named scalingX. (Cardinality: [0, 1])
	*/
	double getScalingX()
	{
		auto property = Thing::getDataProperty<double>("scalingX");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named scalingX. (Cardinality: [0, 1])
	*/
	void setScalingX(double value)
	{
		Thing::setDataProperty("scalingX", &value, 1);
	}

	/**
	* This datatype property is named scalingY. (Cardinality: [0, 1])
	*/
	double getScalingY()
	{
		auto property = Thing::getDataProperty<double>("scalingY");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named scalingY. (Cardinality: [0, 1])
	*/
	void setScalingY(double value)
	{
		Thing::setDataProperty("scalingY", &value, 1);
	}

	/**
	* This datatype property is named scalingX. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getScalingX, put = setScalingX)) double scalingX;

	/**
	* This datatype property is named scalingY. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getScalingY, put = setScalingY)) double scalingY;

protected:

	virtual void postCreate()
	{
		AdvancedFace::postCreate();
	}
};