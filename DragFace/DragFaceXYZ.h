#pragma once

#include "DragFaceImpl.h"

#include <map>

struct FormulaValue;

class DragFaceXYZ : public DragFaceImpl
{
public:
    DragFaceXYZ();
    ~DragFaceXYZ();

protected:
    virtual bool OnStartDrag(VECTOR3 const& startPoint) override;
    virtual void CalculateEffect(VECTOR3& effect) override;
    virtual void OnDragging(SEGMENT3 const& targetLine) override;


private:
    void ModifyInstance(const VECTOR3& targetPoint);
    bool TryModifyByProperty(const PropertyEffect& prop, double distDesired, double& suggestedValue, double& distResult);

};