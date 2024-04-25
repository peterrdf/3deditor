#pragma once

#include "_oglUtils.h"

// ************************************************************************************************
#define EMPTY_INSTANCE L"---<EMPTY>---"

// ************************************************************************************************
class CRDFInstance : public _instance
{

private: // Members	

	_vertices_f* m_pOriginalVertexBuffer; // Vertices	

	// VBO (OpenGL)
	GLuint m_iVBO;
	GLsizei m_iVBOOffset;

public: // Methods

	CRDFInstance(int64_t iID, OwlInstance iInstance, bool bEnable);
	virtual ~CRDFInstance();

	// _instance
	virtual void setEnable(bool bEnable) override;

	// Name/Unique Name
	void UpdateName();

	// Refresh
	void Recalculate();
	void ResetVertexBuffers();

	// Geometry	
	float* GetOriginalVertices() const { return m_pOriginalVertexBuffer != nullptr ? m_pOriginalVertexBuffer->data() : nullptr; }	
	
	GLuint& VBO() { return m_iVBO; }
	GLsizei& VBOOffset() { return m_iVBOOffset; }

protected: // Methods

	void Calculate();
	void Clean();
};

// ************************************************************************************************
struct SORT_RDFINSTANCES
{
	bool operator()(const CRDFInstance* a, const CRDFInstance* b) const
	{
		return wcscmp(a->getName(), b->getName()) < 0;
	}
};

