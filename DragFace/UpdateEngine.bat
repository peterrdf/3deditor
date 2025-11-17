
set PROJ_DIR=%~1
set CONFIG=%~2
set PLATFORM=%~3

set INC_DIR=%PROJ_DIR%..\engine\include\
set LIB_DIR=%PROJ_DIR%..\engine\lib\%PLATFORM%\
set DLL_DIR=%PROJ_DIR%..\%PLATFORM%\Release\

echo Update RDF engine target folders
echo   INC_DIR=%INC_DIR%
echo   LIB_DIR=%LIB_DIR%
echo   DLL_DIR=%DLL_DIR%


@echo on
if .%RDF_ENGINE_INCLUDE%.==.. goto IncUpdated
echo Update RDF engine includes 
copy "%RDF_ENGINE_INCLUDE%engdef.h"  "%INC_DIR%engdef.h"  /y
copy "%RDF_ENGINE_INCLUDE%engine.h"  "%INC_DIR%engine.h"  /y
copy "%RDF_ENGINE_INCLUDE%rdfgeom.h" "%INC_DIR%rdfgeom.h" /y
copy "%RDF_ENGINE_INCLUDE%repo.h"    "%INC_DIR%repo.h"    /y
:IncUpdated

echo Check RDF engine binaries
if .%RDF_ENGINE_LIB%.==.. goto LibUpdated
echo Update RDF engine binaries
REM always use release GK build because of assertions
copy "%RDF_ENGINE_LIB%Release\engine.lib" "%LIB_DIR%engine.lib" /y
copy "%RDF_ENGINE_LIB%Release\engine.dll" "%DLL_DIR%engine.dll" /y

:LibUpdated
