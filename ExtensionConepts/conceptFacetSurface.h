#pragma once

class FacetSurface
{
public:
    static bool CreateClass(OwlModel model);

public:
    static bool GetBoundingBox(OwlInstance inst, void*);
    static void CreateShell(OwlInstance inst, void*);
};

