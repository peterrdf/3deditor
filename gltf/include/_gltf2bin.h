#pragma once

#ifdef _WINDOWS
#include "engine.h"
#else
#include "../../include/engine.h"
#endif

#include "_json.h"
#include "_log.h"
#include "_io.h"
#include "_errors.h"
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

		_exporter(const char* szInputFile, const char* szOutputFile);
		virtual ~_exporter();

		void execute(bool bSaveFile);

		OwlModel getOwlModel(bool bDetach) {
			if (bDetach) {
				OwlModel owlModel = m_iModel;
				m_iModel = 0; // Detach the model

				return owlModel;
			}

			return m_iModel;
		}
	};
};
