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
    if (GetCurrentXYZ(effect)) {//try on normal.
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
bool DragFaceUV::ModifyInstance(const SEGMENT3 targetLine, SEGMENT3& resultPoints)
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

double DragFaceUV::MeasureOfMistake(PropertyEffect& prop, const VECTOR3& targetPoint, const VECTOR3& xyzPoint)
{
    //signed measure equivalent to distance from xyzPoint to target point
    VECTOR3 dirToTarget = targetPoint - xyzPoint;
    double measure = Vec3Dot(prop.effect, dirToTarget);
    return measure;
}

double DragFaceUV::TryModifyByProperty(const SEGMENT3& targetLine, PropertyEffect prop, PropertySuggestion& suggestion)
{    
    double  v[2] = { prop.initialValue,  StandardStep(prop.initialValue) };

    double  d[2]; //measure of mistake from desired

    SEGMENT3 effectLine;
    effectLine.pt[0] = m_dragRay.org;
    effectLine.pt[1] = m_dragRay.org + prop.effect;

    TRACE("Effect line: (%g, %g, %g) - (%g, %g, %g)\n",
        effectLine.pt[0].x, effectLine.pt[0].y, effectLine.pt[0].z,
        effectLine.pt[1].x, effectLine.pt[1].y, effectLine.pt[1].z
    );

    SEGMENT3 targetPoints;
    LineLineClosestPoints(targetPoints, targetLine, effectLine);
    
    TRACE("Closest points: target line (%g, %g, %g) - effect line (%g, %g, %g)\n",
        targetPoints.pt[0].x, targetPoints.pt[0].y, targetPoints.pt[0].z,
        targetPoints.pt[1].x, targetPoints.pt[1].y, targetPoints.pt[1].z
    );

    d[0] = MeasureOfMistake(prop, targetPoints.pt[0], effectLine.pt[0]);
    d[1] = MeasureOfMistake(prop, targetPoints.pt[0], effectLine.pt[1]);

    TRACE("Measure of mistake for start point %g, standard step %g\n", d[0], d[1]);

    if (fabs(d[1] - d[0]) < LENGTH_TOLERANCE) {
        return FLT_MAX; //no change in distance
    }

    //linear interpolation to zero position
    double val = v[0] - d[0] * (v[1] - v[0]) / (d[1] - d[0]);

    double mistake = FLT_MAX;

    SetDatatypeProperty(m_instance, prop.prop, val);
    VECTOR3 facePoint;
    if (GetCurrentXYZ(facePoint)) {
        mistake = MeasureOfMistake(prop, targetPoints.pt[0], facePoint);
        TRACE("Interpolated point (%g, %g, %g) has mistake: %g\n", facePoint.x, facePoint.y, facePoint.z, mistake);
    }
    else {
        TRACE("Failed to get interpolated point\n");
    }
    
    SetDatatypeProperty(m_instance, prop.prop, prop.initialValue);

    if (fabs(mistake) < fabs(d[0]) && fabs(mistake) < fabs(d[1])) {
        //better position from interpolation
        suggestion.prop = prop.prop;
        suggestion.value = val;
        suggestion.points.pt[0] = facePoint;
        suggestion.points.pt[1] = targetPoints.pt[1]; 
        return fabs(mistake);
    }
    else if (fabs(d[1]) < fabs(d[0])) {
        //better position from standard step
        suggestion.prop = prop.prop;
        suggestion.value = v[1];
        suggestion.points.pt[0] = effectLine.pt[1];
        suggestion.points.pt[1] = targetPoints.pt[1];   
        return fabs(d[1]);
    }
    else {
        return FLT_MAX;
    }
}
