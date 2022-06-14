#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"
#include "Point3D.h"

/**
* This class represents the abstract concept SplineCurve, also called a Spline Curve. (http://rdf.bg/SplineCurve)
*/
class SplineCurve : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	SplineCurve()
	{
	}

	virtual ~SplineCurve()
	{
	}

	static const char* getClassName()
	{
		return "SplineCurve";
	}

	/**
	* This datatype property is named closed. (Cardinality: [0, 1])
	*/
	bool getClosed()
	{
		auto property = Thing::getDataProperty<bool>("closed");
		if (property.second == 1) { return property.first[0]; }

		return (bool)0;
	}

	/**
	* This datatype property is named closed. (Cardinality: [0, 1])
	*/
	void setClosed(bool value)
	{
		Thing::setDataProperty("closed", &value, 1);
	}

	/**
	* This object property represents the relation controlPoints. (Cardinality: [2, Inf])
	*/
	vector<Point3D*> getControlPoints()
	{
		auto property = Thing::getThingProperty<Point3D>("controlPoints");
		return property;
	}

	/**
	* This object property represents the relation controlPoints. (Cardinality: [2, Inf])
	*/
	void setControlPoints(const vector<Point3D*>& vecValues)
	{
		Thing::setThingProperty("controlPoints", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named count. (Cardinality: [0, 1])
	*/
	int64_t getCount()
	{
		auto property = Thing::getDataProperty<int64_t>("count");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named count. (Cardinality: [0, 1])
	*/
	void setCount(int64_t value)
	{
		Thing::setDataProperty("count", &value, 1);
	}

	/**
	* This datatype property is named degree. (Cardinality: [0, 1])
	*/
	int64_t getDegree()
	{
		auto property = Thing::getDataProperty<int64_t>("degree");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named degree. (Cardinality: [0, 1])
	*/
	void setDegree(int64_t value)
	{
		Thing::setDataProperty("degree", &value, 1);
	}

	/**
	* This datatype property is named segmentationLength. (Cardinality: [0, 1])
	*/
	double getSegmentationLength()
	{
		auto property = Thing::getDataProperty<double>("segmentationLength");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named segmentationLength. (Cardinality: [0, 1])
	*/
	void setSegmentationLength(double value)
	{
		Thing::setDataProperty("segmentationLength", &value, 1);
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
	* This datatype property is named closed. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getClosed, put = setClosed)) bool closed;

	/**
	* This object property represents the relation controlPoints. (Cardinality: [2, Inf])
	*/
	__declspec(property(get = getControlPoints, put = setControlPoints)) vector<Point3D*> controlPoints;

	/**
	* This datatype property is named count. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getCount, put = setCount)) int64_t count;

	/**
	* This datatype property is named degree. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getDegree, put = setDegree)) int64_t degree;

	/**
	* This datatype property is named segmentationLength. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSegmentationLength, put = setSegmentationLength)) double segmentationLength;

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