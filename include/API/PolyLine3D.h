#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"
#include "Point3D.h"

/**
* This class represents the concept PolyLine3D. (http://rdf.bg/PolyLine3D)
*/
class PolyLine3D : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	PolyLine3D()
	{
	}

	virtual ~PolyLine3D()
	{
	}

	static const char* getClassName()
	{
		return "PolyLine3D";
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, Inf])
	*/
	vector<double> getCoordinates()
	{
		auto property = Thing::getDataProperty<double>("coordinates");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, Inf])
	*/
	void setCoordinates(const vector<double>& vecValues)
	{
		Thing::setDataProperty("coordinates", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This object property represents the relation pointReferences. (Cardinality: [0, Inf])
	*/
	vector<Point3D*> getPointReferences()
	{
		auto property = Thing::getThingProperty<Point3D>("pointReferences");
		return property;
	}

	/**
	* This object property represents the relation pointReferences. (Cardinality: [0, Inf])
	*/
	void setPointReferences(const vector<Point3D*>& vecValues)
	{
		Thing::setThingProperty("pointReferences", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getCoordinates, put = setCoordinates)) vector<double> coordinates;

	/**
	* This object property represents the relation pointReferences. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getPointReferences, put = setPointReferences)) vector<Point3D*> pointReferences;

protected:

	virtual void postCreate()
	{
		Curve::postCreate();
	}
};