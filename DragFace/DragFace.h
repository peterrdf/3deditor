#pragma once

class DragFaceImpl;

class DragFace
    {
    public:
        enum class Method { XYZ, UV };

    public:
        DragFace ();
        ~DragFace ();

        //true from OnStartDrag up to OnFinishDrag
        bool IsActive() const { return m_pImpl != NULL; }

        //call to start dragging operation
        bool StartDrag (OwlInstance inst, int iConceptualFace, VECTOR3 const& startPoint, Method method, RDFGEOM_CALLBACK_LOG logger, void* hostData, bool dynamicCursor);

        //list properties detected as useful for drag 
        RdfProperty GetActivePropertyByIterator(RdfProperty prev, double& effect);
        
        //do not use this property
        void RemoveActiveProperty(RdfProperty prop);

        //update dragging state
        void Dragging (SEGMENT3 const& targetLine);

        //finish dragging and apply changes
        OwlInstance FinishDrag (bool apply);

        //get OwlInstance that represents dynamic state while dragging
        //the call is valid from OnStartDrag up to OnFinishDrag
        OwlInstance GetDynamicDraw();

    private:
        DragFaceImpl* m_pImpl;
    };