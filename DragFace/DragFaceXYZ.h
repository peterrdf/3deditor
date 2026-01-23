#pragma once

#include "DragFaceImpl.h"

#include <map>

struct FormulaValue;

class DragFaceXYZ : public DragFaceImpl
    {
    public:
        DragFaceXYZ ();
        ~DragFaceXYZ ();

    public:
        virtual bool StartDrag(OwlInstance inst, int iConceptualFace, VECTOR3 const& startPoint, RDFGEOM_CALLBACK_LOG logger, void* hostData) override;
        virtual RdfProperty GetActivePropertyByIterator(RdfProperty prev, double& effect) override;
        virtual void RemoveActiveProperty(RdfProperty prop) override;
        virtual void Dragging(SEGMENT3 const& targetLine) override;
        virtual OwlInstance FinishDrag(bool apply) override;
        virtual OwlInstance GetDynamicDraw() override { return m_drawDynamic; };

    private:
        struct PropertyEffect //how standard step of property affects position along normal
        {
            RdfProperty    prop;
            double         initialValue;     //initial property value
            double         distStartToStep;  //distance from start point to result along in-normal direction when property is changed by 'StandardStep'
        };

        typedef std::vector<PropertyEffect>   PropertyEffects;

        struct PropertyState
        {
            PropertyState();
            ~PropertyState();

            bool                derived;
            FormulaValue*       value;
        };

        typedef std::map<RdfProperty, PropertyState>   InstantState;

    private:
        void Cleanup();
        void AssertIsClean();
        void Log(RDFGEOM_LOG_LEVEL level, const char* msgFormat, ...);

        void PrepareDynamicDraw ();
        void UpdateDynamicDraw (const SEGMENT3& targetPoints);
        void ClearDynamicDraw ();

        void RestoreInstance(bool cleanSavedState);
        void ModifyInstance(const VECTOR3& targetPoint);
        bool TryModifyByProperty(const PropertyEffect& prop, double distDesired, double& suggestedValue, double& distResult);
        void CollectEffectiveProperties();

        double StandardStep(double oldValue);

    private:
        RDFGEOM_CALLBACK_LOG m_logger;
        void*                m_hostData;

        OwlInstance          m_instance;

        RAY3                 m_ray;   //start drag point and inward normal direction

        PropertyEffects      m_activeProperties;

        InstantState         m_savedState;
        bool                 m_changed;

        GEOM::Collection     m_drawDynamic;
        GEOM::Transformation m_drawStartPoint;
        GEOM::Transformation m_drawTargetPoints[2]; //[0] - closest point on target line, [1] - closest point on normal
    };