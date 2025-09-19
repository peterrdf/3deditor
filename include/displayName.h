#pragma once
#include "repo.h"

static std::wstring DisplayName(RdfsResource res)
{
    auto model = GetModel(res);

    const char* internalName = "";
    if (OwlClass cls = IsClass(res)) {
        internalName = GetNameOfClass(cls);
    }
    else if (RdfProperty prop = IsProperty(res)) {
        internalName = GetNameOfProperty(prop);
    }
    else {
        assert(false);
    }

    auto geomRepo = repo_GetFile(model, REPO_GEOM_URI);
    repo_SetPrefix(geomRepo, "", REPO_GEOM_URI "#"); //this can be done once

    auto displayName = repo_GetPrefixedName(geomRepo, internalName, '.');

    CA2W a2w(displayName);
    return (const wchar_t*)a2w;
}

