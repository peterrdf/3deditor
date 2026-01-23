#include "pch.h"
#include "DragFaceUV.h"

bool DragFaceUV::StartDrag(OwlInstance inst, int iConceptualFace, VECTOR3 const& startPoint, RDFGEOM_CALLBACK_LOG logger, void* hostData)
{
    return false;
}

RdfProperty DragFaceUV::GetActivePropertyByIterator(RdfProperty prev, double& effect)
{
    return NULL;
}
    
void DragFaceUV::RemoveActiveProperty(RdfProperty prop)
{

}

void DragFaceUV::Dragging(SEGMENT3 const& targetLine)
{

}

OwlInstance DragFaceUV::FinishDrag(bool apply)
{
    return NULL;
}

OwlInstance DragFaceUV::GetDynamicDraw()
{ 
    return NULL; 
};

