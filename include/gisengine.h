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
extern "C" {
	void DECSPEC STDCALL SetGISOptions(const char* szRootFolder, bool bUseEmbeddedSchemas, const void* pLogCallback = nullptr);

	OwlInstance DECSPEC STDCALL ImportGISModel(OwlModel owlModel, const char* szFile, OwlInstance* pOwlSchemaInstance = nullptr);
	OwlInstance DECSPEC STDCALL ImportGISModelW(OwlModel owlModel, const wchar_t* szFile, OwlInstance* pOwlSchemaInstance = nullptr);
	OwlInstance DECSPEC STDCALL ImportGISModelA(OwlModel owlModel, const unsigned char* szData, size_t iSize, OwlInstance* pOwlSchemaInstance = nullptr);
	OwlInstance DECSPEC STDCALL ImportGISModelS(OwlModel owlModel, const void* pReadDataCallback, OwlInstance* pOwlSchemaInstance = nullptr);

	void DECSPEC STDCALL DownloadGISSchemas(const char* szMetadataFile);

#ifdef _DEBUG
	void DECSPEC STDCALL BuildGISEmbeddedSchemaStorage(const char* szSourcesRootFolder);
#endif
};


