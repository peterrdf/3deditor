#pragma once

#include "_oglUtils.h"

// ************************************************************************************************
#define EMPTY_INSTANCE L"---<EMPTY>---"

// ************************************************************************************************
class CRDFInstance : public _instance
{

private: // Members	

	_vertices_f* m_pOriginalVertexBuffer; // Vertices
	bool m_bNeedsRefresh; // The data (geometry) is out of date	

public: // Methods

	CRDFInstance(int64_t iID, OwlInstance iInstance, bool bEnable);
	virtual ~CRDFInstance();

	// _instance
	virtual void setEnable(bool bEnable) override;

	// Name/Unique Name
	void UpdateName();

	// Refresh
	void LoadOriginalData();
	void Recalculate();

protected: // Methods

	void Calculate();
	void Clean();
};
