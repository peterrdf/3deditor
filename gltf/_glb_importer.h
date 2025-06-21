#pragma once

#include "_gltf_importer.h"

#include <set>
#include <vector>
#include <string>
using namespace std;

// ********************************************************************************************
class _glb_importer
	: public _gltf_importer
{

private: // Members

	string m_strInputFile;
	streampos m_nBINDataOffset;

public: // Methods

	_glb_importer(OwlModel iModel, const string& strRootFolder, const string& strOutputFolder);
	virtual ~_glb_importer();

	// _importer_t
	virtual void load(const char* szFile) override;

protected: // Methods

	// _gltf_importer_t
	virtual string getURI(const _json::_object* pBufferObject) const override;
	virtual streampos getBINDataOffset() const override;
};

