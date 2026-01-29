#include "pch.h"
#include "DragFaceXYZ.h"
#include "GeomLib.h"

#if 0
static SEGMENT3& TrimLineToBox(
    const SEGMENT3& line,
    double          box[6])
{
    VECTOR3 lineDir = line.pt[1] - line.pt[0];
    Vec3Normalize(lineDir);

    // Segment range
    double tmm[2] = { FLT_MAX,  -(FLT_MAX-1) };

    // project 8 cube vertices
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {

                auto vert = Vec3Make(
                    (i ? box[0] : box[3]),
                    (j ? box[1] : box[4]),
                    (k ? box[2] : box[5])
                );

                auto diff = vert - line.pt[0];
                double t = Vec3Dot(diff, lineDir);
                if (t < tmm[0]) tmm[0] = t;
                if (t > tmm[1]) tmm[1] = t;
            }
        }
    }

    // 
    SEGMENT3 segment;
    segment.pt[0] = line.pt[0] + lineDir * tmm[0];
    segment.pt[1] = line.pt[1] + lineDir * tmm[1];

    //check
    auto dir = segment.pt[1]-segment.pt[0];
    Vec3Normalize(dir);
    auto collinear = Vec3Dot(&dir, &lineDir);
    assert(fabs(fabs(collinear) - 1) < 1e-7);

    return segment;
}
#endif

#if 0
static void TrimLineToSize(SEGMENT3& line, double size)
{
    VECTOR3 lineDir = line.pt[1] - line.pt[0];
    Vec3Normalize(lineDir);
    VECTOR3 pt0 = line.pt[0];
    VECTOR3 offset = lineDir * (size * 0.5);
    line.pt[0] = pt0 - offset;
    line.pt[1] = pt0 + offset;
}
#endif


/// <summary>
/// 
/// </summary>
DragFaceXYZ::DragFaceXYZ()
{
}


/// <summary>
/// 
/// </summary>
DragFaceXYZ::~DragFaceXYZ()
{
}



/// <summary>
/// 
/// </summary>
bool DragFaceXYZ::OnStartDrag(VECTOR3 const& startDragPoint)
{
    m_dragRay.org = startDragPoint;
    if (FindNormal(m_dragRay.dir, m_dragRay.org, m_instance, m_faceDiscriminator.c_str(), 1e-1)) {
        return true;
    }
    else {
        Log(RDFGEOM_LOG_LEVEL::ERR, "Failed to find normal at start drag point");
    }

    return false;
}

/// <summary>
/// 
/// </summary>
void DragFaceXYZ::CalculateEffect(VECTOR3& effect)
{
    effect.x = GetMinIntersectionPosition(m_instance, m_dragRay);
    effect.y = 0;
    effect.z = 0;
}

/// <summary>
/// 
/// </summary>
void DragFaceXYZ::OnDragging(SEGMENT3 const& targetLine)
{    
    SEGMENT3 targetPoints;
    if (LineLineClosestPoints(targetPoints, targetLine, m_dragRay)) {

        UpdateDynamicDraw(targetPoints);

        RestoreInstance(false);
        ModifyInstance(targetPoints.pt[1]);
        CalculateInstance(m_instance);
    }
}


/// <summary>
/// 
/// </summary>
bool DragFaceXYZ::TryModifyByProperty(const PropertyEffect& prop, double distDesired, double& suggestedValue, double& distResult)
{
    if (!m_instance)
        return false;

    double v[2] = { prop.initialValue,  StandardStep(prop.initialValue)};
    double p[2] = { 0,                  prop.effect.x };

    bool better = false;
    suggestedValue = v[0];
    distResult = p[0];
    
    if (fabs(p[1]-distDesired) < fabs(p[0]-distDesired)) {
        //better position found
        better = true;
        suggestedValue = v[1];
        distResult = p[1];
    }

    if (fabs(p[1] - p[0]) > LENGTH_TOLERANCE) {
        //linear interpolation to zero position
        double val = v[0] + (distDesired - p[0]) * (v[1] - v[0]) / (p[1] - p[0]);
        
        SetDatatypeProperty(m_instance, prop.prop, val);
        double pos = GetMinIntersectionPosition(m_instance, m_dragRay);
        SetDatatypeProperty(m_instance, prop.prop, prop.initialValue);

        if (fabs(pos-distDesired) < fabs(distResult-distDesired)) {
            //better position found
            better = true;
            suggestedValue = val;
            distResult = pos;
        }
    }

    return better;
}



/// <summary>
/// 
/// </summary>
void DragFaceXYZ::ModifyInstance(const VECTOR3& targetPoint)
{
    if (!m_instance)
        return;

    VECTOR3 targetDir = targetPoint - m_dragRay.org;
    double distDesired = Vec3Dot(m_dragRay.dir, targetDir);

    std::map<double, std::pair<PropertyEffect*, double>> results; //map of distFromTarget to (property, suggestedValue)

    for (auto& propEffect : m_activeProperties) {
        
        double suggestedValue = NAN;
        double distResult = NAN;
        
        if (TryModifyByProperty(propEffect, distDesired, suggestedValue, distResult)) {
            auto& pair = results[fabs(distResult-distDesired)];
            pair.first = &propEffect;
            pair.second = suggestedValue;
        }
    }

    //apply best result
    if (!results.empty())
    {
        auto& best = results.begin()->second;

        m_changed = true; 
        SetDatatypeProperty(m_instance, best.first->prop, best.second);

        TRACE(__FUNCTION__ " sets %s=%g\n", GetNameOfProperty(best.first->prop), best.second);
    }
    else
    {
        TRACE(__FUNCTION__ " does not found good change\n");
    }
}

