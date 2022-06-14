#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Appearance.h"
#include "Color.h"
#include "Texture.h"

/**
* This class represents the concept Material. (http://rdf.bg/Material)
*/
class Material : public Appearance
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Material()
	{
	}

	virtual ~Material()
	{
	}

	static const char* getClassName()
	{
		return "Material";
	}

	/**
	* This object property represents the relation color. (Cardinality: [0, 1])
	*/
	Color* getColor()
	{
		auto property = Thing::getThingProperty<Color>("color");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation color. (Cardinality: [0, 1])
	*/
	void setColor(Color* pValue)
	{
		Thing::setThingProperty("color", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation textures. (Cardinality: [0, 2])
	*/
	vector<Texture*> getTextures()
	{
		auto property = Thing::getThingProperty<Texture>("textures");
		return property;
	}

	/**
	* This object property represents the relation textures. (Cardinality: [0, 2])
	*/
	void setTextures(const vector<Texture*>& vecValues)
	{
		Thing::setThingProperty("textures", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This object property represents the relation color. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getColor, put = setColor)) Color* color;

	/**
	* This object property represents the relation textures. (Cardinality: [0, 2])
	*/
	__declspec(property(get = getTextures, put = setTextures)) vector<Texture*> textures;

protected:

	virtual void postCreate()
	{
		Appearance::postCreate();
	}
};