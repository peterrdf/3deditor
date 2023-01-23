#pragma once

///////////////////////////////////////////////////////////////////////////////////////////
//
// convert cityJSON to RDF bin format
//


#ifndef CITYJSONRDF_EXTERN
    #ifdef CITYJSONRDF_BUILD_DLL_INTERNAL
        #define CITYJSONRDF_EXPORT __declspec(dllexport)
    #elif CITYJSONRDF_USE_DLL
        #define CITYJSONRDF_EXPORT __declspec(dllimport)
    #else
        #define CITYJSONRDF_EXPORT /**/
    #endif
#endif

namespace CityJsonRDF
{
    /// <summary>
    /// 
    /// </summary>
    struct IProgress
    {
        virtual void Start(int range) = NULL;
        virtual void Step() = NULL;
        virtual void Finish() = NULL;
    };

    /// <summary>
    /// 
    /// </summary>
    struct ILog
    {
        enum class Level { Info = 0, Warning = 1, Error = 2 };
        virtual void Message(Level level, const char* category, const char* msg, const char* converterState) = NULL;
    };

    /// <summary>
    /// 
    /// </summary>
    extern CITYJSONRDF_EXPORT OwlModel Open(
        const char* filePathCityJson,
        IProgress*  pProgress = NULL,
        ILog*       pLog = NULL
    );

}