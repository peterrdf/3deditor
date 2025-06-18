#include "stdafx.h"
#include "_gltf2bin.h"

// ************************************************************************************************
namespace _gltf2bin
{
	// ************************************************************************************************
	_exporter::_exporter(const char* szInputFile, const char* szOutputFile)
		: _log_client()
		, m_iModel(0)
		, m_strInputFile()
		, m_strOutputFile()
	{
		VERIFY_POINTER(szInputFile);
		VERIFY_POINTER(szOutputFile);

		m_strInputFile = szInputFile;
		m_strOutputFile = szOutputFile;
	}

	/*virtual*/ _exporter::~_exporter()
	{
		if (m_iModel != 0) {
			CloseModel(m_iModel);
		}
	}

	void _exporter::execute(bool bSaveFile)
	{
		if (m_iModel != 0) {
			CloseModel(m_iModel);
			m_iModel = 0;
		}

		m_iModel = CreateModel();
		VERIFY_INSTANCE(m_iModel);

		// Validate input file path
		fs::path pathInputFile = m_strInputFile;
		if (!fs::exists(pathInputFile)) {
			LOG_THROW_ERROR_F("Input file does not exist: '%s'", m_strInputFile.c_str());
		}

		// Ensure the input file has an extension
		string strExtension = pathInputFile.extension().string();
		_string::toLower(strExtension);
		if (strExtension.empty()) {
			LOG_THROW_ERROR_F("Input file has no extension: '%s'", m_strInputFile.c_str());
		}

		fs::path pathOutputFile = m_strOutputFile;

		if (strExtension == ".gltf") {
			_gltf_importer importer(m_iModel, pathInputFile.parent_path().string(), pathOutputFile.parent_path().string());
			importer.setLog(getLog());
			importer.load(m_strInputFile.c_str(), false);
		} else if (strExtension == ".glb") {
			_glb_importer importer(m_iModel, pathInputFile.parent_path().string(), pathOutputFile.parent_path().string());
			importer.setLog(getLog());
			importer.load(m_strInputFile.c_str());
		} else {
			LOG_THROW_ERROR_F("Unsupported file format: '%s'", strExtension.c_str());
		}

		if (bSaveFile) {
			SaveModel(m_iModel, m_strOutputFile.c_str());
		}		
	}
};