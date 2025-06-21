#pragma once

#include "_gltf_importer_t.h"

#include <set>
#include <vector>
#include <string>
using namespace std;

// ********************************************************************************************
class _gltf_importer
	: public _eng::_gltf_importer_t
{

private: // Members	

	string m_strVersion;

	// Cache
	map<int, OwlInstance> m_mapMaterials; // index : instance
	map<int, OwlInstance> m_mapTextures; // index : instance

public: // Methods

	_gltf_importer(OwlModel iModel, const string& strRootFolder, const string& strOutputFolder);
	virtual ~_gltf_importer();

	// _document_site
	virtual void onValueLoaded(_json::_value* pValue) override;
	virtual void onPropertyLoaded(_json::_property* pProperty) override;
};

