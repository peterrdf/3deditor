#include "stdafx.h"

#ifdef _WINDOWS
#include "../include/engine.h"
#else
#include "../../include/engine.h"
#endif

#include "_string.h"
#include "_gltf_importer.h"

#include <chrono>

#include "_net.h"

// ************************************************************************************************
_gltf_importer::_gltf_importer(OwlModel iModel, const string& strRootFolder, const string& strOutputFolder)
	: _gltf_importer_t(iModel, strRootFolder, strOutputFolder, 1)
	, m_strVersion("1.0")
	, m_mapMaterials()
	, m_mapTextures()
{
}

/*virtual*/ _gltf_importer::~_gltf_importer()
{
}

/*virtual*/ void _gltf_importer::onValueLoaded(_json::_value* /*pValue*/) /*override*/
{
}

/*virtual*/ void _gltf_importer::onPropertyLoaded(_json::_property* pProperty) /*override*/
{
	VERIFY_POINTER(pProperty);
}