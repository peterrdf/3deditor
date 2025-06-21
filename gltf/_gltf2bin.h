#pragma once

#ifdef _WINDOWS
#include "engine.h"
#include "_json.h"
#include "_log.h"
#include "_io.h"
#include "_errors.h"
#else
#include "../include/engine.h"
#include "../parsers/_json.h"
#include "../parsers/_log.h"
#include "../parsers/_io.h"
#include "../parsers/_errors.h"
#endif


#include "_gltf_importer.h"
#include "_glb_importer.h"

#include <set>
#include <map>
using namespace std;

// ************************************************************************************************
namespace _gltf2bin
{
	// ********************************************************************************************
	class _exporter : public _log_client
	{

	private: // Members

		OwlModel m_iModel;
		string m_strInputFile;
		string m_strOutputFile;

	public: // Methods

		_exporter(OwlModel iModel, const char* szInputFile, const char* szOutputFile);
		virtual ~_exporter();

		void execute(bool bSaveFile);
	};
};
