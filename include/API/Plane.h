#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Mathematics.h"

/**
* This class represents the concept Plane. (http://rdf.bg/Plane)
*/
class Plane : public Mathematics
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Plane()
	{
	}

	virtual ~Plane()
	{
	}

	static const char* getClassName()
	{
		return "Plane";
	}

	/**
	* This datatype property is named A. (Cardinality: [0, 1])
	*/
	double getA()
	{
		auto property = Thing::getDataProperty<double>("A");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named A. (Cardinality: [0, 1])
	*/
	void setA(double value)
	{
		Thing::setDataProperty("A", &value, 1);
	}

	/**
	* This datatype property is named B. (Cardinality: [0, 1])
	*/
	double getB()
	{
		auto property = Thing::getDataProperty<double>("B");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named B. (Cardinality: [0, 1])
	*/
	void setB(double value)
	{
		Thing::setDataProperty("B", &value, 1);
	}

	/**
	* This datatype property is named C. (Cardinality: [0, 1])
	*/
	double getC()
	{
		auto property = Thing::getDataProperty<double>("C");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named C. (Cardinality: [0, 1])
	*/
	void setC(double value)
	{
		Thing::setDataProperty("C", &value, 1);
	}

	/**
	* This datatype property is named D. (Cardinality: [0, 1])
	*/
	double getD()
	{
		auto property = Thing::getDataProperty<double>("D");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named D. (Cardinality: [0, 1])
	*/
	void setD(double value)
	{
		Thing::setDataProperty("D", &value, 1);
	}

	/**
	* This datatype property is named A. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getA, put = setA)) double A;

	/**
	* This datatype property is named B. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getB, put = setB)) double B;

	/**
	* This datatype property is named C. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getC, put = setC)) double C;

	/**
	* This datatype property is named D. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getD, put = setD)) double D;

protected:

	virtual void postCreate()
	{
		Mathematics::postCreate();
	}
};