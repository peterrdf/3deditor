#pragma once

/**
* RDF LTD, Engine API, Copyright 2020, 2021-01-25-08-57-33
*/

#include "../engine.h"
#include <assert.h>

#include <utility>
#include <vector>
#include <map>
#include <string>

using namespace std;

/**
* C++ Model
*/
class _Model;

/**
* C++ Thing
*/
class _Thing
{

public: // Methods

	/**
	* dtor
	*/
	virtual ~_Thing()
	{
	}

	/**
	* Getter
	*/
	virtual int64_t getInstance() const = 0;

	/**
	* Getter
	*/
	virtual _Model* getModel() const = 0;
};

/**
* C++ Property
*/
class _Property
{

public: // Methods

	/**
	* dtor
	*/
	virtual ~_Property()
	{
	}

	/**
	* Getter
	*/
	virtual const char* getName() const = 0;
};

/**
* Flags
*/
static int flagbit0 = 1;			// 2^^0							 0000.0000..0000.0001
static int flagbit1 = 2;			// 2^^1							 0000.0000..0000.0010
static int flagbit2 = 4;			// 2^^2							 0000.0000..0000.0100
static int flagbit3 = 8;			// 2^^3							 0000.0000..0000.1000

static int flagbit4 = 16;			// 2^^4							 0000.0000..0001.0000
static int flagbit5 = 32;			// 2^^5							 0000.0000..0010.0000
static int flagbit6 = 64;			// 2^^6							 0000.0000..0100.0000
static int flagbit7 = 128;			// 2^^7							 0000.0000..1000.0000

static int flagbit8 = 256;			// 2^^8							 0000.0001..0000.0000
static int flagbit9 = 512;			// 2^^9							 0000.0010..0000.0000
static int flagbit10 = 1024;		// 2^^10						 0000.0100..0000.0000
static int flagbit11 = 2048;		// 2^^11						 0000.1000..0000.0000

static int flagbit12 = 4096;		// 2^^12						 0001.0000..0000.0000
static int flagbit13 = 8192;		// 2^^13						 0010.0000..0000.0000
static int flagbit14 = 16384;		// 2^^14						 0100.0000..0000.0000
static int flagbit15 = 32768;		// 2^^15						 1000.0000..0000.0000

static int flagbit16 = 65536;		// 2^^16   0000.0000..0000.0001  0000.0000..0000.0000
static int flagbit17 = 131072;		// 2^^17   0000.0000..0000.0010  0000.0000..0000.0000
static int flagbit18 = 262144;		// 2^^18   0000.0000..0000.0100  0000.0000..0000.0000
static int flagbit19 = 524288;		// 2^^19   0000.0000..0000.1000  0000.0000..0000.0000

static int flagbit20 = 1048576;		// 2^^20   0000.0000..0001.0000  0000.0000..0000.0000
static int flagbit21 = 2097152;		// 2^^21   0000.0000..0010.0000  0000.0000..0000.0000
static int flagbit22 = 4194304;		// 2^^22   0000.0000..0100.0000  0000.0000..0000.0000
static int flagbit23 = 8388608;		// 2^^23   0000.0000..1000.0000  0000.0000..0000.0000

static int flagbit24 = 16777216;	// 2^^24   0000.0001..0000.0000  0000.0000..0000.0000
static int flagbit25 = 33554432;	// 2^^25   0000.0010..0000.0000  0000.0000..0000.0000
static int flagbit26 = 67108864;	// 2^^26   0000.0100..0000.0000  0000.0000..0000.0000
static int flagbit27 = 134217728;	// 2^^27   0000.1000..0000.0000  0000.0000..0000.0000

static int flagbit28 = 268435456;	// 2^^28   0001.0000..0000.0000  0000.0000..0000.0000
static int flagbit29 = 536870912;	// 2^^29   0010.0000..0000.0000  0000.0000..0000.0000
static int flagbit30 = 1073741824;	// 2^^30   0100.0000..0000.0000  0000.0000..0000.0000
static int flagbit31 = 2147483648;	// 2^^31   1000.0000..0000.0000  0000.0000..0000.0000

/**
* Type
*/
typedef map<int64_t, _Thing*> THINGS;

/**
* Type
*/
typedef map<int64_t, map<string, _Property*>> PROPERTIES;

/**
* C++ Model
*/
class _Model
{
	/**
	* Format
	*/
	class Format
	{	

	private: // Members

		/**
		* Model
		*/
		_Model* m_pModel;

	public: // Methods

		Format(_Model* pModel)
			: m_pModel(pModel)
		{
			assert(m_pModel != nullptr);
		}

		virtual ~Format()
		{
		}

		/**
		* SINGLE / DOUBLE PRECISION (float / double)
		*/
		bool getVertexArrayElementsDoublePrecision()
		{
			return getBit(flagbit2);
		}

		/**
		* SINGLE / DOUBLE PRECISION (float / double)
		*/
		void setVertexArrayElementsDoublePrecision(bool value)
		{
			setBit(flagbit2, value);
		}

		/**
		* 32 / 63 BIT INDEX ARRAY (int32_t / int64_t)
		*/
		bool getIndexArrayElementsInt64()
		{
			return getBit(flagbit3);
		}

		/**
		* 32 / 63 BIT INDEX ARRAY (int32_t / int64_t)
		*/
		void setIndexArrayElementsInt64(bool value)
		{
			setBit(flagbit3, value);
		}

		/**
		* OFF / ON VECTORS (x, y, z)
		*/
		bool getVertexArrayContainsXYZ()
		{
			return getBit(flagbit4);
		}

		/**
		* OFF / ON VECTORS (x, y, z)
		*/
		void setVertexArrayContainsXYZ(bool value)
		{
			setBit(flagbit4, value);
		}

		/**
		* OFF / ON NORMALS (Nx, Ny, Nz)
		*/
		bool getVertexArrayContainsNormals()
		{
			return getBit(flagbit5);
		}

		/**
		* OFF / ON NORMALS (Nx, Ny, Nz)
		*/
		void setVertexArrayContainsNormals(bool value)
		{
			setBit(flagbit5, value);
		}

		/**
		* OFF / ON TEXTURE I (u1, v1)
		*/
		bool getVertexArrayContainsUVTextureI()
		{
			return getBit(flagbit6);
		}

		/**
		* OFF / ON TEXTURE I (u1, v1)
		*/
		void setVertexArrayContainsUVTextureI(bool value)
		{
			setBit(flagbit6, value);
		}

		/**
		* OFF / ON TEXTURE II (u2, v2)
		*/
		bool getVertexArrayContainsUVTextureII()
		{
			return getBit(flagbit7);
		}

		/**
		* OFF / ON TEXTURE II (u2, v2)
		*/
		void setVertexArrayContainsUVTextureII(bool value)
		{
			setBit(flagbit7, value);
		}

		/**
		* OFF / ON TRIANGLES
		*/
		bool getIndexArrayContainsTriangles()
		{
			return getBit(flagbit8);
		}

		/**
		* OFF / ON TRIANGLES
		*/
		void setIndexArrayContainsTriangles(bool value)
		{
			setBit(flagbit8, value);
		}

		/**
		* OFF / ON LINES
		*/
		bool getIndexArrayContainsLines()
		{
			return getBit(flagbit9);
		}

		/**
		* OFF / ON LINES
		*/
		void setIndexArrayContainsLines(bool value)
		{
			setBit(flagbit9, value);
		}

		/**
		* OFF / ON POINTS
		*/
		bool getIndexArrayContainsPoints()
		{
			return getBit(flagbit10);
		}

		/**
		* OFF / ON POINTS
		*/
		void setIndexArrayContainsPoints(bool value)
		{
			setBit(flagbit10, value);
		}

		/**
		* OFF / ON WIREFRAME FACES
		*/
		bool getIndexArrayContainsFacePolygons()
		{
			return getBit(flagbit12);
		}

		/**
		* OFF / ON WIREFRAME FACES
		*/
		void setIndexArrayContainsFacePolygons(bool value)
		{
			setBit(flagbit12, value);
		}

		/**
		* OFF / ON WIREFRAME CONCEPTUAL FACES
		*/
		bool getIndexArrayContainsConceptualFacePolygons()
		{
			return getBit(flagbit13);
		}

		/**
		* OFF / ON WIREFRAME CONCEPTUAL FACES
		*/
		void setIndexArrayContainsConceptualFacePolygons(bool value)
		{
			setBit(flagbit13, value);
		}

		/**
		* OFF / ON STRUCTURE WIREFRAME AS POLYGON / TUPLE
		*/
		bool getIndexArrayPolygonsAsTuples()
		{
			return getBit(flagbit14);
		}

		/**
		* OFF / ON STRUCTURE WIREFRAME AS POLYGON / TUPLE
		*/
		void setIndexArrayPolygonsAsTuples(bool value)
		{
			setBit(flagbit14, value);
		}

		/**
		* OFF / ON ADVANCED NORMALS (I.E. FOLLOWING THE SEMANTIC MEANING OF THE CONCEPT)
		*/
		bool getVertexArraySemanticNormals()
		{
			return getBit(flagbit15);
		}

		/**
		* OFF / ON ADVANCED NORMALS (I.E. FOLLOWING THE SEMANTIC MEANING OF THE CONCEPT)
		*/
		void setVertexArraySemanticNormals(bool value)
		{
			setBit(flagbit15, value);
		}

		/**
		* OFF / ON DIRECTX (COMBINATION SETTING BIT 20, 21, 22, 23)
		*/
		bool getSettingsForDirectX()
		{
			return getBit(flagbit16);
		}

		/**
		* OFF / ON DIRECTX (COMBINATION SETTING BIT 20, 21, 22, 23)
		*/
		void setSettingsForDirectX(bool value)
		{
			setBit(flagbit16, value);
		}

		/**
		* OFF / ON OPENGL (COMBINATION SETTING BIT 20, 21, 22, 23)
		*/
		bool getSettingsForOpenGL()
		{
			return getBit(flagbit17);
		}

		/**
		* OFF / ON OPENGL (COMBINATION SETTING BIT 20, 21, 22, 23)
		*/
		void setSettingsForOpenGL(bool value)
		{
			setBit(flagbit17, value);
		}

		/**
		* OFF / ON EXPORT BOTH SIDES OF TRIANGLES
		*/
		bool getIndexArrayCullingBoth()
		{
			return getBit(flagbit18);
		}

		/**
		* OFF / ON EXPORT BOTH SIDES OF TRIANGLES
		*/
		void setIndexArrayCullingBoth(bool value)
		{
			setBit(flagbit18, value);
		}

		/**
		* OFF / ON EXPERT SETTING TRIANGLE WINDING CW / ACW
		*/
		bool getIndexArrayTrainglesAsRHS()
		{
			return getBit(flagbit20);
		}

		/**
		* OFF / ON EXPERT SETTING TRIANGLE WINDING CW / ACW
		*/
		void setIndexArrayTrainglesAsRHS(bool value)
		{
			setBit(flagbit20, value);
		}

		/**
		* OFF / ON EXPERT SETTING NONE / VERTEX + NORMAL Y = Z / Z = -Y
		*/
		bool getVertexArrayRotateYZ()
		{
			return getBit(flagbit21);
		}

		/**
		* OFF / ON EXPERT SETTING NONE / VERTEX + NORMAL Y = Z / Z = -Y
		*/
		void setVertexArrayRotateYZ(bool value)
		{
			setBit(flagbit21, value);
		}

		/**
		* OFF / ON AMBIENT COLOR COMPONENT
		*/
		bool getVertexArrayContainsAmbientColor()
		{
			return getBit(flagbit24);
		}

		/**
		* OFF / ON AMBIENT COLOR COMPONENT
		*/
		void setVertexArrayContainsAmbientColor(bool value)
		{
			setBit(flagbit24, value);
		}

		/**
		* OFF / ON DIFFUSE COLOR COMPONENT
		*/
		bool getVertexArrayContainsDiffuseColor()
		{
			return getBit(flagbit25);
		}

		/**
		* OFF / ON DIFFUSE COLOR COMPONENT
		*/
		void setVertexArrayContainsDiffuseColor(bool value)
		{
			setBit(flagbit25, value);
		}

		/**
		* OFF / ON EMISSIVE COLOR COMPONENT
		*/
		bool getVertexArrayContainsEmissiveColor()
		{
			return getBit(flagbit26);
		}

		/**
		* OFF / ON EMISSIVE COLOR COMPONENT
		*/
		void setVertexArrayContainsEmissiveColor(bool value)
		{
			setBit(flagbit26, value);
		}

		/**
		* OFF / ON SPECULAR COLOR COMPONENT
		*/
		bool getVertexArrayContainsSpecularColor()
		{
			return getBit(flagbit27);
		}

		/**
		* OFF / ON SPECULAR COLOR COMPONENT
		*/
		void setVertexArrayContainsSpecularColor(bool value)
		{
			setBit(flagbit27, value);
		}

		/**
		* OFF / ON TEXTURE I TANGENT
		*/
		bool getVertexArrayContainsTangentTextureI()
		{
			return getBit(flagbit28);
		}

		/**
		* OFF / ON TEXTURE I TANGENT
		*/
		void setVertexArrayContainsTangentTextureI(bool value)
		{
			setBit(flagbit28, value);
		}

		/**
		*  OFF / ON TEXTURE I BINORMAL
		*/
		bool getVertexArrayContainsBinormalTexture()
		{
			return getBit(flagbit29);
		}

		/**
		*  OFF / ON TEXTURE I BINORMAL
		*/
		void setVertexArrayContainsBinormalTextureI(bool value)
		{
			setBit(flagbit29, value);
		}

		/**
		*  OFF / ON TEXTURE II TANGENT (64 BIT ONLY)
		*/
		bool getVertexArrayContainsTangentTextureII()
		{
			return getBit(flagbit30);
		}

		/**
		*  OFF / ON TEXTURE II TANGENT (64 BIT ONLY)
		*/
		void setVertexArrayContainsTangentTextureII(bool value)
		{
			setBit(flagbit30, value);
		}

		/**
		*  OFF / ON TEXTURE II BINORMAL (64 BIT ONLY)
		*/
		bool getVertexArrayContainsBinormalTextureII()
		{
			return getBit(flagbit31);
		}

		/**
		*  OFF / ON TEXTURE II BINORMAL (64 BIT ONLY)
		*/
		void setVertexArrayContainsBinormalTextureII(bool value)
		{
			setBit(flagbit31, value);
		}

		/**
		* SINGLE / DOUBLE PRECISION (float / double)
		*/
		__declspec(property(get = getVertexArrayElementsDoublePrecision, put = setVertexArrayElementsDoublePrecision)) bool vertexArrayElementsDoublePrecision;

		/**
		* 32 / 63 BIT INDEX ARRAY (int32_t / int64_t)
		*/
		__declspec(property(get = getIndexArrayElementsInt64, put = setIndexArrayElementsInt64)) bool indexArrayElementsInt64;

		/**
		* OFF / ON VECTORS (x, y, z)
		*/
		__declspec(property(get = getVertexArrayContainsXYZ, put = setVertexArrayContainsXYZ)) bool vertexArrayContainsXYZ;

		/**
		* OFF / ON NORMALS (Nx, Ny, Nz)
		*/
		__declspec(property(get = getVertexArrayContainsNormals, put = setVertexArrayContainsNormals)) bool vertexArrayContainsNormals;

		/**
		* OFF / ON TEXTURE I (u1, v1)
		*/
		__declspec(property(get = getVertexArrayContainsUVTextureI, put = setVertexArrayContainsUVTextureI)) bool vertexArrayContainsUVTextureI;

		/**
		* OFF / ON TEXTURE II (u2, v2)
		*/
		__declspec(property(get = getVertexArrayContainsUVTextureII, put = setVertexArrayContainsUVTextureII)) bool vertexArrayContainsUVTextureII;

		/**
		* OFF / ON TRIANGLES
		*/
		__declspec(property(get = getIndexArrayContainsTriangles, put = setIndexArrayContainsTriangles)) bool indexArrayContainsTriangles;

		/**
		* OFF / ON LINES
		*/
		__declspec(property(get = getIndexArrayContainsLines, put = setIndexArrayContainsLines)) bool indexArrayContainsLines;

		/**
		* OFF / ON POINTS
		*/
		__declspec(property(get = getIndexArrayContainsPoints, put = setIndexArrayContainsPoints)) bool indexArrayContainsPoints;

		/**
		* OFF / ON WIREFRAME FACES
		*/
		__declspec(property(get = getIndexArrayContainsFacePolygons, put = setIndexArrayContainsFacePolygons)) bool indexArrayContainsFacePolygons;

		/**
		* OFF / ON WIREFRAME CONCEPTUAL FACES
		*/
		__declspec(property(get = getIndexArrayContainsConceptualFacePolygons, put = setIndexArrayContainsConceptualFacePolygons)) bool indexArrayContainsConceptualFacePolygons;

		/**
		* OFF / ON STRUCTURE WIREFRAME AS POLYGON / TUPLE
		*/
		__declspec(property(get = getIndexArrayPolygonsAsTuples, put = setIndexArrayPolygonsAsTuples)) bool indexArrayPolygonsAsTuples;

		/**
		* OFF / ON ADVANCED NORMALS (I.E. FOLLOWING THE SEMANTIC MEANING OF THE CONCEPT)
		*/
		__declspec(property(get = getVertexArraySemanticNormals, put = setVertexArraySemanticNormals)) bool vertexArraySemanticNormals;

		/**
		* OFF / ON DIRECTX (COMBINATION SETTING BIT 20, 21, 22, 23)
		*/
		__declspec(property(get = getSettingsForDirectX, put = setSettingsForDirectX)) bool settingsForDirectX;

		/**
		* OFF / ON OPENGL (COMBINATION SETTING BIT 20, 21, 22, 23)
		*/
		__declspec(property(get = getSettingsForOpenGL, put = setSettingsForOpenGL)) bool settingsForOpenGL;

		/**
		* OFF / ON EXPORT BOTH SIDES OF TRIANGLES
		*/
		__declspec(property(get = getIndexArrayCullingBoth, put = setIndexArrayCullingBoth)) bool indexArrayCullingBoth;

		/**
		* OFF / ON EXPERT SETTING TRIANGLE WINDING CW / ACW
		*/
		__declspec(property(get = getIndexArrayTrainglesAsRHS, put = setIndexArrayTrainglesAsRHS)) bool indexArrayTrainglesAsRHS;

		/**
		* OFF / ON EXPERT SETTING NONE / VERTEX + NORMAL Y = Z / Z = -Y
		*/
		__declspec(property(get = getVertexArrayRotateYZ, put = setVertexArrayRotateYZ)) bool vertexArrayRotateYZ;

		/**
		* OFF / ON AMBIENT COLOR COMPONENT
		*/
		__declspec(property(get = getVertexArrayContainsAmbientColor, put = setVertexArrayContainsAmbientColor)) bool vertexArrayContainsAmbientColor;

		/**
		* OFF / ON DIFFUSE COLOR COMPONENT
		*/
		__declspec(property(get = getVertexArrayContainsDiffuseColor, put = setVertexArrayContainsDiffuseColor)) bool vertexArrayContainsDiffuseColor;

		/**
		* OFF / ON EMISSIVE COLOR COMPONENT
		*/
		__declspec(property(get = getVertexArrayContainsEmissiveColor, put = setVertexArrayContainsEmissiveColor)) bool vertexArrayContainsEmissiveColor;

		/**
		* OFF / ON SPECULAR COLOR COMPONENT
		*/
		__declspec(property(get = getVertexArrayContainsSpecularColor, put = setVertexArrayContainsSpecularColor)) bool vertexArrayContainsSpecularColor;

		/**
		* OFF / ON TEXTURE I TANGENT
		*/
		__declspec(property(get = getVertexArrayContainsTangentTextureI, put = setVertexArrayContainsTangentTextureI)) bool vertexArrayContainsTangentTextureI;

		/**
		* OFF / ON TEXTURE I BINORMAL
		*/
		__declspec(property(get = getVertexArrayContainsBinormalTexture, put = setVertexArrayContainsBinormalTextureI)) bool vertexArrayContainsBinormalTextureI;

		/**
		* OFF / ON TEXTURE II TANGENT (64 BIT ONLY)
		*/
		__declspec(property(get = getVertexArrayContainsTangentTextureII, put = setVertexArrayContainsTangentTextureII)) bool vertexArrayContainsTangentTextureII;

		/**
		* OFF / ON TEXTURE II BINORMAL (64 BIT ONLY)
		*/
		__declspec(property(get = getVertexArrayContainsBinormalTextureII, put = setVertexArrayContainsBinormalTextureII)) bool vertexArrayContainsBinormalTextureII;

	private: // Methods		

		/**
		* Setter
		*/
		void setBit(int64_t mask, bool value)
		{
			if (value)
			{
				setBit(mask);
			}
			else
			{
				clearBit(mask);
			}
		}

		/**
		* Getter
		*/
		bool getBit(int64_t mask)
		{
			int64_t value = getValue();

			return (value & mask) == mask;
		}

		/**
		* Setter
		*/
		void setBit(int64_t mask)
		{
			int64_t value = getValue();

			value |= mask;

			SetFormat(m_pModel->getInstance(), value, GetFormat(0, 0));
		}

		/**
		* Setter
		*/
		void clearBit(int64_t mask)
		{
			assert(m_pModel->getInstance() != 0);

			int64_t value = getValue();

			value &= ~mask;

			SetFormat(m_pModel->getInstance(), value, GetFormat(0, 0));
		}

		/**
		* Getter
		*/
		int64_t getValue()
		{
			assert(m_pModel->getInstance() != 0);

			return GetFormat(m_pModel->getInstance(), GetFormat(0, 0));
		}
	};	

private: // Members

	/**
	* Model
	*/
	int64_t m_iModel;

	/**
	* Owner
	*/
	bool m_bCloseModel;

	/**
	* Format
	*/
	Format* m_pFormat;

	/**
	* Cache
	*/
	map<int64_t, _Thing*> m_mapThings;	

	/**
	* Properties
	*/
	map<int64_t, map<string, _Property*>> m_mapProperties;	

public: // Methods

	/**
	* ctor
	*/
	_Model()
		: m_iModel(0)
		, m_bCloseModel(false)
		, m_pFormat(NULL)
		, m_mapThings()
		, m_mapProperties()		
	{
	}

	/**
	* ctor
	*/
	_Model(int64_t iModel)
		: m_iModel(iModel)
		, m_bCloseModel(false)
		, m_pFormat(NULL)
		, m_mapThings()
		, m_mapProperties()
	{
	}

	/**
	* dtor
	*/
	virtual ~_Model()
	{
		cleanup();		

		if (m_bCloseModel && (m_iModel != 0))
		{
			CloseModel(m_iModel);
		}

		delete m_pFormat;
		m_pFormat = NULL;
	}

	/**
	* Getter
	*/
	virtual int64_t getInstance() const
	{
		return m_iModel;
	}

	/**
	* Load
	*/
	virtual int64_t open(const wchar_t* szPath)
	{
		if (m_bCloseModel && (m_iModel != 0))
		{
			CloseModel(m_iModel);
		}

		m_iModel = OpenModelW(szPath);
		m_bCloseModel = true;

		return m_iModel;
	}	

	/**
	* Cleanup and Close
	*/
	virtual void close()
	{
		cleanup();

		if (m_bCloseModel && (m_iModel != 0))
		{
			CloseModel(m_iModel);
		}

		m_iModel = 0;
		m_bCloseModel = false;
	}

	/**
	* Format
	*/
	Format* getFormat()
	{
		if (m_pFormat == nullptr)
		{
			m_pFormat = new Format(this);
		}

		return m_pFormat;
	}

	__declspec(property(get = getFormat)) Format* format;

	/**
	* Save
	*/
	virtual int64_t saveModelW(const wchar_t* szPath)
	{
		assert(m_iModel != 0);

		return SaveModelW(m_iModel, szPath);
	}

	/**
	* Getter
	*/
	const THINGS& getThings()
	{
		return m_mapThings;
	}

	/**
	* Factory - Model is the owner
	*/
	template <typename T>
	T* create()
	{
		static_assert(is_base_of<_Thing, T>::value, "T must derive from _Thing.");

		T* t = new T();

		// Inject the Model
		t->m_pModel = this;

		// Create
		int64_t iClass = GetClassByName(m_iModel, T::getClassName());
		assert(iClass != 0);

		int64_t iInstance = CreateInstance(iClass, NULL);
		t->m_iInstance = iInstance;

		// Cache
		assert(m_mapThings.find(iInstance) == m_mapThings.end());
		m_mapThings[iInstance] = t;

		// Extra initialization
		t->postCreate();

		return t;
	}

	/**
	* Factory - Model is the owner
	*/
	template <typename T>
	T* create(int64_t iInstance)
	{
		static_assert(is_base_of<_Thing, T>::value, "T must derive from _Thing.");
		assert(iInstance != 0);	

		if (!IsInstance(iInstance))
		{
			assert(false);

			return NULL;
		}

		THINGS::iterator itThings = m_mapThings.find(iInstance);
		if (itThings == m_mapThings.end())
		{
			// New
			T* t = new T();

			// Inject the Model/Instance
			t->m_pModel = this;
			t->m_iInstance = iInstance;

			// Cache			
			m_mapThings[iInstance] = t;

			// Extra initialization
			t->postCreate();

			return t;
		}
		
		// Reuse
		return dynamic_cast<T*>(itThings->second);
	}

	/**
	* Factory - Model is the owner
	*/
	template <typename T>
	T* createProperty(int64_t iInstance, const char* szName)
	{
		static_assert(is_base_of<_Property, T>::value, "T must derive from _Property.");
		assert(iInstance != 0);

		if (!IsInstance(iInstance))
		{
			assert(false);

			return NULL;
		}

		assert((szName != NULL) && (strlen(szName) > 0));

		T* t = NULL;

		PROPERTIES::iterator itInstanceProperties = m_mapProperties.find(iInstance);
		if (itInstanceProperties == m_mapProperties.end())
		{
			itInstanceProperties = m_mapProperties.insert(pair<int64_t, map<string, _Property*>>(iInstance, map<string, _Property*>())).first;
		}

		map<string, _Property*>::iterator itProperties = itInstanceProperties->second.find(szName);
		if (itProperties == itInstanceProperties->second.end())
		{
			// New
			t = new T();

			// Inject the Model
			t->m_pModel = this;

			// Inject the Instance
			t->m_iInstance = iInstance;

			// Inject the Name
			t->m_strName = szName;

			// Cache
			itInstanceProperties->second[szName] = t;
		}
		else
		{
			// Reuse
			t = dynamic_cast<T*>(itProperties->second);
		}

		return t;
	}

	/**
	* Setter - Data Property
	*/
	template<typename T>
	void setDataProperty(int64_t iInstance, const char* szProperty, const T* pValues, int64_t iCard)
	{
		static_assert(is_integral<T>::value || 
			is_floating_point<T>::value || 
			is_same<T, char*>::value ||
			is_enum<T>::value,
			"T must be integral, float, char* or enum type.");

		assert(m_iModel != 0);
		assert(iInstance != 0);
		assert((szProperty != NULL) && (strlen(szProperty) > 0));

		SetDatatypeProperty(iInstance, GetPropertyByName(m_iModel, szProperty), pValues, iCard);
	}

	/**
	* Setter - Data Property
	*/
	template<typename T>
	pair<T*, int64_t> getDataProperty(int64_t iInstance, const char* szProperty)
	{
		static_assert(is_integral<T>::value ||
			is_floating_point<T>::value ||
			is_same<T, char*>::value ||
			is_enum<T>::value,
			"T must be integral, float, char* or enum type.");

		int64_t iCard = 0;
		T* pValues = NULL;
		GetDatatypeProperty(iInstance, GetPropertyByName(m_iModel, szProperty), (void**)&pValues, &iCard);

		return pair<T*, int64_t>(pValues, iCard);
	}

	/**
	* Setter - Thing Property
	*/
	template <typename T>
	void setThingProperty(int64_t iInstance, const char* szProperty, const T** pValues, int64_t iCard)
	{
		static_assert(is_base_of<_Thing, T>::value, "T must derive from _Thing.");

		assert(m_iModel != 0);
		assert(iInstance != 0);
		assert((szProperty != NULL) && (strlen(szProperty) > 0));

		vector<int64_t> vecValues;
		for (int64_t iThing = 0; iThing < iCard; iThing++)
		{
			if (pValues[iThing] != NULL)
			{
				vecValues.push_back(pValues[iThing]->getInstance());
			}
		}

		SetObjectProperty(iInstance, GetPropertyByName(m_iModel, szProperty), vecValues.data(), vecValues.size());
	}

	/**
	* Setter - Thing Property
	*/
	template <typename T>
	vector<T*> getThingProperty(int64_t iInstance, const char* szProperty)
	{
		static_assert(is_base_of<_Thing, T>::value, "T must derive from _Thing.");

		assert(m_iModel != 0);
		assert(iInstance != 0);
		assert((szProperty != NULL) && (strlen(szProperty) > 0));

		int64_t iCard = 0;
		int64_t* piInstances = NULL;
		GetObjectProperty(iInstance, GetPropertyByName(m_iModel, szProperty), &piInstances, &iCard);

		vector<T*> vecThings;
		for (int64_t iValue = 0; iValue < iCard; iValue++)
		{
			THINGS::iterator itThings = m_mapThings.find(piInstances[iValue]);
			if (itThings == m_mapThings.end())
			{
				// New
				auto value = create<T>(piInstances[iValue]);

				vecThings.push_back(value);
			}
			else
			{
				// Reuse
				vecThings.push_back(dynamic_cast<T*>(itThings->second));
			}
		} // for (int64_t iValue = ...
	}

	/**
	* Release the memory
	*/
	void cleanup()
	{
		/**
		* Things
		*/
		THINGS::iterator itThings = m_mapThings.begin();
		for (; itThings != m_mapThings.end(); itThings++)
		{
			delete itThings->second;
		}

		m_mapThings.clear();

		/**
		* Properties
		*/
		PROPERTIES::iterator itInstanceProperties = m_mapProperties.begin();
		for (; itInstanceProperties != m_mapProperties.end(); itInstanceProperties++)
		{
			map<string, _Property*>::iterator itProperties = itInstanceProperties->second.begin();
			for (; itProperties != itInstanceProperties->second.end(); itProperties++)
			{
				delete itProperties->second;
			}
		}

		m_mapProperties.clear();
	}
};

