#pragma once

#include "_oglUtils.h"

// ************************************************************************************************
#define EMPTY_INSTANCE L"---<EMPTY>---"

// ************************************************************************************************
class CRDFInstance : public _instance
{

private: // Members
	
	wstring m_strName;
	wstring m_strUniqueName;

	// Geometry
	_vertices_f* m_pOriginalVertexBuffer; // Vertices
	float* m_pVertices; // Scaled & Centered Vertices - [-1, 1]
	_indices_i32* m_pIndexBuffer; // Indices	
	int64_t m_iConceptualFacesCount; // Conceptual faces

	// The data (geometry) is out of date
	bool m_bNeedsRefresh;	

	// VBO (OpenGL)
	GLuint m_iVBO;
	GLsizei m_iVBOOffset;

public: // Methods

	CRDFInstance(int64_t iID, OwlInstance iInstance, bool bEnable);
	virtual ~CRDFInstance();

	// Rename
	void UpdateName();

	// Geometry
	void Recalculate();
	void ResetVertexBuffers();

	// Metadata	
	const wchar_t* GetName() const { return m_strName.c_str(); }
	const wchar_t* GetUniqueName() const { return m_strUniqueName.c_str(); }	
	bool IsReferenced() const { return GetInstanceInverseReferencesByIterator(m_iInstance, 0); }
	
	// Geometry
	int32_t* GetIndices() const { return m_pIndexBuffer->data(); }
	int64_t GetIndicesCount() const { return m_pIndexBuffer->size(); }
	float* GetVertices() const { return m_pVertices; }
	float* GetOriginalVertices() const { return m_pOriginalVertexBuffer != nullptr ? m_pOriginalVertexBuffer->data() : nullptr; }
	int64_t GetVerticesCount() const { return m_pOriginalVertexBuffer->size(); }
	uint64_t GetVertexLength() const { return SetFormat(GetModel()) / sizeof(float); }
	int64_t GetConceptualFacesCount() const { return m_iConceptualFacesCount; }
	bool HasGeometry() const { return (m_pOriginalVertexBuffer->size() > 0) && (m_pIndexBuffer->size() > 0); }

	// UI
	void setEnable(bool bEnable);
	bool getEnable() const { return m_bEnable; }

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
	
	GLuint& VBO() { return m_iVBO; }
	GLsizei& VBOOffset() { return m_iVBOOffset; }

private: // Methods

	void Calculate();
	void Clean();
};

// ************************************************************************************************
struct SORT_RDFINSTANCES
{
	bool operator()(const CRDFInstance* a, const CRDFInstance* b) const
	{
		return wcscmp(a->GetName(), b->GetName()) < 0;
	}
};

