#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "GeometricItem.h"

/**
* This class represents the concept Collection. (http://rdf.bg/Collection)
*/
class Collection : public GeometricItem
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Collection()
	{
	}

	virtual ~Collection()
	{
	}

	static const char* getClassName()
	{
		return "Collection";
	}

	/**
	* This datatype property is named consistencyCheck. (Cardinality: [0, 1])
	*/
	int64_t getConsistencyCheck()
	{
		auto property = Thing::getDataProperty<int64_t>("consistencyCheck");
		if (property.second == 1) { return property.first[0]; }

		return (int64_t)0;
	}

	/**
	* This datatype property is named consistencyCheck. (Cardinality: [0, 1])
	*/
	void setConsistencyCheck(int64_t value)
	{
		Thing::setDataProperty("consistencyCheck", &value, 1);
	}

	/**
	* This datatype property is named epsilon. (Cardinality: [0, 1])
	*/
	double getEpsilon()
	{
		auto property = Thing::getDataProperty<double>("epsilon");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named epsilon. (Cardinality: [0, 1])
	*/
	void setEpsilon(double value)
	{
		Thing::setDataProperty("epsilon", &value, 1);
	}

	/**
	* This datatype property is named forceSolid. (Cardinality: [0, 1])
	*/
	bool getForceSolid()
	{
		auto property = Thing::getDataProperty<bool>("forceSolid");
		if (property.second == 1) { return property.first[0]; }

		return (bool)0;
	}

	/**
	* This datatype property is named forceSolid. (Cardinality: [0, 1])
	*/
	void setForceSolid(bool value)
	{
		Thing::setDataProperty("forceSolid", &value, 1);
	}

	/**
	* This datatype property is named fraction. (Cardinality: [0, 1])
	*/
	double getFraction()
	{
		auto property = Thing::getDataProperty<double>("fraction");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named fraction. (Cardinality: [0, 1])
	*/
	void setFraction(double value)
	{
		Thing::setDataProperty("fraction", &value, 1);
	}

	/**
	* This object property represents the relation objects. (Cardinality: [0, Inf])
	*/
	vector<GeometricItem*> getObjects()
	{
		auto property = Thing::getThingProperty<GeometricItem>("objects");
		return property;
	}

	/**
	* This object property represents the relation objects. (Cardinality: [0, Inf])
	*/
	void setObjects(const vector<GeometricItem*>& vecValues)
	{
		Thing::setThingProperty("objects", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named relativeEpsilon. (Cardinality: [0, 1])
	*/
	double getRelativeEpsilon()
	{
		auto property = Thing::getDataProperty<double>("relativeEpsilon");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named relativeEpsilon. (Cardinality: [0, 1])
	*/
	void setRelativeEpsilon(double value)
	{
		Thing::setDataProperty("relativeEpsilon", &value, 1);
	}

	/**
	* This datatype property is named representsSolid. (Cardinality: [0, 1])
	*/
	bool getRepresentsSolid()
	{
		auto property = Thing::getDataProperty<bool>("representsSolid");
		if (property.second == 1) { return property.first[0]; }

		return (bool)0;
	}

	/**
	* This datatype property is named representsSolid. (Cardinality: [0, 1])
	*/
	void setRepresentsSolid(bool value)
	{
		Thing::setDataProperty("representsSolid", &value, 1);
	}

	/**
	* This datatype property is named consistencyCheck. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getConsistencyCheck, put = setConsistencyCheck)) int64_t consistencyCheck;

	/**
	* This datatype property is named epsilon. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getEpsilon, put = setEpsilon)) double epsilon;

	/**
	* This datatype property is named forceSolid. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getForceSolid, put = setForceSolid)) bool forceSolid;

	/**
	* This datatype property is named fraction. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getFraction, put = setFraction)) double fraction;

	/**
	* This object property represents the relation objects. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getObjects, put = setObjects)) vector<GeometricItem*> objects;

	/**
	* This datatype property is named relativeEpsilon. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getRelativeEpsilon, put = setRelativeEpsilon)) double relativeEpsilon;

	/**
	* This datatype property is named representsSolid. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getRepresentsSolid, put = setRepresentsSolid)) bool representsSolid;

protected:

	virtual void postCreate()
	{
		GeometricItem::postCreate();
	}
};