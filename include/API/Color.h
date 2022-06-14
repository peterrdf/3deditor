#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Appearance.h"
#include "ColorComponent.h"

/**
* This class represents the concept Color. (http://rdf.bg/Color)
*/
class Color : public Appearance
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Color()
	{
	}

	virtual ~Color()
	{
	}

	static const char* getClassName()
	{
		return "Color";
	}

	/**
	* This object property represents the relation ambient. (Cardinality: [0, 1])
	*/
	ColorComponent* getAmbient()
	{
		auto property = Thing::getThingProperty<ColorComponent>("ambient");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation ambient. (Cardinality: [0, 1])
	*/
	void setAmbient(ColorComponent* pValue)
	{
		Thing::setThingProperty("ambient", (const _Thing**)&pValue, 1);
	}

	/**
	* This datatype property is named ambientReflectance. (Cardinality: [0, 1])
	*/
	double getAmbientReflectance()
	{
		auto property = Thing::getDataProperty<double>("ambientReflectance");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named ambientReflectance. (Cardinality: [0, 1])
	*/
	void setAmbientReflectance(double value)
	{
		Thing::setDataProperty("ambientReflectance", &value, 1);
	}

	/**
	* This object property represents the relation diffuse. (Cardinality: [0, 1])
	*/
	ColorComponent* getDiffuse()
	{
		auto property = Thing::getThingProperty<ColorComponent>("diffuse");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation diffuse. (Cardinality: [0, 1])
	*/
	void setDiffuse(ColorComponent* pValue)
	{
		Thing::setThingProperty("diffuse", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation emissive. (Cardinality: [0, 1])
	*/
	ColorComponent* getEmissive()
	{
		auto property = Thing::getThingProperty<ColorComponent>("emissive");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation emissive. (Cardinality: [0, 1])
	*/
	void setEmissive(ColorComponent* pValue)
	{
		Thing::setThingProperty("emissive", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation specular. (Cardinality: [0, 1])
	*/
	ColorComponent* getSpecular()
	{
		auto property = Thing::getThingProperty<ColorComponent>("specular");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation specular. (Cardinality: [0, 1])
	*/
	void setSpecular(ColorComponent* pValue)
	{
		Thing::setThingProperty("specular", (const _Thing**)&pValue, 1);
	}

	/**
	* This datatype property is named transparency, if defined it will overrule the W value given by the ColorComponent. (Cardinality: [0, 1])
	*/
	double getTransparency()
	{
		auto property = Thing::getDataProperty<double>("transparency");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named transparency, if defined it will overrule the W value given by the ColorComponent. (Cardinality: [0, 1])
	*/
	void setTransparency(double value)
	{
		Thing::setDataProperty("transparency", &value, 1);
	}

	/**
	* This object property represents the relation ambient. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getAmbient, put = setAmbient)) ColorComponent* ambient;

	/**
	* This datatype property is named ambientReflectance. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getAmbientReflectance, put = setAmbientReflectance)) double ambientReflectance;

	/**
	* This object property represents the relation diffuse. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getDiffuse, put = setDiffuse)) ColorComponent* diffuse;

	/**
	* This object property represents the relation emissive. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getEmissive, put = setEmissive)) ColorComponent* emissive;

	/**
	* This object property represents the relation specular. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getSpecular, put = setSpecular)) ColorComponent* specular;

	/**
	* This datatype property is named transparency, if defined it will overrule the W value given by the ColorComponent. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getTransparency, put = setTransparency)) double transparency;

protected:

	virtual void postCreate()
	{
		Appearance::postCreate();
	}
};