#pragma once

#include "_oglUtils.h"

// ************************************************************************************************
#define EMPTY_INSTANCE L"---<EMPTY>---"

// ************************************************************************************************
class CRDFInstance : public _geometry
{

private: // Members	

	_vertices_f* m_pOriginalVertexBuffer; // Vertices
	bool m_bNeedsRefresh; // The data (geometry) is out of date

public: // Methods

	CRDFInstance(int64_t iID, OwlInstance iInstance, bool bEnable);
	virtual ~CRDFInstance();

	// _instance
	virtual void setEnable(bool bEnable) override;

	bool getDesignTreeConsistency() const { return CheckInstanceConsistency(getInstance(), FLAGBIT(0)) == 0; }

	// Name/Unique Name
	void UpdateName();

	// Refresh
	void LoadOriginalData();
	void Recalculate();

protected: // Methods

	// _instance
	virtual void clean() override;

	void Calculate();	
};
