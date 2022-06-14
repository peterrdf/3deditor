#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"
#include "AlignedSegments.h"

/**
* This class represents the concept Alignment. (http://rdf.bg/Alignment)
*/
class Alignment : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Alignment()
	{
	}

	virtual ~Alignment()
	{
	}

	static const char* getClassName()
	{
		return "Alignment";
	}

	/**
	* This object property represents the relation horizontal. (Cardinality: [0, 1])
	*/
	AlignedSegments* getHorizontal()
	{
		auto property = Thing::getThingProperty<AlignedSegments>("horizontal");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation horizontal. (Cardinality: [0, 1])
	*/
	void setHorizontal(AlignedSegments* pValue)
	{
		Thing::setThingProperty("horizontal", (const _Thing**)&pValue, 1);
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
	* This datatype property is named type. (Cardinality: [1, 1])
	*/
	int64_t getType()
	{
		auto property = Thing::getDataProperty<int64_t>("type");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named type. (Cardinality: [1, 1])
	*/
	void setType(int64_t value)
	{
		Thing::setDataProperty("type", &value, 1);
	}

	/**
	* This object property represents the relation vertical. (Cardinality: [0, 1])
	*/
	AlignedSegments* getVertical()
	{
		auto property = Thing::getThingProperty<AlignedSegments>("vertical");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation vertical. (Cardinality: [0, 1])
	*/
	void setVertical(AlignedSegments* pValue)
	{
		Thing::setThingProperty("vertical", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation horizontal. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getHorizontal, put = setHorizontal)) AlignedSegments* horizontal;

	/**
	* This datatype property is named offsetX. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getOffsetX, put = setOffsetX)) double offsetX;

	/**
	* This datatype property is named type. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getType, put = setType)) int64_t type;

	/**
	* This object property represents the relation vertical. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getVertical, put = setVertical)) AlignedSegments* vertical;

protected:

	virtual void postCreate()
	{
		Curve::postCreate();
	}
};