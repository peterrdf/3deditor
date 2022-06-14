#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Curve.h"
#include "AlignedSegment.h"

/**
* This class represents the concept AlignedSegments. (http://rdf.bg/AlignedSegments)
*/
class AlignedSegments : public Curve
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	AlignedSegments()
	{
	}

	virtual ~AlignedSegments()
	{
	}

	static const char* getClassName()
	{
		return "AlignedSegments";
	}

	/**
	* This object property represents the relation segments. (Cardinality: [0, Inf])
	*/
	vector<AlignedSegment*> getSegments()
	{
		auto property = Thing::getThingProperty<AlignedSegment>("segments");
		return property;
	}

	/**
	* This object property represents the relation segments. (Cardinality: [0, Inf])
	*/
	void setSegments(const vector<AlignedSegment*>& vecValues)
	{
		Thing::setThingProperty("segments", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This object property represents the relation segments. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getSegments, put = setSegments)) vector<AlignedSegment*> segments;

protected:

	virtual void postCreate()
	{
		Curve::postCreate();
	}
};