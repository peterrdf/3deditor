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
            VECTOR3     workingPoints[3];
        };

        typedef std::map<double, PropertySuggestion>   PropertySuggestions; //maps distance of point from targetLine to suggestion

    private:
        bool GetCurrentXYZ(VECTOR3& xyz);
        bool ModifyInstance(const SEGMENT3& targetLine);
        double TryModifyByProperty (const SEGMENT3& targetLine, PropertyEffect prop, PropertySuggestion& suggestion);
        double MeasureOfMistake(PropertyEffect& prop, const VECTOR3& targetPoint, const VECTOR3& xyzPoint);
     
    private:
        VECTOR2     m_uvPoint;
    };