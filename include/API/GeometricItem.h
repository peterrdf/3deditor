#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Thing.h"
#include "Material.h"

/**
* This class represents the abstract concept GeometricItem. (http://rdf.bg/GeometricItem)
*/
class GeometricItem : public Thing
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	GeometricItem()
	{
	}

	virtual ~GeometricItem()
	{
	}

	static const char* getClassName()
	{
		return "GeometricItem";
	}

	/**
	* This object property represents the relation material. (Cardinality: [0, 1])
	*/
	Material* getMaterial()
	{
		auto property = Thing::getThingProperty<Material>("material");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation material. (Cardinality: [0, 1])
	*/
	void setMaterial(Material* pValue)
	{
		Thing::setThingProperty("material", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation material. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getMaterial, put = setMaterial)) Material* material;

protected:

	virtual void postCreate()
	{
		Thing::postCreate();
	}
};