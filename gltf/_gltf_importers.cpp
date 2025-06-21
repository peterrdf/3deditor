#include "_host.h"

#ifdef _WINDOWS
#include "_importer_t.h"
#include "_importer_t.cpp"
#include "_json.h"
#else
#include "../parsers/_importer_t.h"
#include "../parsers/_importer_t.cpp"
#include "../parsers/_json.h"
#endif

// ************************************************************************************************
namespace _eng
{
	// ********************************************************************************************
	template class _importer_t<_json::_document, _json::_document_site>;
};