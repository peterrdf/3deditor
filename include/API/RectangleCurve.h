#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"

/**
* This class represents the concept RectangleCurve. (http://rdf.bg/RectangleCurve)
*/
class RectangleCurve : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	RectangleCurve()
	{
	}

	virtual ~RectangleCurve()
	{
	}

	static const char* getClassName()
	{
		return "RectangleCurve";
	}

	/**
	* This datatype property is named height. (Cardinality: [1, 1])
	*/
	double getHeight()
	{
		auto property = Thing::getDataProperty<double>("height");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named height. (Cardinality: [1, 1])
	*/
	void setHeight(double value)
	{
		Thing::setDataProperty("height", &value, 1);
	}

	/**
	* This datatype property is named offsetX. (Cardinality: [0, 1])
	*/
	double getOffsetX()
	{
		auto property = Thing::getDataProperty<double>("offsetX");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named offsetX. (Cardinality: [0, 1])
	*/
	void setOffsetX(double value)
	{
		Thing::setDataProperty("offsetX", &value, 1);
	}

	/**
	* This datatype property is named offsetY. (Cardinality: [0, 1])
	*/
	double getOffsetY()
	{
		auto property = Thing::getDataProperty<double>("offsetY");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named offsetY. (Cardinality: [0, 1])
	*/
	void setOffsetY(double value)
	{
		Thing::setDataProperty("offsetY", &value, 1);
	}

	/**
	* This datatype property is named offsetZ. (Cardinality: [0, 1])
	*/
	double getOffsetZ()
	{
		auto property = Thing::getDataProperty<double>("offsetZ");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named offsetZ. (Cardinality: [0, 1])
	*/
	void setOffsetZ(double value)
	{
		Thing::setDataProperty("offsetZ", &value, 1);
	}

	/**
	* This datatype property is named width. (Cardinality: [1, 1])
	*/
	double getWidth()
	{
		auto property = Thing::getDataProperty<double>("width");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named width. (Cardinality: [1, 1])
	*/
	void setWidth(double value)
	{
		Thing::setDataProperty("width", &value, 1);
	}

	/**
	* This datatype property is named height. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getHeight, put = setHeight)) double height;

	/**
	* This datatype property is named offsetX. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getOffsetX, put = setOffsetX)) double offsetX;

	/**
	* This datatype property is named offsetY. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getOffsetY, put = setOffsetY)) double offsetY;

	/**
	* This datatype property is named offsetZ. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getOffsetZ, put = setOffsetZ)) double offsetZ;

	/**
	* This datatype property is named width. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getWidth, put = setWidth)) double width;

protected:

	virtual void postCreate()
	{
		Curve::postCreate();
	}
};