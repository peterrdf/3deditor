#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Mathematics.h"

/**
* This class represents the concept Matrix. (http://rdf.bg/Matrix)
*/
class Matrix : public Mathematics
{

	/**
	* Friends
	*/
	friend class _Model;

public:

	Matrix()
	{
	}

	virtual ~Matrix()
	{
	}

	static const char* getClassName()
	{
		return "Matrix";
	}

	/**
	* This datatype property is named _11. (Cardinality: [0, 1])
	*/
	double get_11()
	{
		auto property = Thing::getDataProperty<double>("_11");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named _11. (Cardinality: [0, 1])
	*/
	void set_11(double value)
	{
		Thing::setDataProperty("_11", &value, 1);
	}

	/**
	* This datatype property is named _12. (Cardinality: [0, 1])
	*/
	double get_12()
	{
		auto property = Thing::getDataProperty<double>("_12");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named _12. (Cardinality: [0, 1])
	*/
	void set_12(double value)
	{
		Thing::setDataProperty("_12", &value, 1);
	}

	/**
	* This datatype property is named _13. (Cardinality: [0, 1])
	*/
	double get_13()
	{
		auto property = Thing::getDataProperty<double>("_13");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named _13. (Cardinality: [0, 1])
	*/
	void set_13(double value)
	{
		Thing::setDataProperty("_13", &value, 1);
	}

	/**
	* This datatype property is named _21. (Cardinality: [0, 1])
	*/
	double get_21()
	{
		auto property = Thing::getDataProperty<double>("_21");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named _21. (Cardinality: [0, 1])
	*/
	void set_21(double value)
	{
		Thing::setDataProperty("_21", &value, 1);
	}

	/**
	* This datatype property is named _22. (Cardinality: [0, 1])
	*/
	double get_22()
	{
		auto property = Thing::getDataProperty<double>("_22");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named _22. (Cardinality: [0, 1])
	*/
	void set_22(double value)
	{
		Thing::setDataProperty("_22", &value, 1);
	}

	/**
	* This datatype property is named _23. (Cardinality: [0, 1])
	*/
	double get_23()
	{
		auto property = Thing::getDataProperty<double>("_23");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named _23. (Cardinality: [0, 1])
	*/
	void set_23(double value)
	{
		Thing::setDataProperty("_23", &value, 1);
	}

	/**
	* This datatype property is named _31. (Cardinality: [0, 1])
	*/
	double get_31()
	{
		auto property = Thing::getDataProperty<double>("_31");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named _31. (Cardinality: [0, 1])
	*/
	void set_31(double value)
	{
		Thing::setDataProperty("_31", &value, 1);
	}

	/**
	* This datatype property is named _32. (Cardinality: [0, 1])
	*/
	double get_32()
	{
		auto property = Thing::getDataProperty<double>("_32");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named _32. (Cardinality: [0, 1])
	*/
	void set_32(double value)
	{
		Thing::setDataProperty("_32", &value, 1);
	}

	/**
	* This datatype property is named _33. (Cardinality: [0, 1])
	*/
	double get_33()
	{
		auto property = Thing::getDataProperty<double>("_33");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named _33. (Cardinality: [0, 1])
	*/
	void set_33(double value)
	{
		Thing::setDataProperty("_33", &value, 1);
	}

	/**
	* This datatype property is named _41. (Cardinality: [0, 1])
	*/
	double get_41()
	{
		auto property = Thing::getDataProperty<double>("_41");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named _41. (Cardinality: [0, 1])
	*/
	void set_41(double value)
	{
		Thing::setDataProperty("_41", &value, 1);
	}

	/**
	* This datatype property is named _42. (Cardinality: [0, 1])
	*/
	double get_42()
	{
		auto property = Thing::getDataProperty<double>("_42");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named _42. (Cardinality: [0, 1])
	*/
	void set_42(double value)
	{
		Thing::setDataProperty("_42", &value, 1);
	}

	/**
	* This datatype property is named _43. (Cardinality: [0, 1])
	*/
	double get_43()
	{
		auto property = Thing::getDataProperty<double>("_43");
		if (property.second == 1) { return property.first[0]; }

		return (double)0;
	}

	/**
	* This datatype property is named _43. (Cardinality: [0, 1])
	*/
	void set_43(double value)
	{
		Thing::setDataProperty("_43", &value, 1);
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 12])
	*/
	vector<double> getCoordinates()
	{
		auto property = Thing::getDataProperty<double>("coordinates");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 12])
	*/
	void setCoordinates(const vector<double>& vecValues)
	{
		Thing::setDataProperty("coordinates", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named _11. (Cardinality: [0, 1])
	*/
	__declspec(property(get = get_11, put = set_11)) double _11;

	/**
	* This datatype property is named _12. (Cardinality: [0, 1])
	*/
	__declspec(property(get = get_12, put = set_12)) double _12;

	/**
	* This datatype property is named _13. (Cardinality: [0, 1])
	*/
	__declspec(property(get = get_13, put = set_13)) double _13;

	/**
	* This datatype property is named _21. (Cardinality: [0, 1])
	*/
	__declspec(property(get = get_21, put = set_21)) double _21;

	/**
	* This datatype property is named _22. (Cardinality: [0, 1])
	*/
	__declspec(property(get = get_22, put = set_22)) double _22;

	/**
	* This datatype property is named _23. (Cardinality: [0, 1])
	*/
	__declspec(property(get = get_23, put = set_23)) double _23;

	/**
	* This datatype property is named _31. (Cardinality: [0, 1])
	*/
	__declspec(property(get = get_31, put = set_31)) double _31;

	/**
	* This datatype property is named _32. (Cardinality: [0, 1])
	*/
	__declspec(property(get = get_32, put = set_32)) double _32;

	/**
	* This datatype property is named _33. (Cardinality: [0, 1])
	*/
	__declspec(property(get = get_33, put = set_33)) double _33;

	/**
	* This datatype property is named _41. (Cardinality: [0, 1])
	*/
	__declspec(property(get = get_41, put = set_41)) double _41;

	/**
	* This datatype property is named _42. (Cardinality: [0, 1])
	*/
	__declspec(property(get = get_42, put = set_42)) double _42;

	/**
	* This datatype property is named _43. (Cardinality: [0, 1])
	*/
	__declspec(property(get = get_43, put = set_43)) double _43;

	/**
	* This datatype property is named coordinates. (Cardinality: [0, 12])
	*/
	__declspec(property(get = getCoordinates, put = setCoordinates)) vector<double> coordinates;

protected:

	virtual void postCreate()
	{
		Mathematics::postCreate();
	}
};