#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"

/**
* This class represents the concept AlignedSegment. (http://rdf.bg/AlignedSegment)
*/
class AlignedSegment : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	AlignedSegment()
	{
	}

	virtual ~AlignedSegment()
	{
	}

	static const char* getClassName()
	{
		return "AlignedSegment";
	}

	/**
	* This datatype property is named length. (Cardinality: [0, 1])
	*/
	double getLength()
	{
		auto property = Thing::getDataProperty<double>("length");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named length. (Cardinality: [0, 1])
	*/
	void setLength(double value)
	{
		Thing::setDataProperty("length", &value, 1);
	}

	/**
	* This datatype property is named offsetX. (Cardinality: [1, 1])
	*/
	double getOffsetX()
	{
		auto property = Thing::getDataProperty<double>("offsetX");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named offsetX. (Cardinality: [1, 1])
	*/
	void setOffsetX(double value)
	{
		Thing::setDataProperty("offsetX", &value, 1);
	}

	/**
	* This datatype property is named offsetY. (Cardinality: [1, 1])
	*/
	double getOffsetY()
	{
		auto property = Thing::getDataProperty<double>("offsetY");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named offsetY. (Cardinality: [1, 1])
	*/
	void setOffsetY(double value)
	{
		Thing::setDataProperty("offsetY", &value, 1);
	}

	/**
	* This object property represents the relation segment. (Cardinality: [0, 1])
	*/
	Curve* getSegment()
	{
		auto property = Thing::getThingProperty<Curve>("segment");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation segment. (Cardinality: [0, 1])
	*/
	void setSegment(Curve* pValue)
	{
		Thing::setThingProperty("segment", (const _Thing**)&pValue, 1);
	}

	/**
	* This datatype property is named tangentDirectionStart. (Cardinality: [3, 3])
	*/
	vector<double> getTangentDirectionStart()
	{
		auto property = Thing::getDataProperty<double>("tangentDirectionStart");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named tangentDirectionStart. (Cardinality: [3, 3])
	*/
	void setTangentDirectionStart(const vector<double>& vecValues)
	{
		Thing::setDataProperty("tangentDirectionStart", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named length. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getLength, put = setLength)) double length;

	/**
	* This datatype property is named offsetX. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getOffsetX, put = setOffsetX)) double offsetX;

	/**
	* This datatype property is named offsetY. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getOffsetY, put = setOffsetY)) double offsetY;

	/**
	* This object property represents the relation segment. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSegment, put = setSegment)) Curve* segment;

	/**
	* This datatype property is named tangentDirectionStart. (Cardinality: [3, 3])
	*/
	__declspec(property(get = getTangentDirectionStart, put = setTangentDirectionStart)) vector<double> tangentDirectionStart;

protected:

	virtual void postCreate()
	{
		Curve::postCreate();
	}
};