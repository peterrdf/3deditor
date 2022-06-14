#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"

/**
* This class represents the concept TransitionalCurve. (http://rdf.bg/TransitionalCurve)
*/
class TransitionalCurve : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	TransitionalCurve()
	{
	}

	virtual ~TransitionalCurve()
	{
	}

	static const char* getClassName()
	{
		return "TransitionalCurve";
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
	* This object property represents the relation path. (Cardinality: [0, 1])
	*/
	Curve* getPath()
	{
		auto property = Thing::getThingProperty<Curve>("path");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation path. (Cardinality: [0, 1])
	*/
	void setPath(Curve* pValue)
	{
		Thing::setThingProperty("path", (const _Thing**)&pValue, 1);
	}

	/**
	* This datatype property is named radiusI. (Cardinality: [0, 1])
	*/
	double getRadiusI()
	{
		auto property = Thing::getDataProperty<double>("radiusI");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named radiusI. (Cardinality: [0, 1])
	*/
	void setRadiusI(double value)
	{
		Thing::setDataProperty("radiusI", &value, 1);
	}

	/**
	* This datatype property is named radiusII. (Cardinality: [0, 1])
	*/
	double getRadiusII()
	{
		auto property = Thing::getDataProperty<double>("radiusII");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named radiusII. (Cardinality: [0, 1])
	*/
	void setRadiusII(double value)
	{
		Thing::setDataProperty("radiusII", &value, 1);
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
	* This datatype property is named length. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getLength, put = setLength)) double length;

	/**
	* This object property represents the relation path. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getPath, put = setPath)) Curve* path;

	/**
	* This datatype property is named radiusI. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getRadiusI, put = setRadiusI)) double radiusI;

	/**
	* This datatype property is named radiusII. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getRadiusII, put = setRadiusII)) double radiusII;

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