#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"
#include "Point3D.h"

/**
* This class represents the concept TriangleCurve. (http://rdf.bg/TriangleCurve)
*/
class TriangleCurve : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	TriangleCurve()
	{
	}

	virtual ~TriangleCurve()
	{
	}

	static const char* getClassName()
	{
		return "TriangleCurve";
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 9])
	*/
	vector<double> getCoordinates()
	{
		auto property = Thing::getDataProperty<double>("coordinates");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 9])
	*/
	void setCoordinates(const vector<double>& vecValues)
	{
		Thing::setDataProperty("coordinates", (const double*)vecValues.data(), vecValues.size());
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
	* This datatype property is named offsetY. (Cardinality: [0, 1])
	*/
	double getOffsetY()
	{
		auto property = Thing::getDataProperty<double>("offsetY");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named offsetY. (Cardinality: [0, 1])
	*/
	void setOffsetY(double value)
	{
		Thing::setDataProperty("offsetY", &value, 1);
	}

	/**
	* This datatype property is named offsetZ. (Cardinality: [0, 1])
	*/
	double getOffsetZ()
	{
		auto property = Thing::getDataProperty<double>("offsetZ");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named offsetZ. (Cardinality: [0, 1])
	*/
	void setOffsetZ(double value)
	{
		Thing::setDataProperty("offsetZ", &value, 1);
	}

	/**
	* This object property represents the relation pointReferences. (Cardinality: [0, 3])
	*/
	vector<Point3D*> getPointReferences()
	{
		auto property = Thing::getThingProperty<Point3D>("pointReferences");
		return property;
	}

	/**
	* This object property represents the relation pointReferences. (Cardinality: [0, 3])
	*/
	void setPointReferences(const vector<Point3D*>& vecValues)
	{
		Thing::setThingProperty("pointReferences", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 9])
	*/
	__declspec(property(get = getCoordinates, put = setCoordinates)) vector<double> coordinates;

	/**
	* This datatype property is named offsetX. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getOffsetX, put = setOffsetX)) double offsetX;

	/**
	* This datatype property is named offsetY. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getOffsetY, put = setOffsetY)) double offsetY;

	/**
	* This datatype property is named offsetZ. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getOffsetZ, put = setOffsetZ)) double offsetZ;

	/**
	* This object property represents the relation pointReferences. (Cardinality: [0, 3])
	*/
	__declspec(property(get = getPointReferences, put = setPointReferences)) vector<Point3D*> pointReferences;

protected:

	virtual void postCreate()
	{
		Curve::postCreate();
	}
};