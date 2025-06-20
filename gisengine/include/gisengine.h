#pragma once

#include "../include/engine.h"

// ************************************************************************************************
#ifdef _WINDOWS
#ifdef _USRDLL
#define DECSPEC __declspec(dllexport)  
#else
#define DECSPEC __declspec(dllimport) 
#endif // _USRDLL
#define STDCALL __stdcall
#else
#define DECSPEC /*nothing*/
#define STDCALL /*nothing*/
#endif // _WINDOWS

// ************************************************************************************************
typedef size_t(STDCALL* ReadDataCallback)(unsigned char* szData, size_t iSize);

/* C interface */
#ifdef __cplusplus
extern "C" {
#endif
	/* 
		Validation:
			0 - file format (duplicated Ids)
			1 - Geometry/Materials

		Log:
			info = 0,
			warning = 1,
			error = 2,
	*/
	void DECSPEC STDCALL SetGISOptions(const char* szRootFolder, bool bUseEmbeddedSchemas, void* pLogCallback = nullptr, int iValidationLevel = 0);
	void DECSPEC STDCALL SetGISOptionsW(const wchar_t* szRootFolder, bool bUseEmbeddedSchemas, void* pLogCallback = nullptr, int iValidationLevel = 0);

	OwlInstance DECSPEC STDCALL ImportGISModel(OwlModel iModel, const char* szFile, OwlInstance* pSchemaInstance = nullptr);
	OwlInstance DECSPEC STDCALL ImportGISModelW(OwlModel iModel, const wchar_t* szFile, OwlInstance* pSchemaInstance = nullptr);
	OwlInstance DECSPEC STDCALL ImportGISModelA(OwlModel iModel, const unsigned char* szData, size_t iSize, OwlInstance* pSchemaInstance = nullptr);
	OwlInstance DECSPEC STDCALL ImportGISModelS(OwlModel iModel, ReadDataCallback pReadDataCallback, OwlInstance* pSchemaInstance = nullptr);

	bool DECSPEC STDCALL IsGML(OwlModel iModel);

	bool DECSPEC STDCALL IsCityGML(OwlModel iModel);
	void DECSPEC STDCALL SaveAsCityGML(OwlModel iModel, const char* szFile);
	void DECSPEC STDCALL SaveAsCityGMLW(OwlModel iModel, const wchar_t* szFile);

	bool DECSPEC STDCALL IsInfraGML(OwlModel iModel);
	void DECSPEC STDCALL SaveAsInfraGML(OwlModel iModel, const char* szFile);
	void DECSPEC STDCALL SaveAsInfraGMLW(OwlModel iModel, const wchar_t* szFile);

	bool DECSPEC STDCALL IsLandXML(OwlModel iModel);
	void DECSPEC STDCALL SaveAsLandXML(OwlModel iModel, const char* szFile);
	void DECSPEC STDCALL SaveAsLandXMLW(OwlModel iModel, const wchar_t* szFile);

	bool DECSPEC STDCALL IsCityJSON(OwlModel iModel);

#if _LOAD_SCHEMAS
	void DECSPEC STDCALL DownloadGISSchemas(const char* szMetadataFile);	
	void DECSPEC STDCALL BuildGISEmbeddedSchemaStorage(const char* szSourcesRootFolder);
#endif
#ifdef __cplusplus
};
#endif


