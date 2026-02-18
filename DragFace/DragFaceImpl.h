#pragma once

#include "PropertyValue.h"

class DragFaceImpl
    {
    public:
        DragFaceImpl();
        virtual ~DragFaceImpl () {}

        bool StartDrag(OwlInstance inst, int iConceptualFace, VECTOR3 const& startPoint, VECTOR3 const& eyeVector, RDFGEOM_CALLBACK_LOG logger, void* hostData, bool dynamicCursor, bool advanced);
        RdfProperty GetActivePropertyByIterator(RdfProperty prev, double& effect);
        void RemoveActiveProperty(RdfProperty prop);
        void Dragging(SEGMENT3 const& targetLine);
        OwlInstance FinishDrag(bool apply);
        OwlInstance GetDynamicDraw() { return m_drawDynamic; }

    protected:
        struct PropertyEffect //how standard step of property affects position along normal
        {
            RdfProperty    prop;
            double         initialValue_;     //initial property value
            double         factor;           //if non-zero new factor to use directly
            VECTOR3        effect;           //describes effect of changing property by 'StandardStep', meaning vary by implementation:
                                             //     UV-  vector from start point to result
                                             //     XYZ- x is distance from start point to result along in-normal direction
        };

        typedef std::vector<PropertyEffect>   PropertyEffects;

    private:
        struct PropertyState
        {
            PropertyState();
            ~PropertyState();

            bool           derived;
            PropertyValue  value;
        };

        typedef std::map<RdfProperty, PropertyState>   InstantState;

    protected:
        virtual bool   OnStartDrag(VECTOR3 const& startPoint) = 0;
        virtual void   CalculateEffect(VECTOR3& effect) = 0; //calculate current 'effect' 
        virtual void   OnDragging(SEGMENT3 const& targetLine) = 0;

    protected:
        void   CollectEffectiveProperties();
        double StandardStep(double oldValue);
        void   RestoreInstance(bool cleanSavedState);
        void   UpdateDynamicDraw();

        void Log(RDFGEOM_LOG_LEVEL level, const char* msgFormat, ...);

    private:
        void InitDynamicDraw();
        void ClearDynamicDraw();

    protected:
        OwlInstance          m_instance = NULL;
        int                  m_iConceptualFace = 0;
        std::string          m_faceDiscriminator;

        RAY3                 m_dragRay;   //start drag point and inward normal direction
        VECTOR3              m_eyeVector;
        PropertyEffects      m_activeProperties;

        bool                 m_changed;

    protected:
        //dynamic view
        VECTOR3              m_workingPoints[3]; //[0] - closest point on target (mouse) line, 
                                                 //[1] - closest point on normal / effect line,
                                                 //[2] - found point on dragging surface

    private:
        RDFGEOM_CALLBACK_LOG m_logger = NULL;
        void*                m_hostData = nullptr;
        bool                 m_dynamicCursor = false;

        InstantState         m_savedState;

        GEOM::Collection     m_drawDynamic;
        GEOM::Transformation m_drawStartPoint;
        GEOM::Transformation m_drawWorkingPoints[3]; 
};
