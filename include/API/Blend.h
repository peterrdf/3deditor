#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"
#include "Curve.h"

/**
* This class represents the concept Blend. (http://rdf.bg/Blend)
*/
class Blend : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Blend()
	{
	}

	virtual ~Blend()
	{
	}

	static const char* getClassName()
	{
		return "Blend";
	}

	/**
	* This object property represents the relation bottomPolygon. (Cardinality: [1, 1])
	*/
	Curve* getBottomPolygon()
	{
		auto property = Thing::getThingProperty<Curve>("bottomPolygon");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation bottomPolygon. (Cardinality: [1, 1])
	*/
	void setBottomPolygon(Curve* pValue)
	{
		Thing::setThingProperty("bottomPolygon", (const _Thing**)&pValue, 1);
	}

	/**
	* This datatype property is named forceSolid. (Cardinality: [0, 1])
	*/
	bool getForceSolid()
	{
		auto property = Thing::getDataProperty<bool>("forceSolid");
		if (property.second == 1) { return property.first[0]; }

		return (bool)0;
	}

	/**
	* This datatype property is named forceSolid. (Cardinality: [0, 1])
	*/
	void setForceSolid(bool value)
	{
		Thing::setDataProperty("forceSolid", &value, 1);
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
	* This datatype property is named hasBottom. (Cardinality: [0, 1])
	*/
	bool getHasBottom()
	{
		auto property = Thing::getDataProperty<bool>("hasBottom");
		if (property.second == 1) { return property.first[0]; }

		return (bool)0;
	}

	/**
	* This datatype property is named hasBottom. (Cardinality: [0, 1])
	*/
	void setHasBottom(bool value)
	{
		Thing::setDataProperty("hasBottom", &value, 1);
	}

	/**
	* This datatype property is named hasTop. (Cardinality: [0, 1])
	*/
	bool getHasTop()
	{
		auto property = Thing::getDataProperty<bool>("hasTop");
		if (property.second == 1) { return property.first[0]; }

		return (bool)0;
	}

	/**
	* This datatype property is named hasTop. (Cardinality: [0, 1])
	*/
	void setHasTop(bool value)
	{
		Thing::setDataProperty("hasTop", &value, 1);
	}

	/**
	* This datatype property is named height. (Cardinality: [0, 1])
	*/
	double getHeight()
	{
		auto property = Thing::getDataProperty<double>("height");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named height. (Cardinality: [0, 1])
	*/
	void setHeight(double value)
	{
		Thing::setDataProperty("height", &value, 1);
	}

	/**
	* This object property represents the relation topPolygon. (Cardinality: [1, 1])
	*/
	Curve* getTopPolygon()
	{
		auto property = Thing::getThingProperty<Curve>("topPolygon");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation topPolygon. (Cardinality: [1, 1])
	*/
	void setTopPolygon(Curve* pValue)
	{
		Thing::setThingProperty("topPolygon", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation bottomPolygon. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getBottomPolygon, put = setBottomPolygon)) Curve* bottomPolygon;

	/**
	* This datatype property is named forceSolid. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getForceSolid, put = setForceSolid)) bool forceSolid;

	/**
	* This datatype property is named fraction. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getFraction, put = setFraction)) double fraction;

	/**
	* This datatype property is named hasBottom. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getHasBottom, put = setHasBottom)) bool hasBottom;

	/**
	* This datatype property is named hasTop. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getHasTop, put = setHasTop)) bool hasTop;

	/**
	* This datatype property is named height. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getHeight, put = setHeight)) double height;

	/**
	* This object property represents the relation topPolygon. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getTopPolygon, put = setTopPolygon)) Curve* topPolygon;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();
	}
};