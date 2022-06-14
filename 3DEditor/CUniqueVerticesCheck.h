#pragma once

#include <stdint.h>
#include <vector>
#include <fstream>

using namespace std;

#include "RDFInstance.h"

// ------------------------------------------------------------------------------------------------
class CUniqueVerticesCheck
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// Bins count
	uint32_t m_iBinsCount;

	// --------------------------------------------------------------------------------------------
	// X
	vector<vector<int64_t> *> m_vecXBinsIndices;

	// --------------------------------------------------------------------------------------------
	// Y
	vector<vector<int64_t> *> m_vecYBinsIndices;

	// --------------------------------------------------------------------------------------------
	// Z
	vector<vector<int64_t> *> m_vecZBinsIndices;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CUniqueVerticesCheck(uint32_t iBinsCount = 10000);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CUniqueVerticesCheck();

	// --------------------------------------------------------------------------------------------
	// Initialization
	int32_t Check(CRDFInstance * pInstance, wofstream & output);

private: // Methods

};

