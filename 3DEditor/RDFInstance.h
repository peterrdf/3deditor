#pragma once

#include "_drawUtils.h"

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

class CRDFInstance
{

private: // Members

	int64_t m_iID; // ID (1-based index)	
	int64_t m_iInstance; // RDF Instance

	wstring m_strName;
	wstring m_strUniqueName;

	// Geometry
	_vertices_f* m_pOriginalVertexBuffer;
	float * m_pVertices; // Scaled & Centered Vertices - [-1, 1]
	_indices_i32* m_pIndexBuffer; // Indices	
	int64_t m_iConceptualFacesCount; // Conceptual faces
	
	// Primitives
	vector<_primitives> m_vecTriangles;
	vector<_primitives> m_vecFacePolygons;
	vector<_primitives> m_vecConcFacePolygons;
	vector<_primitives> m_vecLines;	
	vector<_primitives> m_vecPoints;
	
	// Materials
	vector<_facesCohort*> m_vecConcFacesCohorts;
	
	// Cohorts
	vector<_cohort*> m_vecFacePolygonsCohorts;
	vector<_cohort*> m_vecConcFacePolygonsCohorts;
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

	vector<_cohort*>& facePolygonsCohorts();
	vector<_cohort*>& concFacePolygonsCohorts();
	vector<_cohort*>& linesCohorts();	
	vector<_cohort*>& pointsCohorts();

	vector<_facesCohort*>& concFacesCohorts();
	
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

