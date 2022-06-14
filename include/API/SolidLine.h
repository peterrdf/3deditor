#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"

/**
* This class represents the concept SolidLine. (http://rdf.bg/SolidLine)
*/
class SolidLine : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	SolidLine()
	{
	}

	virtual ~SolidLine()
	{
	}

	static const char* getClassName()
	{
		return "SolidLine";
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 6])
	*/
	vector<double> getCoordinates()
	{
		auto property = Thing::getDataProperty<double>("coordinates");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 6])
	*/
	void setCoordinates(const vector<double>& vecValues)
	{
		Thing::setDataProperty("coordinates", (const double*)vecValues.data(), vecValues.size());
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
	* This datatype property is named thickness. (Cardinality: [1, 1])
	*/
	double getThickness()
	{
		auto property = Thing::getDataProperty<double>("thickness");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named thickness. (Cardinality: [1, 1])
	*/
	void setThickness(double value)
	{
		Thing::setDataProperty("thickness", &value, 1);
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 6])
	*/
	__declspec(property(get = getCoordinates, put = setCoordinates)) vector<double> coordinates;

	/**
	* This datatype property is named segmentationParts. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSegmentationParts, put = setSegmentationParts)) int64_t segmentationParts;

	/**
	* This datatype property is named thickness. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getThickness, put = setThickness)) double thickness;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();
	}
};