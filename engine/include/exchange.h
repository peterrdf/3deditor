#pragma once


////////////////////////////////////////////////////////////////////////
// 


#ifdef __cplusplus
extern "C" {
#endif


//returns RdfFile or 0 if fails, see paralib_GetLastError
RdfFile      DECL STDC  exchange_Read            (ParaModelPtr model, const char* path);

//saves file
//returns 1 or 0 (see paralib_GetLastError if fails)
int64_t      DECL STDC  exchange_WriteTTL           (RdfFile file, const char* path, bool rulesAsTextFormulas);

//
bool         DECL STDC  exchange_ImportRdfBin       (RdfFile file, const char* path);


#ifdef __cplusplus
}
#endif


