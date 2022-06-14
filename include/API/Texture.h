#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Appearance.h"

/**
* This class represents the concept Texture. (http://rdf.bg/Texture)
*/
class Texture : public Appearance
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Texture()
	{
	}

	virtual ~Texture()
	{
	}

	static const char* getClassName()
	{
		return "Texture";
	}

	/**
	* This datatype property is named name. (Cardinality: [0, 1])
	*/
	char* getName()
	{
		auto property = Thing::getDataProperty<char*>("name");
		if (property.second == 1) { return property.first[0]; }

		return (char*)0;
	}

	/**
	* This datatype property is named name. (Cardinality: [0, 1])
	*/
	void setName(char* value)
	{
		Thing::setDataProperty("name", &value, 1);
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
	* This datatype property is named origin. (Cardinality: [0, 3])
	*/
	vector<double> getOrigin()
	{
		auto property = Thing::getDataProperty<double>("origin");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named origin. (Cardinality: [0, 3])
	*/
	void setOrigin(const vector<double>& vecValues)
	{
		Thing::setDataProperty("origin", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named rotation. (Cardinality: [0, 1])
	*/
	double getRotation()
	{
		auto property = Thing::getDataProperty<double>("rotation");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named rotation. (Cardinality: [0, 1])
	*/
	void setRotation(double value)
	{
		Thing::setDataProperty("rotation", &value, 1);
	}

	/**
	* This datatype property is named scalingX. (Cardinality: [0, 1])
	*/
	double getScalingX()
	{
		auto property = Thing::getDataProperty<double>("scalingX");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named scalingX. (Cardinality: [0, 1])
	*/
	void setScalingX(double value)
	{
		Thing::setDataProperty("scalingX", &value, 1);
	}

	/**
	* This datatype property is named scalingY. (Cardinality: [0, 1])
	*/
	double getScalingY()
	{
		auto property = Thing::getDataProperty<double>("scalingY");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named scalingY. (Cardinality: [0, 1])
	*/
	void setScalingY(double value)
	{
		Thing::setDataProperty("scalingY", &value, 1);
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
	* This datatype property is named name. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getName, put = setName)) char* name;

	/**
	* This datatype property is named offsetX. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getOffsetX, put = setOffsetX)) double offsetX;

	/**
	* This datatype property is named offsetY. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getOffsetY, put = setOffsetY)) double offsetY;

	/**
	* This datatype property is named origin. (Cardinality: [0, 3])
	*/
	__declspec(property(get = getOrigin, put = setOrigin)) vector<double> origin;

	/**
	* This datatype property is named rotation. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getRotation, put = setRotation)) double rotation;

	/**
	* This datatype property is named scalingX. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getScalingX, put = setScalingX)) double scalingX;

	/**
	* This datatype property is named scalingY. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getScalingY, put = setScalingY)) double scalingY;

	/**
	* This datatype property is named type. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getType, put = setType)) int64_t type;

protected:

	virtual void postCreate()
	{
		Appearance::postCreate();
	}
};