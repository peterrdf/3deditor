#pragma once


class DragFaceImpl
    {
    public:
        DragFaceImpl () {}
        virtual ~DragFaceImpl () {}

    public:
        virtual bool StartDrag(OwlInstance inst, int iConceptualFace, VECTOR3 const& startPoint, RDFGEOM_CALLBACK_LOG logger, void* hostData) = 0;
        virtual RdfProperty GetActivePropertyByIterator(RdfProperty prev, double& effect) = 0;
        virtual void RemoveActiveProperty(RdfProperty prop) = 0;
        virtual void Dragging(SEGMENT3 const& targetLine) = 0;
        virtual OwlInstance FinishDrag(bool apply) = 0;
        virtual OwlInstance GetDynamicDraw() = 0;

    };