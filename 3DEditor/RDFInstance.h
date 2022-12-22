#pragma once

#include "_oglUtils.h"

#include <string>
#include <vector>
#include <map>
#include "engine.h"
using namespace std;

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
	vector<_facesCohort*> m_vecPointsCohorts;

	vector<_cohort*> m_vecNormalVecsCohorts;
	vector<_cohort*> m_vecBiNormalVecsCohorts;
	vector<_cohort*> m_vecTangentVecsCohorts;

	// Bounding box
	_vector3d* m_vecOriginalBoundingBoxMin;
	_vector3d* m_vecOriginalBoundingBoxMax;
	_matrix* m_mtxOriginalBoundingBoxTransformation;
	_vector3d* m_vecBoundingBoxMin;
	_vector3d* m_vecBoundingBoxMax;
	_matrix* m_mtxBoundingBoxTransformation;
	
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
	int64_t getVertexLength() const;

	int64_t getConceptualFacesCount() const;

	const vector<_primitives>& getTriangles() const;
	const vector<_primitives>& getLines() const;
	const vector<_primitives>& getPoints() const;
	const vector<_primitives>& getFacePolygons() const;
	const vector<_primitives>& getConcFacePolygons() const;	

	vector<_cohort*>& facePolygonsCohorts();
	vector<_cohort*>& concFacePolygonsCohorts();
	vector<_cohort*>& linesCohorts();	
	vector<_facesCohort*>& pointsCohorts();

	vector<_facesCohort*>& concFacesCohorts();
	
	vector<_cohort*>& normalVecsCohorts();	
	vector<_cohort*>& biNormalVecsCohorts();
	vector<_cohort*>& tangentVecsCohorts();
	
	_vector3d* getBoundingBoxMin() const;
	_vector3d* getBoundingBoxMax() const;
	_matrix* getBoundingBoxTransformation() const;
	
	void setEnable(bool bEnable);
	bool getEnable() const;
	
	void CalculateMinMax(float & fXmin, float & fXmax, float & fYmin, float & fYmax, float & fZmin, float & fZmax);	
	void ScaleAndCenter(float fXmin, float fXmax, float fYmin, float fYmax, float fZmin, float fZmax, float fResoltuion);
	
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

