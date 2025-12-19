#pragma once

struct FormulaValue;

class DragFace
    {
    public:
        DragFace ();
        ~DragFace ();

        //call to start dragging operation
        bool StartDrag (OwlInstance inst, int iConceptualFace, VECTOR3 const& startPoint, RDFGEOM_CALLBACK_LOG logger, void* hostData);

        //list properties detected as useful for drag 
        RdfProperty GetActivePropertyByIterator(RdfProperty prev, double& effect);
        
        //do not use this property
        void RemoveActiveProperty(RdfProperty prop);

        //update dragging state
        void Dragging (SEGMENT3 const& targetLine);

        //finish dragging and apply changes
        OwlInstance FinishDrag (bool apply);

        //true from OnStartDrag up to OnFinishDrag
        bool IsActive() const { return m_instance != NULL; }

        //get OwlInstance that represents dynamic state while dragging
        //the call is valid from OnStartDrag up to OnFinishDrag
        OwlInstance GetDynamicDraw () const { return m_drawDynamic; }

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
        bool TryModifyByProperty(const PropertyEffect& prop, const RAY3& ray, double distTargetToStart, double& suggestedValue, double& distFromTarget);
        void CollectEffectiveProperties();

        double StandardStep(double oldValue);

    private:
        RDFGEOM_CALLBACK_LOG m_logger;
        void*                m_hostData;

        OwlInstance          m_instance;
        SEGMENT3             m_startNormal;   //start drag point and point at normal direction

        PropertyEffects      m_activeProperties;

        InstantState         m_savedState;
        bool                 m_changed;

        GEOM::Collection     m_drawDynamic;
        GEOM::Transformation m_drawStartPoint;
        GEOM::Transformation m_drawTargetPoints[2]; //[0] - closest point on target line, [1] - closest point on normal
    };