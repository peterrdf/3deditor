#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "FiniteSurface.h"
#include "Point3D.h"

/**
* This class represents the abstract concept SplineSurface. (http://rdf.bg/SplineSurface)
*/
class SplineSurface : public FiniteSurface
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	SplineSurface()
	{
	}

	virtual ~SplineSurface()
	{
	}

	static const char* getClassName()
	{
		return "SplineSurface";
	}

	/**
	* This object property represents the relation controlPoints. (Cardinality: [4, Inf])
	*/
	vector<Point3D*> getControlPoints()
	{
		auto property = Thing::getThingProperty<Point3D>("controlPoints");
		return property;
	}

	/**
	* This object property represents the relation controlPoints. (Cardinality: [4, Inf])
	*/
	void setControlPoints(const vector<Point3D*>& vecValues)
	{
		Thing::setThingProperty("controlPoints", (const _Thing**)vecValues.data(), vecValues.size());
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
	* This datatype property is named uClosed. (Cardinality: [0, 1])
	*/
	bool getUClosed()
	{
		auto property = Thing::getDataProperty<bool>("uClosed");
		if (property.second == 1) { return property.first[0]; }

		return (bool)0;
	}

	/**
	* This datatype property is named uClosed. (Cardinality: [0, 1])
	*/
	void setUClosed(bool value)
	{
		Thing::setDataProperty("uClosed", &value, 1);
	}

	/**
	* This datatype property is named uCount. (Cardinality: [1, 1])
	*/
	int64_t getUCount()
	{
		auto property = Thing::getDataProperty<int64_t>("uCount");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named uCount. (Cardinality: [1, 1])
	*/
	void setUCount(int64_t value)
	{
		Thing::setDataProperty("uCount", &value, 1);
	}

	/**
	* This datatype property is named uDegree. (Cardinality: [0, 1])
	*/
	int64_t getUDegree()
	{
		auto property = Thing::getDataProperty<int64_t>("uDegree");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named uDegree. (Cardinality: [0, 1])
	*/
	void setUDegree(int64_t value)
	{
		Thing::setDataProperty("uDegree", &value, 1);
	}

	/**
	* This datatype property is named uSegmentationParts. (Cardinality: [0, 1])
	*/
	int64_t getUSegmentationParts()
	{
		auto property = Thing::getDataProperty<int64_t>("uSegmentationParts");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named uSegmentationParts. (Cardinality: [0, 1])
	*/
	void setUSegmentationParts(int64_t value)
	{
		Thing::setDataProperty("uSegmentationParts", &value, 1);
	}

	/**
	* This datatype property is named vClosed. (Cardinality: [0, 1])
	*/
	bool getVClosed()
	{
		auto property = Thing::getDataProperty<bool>("vClosed");
		if (property.second == 1) { return property.first[0]; }

		return (bool)0;
	}

	/**
	* This datatype property is named vClosed. (Cardinality: [0, 1])
	*/
	void setVClosed(bool value)
	{
		Thing::setDataProperty("vClosed", &value, 1);
	}

	/**
	* This datatype property is named vCount. (Cardinality: [1, 1])
	*/
	int64_t getVCount()
	{
		auto property = Thing::getDataProperty<int64_t>("vCount");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named vCount. (Cardinality: [1, 1])
	*/
	void setVCount(int64_t value)
	{
		Thing::setDataProperty("vCount", &value, 1);
	}

	/**
	* This datatype property is named vDegree. (Cardinality: [0, 1])
	*/
	int64_t getVDegree()
	{
		auto property = Thing::getDataProperty<int64_t>("vDegree");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named vDegree. (Cardinality: [0, 1])
	*/
	void setVDegree(int64_t value)
	{
		Thing::setDataProperty("vDegree", &value, 1);
	}

	/**
	* This datatype property is named vSegmentationParts. (Cardinality: [0, 1])
	*/
	int64_t getVSegmentationParts()
	{
		auto property = Thing::getDataProperty<int64_t>("vSegmentationParts");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named vSegmentationParts. (Cardinality: [0, 1])
	*/
	void setVSegmentationParts(int64_t value)
	{
		Thing::setDataProperty("vSegmentationParts", &value, 1);
	}

	/**
	* This object property represents the relation controlPoints. (Cardinality: [4, Inf])
	*/
	__declspec(property(get = getControlPoints, put = setControlPoints)) vector<Point3D*> controlPoints;

	/**
	* This datatype property is named segmentationLength. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSegmentationLength, put = setSegmentationLength)) double segmentationLength;

	/**
	* This datatype property is named segmentationParts. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSegmentationParts, put = setSegmentationParts)) int64_t segmentationParts;

	/**
	* This datatype property is named uClosed. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getUClosed, put = setUClosed)) bool uClosed;

	/**
	* This datatype property is named uCount. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getUCount, put = setUCount)) int64_t uCount;

	/**
	* This datatype property is named uDegree. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getUDegree, put = setUDegree)) int64_t uDegree;

	/**
	* This datatype property is named uSegmentationParts. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getUSegmentationParts, put = setUSegmentationParts)) int64_t uSegmentationParts;

	/**
	* This datatype property is named vClosed. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getVClosed, put = setVClosed)) bool vClosed;

	/**
	* This datatype property is named vCount. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getVCount, put = setVCount)) int64_t vCount;

	/**
	* This datatype property is named vDegree. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getVDegree, put = setVDegree)) int64_t vDegree;

	/**
	* This datatype property is named vSegmentationParts. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getVSegmentationParts, put = setVSegmentationParts)) int64_t vSegmentationParts;

protected:

	virtual void postCreate()
	{
		FiniteSurface::postCreate();
	}
};