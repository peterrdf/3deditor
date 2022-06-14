#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "Solid.h"
#include "Face.h"
#include "Property.h"

/**
* This class represents the concept BoundaryRepresentation. (http://rdf.bg/BoundaryRepresentation)
*/
class BoundaryRepresentation : public Solid
{

	/**
	* Friends
	*/
	friend class _Model;

	/**
	* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
	*/
	class BoundaryRepresentation_consistencyCheck : public BitsetDataProperty<int64_t>
	{

	public:

		BoundaryRepresentation_consistencyCheck()
		{
		}

		virtual ~BoundaryRepresentation_consistencyCheck()
		{
		}

		/**
		* Merge elements in the vertex array are duplicated (epsilon used as distance).
		*/
		bool getMergeVertices()
		{
			return getBit(1);
		}

		/**
		* Merge elements in the vertex array are duplicated (epsilon used as distance).
		*/
		void setMergeVertices(bool value)
		{
			setBit(1, value);
		}

		/**
		* Remove elements in the vertex array that are not referenced by elements in the index array (interpreted as SET if flags are defined).
		*/
		bool getCleanVertices()
		{
			return getBit(2);
		}

		/**
		* Remove elements in the vertex array that are not referenced by elements in the index array (interpreted as SET if flags are defined).
		*/
		void setCleanVertices(bool value)
		{
			setBit(2, value);
		}

		/**
		* Merge polygons placed in the same plane and sharing at least one edge.
		*/
		bool getSimpleMergePolygons()
		{
			return getBit(4);
		}

		/**
		* Merge polygons placed in the same plane and sharing at least one edge.
		*/
		void setSimpleMergePolygons(bool value)
		{
			setBit(4, value);
		}

		/**
		* Merge polygons advanced (check of polygons have the opposite direction and are overlapping, but don't share points).
		*/
		bool getComplexMergePolygons()
		{
			return getBit(8);
		}

		/**
		* Merge polygons advanced (check of polygons have the opposite direction and are overlapping, but don't share points).
		*/
		void setComplexMergePolygons(bool value)
		{
			setBit(8, value);
		}

		/**
		* Check if faces are wrongly turned opposite from each other.
		*/
		bool getCheckWinding()
		{
			return getBit(16);
		}

		/**
		* Check if faces are wrongly turned opposite from each other.
		*/
		void setCheckWinding(bool value)
		{
			setBit(16, value);
		}

		/**
		* Check if faces are inside-out.
		*/
		bool getCheckInverted()
		{
			return getBit(32);
		}

		/**
		* Check if faces are inside-out.
		*/
		void setCheckInverted(bool value)
		{
			setBit(32, value);
		}

		/**
		* Check if faces result in solid, if not generate both sided faces.
		*/
		bool getCheckIfSolid()
		{
			return getBit(64);
		}

		/**
		* Check if faces result in solid, if not generate both sided faces.
		*/
		void setCheckIfSolid(bool value)
		{
			setBit(64, value);
		}

		/**
		* Invert direction of the faces / normals.
		*/
		bool getInvertFaces()
		{
			return getBit(128);
		}

		/**
		* Invert direction of the faces / normals.
		*/
		void setInvertFaces(bool value)
		{
			setBit(128, value);
		}

		/**
		* Export all faces as one conceptual face.
		*/
		bool getMergeAllFaces()
		{
			return getBit(256);
		}

		/**
		* Export all faces as one conceptual face.
		*/
		void setMergeAllFaces(bool value)
		{
			setBit(256, value);
		}

		/**
		* Remove irrelevant intermediate points on lines.
		*/
		bool getOptimizeVertices()
		{
			return getBit(512);
		}

		/**
		* Remove irrelevant intermediate points on lines.
		*/
		void setOptimizeVertices(bool value)
		{
			setBit(512, value);
		}

		/**
		* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
		*/
		__declspec(property(get = getMergeVertices, put = setMergeVertices)) bool mergeVertices;

		/**
		* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
		*/
		__declspec(property(get = getCleanVertices, put = setCleanVertices)) bool cleanVertices;

		/**
		* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
		*/
		__declspec(property(get = getSimpleMergePolygons, put = setSimpleMergePolygons)) bool simpleMergePolygons;

		/**
		* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
		*/
		__declspec(property(get = getComplexMergePolygons, put = setComplexMergePolygons)) bool complexMergePolygons;

		/**
		* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
		*/
		__declspec(property(get = getCheckWinding, put = setCheckWinding)) bool checkWinding;

		/**
		* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
		*/
		__declspec(property(get = getCheckInverted, put = setCheckInverted)) bool checkInverted;

		/**
		* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
		*/
		__declspec(property(get = getCheckIfSolid, put = setCheckIfSolid)) bool checkIfSolid;

		/**
		* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
		*/
		__declspec(property(get = getInvertFaces, put = setInvertFaces)) bool invertFaces;

		/**
		* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
		*/
		__declspec(property(get = getMergeAllFaces, put = setMergeAllFaces)) bool mergeAllFaces;

		/**
		* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
		*/
		__declspec(property(get = getOptimizeVertices, put = setOptimizeVertices)) bool optimizeVertices;
	};
	BoundaryRepresentation_consistencyCheck* m_pConsistencyCheck;

public:

	BoundaryRepresentation()
	{
	}

	virtual ~BoundaryRepresentation()
	{
	}

	static const char* getClassName()
	{
		return "BoundaryRepresentation";
	}

	/**
	* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
	*/
	BoundaryRepresentation_consistencyCheck* getConsistencyCheck()
	{
		return m_pConsistencyCheck;
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
	* This object property represents the relation faces. (Cardinality: [0, Inf])
	*/
	vector<Face*> getFaces()
	{
		auto property = Thing::getThingProperty<Face>("faces");
		return property;
	}

	/**
	* This object property represents the relation faces. (Cardinality: [0, Inf])
	*/
	void setFaces(const vector<Face*>& vecValues)
	{
		Thing::setThingProperty("faces", (const _Thing**)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named flags. (Cardinality: [0, Inf])
	*/
	vector<int64_t> getFlags()
	{
		auto property = Thing::getDataProperty<int64_t>("flags");
		vector<int64_t> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named flags. (Cardinality: [0, Inf])
	*/
	void setFlags(const vector<int64_t>& vecValues)
	{
		Thing::setDataProperty("flags", (const int64_t*)vecValues.data(), vecValues.size());
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
	* This datatype property is named indices. (Cardinality: [0, Inf])
	*/
	vector<int64_t> getIndices()
	{
		auto property = Thing::getDataProperty<int64_t>("indices");
		vector<int64_t> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named indices. (Cardinality: [0, Inf])
	*/
	void setIndices(const vector<int64_t>& vecValues)
	{
		Thing::setDataProperty("indices", (const int64_t*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named normalCoordinates. (Cardinality: [0, Inf])
	*/
	vector<double> getNormalCoordinates()
	{
		auto property = Thing::getDataProperty<double>("normalCoordinates");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named normalCoordinates. (Cardinality: [0, Inf])
	*/
	void setNormalCoordinates(const vector<double>& vecValues)
	{
		Thing::setDataProperty("normalCoordinates", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named normalIndices. (Cardinality: [0, Inf])
	*/
	vector<int64_t> getNormalIndices()
	{
		auto property = Thing::getDataProperty<int64_t>("normalIndices");
		vector<int64_t> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named normalIndices. (Cardinality: [0, Inf])
	*/
	void setNormalIndices(const vector<int64_t>& vecValues)
	{
		Thing::setDataProperty("normalIndices", (const int64_t*)vecValues.data(), vecValues.size());
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
	* This datatype property is named textureCoordinates. (Cardinality: [0, Inf])
	*/
	vector<double> getTextureCoordinates()
	{
		auto property = Thing::getDataProperty<double>("textureCoordinates");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named textureCoordinates. (Cardinality: [0, Inf])
	*/
	void setTextureCoordinates(const vector<double>& vecValues)
	{
		Thing::setDataProperty("textureCoordinates", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named textureIndices. (Cardinality: [0, Inf])
	*/
	vector<int64_t> getTextureIndices()
	{
		auto property = Thing::getDataProperty<int64_t>("textureIndices");
		vector<int64_t> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named textureIndices. (Cardinality: [0, Inf])
	*/
	void setTextureIndices(const vector<int64_t>& vecValues)
	{
		Thing::setDataProperty("textureIndices", (const int64_t*)vecValues.data(), vecValues.size());
	}

	/**
	* This datatype property is named vertices. (Cardinality: [0, Inf])
	*/
	vector<double> getVertices()
	{
		auto property = Thing::getDataProperty<double>("vertices");
		vector<double> vecValues(property.first, property.first + property.second);
		return vecValues;
	}

	/**
	* This datatype property is named vertices. (Cardinality: [0, Inf])
	*/
	void setVertices(const vector<double>& vecValues)
	{
		Thing::setDataProperty("vertices", (const double*)vecValues.data(), vecValues.size());
	}

	/**
	* The type of the BooleanOperation affecting the resulting shape of this instance. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getConsistencyCheck)) BoundaryRepresentation_consistencyCheck* consistencyCheck;

	/**
	* This datatype property is named epsilon. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getEpsilon, put = setEpsilon)) double epsilon;

	/**
	* This object property represents the relation faces. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getFaces, put = setFaces)) vector<Face*> faces;

	/**
	* This datatype property is named flags. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getFlags, put = setFlags)) vector<int64_t> flags;

	/**
	* This datatype property is named fraction. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getFraction, put = setFraction)) double fraction;

	/**
	* This datatype property is named indices. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getIndices, put = setIndices)) vector<int64_t> indices;

	/**
	* This datatype property is named normalCoordinates. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getNormalCoordinates, put = setNormalCoordinates)) vector<double> normalCoordinates;

	/**
	* This datatype property is named normalIndices. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getNormalIndices, put = setNormalIndices)) vector<int64_t> normalIndices;

	/**
	* This datatype property is named relativeEpsilon. (Cardinality: [0, 1])
	*/
	__declspec(property(get = getRelativeEpsilon, put = setRelativeEpsilon)) double relativeEpsilon;

	/**
	* This datatype property is named textureCoordinates. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getTextureCoordinates, put = setTextureCoordinates)) vector<double> textureCoordinates;

	/**
	* This datatype property is named textureIndices. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getTextureIndices, put = setTextureIndices)) vector<int64_t> textureIndices;

	/**
	* This datatype property is named vertices. (Cardinality: [0, Inf])
	*/
	__declspec(property(get = getVertices, put = setVertices)) vector<double> vertices;

protected:

	virtual void postCreate()
	{
		Solid::postCreate();

		m_pConsistencyCheck = createProperty<BoundaryRepresentation_consistencyCheck>("consistencyCheck");
	}
};