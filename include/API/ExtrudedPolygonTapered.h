#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"

/**
* This class represents the concept ExtrudedPolygonTapered. (http://rdf.bg/ExtrudedPolygonTapered)
*/
class ExtrudedPolygonTapered : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	ExtrudedPolygonTapered()
	{
	}

	virtual ~ExtrudedPolygonTapered()
	{
	}

	static const char* getClassName()
	{
		return "ExtrudedPolygonTapered";
	}

	/**
	* This datatype property is named extrusionLength. (Cardinality: [1, 1])
	*/
	double getExtrusionLength()
	{
		auto property = Thing::getDataProperty<double>("extrusionLength");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named extrusionLength. (Cardinality: [1, 1])
	*/
	void setExtrusionLength(double value)
	{
		Thing::setDataProperty("extrusionLength", &value, 1);
	}

	/**
	* This datatype property is named fraction. (Cardinality: [0, 1])
	*/
	double getFraction()
	{
		auto property = Thing::getDataProperty<double>("fraction");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named fraction. (Cardinality: [0, 1])
	*/
	void setFraction(double value)
	{
		Thing::setDataProperty("fraction", &value, 1);
	}

	/**
	* This datatype property is named openingPoints. (Cardinality: [0, Inf])
	*/
	vector<double> getOpeningPoints()
	{
		auto property = Thing::getDataProperty<double>("openingPoints");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named openingPoints. (Cardinality: [0, Inf])
	*/
	void setOpeningPoints(const vector<double>& vecValues)
	{
		Thing::setDataProperty("openingPoints", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named openingPointsEnd. (Cardinality: [0, Inf])
	*/
	vector<double> getOpeningPointsEnd()
	{
		auto property = Thing::getDataProperty<double>("openingPointsEnd");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named openingPointsEnd. (Cardinality: [0, Inf])
	*/
	void setOpeningPointsEnd(const vector<double>& vecValues)
	{
		Thing::setDataProperty("openingPointsEnd", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named openingSizes. (Cardinality: [0, Inf])
	*/
	vector<int64_t> getOpeningSizes()
	{
		auto property = Thing::getDataProperty<int64_t>("openingSizes");
		vector<int64_t> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named openingSizes. (Cardinality: [0, Inf])
	*/
	void setOpeningSizes(const vector<int64_t>& vecValues)
	{
		Thing::setDataProperty("openingSizes", (const int64_t*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named openingSizesEnd. (Cardinality: [0, Inf])
	*/
	vector<int64_t> getOpeningSizesEnd()
	{
		auto property = Thing::getDataProperty<int64_t>("openingSizesEnd");
		vector<int64_t> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named openingSizesEnd. (Cardinality: [0, Inf])
	*/
	void setOpeningSizesEnd(const vector<int64_t>& vecValues)
	{
		Thing::setDataProperty("openingSizesEnd", (const int64_t*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named points. (Cardinality: [6, Inf])
	*/
	vector<double> getPoints()
	{
		auto property = Thing::getDataProperty<double>("points");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named points. (Cardinality: [6, Inf])
	*/
	void setPoints(const vector<double>& vecValues)
	{
		Thing::setDataProperty("points", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named pointsEnd. (Cardinality: [6, Inf])
	*/
	vector<double> getPointsEnd()
	{
		auto property = Thing::getDataProperty<double>("pointsEnd");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named pointsEnd. (Cardinality: [6, Inf])
	*/
	void setPointsEnd(const vector<double>& vecValues)
	{
		Thing::setDataProperty("pointsEnd", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named polygonDirection. (Cardinality: [0, 3])
	*/
	vector<double> getPolygonDirection()
	{
		auto property = Thing::getDataProperty<double>("polygonDirection");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named polygonDirection. (Cardinality: [0, 3])
	*/
	void setPolygonDirection(const vector<double>& vecValues)
	{
		Thing::setDataProperty("polygonDirection", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named extrusionLength. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getExtrusionLength, put = setExtrusionLength)) double extrusionLength;

	/**
	* This datatype property is named fraction. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getFraction, put = setFraction)) double fraction;

	/**
	* This datatype property is named openingPoints. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getOpeningPoints, put = setOpeningPoints)) vector<double> openingPoints;

	/**
	* This datatype property is named openingPointsEnd. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getOpeningPointsEnd, put = setOpeningPointsEnd)) vector<double> openingPointsEnd;

	/**
	* This datatype property is named openingSizes. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getOpeningSizes, put = setOpeningSizes)) vector<int64_t> openingSizes;

	/**
	* This datatype property is named openingSizesEnd. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getOpeningSizesEnd, put = setOpeningSizesEnd)) vector<int64_t> openingSizesEnd;

	/**
	* This datatype property is named points. (Cardinality: [6, Inf])
	*/
	__declspec(property(get = getPoints, put = setPoints)) vector<double> points;

	/**
	* This datatype property is named pointsEnd. (Cardinality: [6, Inf])
	*/
	__declspec(property(get = getPointsEnd, put = setPointsEnd)) vector<double> pointsEnd;

	/**
	* This datatype property is named polygonDirection. (Cardinality: [0, 3])
	*/
	__declspec(property(get = getPolygonDirection, put = setPolygonDirection)) vector<double> polygonDirection;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();
	}
};