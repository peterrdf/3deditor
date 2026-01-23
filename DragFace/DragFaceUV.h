#pragma once

#include "DragFaceImpl.h"


class DragFaceUV : public DragFaceImpl
    {
    public:
        DragFaceUV (){}
        ~DragFaceUV() {}

    public:
        virtual bool StartDrag(OwlInstance inst, int iConceptualFace, VECTOR3 const& startPoint, RDFGEOM_CALLBACK_LOG logger, void* hostData) override;
        virtual RdfProperty GetActivePropertyByIterator(RdfProperty prev, double& effect) override;
        virtual void RemoveActiveProperty(RdfProperty prop) override;
        virtual void Dragging(SEGMENT3 const& targetLine) override;
        virtual OwlInstance FinishDrag(bool apply) override;
        virtual OwlInstance GetDynamicDraw() override;

    };