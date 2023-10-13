#pragma once

#include "engine.h"

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
	void DECSPEC STDCALL SetGISOptions(const char* szRootFolder, bool bUseEmbeddedSchemas, const void* pLogCallback = nullptr, int iValidationLevel = 0);

	OwlInstance DECSPEC STDCALL ImportGISModel(OwlModel iModel, const char* szFile, OwlInstance* pSchemaInstance = nullptr);
	OwlInstance DECSPEC STDCALL ImportGISModelW(OwlModel iModel, const wchar_t* szFile, OwlInstance* pSchemaInstance = nullptr);
	OwlInstance DECSPEC STDCALL ImportGISModelA(OwlModel iModel, const unsigned char* szData, size_t iSize, OwlInstance* pSchemaInstance = nullptr);
	OwlInstance DECSPEC STDCALL ImportGISModelS(OwlModel iModel, const void* pReadDataCallback, OwlInstance* pSchemaInstance = nullptr);

	void DECSPEC STDCALL DownloadGISSchemas(const char* szMetadataFile);	
#ifdef _DEBUG
	void DECSPEC STDCALL BuildGISEmbeddedSchemaStorage(const char* szSourcesRootFolder);
#endif
#ifdef __cplusplus
};
#endif


