#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"
#include "Curve.h"

/**
* This class represents the concept SweptAreaSolidTapered. (http://rdf.bg/SweptAreaSolidTapered)
*/
class SweptAreaSolidTapered : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	SweptAreaSolidTapered()
	{
	}

	virtual ~SweptAreaSolidTapered()
	{
	}

	static const char* getClassName()
	{
		return "SweptAreaSolidTapered";
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
	* This object property represents the relation path. (Cardinality: [1, 1])
	*/
	Curve* getPath()
	{
		auto property = Thing::getThingProperty<Curve>("path");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation path. (Cardinality: [1, 1])
	*/
	void setPath(Curve* pValue)
	{
		Thing::setThingProperty("path", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation sweptArea. (Cardinality: [1, 1])
	*/
	Curve* getSweptArea()
	{
		auto property = Thing::getThingProperty<Curve>("sweptArea");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation sweptArea. (Cardinality: [1, 1])
	*/
	void setSweptArea(Curve* pValue)
	{
		Thing::setThingProperty("sweptArea", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation sweptAreaEnd. (Cardinality: [1, 1])
	*/
	Curve* getSweptAreaEnd()
	{
		auto property = Thing::getThingProperty<Curve>("sweptAreaEnd");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation sweptAreaEnd. (Cardinality: [1, 1])
	*/
	void setSweptAreaEnd(Curve* pValue)
	{
		Thing::setThingProperty("sweptAreaEnd", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation sweptAreaEndOpenings. (Cardinality: [0, Inf])
	*/
	vector<Curve*> getSweptAreaEndOpenings()
	{
		auto property = Thing::getThingProperty<Curve>("sweptAreaEndOpenings");
		return property;
	}

	/**
	* This object property represents the relation sweptAreaEndOpenings. (Cardinality: [0, Inf])
	*/
	void setSweptAreaEndOpenings(const vector<Curve*>& vecValues)
	{
		Thing::setThingProperty("sweptAreaEndOpenings", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This object property represents the relation sweptAreaOpenings. (Cardinality: [0, Inf])
	*/
	vector<Curve*> getSweptAreaOpenings()
	{
		auto property = Thing::getThingProperty<Curve>("sweptAreaOpenings");
		return property;
	}

	/**
	* This object property represents the relation sweptAreaOpenings. (Cardinality: [0, Inf])
	*/
	void setSweptAreaOpenings(const vector<Curve*>& vecValues)
	{
		Thing::setThingProperty("sweptAreaOpenings", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named fraction. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getFraction, put = setFraction)) double fraction;

	/**
	* This object property represents the relation path. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getPath, put = setPath)) Curve* path;

	/**
	* This object property represents the relation sweptArea. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getSweptArea, put = setSweptArea)) Curve* sweptArea;

	/**
	* This object property represents the relation sweptAreaEnd. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getSweptAreaEnd, put = setSweptAreaEnd)) Curve* sweptAreaEnd;

	/**
	* This object property represents the relation sweptAreaEndOpenings. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getSweptAreaEndOpenings, put = setSweptAreaEndOpenings)) vector<Curve*> sweptAreaEndOpenings;

	/**
	* This object property represents the relation sweptAreaOpenings. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getSweptAreaOpenings, put = setSweptAreaOpenings)) vector<Curve*> sweptAreaOpenings;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();
	}
};