#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"
#include "Vector.h"

/**
* This class represents the concept Clothoid (Euler spiral). Length is defined away from the origin (negative length means going in the direction of the origin). (http://rdf.bg/Clothoid)
*/
class Clothoid : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Clothoid()
	{
	}

	virtual ~Clothoid()
	{
	}

	static const char* getClassName()
	{
		return "Clothoid";
	}

	/**
	* This datatype property is named A. (Cardinality: [1, 1])
	*/
	double getA()
	{
		auto property = Thing::getDataProperty<double>("A");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named A. (Cardinality: [1, 1])
	*/
	void setA(double value)
	{
		Thing::setDataProperty("A", &value, 1);
	}

	/**
	* This object property represents the relation direction. (Cardinality: [1, 1])
	*/
	Vector* getDirection()
	{
		auto property = Thing::getThingProperty<Vector>("direction");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation direction. (Cardinality: [1, 1])
	*/
	void setDirection(Vector* pValue)
	{
		Thing::setThingProperty("direction", (const _Thing**)&pValue, 1);
	}

	/**
	* This datatype property is named length. (Cardinality: [1, 1])
	*/
	double getLength()
	{
		auto property = Thing::getDataProperty<double>("length");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named length. (Cardinality: [1, 1])
	*/
	void setLength(double value)
	{
		Thing::setDataProperty("length", &value, 1);
	}

	/**
	* value 0 (or cardinality is zero) means clockwise, any other value means counter-clockwise. (Cardinality: [0, 1])
	*/
	int64_t getOrientation()
	{
		auto property = Thing::getDataProperty<int64_t>("orientation");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* value 0 (or cardinality is zero) means clockwise, any other value means counter-clockwise. (Cardinality: [0, 1])
	*/
	void setOrientation(int64_t value)
	{
		Thing::setDataProperty("orientation", &value, 1);
	}

	/**
	* This datatype property is named radius. (Cardinality: [0, 1])
	*/
	double getRadius()
	{
		auto property = Thing::getDataProperty<double>("radius");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named radius. (Cardinality: [0, 1])
	*/
	void setRadius(double value)
	{
		Thing::setDataProperty("radius", &value, 1);
	}

	/**
	* This datatype property is named segmentationParts. (Cardinality: [0, 1])
	*/
	int64_t getSegmentationParts()
	{
		auto property = Thing::getDataProperty<int64_t>("segmentationParts");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named segmentationParts. (Cardinality: [0, 1])
	*/
	void setSegmentationParts(int64_t value)
	{
		Thing::setDataProperty("segmentationParts", &value, 1);
	}

	/**
	* This datatype property is named A. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getA, put = setA)) double A;

	/**
	* This object property represents the relation direction. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getDirection, put = setDirection)) Vector* direction;

	/**
	* This datatype property is named length. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getLength, put = setLength)) double length;

	/**
	* value 0 (or cardinality is zero) means clockwise, any other value means counter-clockwise. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getOrientation, put = setOrientation)) int64_t orientation;

	/**
	* This datatype property is named radius. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getRadius, put = setRadius)) double radius;

	/**
	* This datatype property is named segmentationParts. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSegmentationParts, put = setSegmentationParts)) int64_t segmentationParts;

protected:

	virtual void postCreate()
	{
		Curve::postCreate();
	}
};