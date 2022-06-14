#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Vector.h"

/**
* This class represents the concept Vector3. (http://rdf.bg/Vector3)
*/
class Vector3 : public Vector
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Vector3()
	{
	}

	virtual ~Vector3()
	{
	}

	static const char* getClassName()
	{
		return "Vector3";
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 3])
	*/
	vector<double> getCoordinates()
	{
		auto property = Thing::getDataProperty<double>("coordinates");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 3])
	*/
	void setCoordinates(const vector<double>& vecValues)
	{
		Thing::setDataProperty("coordinates", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named x. (Cardinality: [0, 1])
	*/
	double getX()
	{
		auto property = Thing::getDataProperty<double>("x");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named x. (Cardinality: [0, 1])
	*/
	void setX(double value)
	{
		Thing::setDataProperty("x", &value, 1);
	}

	/**
	* This datatype property is named y. (Cardinality: [0, 1])
	*/
	double getY()
	{
		auto property = Thing::getDataProperty<double>("y");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named y. (Cardinality: [0, 1])
	*/
	void setY(double value)
	{
		Thing::setDataProperty("y", &value, 1);
	}

	/**
	* This datatype property is named z. (Cardinality: [0, 1])
	*/
	double getZ()
	{
		auto property = Thing::getDataProperty<double>("z");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named z. (Cardinality: [0, 1])
	*/
	void setZ(double value)
	{
		Thing::setDataProperty("z", &value, 1);
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 3])
	*/
	__declspec(property(get = getCoordinates, put = setCoordinates)) vector<double> coordinates;

	/**
	* This datatype property is named x. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getX, put = setX)) double x;

	/**
	* This datatype property is named y. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getY, put = setY)) double y;

	/**
	* This datatype property is named z. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getZ, put = setZ)) double z;

protected:

	virtual void postCreate()
	{
		Vector::postCreate();
	}
};