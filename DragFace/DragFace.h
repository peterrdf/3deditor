#pragma once


class DragFace
    {
    public:
        DragFace ();
        ~DragFace ();

        //call to start dragging operation
        void StartDrag (OwlInstance inst, int iConceptualFace, VECTOR3 const& startPoint);

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
        struct PropertyResult
        {
            RdfProperty    prop;
            double         oldValue;
            double         newValue;
            double         distance; //along ray from target point; signed positive in direction to start drag point,
        };

        typedef std::map<RdfProperty, PropertyResult>   PropertyResults;

    private:
        void Cleanup();
        void AssertIsClean();

        void PrepareDynamicDraw ();
        void UpdateDynamicDraw (const SEGMENT3& targetPoints);
        void ClearDynamicDraw ();

        void RestoreInstance();
        void ModifyInstance(const VECTOR3& targetPoint);
        bool TryProperty(PropertyResult& result, const RAY3& ray);
        void CollectEffectiveProperties(PropertyResults& results, const VECTOR3& targetPoint);
        PropertyResult* FindBestProperty(PropertyResults& results);

    private:
        OwlInstance          m_instance;
        SEGMENT3             m_startNormal;   //start drag point and point at normal direction

        RdfProperty          m_changedProperty;
        double               m_oldValue;

        GEOM::Collection     m_drawDynamic;
        GEOM::Transformation m_drawStartPoint;
        GEOM::Transformation m_drawTargetPoints[2]; //[0] - closest point on target line, [1] - closest point on normal
    };