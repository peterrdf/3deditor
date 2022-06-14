#pragma once

#include <map>
#include <stdint.h>
#include <vector>
#include <fstream>

using namespace std;

#include "RDFInstance.h"

// ------------------------------------------------------------------------------------------------
// Epsilon = 0
class CUniqueVerticesCheckE0
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// X; VALUE : INDICES
	map<float, vector<int32_t> *> m_mapXDistribution;

	// --------------------------------------------------------------------------------------------
	// Y; VALUE : INDICES
	map<float, vector<int32_t> *> m_mapYDistribution;

	// --------------------------------------------------------------------------------------------
	// Z; VALUE : INDICES
	map<float, vector<int32_t> *> m_mapZDistribution;

	// --------------------------------------------------------------------------------------------
	/*
	Zero-based index = Vertex index; used for removing of the duplicates/updating of the index buffer

	Example:
		Vertices:
			0 1 2 3 4 5 6 7 8 9
		Duplicates:
			0 1 2
		Result - keep the first duplicated vertex and replace the rest:
			0 0 0 3 4 5 6 7 8 9	
	*/
	vector<int32_t> m_vecVerticesMapping;

	// --------------------------------------------------------------------------------------------
	/* 
	Old vertex index => new vertex index; -1 means unused/duplicated vertex.

	Example:
		0 -1 -1 1 -1 -1 2 3
	*/
	vector<int32_t> m_vecVerticesConvertor;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CUniqueVerticesCheckE0();

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CUniqueVerticesCheckE0();

	// --------------------------------------------------------------------------------------------
	// Check
	int32_t Check(CRDFInstance * pInstance, int32_t iVertexLength, wofstream & output);

	// --------------------------------------------------------------------------------------------
	// Clean => Vertices : Vertices Count : Indices (Indices Count is the same)
	tuple<float *, int64_t, int32_t *> RemoveDuplicates(CRDFInstance * pInstance, int32_t iVertexLength);

private: // Methods

	// --------------------------------------------------------------------------------------------
	// Release the memory
	void Clear(map<float, vector<int32_t> *>& mapDistribution) const;
};

