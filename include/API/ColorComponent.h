#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Appearance.h"

/**
* This class represents the concept ColorComponent. (http://rdf.bg/ColorComponent)
*/
class ColorComponent : public Appearance
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	ColorComponent()
	{
	}

	virtual ~ColorComponent()
	{
	}

	static const char* getClassName()
	{
		return "ColorComponent";
	}

	/**
	* This datatype property is named B. (Cardinality: [1, 1])
	*/
	double getB()
	{
		auto property = Thing::getDataProperty<double>("B");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named B. (Cardinality: [1, 1])
	*/
	void setB(double value)
	{
		Thing::setDataProperty("B", &value, 1);
	}

	/**
	* This datatype property is named G. (Cardinality: [1, 1])
	*/
	double getG()
	{
		auto property = Thing::getDataProperty<double>("G");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named G. (Cardinality: [1, 1])
	*/
	void setG(double value)
	{
		Thing::setDataProperty("G", &value, 1);
	}

	/**
	* This datatype property is named R. (Cardinality: [1, 1])
	*/
	double getR()
	{
		auto property = Thing::getDataProperty<double>("R");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named R. (Cardinality: [1, 1])
	*/
	void setR(double value)
	{
		Thing::setDataProperty("R", &value, 1);
	}

	/**
	* This datatype property is named W. (Cardinality: [1, 1])
	*/
	double getW()
	{
		auto property = Thing::getDataProperty<double>("W");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named W. (Cardinality: [1, 1])
	*/
	void setW(double value)
	{
		Thing::setDataProperty("W", &value, 1);
	}

	/**
	* This datatype property is named B. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getB, put = setB)) double B;

	/**
	* This datatype property is named G. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getG, put = setG)) double G;

	/**
	* This datatype property is named R. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getR, put = setR)) double R;

	/**
	* This datatype property is named W. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getW, put = setW)) double W;

protected:

	virtual void postCreate()
	{
		Appearance::postCreate();
	}
};