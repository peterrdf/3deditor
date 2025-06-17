#pragma once

#ifdef _WINDOWS
#include "engine.h"
#else
#include "../../include/engine.h"
#endif

#include "_log.h"
#include "_io.h"
#include "_errors.h"
#include "_reader.h"
#include "_material.h"

#include <set>
#include <map>
#include <vector>
using namespace std;

// ************************************************************************************************
namespace _obj2bin
{
	// ********************************************************************************************
	class _brep; // Forward declaration

	// ********************************************************************************************
	class _exporter : public _log_client
	{

	protected: // Fields

		// Model
		OwlModel m_owlModel;
		bool m_bExternalModel;

		string m_strInputFile;
		string m_strOutputFile;

		// Texture
		bool m_bFlipTextureV;

		// Materials
		set<string> m_setMaterialLibraries;
		vector<string> m_vecMaterials;
		map<string, pair<_material*, OwlInstance>> m_mapMaterials;
		OwlInstance m_iDefaultMaterialInstance;

		// Input
		vector<double> m_vecVertices;
		vector<double> m_vecNormals;
		vector<double> m_vecTextureUVs;
		vector<string> m_vecFaces;

		// BRep (Temp)
		vector<int64_t> m_vecBRepIndices;
		vector<double> m_vecBRepVertices;
		vector<double> m_vecBRepNormals;
		vector<double> m_vecBRepTextureUVs;

		vector<_brep*> m_vecBReps;

	public: // Methods

		_exporter(const char* szInputFile, bool bFlipTextureV = false);
		_exporter(const char* szInputFile, const char* szOutputFile, bool bFlipTextureV = false);
		_exporter(const char* szInputFile, OwlModel owlModel, bool bFlipTextureV = false);

	public: // Methods

		virtual ~_exporter();

		virtual void execute();

	protected: // Methods

		void load();

	private: // Methods

		void processOBJLine(const string& strLine);

		void loadMaterials();		
		OwlInstance createColorComponentInstance(double dR, double dG, double dB);
		void createDefaultMaterial();
		OwlInstance getDefaultMaterialInstance();
		OwlInstance getMaterialInstance(size_t iIndex);
	};

	// ********************************************************************************************
	class _brep
	{

	private: // Fields

		vector<string> m_vecFaces;

	public: // Methods
		_brep()
			: m_vecFaces()
		{}

		vector<string>& faces() { return m_vecFaces; }
	};
};

