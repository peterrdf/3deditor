#pragma once

#ifndef DECL
#ifdef _WINDOWS
    #ifdef _USRDLL
        #define DECL //__declspec(dllexport)
        //	The use of the Microsoft-specific
        //	__declspec(dllexport)
        //	is needed only when
        //	not using a .def file.
    #else
        #define DECL __declspec(dllimport) 
    #endif
    #define STDC __stdcall
#else
    #define DECL /*nothing*/
    #define STDC /*nothing*/
#endif
#endif // !DECL



////////////////////////////////////////////////////////////////////////
// 

/// <summary>
/// Use self-closing ParaModel to use with ParaLib functions
/// Call paralib_CreateModel
/// Model will be closed automatically when no references
/// </summary>
struct ParaModel;
typedef std::shared_ptr<ParaModel> ParaModelPtr;

struct ParaModel
{
    _declspec(dllexport) ~ParaModel();

    OwlModel owl; //wrapped GK model
};

//
ParaModelPtr DECL STDC paralib_CreateModel();

//
ParaModelPtr DECL STDC paralib_GetModelPtr(OwlModel model);


//
std::string     DECL STDC   paralib_GetDisplayName(RdfsResource res, RdfFile file = NULL);


#ifdef __cplusplus
extern "C" {
#endif

    // 
    bool            DECL STDC   paralib_SetDefaultFile(RdfFile file);
    RdfFile         DECL STDC   paralib_GetDefaultFile(OwlModel model);

//returns code of an error happened with the last call of a ParaLib function
//Empty sting indicates no error (result status is OK)
//The pointer is valid until next call of any repo_ function
//Do not free the pointer
//
const char      DECL* STDC  paralib_GetLastError               ();

//
void DECL STDC paralib_UrlEncode(std::string& name);

//for debugging
bool            DECL STDC   paralib_CheckObjectCounters();


#ifdef __cplusplus
}
#endif





