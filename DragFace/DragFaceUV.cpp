#include "pch.h"
#include "GeomLib.h"
#include "DragFaceUV.h"

/// <summary>
/// 
/// </summary>
bool DragFaceUV::OnStartDrag(VECTOR3 const& startPoint)
{
    if (GetConceptualFaceXYZ2UV(m_instance, m_iConceptualFace, Vec2Coordinates(m_uvPoint), Vec3Coordinates(startPoint))) {
        if (GetConceptualFaceUV2XYZ(m_instance, m_iConceptualFace, Vec3Coordinates(m_dragRay.org), Vec3Coordinates(m_dragRay.dir), Vec2Coordinates(m_uvPoint))) {

            Vec3Invert(&m_dragRay.dir);

            return true;
        }
        else {
            Log(RDFGEOM_LOG_LEVEL::ERR, "Failed GetConceptualFaceUV2XYZ(%lld, %d, %g, %g)", m_instance, m_iConceptualFace, m_uvPoint.u, m_uvPoint.v);
        }
    }
    else {
        Log(RDFGEOM_LOG_LEVEL::ERR, "Failed GetConceptualFaceXYZ2UV(%lld, %d, %g, %g, %g)", m_instance, m_iConceptualFace, startPoint.x, startPoint.y, startPoint.z);
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

    if (ModifyInstance(targetLine)) {
        UpdateDynamicDraw();
    }

    CalculateInstance(m_instance);
}

/// <summary>
/// 
/// </summary>
bool DragFaceUV::ModifyInstance(const SEGMENT3& targetLine)
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
        if (best.factor == 0.)
            SetDatatypeProperty(m_instance, best.prop, best.value_);

        for (int i = 0; i < 3; i++) {
            m_workingPoints[i] = best.workingPoints[i];
        }

        TRACE(__FUNCTION__ " sets %s=%g\n", GetNameOfProperty(best.prop), best.value_);
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
    double  v[2] = { prop.initialValue_,  StandardStep(prop.initialValue_) };

    double  d[2]; //measure of mistake from desired

    SEGMENT3 effectLine;
    effectLine.pt[0] = m_dragRay.org;

    if (prop.factor == 0.)
        effectLine.pt[1] = m_dragRay.org + prop.effect;
    else
        effectLine.pt[1] = m_dragRay.org + m_dragRay.dir;

    TRACE("Effect line: (%g, %g, %g) - (%g, %g, %g)\n",
        effectLine.pt[0].x, effectLine.pt[0].y, effectLine.pt[0].z,
        effectLine.pt[1].x, effectLine.pt[1].y, effectLine.pt[1].z
    );

    SEGMENT3 targetPoints;
    LineLineClosestPoints(targetPoints, targetLine, effectLine);

    suggestion.workingPoints[0] = targetPoints.pt[0];
    suggestion.workingPoints[1] = targetPoints.pt[1];

    double len = 0.;
    if (prop.factor) {
        VECTOR3 pnt = targetPoints.pt[1];
        Vec3Subtract(&pnt, &m_dragRay.org);
        len = Vec3Length(&pnt);

        if (len) {
            double  dotproduct = Vec3Dot(&pnt, &m_dragRay.dir);

            if (dotproduct < 0.) {
                len = - len;
            }
        }
    }

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

    if (prop.factor) {

        //if (prop.factor != -1.) {
        //    int u = 0;
        //}

        SetDatatypeProperty(m_instance, prop.prop, prop.initialValue_ + len * prop.factor);

        Log(RDFGEOM_LOG_LEVEL::ERR, "LEN: %g", len);

    }
    else {
        SetDatatypeProperty(m_instance, prop.prop, val);
    }

    VECTOR3 facePoint;
    if (GetCurrentXYZ(facePoint)) {
        mistake = MeasureOfMistake(prop, targetPoints.pt[0], facePoint);
        TRACE("Interpolated point (%g, %g, %g) has mistake: %g\n", facePoint.x, facePoint.y, facePoint.z, mistake);
    }
    else {
        TRACE("Failed to get interpolated point\n");
    }
    
    if (prop.factor == 0.)
        SetDatatypeProperty(m_instance, prop.prop, prop.initialValue_);

    if (fabs(mistake) < fabs(d[0]) && fabs(mistake) < fabs(d[1])) {
        //better position from interpolation
        suggestion.prop = prop.prop;
        suggestion.value_ = val;
        suggestion.factor = prop.factor;
        suggestion.workingPoints[2] = facePoint;
        return fabs(mistake);
    }
    else if (fabs(d[1]) < fabs(d[0])) {
        //better position from standard step
        suggestion.prop = prop.prop;
        suggestion.value_ = v[1];
        suggestion.factor = prop.factor;
        suggestion.workingPoints[2] = effectLine.pt[1];
        return fabs(d[1]);
    }
    else {
        return FLT_MAX;
    }
}
