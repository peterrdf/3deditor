#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "AdvancedFace.h"
#include "Face.h"

/**
* This class represents the concept AdvancedFace3D. (http://rdf.bg/AdvancedFace3D)
*/
class AdvancedFace3D : public AdvancedFace
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	AdvancedFace3D()
	{
	}

	virtual ~AdvancedFace3D()
	{
	}

	static const char* getClassName()
	{
		return "AdvancedFace3D";
	}

protected:

	virtual void postCreate()
	{
		AdvancedFace::postCreate();
	}
};