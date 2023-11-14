#pragma once

#include "_oglUtils.h"
#include "_3DUtils.h"

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

	// BB
	_matrix* m_pmtxOriginalBBTransformation;
	_vector3d* m_pvecOriginalBBMin;
	_vector3d* m_pvecOriginalBBMax;	
	_vector3d* m_pvecAABBMin;
	_vector3d* m_pvecAABBMax;
	_vector3d* m_pvecBBMin;
	_vector3d* m_pvecBBMax;	
	
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

	bool m_bEnable; // UI

	bool m_bNeedsRefresh; // The data (geometry) is out of date

	// VBO (OpenGL)
	GLuint m_iVBO;
	GLsizei m_iVBOOffset;

public: // Methods

	CRDFInstance(int64_t iID, int64_t iInstance, bool bEnable);
	virtual ~CRDFInstance();

	void LoadName();
	static void BuildInstanceNames(OwlModel iModel, OwlInstance iInstance, wstring& strName, wstring& strUniqueName);

	void Recalculate();

	int64_t GetID() const;

	int64_t GetModel() const;

	int64_t GetInstance() const;
	int64_t GetClassInstance() const;

	const wchar_t* GetName() const;
	const wchar_t* GetUniqueName() const;
	
	bool IsReferenced() const;
	bool HasGeometry() const;
	
	int32_t* GetIndices() const;
	int64_t GetIndicesCount() const;	

	float* GetVertices() const;
	float* GetOriginalVertices() const;	
	int64_t GetVerticesCount() const;
	int64_t GetVertexLength() const;

	int64_t GetConceptualFacesCount() const;

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
	
	_vector3d* getBBMin() const;
	_vector3d* getBBMax() const;

	_vector3d* getOriginalBBMin() const;
	_vector3d* getOriginalBBMax() const;

	_vector3d* getAABBMin() const;
	_vector3d* getAABBMax() const;
	
	void setEnable(bool bEnable);
	bool getEnable() const;
	
	void ResetVertexBuffers();

	void CalculateMinMax(
		float& fXmin, float& fXmax, 
		float& fYmin, float& fYmax, 
		float& fZmin, float& fZmax);

	static void CalculateBBMinMax(
		OwlInstance iInstance,
		double& dXmin, double& dXmax,
		double& dYmin, double& dYmax,
		double& dZmin, double& dZmax);

	void ScaleAndCenter(
		float fXmin, float fXmax, 
		float fYmin, float fYmax, 
		float fZmin, float fZmax,
		float fScaleFactor,		
		bool bScale);


	void Scale(float fScaleFactor);
	void Translate(float fX, float fY, float fZ);
	
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
		return wcscmp(a->GetName(), b->GetName()) < 0;
	}
};

