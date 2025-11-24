#pragma once


class DragFace
    {
    public:
        DragFace ();
        ~DragFace ();

        //call to start dragging operation
        bool StartDrag (OwlInstance inst, int iConceptualFace, VECTOR3 const& startPoint);

        RdfProperty GetEffectivePropertyByIterator(RdfProperty prev, double& effect);

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

    private:
        void Cleanup();
        void AssertIsClean();
        void LogError(const char*) { assert(false); }

        void PrepareDynamicDraw ();
        void UpdateDynamicDraw (const SEGMENT3& targetPoints);
        void ClearDynamicDraw ();

        void RestoreInstance();
        void ModifyInstance(const VECTOR3& targetPoint);
        bool TryModifyByProperty(const PropertyEffect& prop, const RAY3& ray, double distTargetToStart, double& suggestedValue, double& distFromTarget);
        void CollectEffectiveProperties();

        double StandardStep(double oldValue);

    private:
        OwlInstance          m_instance;
        SEGMENT3             m_startNormal;   //start drag point and point at normal direction

        PropertyEffects      m_effectiveProperties;

        PropertyEffect       *m_changedProperty;

        GEOM::Collection     m_drawDynamic;
        GEOM::Transformation m_drawStartPoint;
        GEOM::Transformation m_drawTargetPoints[2]; //[0] - closest point on target line, [1] - closest point on normal
    };