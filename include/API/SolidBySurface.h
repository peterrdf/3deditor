#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"
#include "Face.h"

/**
* This class represents the concept SolidBySurface. (http://rdf.bg/SolidBySurface)
*/
class SolidBySurface : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	SolidBySurface()
	{
	}

	virtual ~SolidBySurface()
	{
	}

	static const char* getClassName()
	{
		return "SolidBySurface";
	}

	/**
	* This object property represents the relation faces. (Cardinality: [1, Inf])
	*/
	vector<Face*> getFaces()
	{
		auto property = Thing::getThingProperty<Face>("faces");
		return property;
	}

	/**
	* This object property represents the relation faces. (Cardinality: [1, Inf])
	*/
	void setFaces(const vector<Face*>& vecValues)
	{
		Thing::setThingProperty("faces", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This object property represents the relation faces. (Cardinality: [1, Inf])
	*/
	__declspec(property(get = getFaces, put = setFaces)) vector<Face*> faces;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();
	}
};