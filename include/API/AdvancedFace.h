#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "GeometricItem.h"
#include "Curve.h"
#include "Surface.h"

/**
* This class represents the concept AdvancedFace. (http://rdf.bg/AdvancedFace)
*/
class AdvancedFace : public GeometricItem
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	AdvancedFace()
	{
	}

	virtual ~AdvancedFace()
	{
	}

	static const char* getClassName()
	{
		return "AdvancedFace";
	}

	/**
	* This object property represents the relation bounds. (Cardinality: [0, Inf])
	*/
	vector<Curve*> getBounds()
	{
		auto property = Thing::getThingProperty<Curve>("bounds");
		return property;
	}

	/**
	* This object property represents the relation bounds. (Cardinality: [0, Inf])
	*/
	void setBounds(const vector<Curve*>& vecValues)
	{
		Thing::setThingProperty("bounds", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This object property represents the relation surface. (Cardinality: [1, 1])
	*/
	Surface* getSurface()
	{
		auto property = Thing::getThingProperty<Surface>("surface");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation surface. (Cardinality: [1, 1])
	*/
	void setSurface(Surface* pValue)
	{
		Thing::setThingProperty("surface", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation bounds. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getBounds, put = setBounds)) vector<Curve*> bounds;

	/**
	* This object property represents the relation surface. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getSurface, put = setSurface)) Surface* surface;

protected:

	virtual void postCreate()
	{
		GeometricItem::postCreate();
	}
};