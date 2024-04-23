#pragma once

#include "_oglUtils.h"
#include "_3DUtils.h"

#include <string>
#include <vector>
#include <map>
#include "engine.h"
using namespace std;

// ************************************************************************************************
#define EMPTY_INSTANCE L"---<EMPTY>---"

// ************************************************************************************************
class CRDFInstance
{

private: // Members

	int64_t m_iID; // ID (1-based index)
	OwlInstance m_iInstance;	
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
	_matrix* m_pmtxBBTransformation;
	_vector3d* m_pvecBBMin;
	_vector3d* m_pvecBBMax;
	_vector3d* m_pvecAABBMin;
	_vector3d* m_pvecAABBMax;
	
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
	vector<_facesCohort*> m_vecLinesCohorts;
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

	CRDFInstance(int64_t iID, OwlInstance iInstance, bool bEnable);
	virtual ~CRDFInstance();

	void UpdateName();

	void Recalculate();

	int64_t GetID() const { return m_iID; }
	OwlInstance GetInstance() const { return m_iInstance; }
	OwlClass GetClassInstance() const { return GetInstanceClass(m_iInstance); }
	OwlModel GetModel() const { return ::GetModel(m_iInstance); }
	const wchar_t* GetName() const { return m_strName.c_str(); }
	const wchar_t* GetUniqueName() const { return m_strUniqueName.c_str(); }	
	bool IsReferenced() const { return GetInstanceInverseReferencesByIterator(m_iInstance, 0); }
	
	int32_t* GetIndices() const { return m_pIndexBuffer->data(); }
	int64_t GetIndicesCount() const { return m_pIndexBuffer->size(); }
	float* GetVertices() const { return m_pVertices; }
	float* GetOriginalVertices() const { return m_pOriginalVertexBuffer != nullptr ? m_pOriginalVertexBuffer->data() : nullptr; }
	int64_t GetVerticesCount() const { return m_pOriginalVertexBuffer->size(); }
	uint64_t GetVertexLength() const { return SetFormat(GetModel(), 0, 0) / sizeof(float); }
	int64_t GetConceptualFacesCount() const { return m_iConceptualFacesCount; }
	bool HasGeometry() const { return (m_pOriginalVertexBuffer->size() > 0) && (m_pIndexBuffer->size() > 0); }

	const vector<_primitives>& getTriangles() const { return m_vecTriangles; }
	const vector<_primitives>& getLines() const { return m_vecLines; }
	const vector<_primitives>& getPoints() const { return m_vecPoints; }
	const vector<_primitives>& getFacePolygons() const { return m_vecFacePolygons; }
	const vector<_primitives>& getConcFacePolygons() const { return m_vecConcFacePolygons; }

	vector<_cohort*>& facePolygonsCohorts();
	vector<_cohort*>& concFacePolygonsCohorts();
	vector<_facesCohort*>& linesCohorts();
	vector<_facesCohort*>& pointsCohorts();

	vector<_facesCohort*>& concFacesCohorts();
	
	vector<_cohort*>& normalVecsCohorts();	
	vector<_cohort*>& biNormalVecsCohorts();
	vector<_cohort*>& tangentVecsCohorts();

	_vector3d* getOriginalBBMin() const;
	_vector3d* getOriginalBBMax() const;
	_matrix* getBBTransformation() const { return m_pmtxBBTransformation; }
	_vector3d* getBBMin() const;
	_vector3d* getBBMax() const;
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

