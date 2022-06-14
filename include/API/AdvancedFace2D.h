#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "AdvancedFace.h"
#include "Face.h"

/**
* This class represents the concept AdvancedFace2D. (http://rdf.bg/AdvancedFace2D)
*/
class AdvancedFace2D : public AdvancedFace
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	AdvancedFace2D()
	{
	}

	virtual ~AdvancedFace2D()
	{
	}

	static const char* getClassName()
	{
		return "AdvancedFace2D";
	}

	/**
	* This datatype property is named clipped. (Cardinality: [0, 1])
	*/
	bool getClipped()
	{
		auto property = Thing::getDataProperty<bool>("clipped");
		if (property.second == 1) { return property.first[0]; }

		return (bool)0;
	}

	/**
	* This datatype property is named clipped. (Cardinality: [0, 1])
	*/
	void setClipped(bool value)
	{
		Thing::setDataProperty("clipped", &value, 1);
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
	* This datatype property is named clipped. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getClipped, put = setClipped)) bool clipped;

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