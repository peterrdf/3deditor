#pragma once

#include "_geometry.h"

#include <string>
#include <vector>
#include <map>
#include "engine.h"

using namespace std;

// ------------------------------------------------------------------------------------------------
struct VECTOR3
{
	double x;
	double y;
	double z;
};

// ------------------------------------------------------------------------------------------------
struct MATRIX
{
	double _11, _12, _13;
	double _21, _22, _23;
	double _31, _32, _33;
	double _41, _42, _43;
};

// ------------------------------------------------------------------------------------------------
struct OGLMATRIX
{
	double _11, _12, _13, _14;
	double _21, _22, _23, _24;
	double _31, _32, _33, _34;
	double _41, _42, _43, _44;
};

// ------------------------------------------------------------------------------------------------
static void OGLMatrixIdentity(OGLMATRIX* pOut)
{
	memset(pOut, 0, sizeof(OGLMATRIX));

	pOut->_11 = pOut->_22 = pOut->_33 = pOut->_44 = 1.;
}

// ------------------------------------------------------------------------------------------------
static void	Transform(const VECTOR3 * pV, const MATRIX *pM, VECTOR3 * pOut)
{
	VECTOR3	pTmp;
	pTmp.x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
	pTmp.y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
	pTmp.z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;

	pOut->x = pTmp.x;
	pOut->y = pTmp.y;
	pOut->z = pTmp.z;
}

/**
* C++ Wrapper
*/
template<class V>
class _VertexBuffer
{
	/**
	* Thing will populate the buffer
	*/
	friend class Thing;

public: // Members

	/**
	* Vertices
	*/
	V* m_pVertices;

	/**
	* Count
	*/
	int64_t m_iVerticesCount;

	/**
	* Length
	*/
	int64_t m_iVertexLength;

public: // Methods

	/**
	* ctor
	*/
	_VertexBuffer()
		: m_pVertices(nullptr)
		, m_iVerticesCount(0)
		, m_iVertexLength(0)
	{
		static_assert(
			is_same<V, float>::value ||
			is_same<V, double>::value,
			"V must be float or double type.");
	}

	/**
	* dtor
	*/
	virtual ~_VertexBuffer()
	{
		delete[] m_pVertices;
	}

	/**
	* Getter
	*/
	V* getVertices() const
	{
		return m_pVertices;
	}

	/**
	* Getter
	*/
	int64_t getVerticesCount() const
	{
		return m_iVerticesCount;
	}

	/**
	* Getter
	*/
	int64_t getVertexLength() const
	{
		return m_iVertexLength;
	}
};

/**
* C++ Wrapper
*/
template<class I>
class _IndexBuffer
{
	/**
	* Thing will populate the buffer
	*/
	friend class Thing;

public: // Members

	/**
	* Indices
	*/
	I* m_pIndices;

	/**
	* Count
	*/
	int64_t m_iIndicesCount;

public: // Methods

	/**
	* ctor
	*/
	_IndexBuffer()
		: m_pIndices(nullptr)
		, m_iIndicesCount(0)
	{
		static_assert(
			is_same<I, int32_t>::value ||
			is_same<I, int64_t>::value,
			"I must be int32_t or int64_t type.");
	}

	/**
	* dtor
	*/
	virtual ~_IndexBuffer()
	{
		delete[] m_pIndices;
	}

	/**
	* Getter
	*/
	I* getIndices() const
	{
		return m_pIndices;
	}

	/**
	* Getter
	*/
	int64_t getIndicesCount() const
	{
		return m_iIndicesCount;
	}
};

/**
* int32_t
*/
typedef _IndexBuffer<int32_t> IndexBuffer;

/**
* Double
*/
typedef _IndexBuffer<int64_t> DoubleIndexBuffer;

/**
* Float
*/
typedef _VertexBuffer<float> VertexBuffer;

/**
* Double
*/
typedef _VertexBuffer<double> DoubleVertexBuffer;

// ------------------------------------------------------------------------------------------------
// (255 * 255 * 255)[R] + (255 * 255)[G] + 255[B]
class CIn64RGBCoder
{	

public: // Methods

	// --------------------------------------------------------------------------------------------
	static void Encode(int64_t i, float& fR, float& fG, float& fB)
	{
		static const float STEP = 1.f / 255.f;

		fR = 0.f;
		fG = 0.f;
		fB = 0.f;

		// R
		if (i >= (255 * 255))
		{
			int64_t iR = i / (255 * 255);
			fR = iR * STEP;

			i -= iR * (255 * 255);
		}

		// G
		if (i >= 255)
		{
			int64_t iG = i / 255;
			fG = iG * STEP;

			i -= iG * 255;
		}

		// B		
		fB = i * STEP;
	}

	// --------------------------------------------------------------------------------------------
	static int64_t Decode(unsigned char R, unsigned char G, unsigned char B)
	{
		int64_t i = 0;

		// R
		i += R * (255 * 255);

		// G
		i += G * 255;

		// B
		i += B;

		return i;
	}
};

// ------------------------------------------------------------------------------------------------
class CRDFColor
{

private: // Members

	// --------------------------------------------------------------------------------------------
	float m_fR;

	// --------------------------------------------------------------------------------------------
	float m_fG;

	// --------------------------------------------------------------------------------------------
	float m_fB;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFColor()
		: m_fR(0.f)
		, m_fG(0.f)
		, m_fB(0.f)
	{
	}

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFColor(float fR, float fG, float fB)
		: m_fR(fR)
		, m_fG(fG)
		, m_fB(fB)
	{
	}

	// --------------------------------------------------------------------------------------------
	// Set up
	void Init(unsigned int iColor)
	{
		m_fR = (float)(iColor & ((unsigned int)255 * 256 * 256 * 256)) / (256 * 256 * 256);
		m_fR /= 255.f;

		m_fG = (float)(iColor & (255 * 256 * 256)) / (256 * 256);
		m_fG /= 255.f;

		m_fB = (float)(iColor & (255 * 256)) / 256;
		m_fB /= 255.f;
	}

	// --------------------------------------------------------------------------------------------
	// Set up
	void Init(float fR, float fG, float fB)
	{
		m_fR = fR;
		m_fG = fG;
		m_fB = fB;
	}

	// --------------------------------------------------------------------------------------------
	// Getter
	float R() const
	{
		return m_fR;
	}

	// --------------------------------------------------------------------------------------------
	// Getter
	float G() const
	{
		return m_fG;
	}

	// --------------------------------------------------------------------------------------------
	// Getter
	float B() const
	{
		return m_fB;
	}

	// --------------------------------------------------------------------------------------------
	// operator
	bool operator == (const CRDFColor& c) const
	{
		return
			((m_fR == c.m_fR) &&
				(m_fG == c.m_fG) &&
				(m_fB == c.m_fB));
	}

	// --------------------------------------------------------------------------------------------
	// operator
	bool operator < (const CRDFColor& c) const
	{
		if (m_fR < c.m_fR)
		{
			return true;
		}

		if (m_fR > c.m_fR)
		{
			return false;
		}

		if (m_fG < c.m_fG)
		{
			return true;
		}

		if (m_fG > c.m_fG)
		{
			return false;
		}

		if (m_fB < c.m_fB)
		{
			return true;
		}

		if (m_fB > c.m_fB)
		{
			return false;
		}

		return false;
	}

	// --------------------------------------------------------------------------------------------
	// operator
	bool operator > (const CRDFColor& c) const
	{
		if (m_fR > c.m_fR)
		{
			return true;
		}

		if (m_fR < c.m_fR)
		{
			return false;
		}

		if (m_fG > c.m_fG)
		{
			return true;
		}

		if (m_fG < c.m_fG)
		{
			return false;
		}

		if (m_fB > c.m_fB)
		{
			return true;
		}

		if (m_fB < c.m_fB)
		{
			return false;
		}

		return false;
	}
};

// ------------------------------------------------------------------------------------------------
class CRDFMaterial
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// Color
	CRDFColor m_clrAmbient;

	// --------------------------------------------------------------------------------------------
	// Color
	CRDFColor m_clrDiffuse;

	// --------------------------------------------------------------------------------------------
	// Color
	CRDFColor m_clrEmissive;

	// --------------------------------------------------------------------------------------------
	// Color
	CRDFColor m_clrSpecular;

	// ------------------------------------------------------------------------------------------------
	float m_fA;

	// ------------------------------------------------------------------------------------------------
	// Texture
	wstring m_strTexture;

public: // Methods

	// ------------------------------------------------------------------------------------------------
	// ctor
	CRDFMaterial()
		: m_clrAmbient()
		, m_clrDiffuse()
		, m_clrEmissive()
		, m_clrSpecular()
		, m_fA(1.f)
		, m_strTexture(L"")
	{
	}

	// ------------------------------------------------------------------------------------------------
	// ctor
	CRDFMaterial(unsigned int iAmbientColor, unsigned int iDiffuseColor, unsigned int iEmissiveColor, unsigned int iSpecularColor, float fTransparency)
		: m_clrAmbient()
		, m_clrDiffuse()
		, m_clrEmissive()
		, m_clrSpecular()
		, m_fA(1.f)
		, m_strTexture(L"")
	{
		if ((iAmbientColor == 0) && (iDiffuseColor == 0) && (iEmissiveColor == 0) && (iSpecularColor == 0) && (fTransparency == 0.f))
		{
			/*
			* There is no material - use non-transparent black
			*/
			m_clrAmbient.Init(0);
			m_clrDiffuse.Init(0);
			m_clrEmissive.Init(0);
			m_clrSpecular.Init(0);

			m_fA = 1.f;
		}
		else
		{
			m_clrAmbient.Init(iAmbientColor);
			m_clrDiffuse.Init(iDiffuseColor == 0 ? iAmbientColor : iDiffuseColor);
			m_clrEmissive.Init(iEmissiveColor);
			m_clrSpecular.Init(iSpecularColor);

			m_fA = fTransparency;
		}
	}

	// --------------------------------------------------------------------------------------------
	// Initialize
	void init(
		unsigned int iAmbientColor, 
		unsigned int iDiffuseColor, 
		unsigned int iEmissiveColor, 
		unsigned int iSpecularColor, 
		float fTransparency,
		const wchar_t* szTexture)
	{
		if ((iAmbientColor == 0) && (iDiffuseColor == 0) && (iEmissiveColor == 0) && (iSpecularColor == 0) && (fTransparency == 0.f))
		{
			/*
			* There is no material - use non-transparent black
			*/
			m_clrAmbient.Init(0);
			m_clrDiffuse.Init(0);
			m_clrEmissive.Init(0);
			m_clrSpecular.Init(0);

			m_fA = 1.f;
		}
		else
		{
			m_clrAmbient.Init(iAmbientColor);
			m_clrDiffuse.Init(iDiffuseColor == 0 ? iAmbientColor : iDiffuseColor);
			m_clrEmissive.Init(iEmissiveColor);
			m_clrSpecular.Init(iSpecularColor);

			m_fA = fTransparency;
		}

		if ((szTexture != NULL) && (wcslen(szTexture) > 0))
		{
			m_strTexture = szTexture;
		}
	}

	// --------------------------------------------------------------------------------------------
	// Initialize
	void set(float ambientR, float ambientG, float ambientB, 
		float diffuseR, float diffuseG, float diffuseB, 
		float emissiveR, float emissiveG, float emissiveB, 
		float specularR, float specularG, float specularB, 
		float transparency, const wchar_t* szTexture)
	{
		m_clrAmbient.Init(ambientR, ambientG, ambientB);
		m_clrDiffuse.Init(diffuseR, diffuseG, diffuseB);
		m_clrEmissive.Init(emissiveR, emissiveG, emissiveB);
		m_clrSpecular.Init(specularR, specularG, specularB);

		m_fA = transparency;

		if ((szTexture != NULL) && (wcslen(szTexture) > 0))
		{
			m_strTexture = szTexture;
		}		
	}

	// --------------------------------------------------------------------------------------------
	// Getter
	const CRDFColor & getAmbientColor() const
	{
		return m_clrAmbient;
	}

	// --------------------------------------------------------------------------------------------
	// Getter
	const CRDFColor & getDiffuseColor() const
	{
		return m_clrDiffuse;
	}

	// --------------------------------------------------------------------------------------------
	// Getter
	const CRDFColor & getEmissiveColor() const
	{
		return m_clrEmissive;
	}

	// --------------------------------------------------------------------------------------------
	// Getter
	const CRDFColor & getSpecularColor() const
	{
		return m_clrSpecular;
	}

	// ------------------------------------------------------------------------------------------------
	// Getter
	float A() const
	{
		return m_fA;
	}

	// ------------------------------------------------------------------------------------------------
	// Getter
	bool hasTexture() const
	{
		return m_strTexture.size() > 0;
	}

	// ------------------------------------------------------------------------------------------------
	// Getter
	const wchar_t * getTexture() const
	{
		return m_strTexture.c_str();
	}

	// ------------------------------------------------------------------------------------------------
	// Getter
	void setTexture(const wstring & strTexture)
	{
		m_strTexture = strTexture;
	}
};

// ------------------------------------------------------------------------------------------------
class CConceptualFace
{

private: // Fields

	// --------------------------------------------------------------------------------------------
	// Zero-based index
	int_t m_iIndex;

	// --------------------------------------------------------------------------------------------
	// Indices
	int_t m_iTrianglesStartIndex;

	// --------------------------------------------------------------------------------------------
	// Indices
	int_t m_iTrianglesIndicesCount;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CConceptualFace()
		: m_iIndex(-1)
		, m_iTrianglesStartIndex(0)
		, m_iTrianglesIndicesCount(0)
	{
	}

	// --------------------------------------------------------------------------------------------
	// dtor
	~CConceptualFace()
	{
	}

	// --------------------------------------------------------------------------------------------
	// Accessor
	int_t& index()
	{
		return m_iIndex;
	}

	// --------------------------------------------------------------------------------------------
	// Accessor
	int_t& trianglesStartIndex()
	{
		return m_iTrianglesStartIndex;
	}

	// --------------------------------------------------------------------------------------------
	// Accessor
	int_t& trianglesIndicesCount()
	{
		return m_iTrianglesIndicesCount;
	}
};

// ------------------------------------------------------------------------------------------------
class CRDFGeometryWithMaterial
{
	// --------------------------------------------------------------------------------------------
	friend class CRDFModel;

private: // Members

	CRDFMaterial* m_pMaterial;

	// --------------------------------------------------------------------------------------------
	// Conceptual faces
	vector<CConceptualFace> m_vecConceptualFaces;

	// --------------------------------------------------------------------------------------------
	// Geometry
	vector<GLuint> m_vecIndices;

	// --------------------------------------------------------------------------------------------
	// IBO
	GLuint m_iIBO;

	// --------------------------------------------------------------------------------------------
	// IBO - Offset
	GLsizei m_iIBOOffset;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFGeometryWithMaterial()
		: m_pMaterial(NULL)
		, m_vecConceptualFaces()
		, m_vecIndices()
		, m_iIBO(0)
		, m_iIBOOffset(0)
	{
	}

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFGeometryWithMaterial(
		unsigned int iAmbientColor, 
		unsigned int iDiffuseColor, 
		unsigned int iEmissiveColor, 
		unsigned int iSpecularColor, 
		float fTransparency,
		const wchar_t* szTexture)
		: CRDFGeometryWithMaterial()
	{
		m_pMaterial = new CRDFMaterial();
		m_pMaterial->init(iAmbientColor, iDiffuseColor, iEmissiveColor, iSpecularColor, fTransparency, szTexture);
	}

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFGeometryWithMaterial(const CRDFGeometryWithMaterial& material)
		: CRDFGeometryWithMaterial()
	{
		m_pMaterial = new CRDFMaterial();
		m_pMaterial->set(
			material.getMaterial()->getAmbientColor().R(),
			material.getMaterial()->getAmbientColor().G(),
			material.getMaterial()->getAmbientColor().B(),
			material.getMaterial()->getDiffuseColor().R(),
			material.getMaterial()->getDiffuseColor().G(),
			material.getMaterial()->getDiffuseColor().B(),
			material.getMaterial()->getEmissiveColor().R(),
			material.getMaterial()->getEmissiveColor().G(),
			material.getMaterial()->getEmissiveColor().B(),
			material.getMaterial()->getSpecularColor().R(),
			material.getMaterial()->getSpecularColor().G(),
			material.getMaterial()->getSpecularColor().B(),
			material.getMaterial()->A(),
			material.getMaterial()->getTexture());
	}

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CRDFGeometryWithMaterial()
	{
		delete m_pMaterial;
	}

	// --------------------------------------------------------------------------------------------
	// Getter
	const CRDFMaterial* getMaterial() const
	{
		return m_pMaterial;
	}

	// --------------------------------------------------------------------------------------------
	// Getter
	void  setMaterial(CRDFMaterial* pMaterial)
	{
		m_pMaterial->set(
			pMaterial->getAmbientColor().R(),
			pMaterial->getAmbientColor().G(),
			pMaterial->getAmbientColor().B(),
			pMaterial->getDiffuseColor().R(),
			pMaterial->getDiffuseColor().G(),
			pMaterial->getDiffuseColor().B(),
			pMaterial->getEmissiveColor().R(),
			pMaterial->getEmissiveColor().G(),
			pMaterial->getEmissiveColor().B(),
			pMaterial->getSpecularColor().R(),
			pMaterial->getSpecularColor().G(),
			pMaterial->getSpecularColor().B(),
			pMaterial->A(),
			pMaterial->getTexture()
		);
	}

	// --------------------------------------------------------------------------------------------
	// Accessor
	vector<CConceptualFace>& conceptualFaces()
	{
		return m_vecConceptualFaces;
	}

	// --------------------------------------------------------------------------------------------
	// Setter
	void addIndex(GLuint index)
	{
		m_vecIndices.push_back(index);
	}

	// --------------------------------------------------------------------------------------------
	// Getter
	GLuint* getIndices()
	{
		return &(m_vecIndices[0]);
	}

	// --------------------------------------------------------------------------------------------
	// Getter
	int_t getIndicesCount()
	{
		return m_vecIndices.size();
	}

	// --------------------------------------------------------------------------------------------
	// Accessor
	GLuint& IBO()
	{
		return m_iIBO;
	}

	// --------------------------------------------------------------------------------------------
	// Accessor
	GLsizei& IBOOffset()
	{
		return m_iIBOOffset;
	}
};

// ------------------------------------------------------------------------------------------------
class CRDFGeometryWithMaterialComparator
{

public:

	// --------------------------------------------------------------------------------------------
	bool operator()(const CRDFGeometryWithMaterial& left, const CRDFGeometryWithMaterial& right) const
	{
		if (left.getMaterial()->getAmbientColor() < right.getMaterial()->getAmbientColor())
		{
			return true;
		}

		if (left.getMaterial()->getAmbientColor() > right.getMaterial()->getAmbientColor())
		{
			return false;
		}

		if (left.getMaterial()->getDiffuseColor() < right.getMaterial()->getDiffuseColor())
		{
			return true;
		}

		if (left.getMaterial()->getDiffuseColor() > right.getMaterial()->getDiffuseColor())
		{
			return false;
		}

		if (left.getMaterial()->getEmissiveColor() < right.getMaterial()->getEmissiveColor())
		{
			return true;
		}

		if (left.getMaterial()->getEmissiveColor() > right.getMaterial()->getEmissiveColor())
		{
			return false;
		}

		if (left.getMaterial()->getSpecularColor() < right.getMaterial()->getSpecularColor())
		{
			return true;
		}

		if (left.getMaterial()->getSpecularColor() > right.getMaterial()->getSpecularColor())
		{
			return false;
		}

		if (left.getMaterial()->A() < right.getMaterial()->A())
		{
			return true;
		}

		if (left.getMaterial()->A() > right.getMaterial()->A())
		{
			return false;
		}

		wstring strLeftTexture = left.getMaterial()->getTexture() != NULL ? left.getMaterial()->getTexture() : L"";
		wstring strRighttTexture = right.getMaterial()->getTexture() != NULL ? right.getMaterial()->getTexture() : L"";

		if (strLeftTexture < strRighttTexture)
		{
			return true;
		}

		if (strLeftTexture > strRighttTexture)
		{
			return false;
		}

		return false;
	}
};

// ------------------------------------------------------------------------------------------------
typedef map<CRDFGeometryWithMaterial, vector<CConceptualFace>, CRDFGeometryWithMaterialComparator> MATERIALS;

class CRDFInstance
{

private: // Members

	int64_t m_iID; // ID (1-based index)	
	int64_t m_iInstance; // RDF Instance

	wstring m_strName;
	wstring m_strUniqueName;

	// Geometry
	VertexBuffer* m_pOriginalVertexBuffer;	
	float * m_pVertices; // Scaled & Centered Vertices - [-1, 1]
	IndexBuffer* m_pIndexBuffer; // Indices	
	int64_t m_iConceptualFacesCount; // Conceptual faces
	
	// Primitives
	vector<_primitives> m_vecTriangles;
	vector<_primitives> m_vecFacePolygons;
	vector<_primitives> m_vecConcFacePolygons;
	vector<_primitives> m_vecLines;	
	vector<_primitives> m_vecPoints;
	
	// Materials
	vector<CRDFGeometryWithMaterial*> m_vecMaterials;
	
	// Cohorts
	vector<_cohort*> m_vecConcFacesCohorts;
	vector<_cohort*> m_vecFacesCohorts;
	vector<_cohort*> m_vecLinesCohorts;
	vector<_cohort*> m_vecPointsCohorts;	

	vector<_cohort*> m_vecNormalVecsCohorts;
	vector<_cohort*> m_vecBiNormalVecsCohorts;
	vector<_cohort*> m_vecTangentVecsCohorts;

	// Bounding box
	VECTOR3 * m_vecBoundingBoxMin;
	VECTOR3 * m_vecBoundingBoxMax;
	MATRIX * m_mtxBoundingBoxTransformation;
	
	bool m_bEnable; // UI

	bool m_bNeedsRefresh; // The data (geometry) is out of date

	// VBO (OpenGL)
	GLuint m_iVBO;
	GLsizei m_iVBOOffset;

public: // Methods

	CRDFInstance(int64_t iID, int64_t iInstance);
	virtual ~CRDFInstance();

	void Recalculate();

	int64_t getID() const;

	int64_t GetModel() const;

	int64_t getInstance() const;
	int64_t getClassInstance() const;

	const wchar_t * getName() const;
	const wchar_t * getUniqueName() const;

	bool isReferenced() const;
	bool hasGeometry() const;
	
	int32_t * getIndices() const;
	int64_t getIndicesCount() const;	

	float * getVertices() const;
	float* getOriginalVertices() const;	
	int64_t getVerticesCount() const;

	int64_t getConceptualFacesCount() const;

	const vector<_primitives>& getTriangles() const;
	const vector<_primitives>& getLines() const;	
	const vector<_primitives>& getPoints() const;
	const vector<_primitives>& getFacePolygons() const;
	const vector<_primitives>& getConcFacePolygons() const;	

	vector<_cohort*>& linesCohorts();	
	vector<_cohort*>& pointsCohorts();
	vector<_cohort*>& concFacesCohorts();
	vector<_cohort*>& facesCohorts();

	vector<CRDFGeometryWithMaterial*>& conceptualFacesMaterials();
	
	vector<_cohort*>& normalVecsCohorts();	
	vector<_cohort*>& biNormalVecsCohorts();
	vector<_cohort*>& tangentVecsCohorts();
	
	VECTOR3* getBoundingBoxMin() const;
	VECTOR3* getBoundingBoxMax() const;
	MATRIX* getBoundingBoxTransformation() const;
	
	void setEnable(bool bEnable);
	bool getEnable() const;
	
	void CalculateMinMax(float & fXmin, float & fXmax, float & fYmin, float & fYmax, float & fZmin, float & fZmax);	
	void ScaleAndCenter(float fXmin, float fXmax, float fYmin, float fYmax, float fZmin, float fZmax, float fResoltuion);
	
	float* BuildFacesVertices();
	
	void UpdateVertices(float * pVertices, int64_t iVerticesCount, int32_t * pIndices);
	
	GLuint& VBO();
	GLsizei& VBOOffset();	

private: // Methods

	void Calculate();

	void Clean();
};

struct SORT_RDFINSTANCES
{
	bool operator()(const CRDFInstance * a, const CRDFInstance * b) const
	{
		return wcscmp(a->getName(), b->getName()) < 0;
	}
};

