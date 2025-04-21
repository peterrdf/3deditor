#pragma once

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

#ifdef _USE_LIBZIP
#include "zip.h"
#include "zlib.h"
#pragma comment(lib, "libz-static.lib")
#pragma comment(lib, "libzip-static.lib")

// ************************************************************************************************
const int_t	BLOCK_LENGTH_READ = 20000; // MAX: 65535

// ************************************************************************************************
static zip_file* g_pZipFile = nullptr;

// ************************************************************************************************
static int_t __stdcall	ReadCallBackFunction(unsigned char* szContent)
{
    if (g_pZipFile == nullptr) {
        return -1;
    }

    return (int_t)zip_fread(g_pZipFile, szContent, BLOCK_LENGTH_READ);
}
#endif // _USE_LIBZIP

// ************************************************************************************************
class _ap_model_factory
{

public: // Methods

    static _ap_model* load(const wchar_t* szModel, bool bMultipleModels, _model* pWorld, bool bLoadInstancesOnDemand)
    {
#ifdef _USE_LIBZIP
        fs::path pathModel = szModel;
        string strExtension = pathModel.extension().string();
        std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), ::tolower);

        /*
        * IFCZIP
        */
        if (strExtension == ".ifczip") {
            auto sdaiModel = openIFCZipModel(pathModel);
            if (sdaiModel == 0) {
                MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Failed to open the model.", L"Error", MB_ICONERROR | MB_OK);

                return nullptr;
            }

            auto pModel = new _ifc_model(bMultipleModels, bLoadInstancesOnDemand);
            pModel->attachModel(szModel, sdaiModel, pWorld);

            return pModel;
        } // IFCZIP

        /*
        * STEPGZip
        */
        if (strExtension == ".stpz") {
            auto sdaiModel = openSTEPGZipModel(pathModel);
            if (sdaiModel == 0) {
                MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Failed to open the model.", L"Error", MB_ICONERROR | MB_OK);

                return nullptr;
            }

            auto pModel = new _ap242_model(bLoadInstancesOnDemand);
            pModel->attachModel(szModel, sdaiModel, nullptr);

            return pModel;
        } // STEPZIP
#endif // _USE_LIBZIP

        auto sdaiModel = sdaiOpenModelBNUnicode(0, szModel, L"");
        if (sdaiModel == 0) {
            MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Failed to open the model.", L"Error", MB_ICONERROR | MB_OK);

            return nullptr;
        }

        wchar_t* szFileSchema = 0;
        GetSPFFHeaderItem(sdaiModel, 9, 0, sdaiUNICODE, (char**)&szFileSchema);
        if (szFileSchema == nullptr) {
            MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Unknown file schema.", L"Error", MB_ICONERROR | MB_OK);

            return nullptr;
        }

        wstring strFileSchema = szFileSchema;
        std::transform(strFileSchema.begin(), strFileSchema.end(), strFileSchema.begin(), ::towupper);

        /*
        * STEP
        */
        if ((strFileSchema.find(L"CONFIG_CONTROL_DESIGN") == 0) ||
            (strFileSchema.find(L"CONFIG_CONTROL_3D_DESIGN") == 0) ||
            (strFileSchema.find(L"CONFIG_CONTROL_DESIGN_LINE") == 0) ||
            (strFileSchema.find(L"CONFIGURATION_CONTROL_DESIGN") == 0) ||
            (strFileSchema.find(L"CONFIGURATION_CONTROL_3D_DESIGN") == 0) ||
            (strFileSchema.find(L"AUTOMOTIVE_DESIGN") == 0) ||
            (strFileSchema.find(L"AP203") == 0) ||
            (strFileSchema.find(L"AP209") == 0) ||
            (strFileSchema.find(L"AP214") == 0) ||
            (strFileSchema.find(L"AP242") == 0)) {
            auto pModel = new _ap242_model(bLoadInstancesOnDemand);
            pModel->attachModel(szModel, sdaiModel, nullptr);

            return pModel;
        }

        /*
        * IFC
        */
        if (strFileSchema.find(L"IFC") == 0) {
            auto pModel = new _ifc_model(bMultipleModels, bLoadInstancesOnDemand);
            pModel->attachModel(szModel, sdaiModel, pWorld);

            return pModel;
        }

#ifdef _CIS2_EXPERIMENTAL
        /*
        * CIS2
        */
        if (strFileSchema.find(L"STRUCTURAL_FRAME_SCHEMA") == 0) {
            auto pModel = new CCIS2Model();
            pModel->attachModel(szModel, sdaiModel, nullptr);

            return pModel;
        }
#endif // _CIS2_EXPERIMENTAL

        return nullptr;
    }    

#ifdef _USE_LIBZIP
    static vector<_model*> loadIFCZIP(const wchar_t* szIFCZIP)
    {
        vector<_model*> vecModels;

        auto vecSdaiModels = openIFCZip(szIFCZIP);
        for (auto prSdaiModel : vecSdaiModels) {
            auto pModel = new _ifc_model(vecSdaiModels.size() > 1, false);
            pModel->attachModel(prSdaiModel.first.wstring().c_str(), prSdaiModel.second, !vecModels.empty() ? vecModels[0] : nullptr);

            vecModels.push_back(pModel);
        }

        return vecModels;
    }

    // ********************************************************************************************
    // Zip support
    // ********************************************************************************************
    static SdaiModel openIFCZipModel(const fs::path& pathIfcZip)
    {
        string strIFCFileName = pathIfcZip.stem().string();
        strIFCFileName += ".ifc";

        int iError = 0;
        zip* pZip = zip_open(pathIfcZip.string().c_str(), 0, &iError);
        if (iError != 0) {
            return 0;
        }

        struct zip_stat zipStat;
        zip_stat_init(&zipStat);
        zip_stat(pZip, strIFCFileName.c_str(), 0, &zipStat);

        g_pZipFile = zip_fopen(pZip, strIFCFileName.c_str(), 0);
        if (g_pZipFile == nullptr) {
            return 0;
        }

        auto sdaiModel = engiOpenModelByStream(0, ReadCallBackFunction, "");

        zip_fclose(g_pZipFile);
        g_pZipFile = nullptr;
        zip_close(pZip);

        return sdaiModel;
    }

    // ********************************************************************************************
    // Zip support
    // ********************************************************************************************
    static vector<pair<fs::path, SdaiModel>> openIFCZip(const wchar_t* szIFCZIP)
    {
        vector<pair<fs::path, SdaiModel>> vecSdaiModels;

        fs::path pathIfcZip = szIFCZIP;

        int iError = 0;
        zip* pZip = zip_open(pathIfcZip.string().c_str(), 0, &iError);
        if (iError != 0) {
            return vecSdaiModels;
        }

        {
            auto iEntries = zip_get_num_entries(pZip, 0);
            for (auto i = 0; i < iEntries; ++i) {
                const char* szName = zip_get_name(pZip, i, 0);
                if (szName == nullptr) {
                    continue;
                }

                fs::path pathEntry = szName;
                string strExtension = pathEntry.extension().string();
                std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), ::tolower);
                if (strExtension != ".ifc") {
                    continue;
                }

                struct zip_stat zipStat;
                zip_stat_init(&zipStat);
                zip_stat(pZip, pathEntry.string().c_str(), 0, &zipStat);

                g_pZipFile = zip_fopen(pZip, pathEntry.string().c_str(), 0);
                if (g_pZipFile == nullptr) {
                    continue;
                }

                vecSdaiModels.push_back({ pathEntry, engiOpenModelByStream(0, ReadCallBackFunction, "") });
            }
        }

        zip_fclose(g_pZipFile);
        g_pZipFile = nullptr;
        zip_close(pZip);

        return vecSdaiModels;
    }

    // ********************************************************************************************
    // GZip support
    // https://windrealm.org/tutorials/decompress-gzip-stream.php
    // ********************************************************************************************	
    static bool gzipInflate(const std::string& compressedBytes, std::string& uncompressedBytes)
    {
        if (compressedBytes.size() == 0) {
            uncompressedBytes = compressedBytes;
            return true;
        }

        uncompressedBytes.clear();

        unsigned full_length = (unsigned)compressedBytes.size();
        unsigned half_length = (unsigned)compressedBytes.size() / 2;

        unsigned uncompLength = full_length;
        char* uncomp = (char*)calloc(sizeof(char), uncompLength);

        z_stream strm;
        strm.next_in = (Bytef*)compressedBytes.c_str();
        strm.avail_in = (unsigned)compressedBytes.size();
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
            uncompressedBytes += uncomp[i];
        }
        free(uncomp);
        return true;
    }

    /* Reads a file into memory. */
    static bool loadBinaryFile(const std::string& filename, std::string& contents)
    {
        // Open the gzip file in binary mode  
        FILE* f = fopen(filename.c_str(), "rb");
        if (f == NULL)
            return false;

        // Clear existing bytes in output vector  
        contents.clear();

        // Read all the bytes in the file  
        int c = fgetc(f);
        while (c != EOF) {
            contents += (char)c;
            c = fgetc(f);
        }
        fclose(f);

        return true;
    }

    static SdaiModel openSTEPGZipModel(const fs::path& pathStepGZip)
    {
        // Read the gzip file data into memory  
        std::string fileData;
        if (!loadBinaryFile(pathStepGZip.string(), fileData)) {
            printf("Error loading input file.");
            return 0;
        }

        std::string data;
        if (!gzipInflate(fileData, data)) {
            printf("Error decompressing file.");
            return 0;
        }

        return engiOpenModelByArray(0, (unsigned char*)data.c_str(), (int_t)data.size(), "");
    }
#endif _USE_LIBZIP
};

