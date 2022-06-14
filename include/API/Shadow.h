#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Face.h"
#include "Vector3.h"
#include "Point3D.h"
#include "GeometricItem.h"
#include "Plane.h"

/**
* This class represents the concept Shadow. (http://rdf.bg/Shadow)
*/
class Shadow : public Face
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Shadow()
	{
	}

	virtual ~Shadow()
	{
	}

	static const char* getClassName()
	{
		return "Shadow";
	}

	/**
	* This object property represents the relation lightDirection. (Cardinality: [0, 1])
	*/
	Vector3* getLightDirection()
	{
		auto property = Thing::getThingProperty<Vector3>("lightDirection");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation lightDirection. (Cardinality: [0, 1])
	*/
	void setLightDirection(Vector3* pValue)
	{
		Thing::setThingProperty("lightDirection", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation lightPoint. (Cardinality: [0, 1])
	*/
	Point3D* getLightPoint()
	{
		auto property = Thing::getThingProperty<Point3D>("lightPoint");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation lightPoint. (Cardinality: [0, 1])
	*/
	void setLightPoint(Point3D* pValue)
	{
		Thing::setThingProperty("lightPoint", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation object. (Cardinality: [1, 1])
	*/
	GeometricItem* getObject()
	{
		auto property = Thing::getThingProperty<GeometricItem>("object");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation object. (Cardinality: [1, 1])
	*/
	void setObject(GeometricItem* pValue)
	{
		Thing::setThingProperty("object", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation plane. (Cardinality: [1, 1])
	*/
	Plane* getPlane()
	{
		auto property = Thing::getThingProperty<Plane>("plane");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation plane. (Cardinality: [1, 1])
	*/
	void setPlane(Plane* pValue)
	{
		Thing::setThingProperty("plane", (const _Thing**)&pValue, 1);
	}

	/**
	* This object property represents the relation planeRefDirection. (Cardinality: [0, 1])
	*/
	Vector3* getPlaneRefDirection()
	{
		auto property = Thing::getThingProperty<Vector3>("planeRefDirection");
		if (property.size() == 1) { return property[0]; }

		return nullptr;
	}

	/**
	* This object property represents the relation planeRefDirection. (Cardinality: [0, 1])
	*/
	void setPlaneRefDirection(Vector3* pValue)
	{
		Thing::setThingProperty("planeRefDirection", (const _Thing**)&pValue, 1);
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
	* This object property represents the relation lightDirection. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getLightDirection, put = setLightDirection)) Vector3* lightDirection;

	/**
	* This object property represents the relation lightPoint. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getLightPoint, put = setLightPoint)) Point3D* lightPoint;

	/**
	* This object property represents the relation object. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getObject, put = setObject)) GeometricItem* object;

	/**
	* This object property represents the relation plane. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getPlane, put = setPlane)) Plane* plane;

	/**
	* This object property represents the relation planeRefDirection. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getPlaneRefDirection, put = setPlaneRefDirection)) Vector3* planeRefDirection;

	/**
	* This datatype property is named type. (Cardinality: [1, 1])
	*/
	__declspec(property(get = getType, put = setType)) int64_t type;

protected:

	virtual void postCreate()
	{
		Face::postCreate();
	}
};