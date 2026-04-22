#pragma once

#include "_log.h"
#include "_mvc.h"
#include "_ifc_model.h"
#include "_ap242_model.h"
#ifdef _CIS2_EXPERIMENTAL
#include "CIS2Model.h"
#endif

#ifdef _WINDOWS
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

// https://github.com/madler/zlib/tree/develop
#include "zlib.h"

// https://github.com/kuba--/zip
#include "zip.h"

// ************************************************************************************************
const size_t BLOCK_LENGTH_READ = 65000; // MAX: 65535

// ************************************************************************************************
static unsigned char* m_szZipEntryBuffer = nullptr;
static size_t m_iZipEntrySize = 0;
static size_t m_iZipEntryOffset = 0;

// ************************************************************************************************
// Define calling convention conditionally
#ifdef _WINDOWS
#define CALLBACK_CONVENTION __stdcall
#else
#define CALLBACK_CONVENTION
#endif

static int_t CALLBACK_CONVENTION ReadCallBackFunction(unsigned char* szContent)
{
	if (m_szZipEntryBuffer == nullptr) {
		return -1;
	}

	if (m_iZipEntryOffset >= m_iZipEntrySize) {
		return 0;
	}

	size_t iBlockLength = (m_iZipEntrySize - m_iZipEntryOffset) < BLOCK_LENGTH_READ ? (m_iZipEntrySize - m_iZipEntryOffset) : BLOCK_LENGTH_READ;
	if (iBlockLength == 0) {
		return 0;
	}

	memcpy(szContent, m_szZipEntryBuffer + m_iZipEntryOffset, iBlockLength);
	m_iZipEntryOffset += BLOCK_LENGTH_READ;

	return iBlockLength;
}

// ************************************************************************************************
class _ap_model_factory
{

private: // Methods

	static std::wstring getSchemaName(SdaiModel sdaiModel)
    {
		const wchar_t* schema = nullptr;
		GetSPFFHeaderItem(sdaiModel, 9, 0, sdaiUNICODE, (char**)&schema);

		std::wstring strFileSchema = schema ? schema : L"";
		std::transform(strFileSchema.begin(), strFileSchema.end(), strFileSchema.begin(), ::towupper);

		return strFileSchema;
	}

public: // Methods

	static _ap_model* load(_log* pLog, const wchar_t* szModel, bool bMultipleModels, _model* pWorld, bool bLoadInstancesOnDemand)
	{
		fs::path pathModel = szModel;
		string strExtension = pathModel.extension().string();
		std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), ::tolower);

		/*
		* IFCZIP
		*/
		if (strExtension == ".ifczip") {
			return nullptr;
		}

		/*
		* STEPGZip
		*/
		if (strExtension == ".stpz") {
			return nullptr;
		}

		//
		// Read header and recognize schema
		//
		std::wstring schemaName;
		if (auto header = sdaiOpenModelBNUnicode(0, szModel, NULL)) {
			schemaName = getSchemaName(header);
			sdaiCloseModel(header);
		}

		if (schemaName.empty()) {
#ifdef _WINDOWS
			MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Failed to get schema name form file header.", L"Error", MB_ICONERROR | MB_OK);
#endif
			return nullptr;
		}

		//
		// Create appropriate sdaiModel wrapper
		//

		_ap_model* pModel = nullptr;

		/*
		* STEP
		*/
		if ((schemaName.find(L"CONFIG_CONTROL_DESIGN") == 0) ||
			(schemaName.find(L"CONFIG_CONTROL_3D_DESIGN") == 0) ||
			(schemaName.find(L"CONFIG_CONTROL_DESIGN_LINE") == 0) ||
			(schemaName.find(L"CONFIGURATION_CONTROL_DESIGN") == 0) ||
			(schemaName.find(L"CONFIGURATION_CONTROL_3D_DESIGN") == 0) ||
			(schemaName.find(L"AUTOMOTIVE_DESIGN") == 0) ||
			(schemaName.find(L"AP203") == 0) ||
			(schemaName.find(L"AP209") == 0) ||
			(schemaName.find(L"AP214") == 0) ||
			(schemaName.find(L"AP242") == 0)) {

			pModel = new _ap242_model(pLog, true, bLoadInstancesOnDemand);
		}

		/*
		* IFC
		*/
		else if (schemaName.find(L"IFC") == 0) {

			pModel = new _ifc_model(pLog, bMultipleModels, bLoadInstancesOnDemand);
		}

#ifdef _CIS2_EXPERIMENTAL
		/*
		* CIS2
		*/
		else if (schemaName.find(L"STRUCTURAL_FRAME_SCHEMA") == 0) {

			pModel = new CCIS2Model(pLog);
		}
#endif // _CIS2_EXPERIMENTAL

		/*
		** Generic model
		*/
		else {
			pModel = new _ifc_model(pLog, bMultipleModels, bLoadInstancesOnDemand);
		}

		//
		// Read model file
		//
		if (pModel) {
			auto sdaiModel = sdaiOpenModelBNUnicode(0, szModel, schemaName.c_str());
			if (!sdaiModel) { // embedded schema not found, ask to locate schema file
#ifdef _WINDOWS
				CFileDialog dlgOpen(TRUE, L"exp", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
					L"EXPRESS Schema Files (*.exp)|*.exp|All Files (*.*)|*.*||", ::AfxGetMainWnd());

				CString title;
				title.Format(L"Locate EXPRESS schema %s", schemaName.c_str());
				dlgOpen.m_ofn.lpstrTitle = title;

				if (dlgOpen.DoModal() == IDOK) {
					auto schemaFilePath = dlgOpen.GetPathName();
					sdaiModel = sdaiOpenModelBNUnicode(0, szModel, schemaFilePath);
				}
			}
#endif	
			if (sdaiModel) {
				pModel->attachModel(szModel, sdaiModel, nullptr);
			}
			else {
				delete pModel;
				pModel = nullptr;
			}
		}

		if (!pModel) {
#ifdef _WINDOWS
			MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Failed to load model.", L"Error", MB_ICONERROR | MB_OK);
#endif
		}

		return pModel;
	}

	static vector<_model*> loadIFCZIP(_log* pLog, const wchar_t* szIFCZIP)
	{
		vector<_model*> vecModels;

		auto vecSdaiModels = openIFCZip(szIFCZIP);
		for (auto prSdaiModel : vecSdaiModels) {
			auto pModel = new _ifc_model(pLog, vecSdaiModels.size() > 1, false);
			pModel->attachModel(prSdaiModel.first.wstring().c_str(), prSdaiModel.second, !vecModels.empty() ? vecModels[0] : nullptr);

			vecModels.push_back(pModel);
		}

		return vecModels;
	}

	static vector<_model*> loadSTEPGZip(_log* pLog, const wchar_t* szIFCZIP)
	{
		vector<_model*> vecModels;

		auto vecSdaiModels = openSTEPGZip(szIFCZIP);
		if (vecSdaiModels.size() != 1) {
			return vecModels;
		}

		auto pModel = new _ap242_model(pLog, true, false);
		pModel->attachModel(vecSdaiModels.front().first.wstring().c_str(), vecSdaiModels.front().second, nullptr);
		vecModels.push_back(pModel);

		return vecModels;
	}

	// ********************************************************************************************
	// Zip support
	// ********************************************************************************************
	static vector<pair<fs::path, SdaiModel>> openIFCZip(const wchar_t* szIFCZIP)
	{
		vector<pair<fs::path, SdaiModel>> vecSdaiModels;

		fs::path pathIfcZip = szIFCZIP;

		zip_t* pZip = zip_open(pathIfcZip.string().c_str(), 0, 'r');
		if (pZip == NULL) {
			return vecSdaiModels;
		}

		auto iEntries = zip_entries_total(pZip);
		for (auto i = 0; i < iEntries; ++i) {
			zip_entry_openbyindex(pZip, i);
			if (zip_entry_isdir(pZip))
				continue;

			string strName = zip_entry_name(pZip);
			if (strName.empty()) {
				continue;
			}

			fs::path pathEntry = strName;
			string strExtension = pathEntry.extension().string();
			std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), ::tolower);
			if (strExtension != ".ifc") {
				continue;
			}

			zip_entry_open(pZip, strName.c_str());

			m_szZipEntryBuffer = nullptr;
			m_iZipEntrySize = 0;
			m_iZipEntryOffset = 0;
			zip_entry_read(pZip, (void**)&m_szZipEntryBuffer, &m_iZipEntrySize);

			vecSdaiModels.push_back({ pathEntry, engiOpenModelByStream(0, (void*)ReadCallBackFunction, "") });

			zip_entry_close(pZip);
		}

		m_szZipEntryBuffer = nullptr;
		m_iZipEntrySize = 0;
		m_iZipEntryOffset = 0;
		zip_close(pZip);

		return vecSdaiModels;
	}

	// ********************************************************************************************
	// GZip support
	// https://windrealm.org/tutorials/decompress-gzip-stream.php
	// ********************************************************************************************	
	static bool gzipInflate(const vector<unsigned char> compressed, vector<unsigned char>& uncompressed)
	{
		if (compressed.size() == 0) {
			return false;
		}

		uncompressed.clear();

		unsigned full_length = (unsigned)compressed.size();
		unsigned half_length = (unsigned)compressed.size() / 2;

		unsigned uncompLength = full_length;
		char* uncomp = (char*)calloc(sizeof(char), uncompLength);

		z_stream strm;
		strm.next_in = (Bytef*)compressed.data();
		strm.avail_in = (unsigned)compressed.size();
		strm.total_out = 0;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;

		bool done = false;

		if (inflateInit2(&strm, (16 + MAX_WBITS)) != Z_OK) {
			free(uncomp);
			return false;
		}

		while (!done) {
			// If our output buffer is too small  
			if (strm.total_out >= uncompLength) {
				// Increase size of output buffer  
				char* uncomp2 = (char*)calloc(sizeof(char), uncompLength + half_length);
				memcpy(uncomp2, uncomp, uncompLength);
				uncompLength += half_length;
				free(uncomp);
				uncomp = uncomp2;
			}

			strm.next_out = (Bytef*)(uncomp + strm.total_out);
			strm.avail_out = uncompLength - strm.total_out;

			// Inflate another chunk.  
			int err = inflate(&strm, Z_SYNC_FLUSH);
			if (err == Z_STREAM_END) done = true;
			else if (err != Z_OK) {
				break;
			}
		}

		if (inflateEnd(&strm) != Z_OK) {
			free(uncomp);
			return false;
		}

		for (size_t i = 0; i < strm.total_out; ++i) {
			uncompressed.push_back(uncomp[i]);
		}

		free(uncomp);

		return true;
	}

	static vector<pair<fs::path, SdaiModel>> openSTEPGZip(const fs::path& pathStepGZip)
	{
		vector<pair<fs::path, SdaiModel>> vecSdaiModels;

		FILE* f = fopen(pathStepGZip.string().c_str(), "rb");
		if (f == NULL) {
#ifdef _WINDOWS
			MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Failed to open the model.", L"Error", MB_ICONERROR | MB_OK);
#endif // _WINDOWS			
			return vecSdaiModels;
		}

		vector<unsigned char> compressed;
		int c = fgetc(f);
		while (c != EOF) {
			compressed.push_back((unsigned char)c);
			c = fgetc(f);
		}
		fclose(f);

		vector<unsigned char> uncompressed;
		if (!gzipInflate(compressed, uncompressed)) {
#ifdef _WINDOWS
			MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Failed decompress the model.", L"Error", MB_ICONERROR | MB_OK);
#endif
			return vecSdaiModels;
		}

		vecSdaiModels.push_back({ pathStepGZip, engiOpenModelByArray(0, (unsigned char*)uncompressed.data(), (int_t)uncompressed.size(), "") });

		return vecSdaiModels;
	}
};

