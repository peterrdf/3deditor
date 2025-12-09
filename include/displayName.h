#pragma once
#include "repo.h"
#include "_string.h"

static std::wstring DisplayName(RdfsResource res, OwlModel activeModel = NULL)
{
    auto model = GetModel(res);

    std::string ownName;

    if (activeModel && model != activeModel) {
        ownName += "external ";
    }

    if (OwlInstance inst = IsInstance(res)) {
        auto instName = GetNameOfInstance(inst);
        if (instName) {
            ownName += instName;
            ownName += ":";
        }
        res = GetInstanceClass(inst);
    }

    std::string uriName;

    if (OwlClass cls = IsClass(res)) {
        uriName = GetNameOfClass(cls);
    }
    else if (RdfProperty prop = IsProperty(res)) {
        uriName = GetNameOfProperty(prop);
    }
    else {
        ownName += "<ERROR>";
        assert(false);
    }


    std::string pfxName;
    if (!uriName.empty()) {
        auto geomRepo = repo_GetFile(activeModel ? activeModel : model, REPO_GEOM_URI);
        repo_SetPrefix(geomRepo, "", REPO_GEOM_URI "#"); //this can be done once

        pfxName = repo_GetPrefixedName(geomRepo, uriName.c_str(), '.');
    }

    ownName += pfxName;

    CA2W a2w(ownName.c_str());
    return (const wchar_t*)a2w;
}

