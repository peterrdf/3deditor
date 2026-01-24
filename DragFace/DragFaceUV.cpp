#include "pch.h"
#include "GeomLib.h"
#include "DragFaceUV.h"

/// <summary>
/// 
/// </summary>
bool DragFaceUV::OnStartDrag(VECTOR3 const& startPoint)
{
    if (auto descr = GetConceptualFaceDiscriminator(m_instance, m_iConceptualFace)) {
        if (GetConceptualFaceXYZ2UV(m_instance, m_iConceptualFace, Vec2Coordinates(m_uvPoint), Vec3Coordinates(startPoint))) {
            if (GetConceptualFaceUV2XYZ(m_instance, m_iConceptualFace, Vec3Coordinates(m_dragRay.org), Vec3Coordinates(m_dragRay.dir), Vec2Coordinates(m_uvPoint))) {
                
                Vec3Invert(&m_dragRay.dir);
                m_faceDiscriminator = descr;

                return true;
            }
            else {
                Log(RDFGEOM_LOG_LEVEL::ERR, "Failed GetConceptualFaceUV2XYZ(%lld, %d, %g, %g)", m_instance, m_iConceptualFace, m_uvPoint.u, m_uvPoint.v);
            }
        }
        else {
            Log(RDFGEOM_LOG_LEVEL::ERR, "Failed GetConceptualFaceXYZ2UV(%lld, %d, %g, %g, %g)", m_instance, m_iConceptualFace, startPoint.x, startPoint.y, startPoint.z);
        }
    }
    else {
        Log(RDFGEOM_LOG_LEVEL::ERR, "Failed GetConceptualFaceDiscriminator(%lld, %d)", m_instance, m_iConceptualFace);
    }

    return false;
}

/// <summary>
/// 
/// </summary>
bool DragFaceUV::GetCurrentXYZ(VECTOR3& xyz)
{
    CalculateInstance(m_instance);

    if (auto descr = GetConceptualFaceDiscriminator(m_instance, m_iConceptualFace)) {
        if (descr == m_faceDiscriminator) {
            if (GetConceptualFaceUV2XYZ(m_instance, m_iConceptualFace, Vec3Coordinates(xyz), Vec2Coordinates(m_uvPoint))) {
                return true;
            }
            else {
                Log(RDFGEOM_LOG_LEVEL::ERR, "Failed GetConceptualFaceUV2XYZ(%lld, %d, %g, %g)", m_instance, m_iConceptualFace, m_uvPoint.u, m_uvPoint.v);
            }
        }
    }
    else {
        Log(RDFGEOM_LOG_LEVEL::ERR, "Failed GetConceptualFaceDiscriminator(%lld, %d)", m_instance, m_iConceptualFace);
    }

    return false;
}

/// <summary>
/// 
/// </summary>
void DragFaceUV::CalculateEffect(VECTOR3& effect)
{
    if (GetCurrentXYZ(effect)) {
        effect = effect - m_dragRay.org;
    }
    else {
        Vec3Init(effect);
    }
}

/// <summary>
/// 
/// </summary>
void DragFaceUV::OnDragging(SEGMENT3 const& targetLine)
{
    RestoreInstance(false);

    SEGMENT3 resultPoints;
    if (ModifyInstance(targetLine, resultPoints)) {
        UpdateDynamicDraw(resultPoints);
    }

    CalculateInstance(m_instance);
}

/// <summary>
/// 
/// </summary>
bool DragFaceUV::ModifyInstance(SEGMENT3 targetLine, SEGMENT3& resultPoints)
{
    if (!m_instance)
        return false;

    PropertySuggestions suggestions;

    for (auto& propEffect : m_activeProperties) {

        PropertySuggestion suggestion;
        auto dist = TryModifyByProperty(targetLine, propEffect, suggestion);
        if (dist < FLT_MAX) {
            suggestions[dist] = suggestion;
        }
    }

    //apply best result
    if (!suggestions.empty())
    {
        auto& best = suggestions.begin()->second;

        m_changed = true;
        SetDatatypeProperty(m_instance, best.prop, best.value);

        resultPoints = best.points;

        TRACE(__FUNCTION__ " sets %s=%g\n", GetNameOfProperty(best.prop), best.value);
        return true;
    }
    else
    {
        TRACE(__FUNCTION__ " does not found good change\n");
        return false;
    }
}

double DragFaceUV::TryModifyByProperty(const SEGMENT3& targetLine, PropertyEffect prop, PropertySuggestion& suggestion)
{    
    double  v[2] = { prop.initialValue,  StandardStep(prop.initialValue) };

    double  d[2]; //distance to target line
    VECTOR3 pts[2];  //closest points on target line 
    
    d[0] = LinePointDistance(targetLine, m_dragRay.org, pts);
    d[1] = LinePointDistance(targetLine, m_dragRay.org + prop.effect, pts + 1);

    if (fabs(d[1] - d[0]) < LENGTH_TOLERANCE) {
        return FLT_MAX; //no change in distance
    }

    //linear interpolation to zero position
    double val = v[0] + d[0] * (v[1] - v[0]) / (d[1] - d[0]);

    VECTOR3 facePoint;
    VECTOR3 linePoint;
    Vec3Init(linePoint);
    double dist = FLT_MAX;

    SetDatatypeProperty(m_instance, prop.prop, val);
    if (GetCurrentXYZ(facePoint)) {
        dist = LinePointDistance(targetLine, facePoint, &linePoint);
    }
    SetDatatypeProperty(m_instance, prop.prop, prop.initialValue);

    if (dist < d[0] && dist < d[1]) {
        //better position from interpolation
        suggestion.prop = prop.prop;
        suggestion.value = val;
        suggestion.points.pt[0] = facePoint;
        suggestion.points.pt[1] = linePoint;
        return dist;
    }
    else if (d[1] < d[0]) {
        //better position from standard step
        suggestion.prop = prop.prop;
        suggestion.value = v[1];
        suggestion.points.pt[0] = m_dragRay.org + prop.effect;
        suggestion.points.pt[1] = pts[1];
        return d[1];
    }
    else {
        return FLT_MAX;
    }
}
