#pragma once

#include "DragFaceImpl.h"


class DragFaceUV : public DragFaceImpl
    {
    public:
        DragFaceUV (){}
        ~DragFaceUV() {}

    protected:
        virtual bool OnStartDrag(VECTOR3 const& startPoint) override;
        virtual void CalculateEffect(VECTOR3& effect) override;
        virtual void OnDragging(SEGMENT3 const& targetLine) override;

    private:
        struct PropertySuggestion
        {
            RdfProperty prop;
            double      value;
            SEGMENT3    points; //point on face corresponding to UV and closest point on drag line to facePoint
        };

        typedef std::map<double, PropertySuggestion>   PropertySuggestions; //maps distance of point from targetLine to suggestion

    private:
        bool GetCurrentXYZ(VECTOR3& xyz);
        bool ModifyInstance(SEGMENT3 targetLine, SEGMENT3& resultPoints);
        double TryModifyByProperty (const SEGMENT3& targetLine, PropertyEffect prop, PropertySuggestion& suggestion);
     

    private:
        VECTOR2     m_uvPoint;
        std::string m_faceDiscriminator;
    };