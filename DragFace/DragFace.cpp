#include "pch.h"
#include "DragFace.h"
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

static GEOM::Transformation DrawPoint(OwlModel model, VECTOR3 const& pt, double size, const char* name)
{
    char full_name[256];
    sprintf_s(full_name, "%s (%g, %g, %g)", name, pt.x, pt.y, pt.z);

	auto sphere = GEOM::Sphere::Create(model, full_name);
	sphere.set_radius(size);
	sphere.set_segmentationParts(36);

	auto T = GEOM::Matrix::Create(model);
	T.set__41(pt.x);
	T.set__42(pt.y);
	T.set__43(pt.z);

	auto trans = GEOM::Transformation::Create(model, name);
	trans.set_object(sphere);
	trans.set_matrix(T);

	return trans;
}

static double GetMinIntersectionPosition(OwlInstance inst, const RAY3& ray, RdfProperty prop, double value)
{
    SetDatatypeProperty(inst, prop, value);
    CalculateInstance(inst);

    std::vector<VECTOR3> intersections;
    IntersectLineInstance(intersections, ray, inst);

    double minDot = FLT_MAX;

    for (auto& pt : intersections) {
        auto vecToPoint = pt - ray.org;
        double dot = Vec3Dot(&ray.dir, &vecToPoint);
        minDot = min(minDot, dot);
    }

    return minDot;
}

struct PropertyResult
{
    RdfProperty    prop;
    double         oldValue;
    double         newValue;
    double         distance; //along ray from target point; signed positive in direction to start drag point,
};

static bool TryProperty(PropertyResult& result, OwlInstance inst, const RAY3& ray)
{
    bool better = false;

    double v[2] = { result.oldValue, NAN };
    double p[2] = { result.distance, NAN };

    v[1] = fabs(result.oldValue) > 0.1 ? result.oldValue * 1.1 : 1;
    p[1] = GetMinIntersectionPosition(inst, ray, result.prop, v[1]);

    if (fabs(p[1]) < fabs(result.distance)) {
        //better position found
        better = true;
        result.newValue = v[1];
        result.distance = p[1];
    }

    if (fabs(p[1]-p[0]) > LENGTH_TOLERANCE) {
        //linear interpolation to zero position
        double val = v[0] - p[0] * (v[1] - v[0]) / (p[1] - p[0]);
        double pos = GetMinIntersectionPosition(inst, ray, result.prop, val);
     
        if (fabs(pos) < fabs(result.distance)) {
            //better position found
            better = true;
            result.newValue = val;
            result.distance = pos;
        }
    }

    return better;
}


/// <summary>
/// 
/// </summary>
DragFace::DragFace()
{
    Cleanup();
}


/// <summary>
/// 
/// </summary>
DragFace::~DragFace()
{
    AssertIsClean();
}

/// <summary>
/// 
/// </summary>
void DragFace::AssertIsClean()
{
    ASSERT(!m_instance && !m_changedProperty && isnan(m_oldValue) && !m_drawDynamic && !m_drawStartPoint && !m_drawTargetPoints[0] && !m_drawTargetPoints[1]);
}

/// <summary>
/// 
/// </summary>
void DragFace::Cleanup()
{
    m_instance = NULL;
    m_changedProperty = NULL;
    m_oldValue = NAN;
    m_drawDynamic = NULL;
    m_drawStartPoint = NULL;
    m_drawTargetPoints[0] = NULL;
    m_drawTargetPoints[1] = NULL;

    AssertIsClean();
}

/// <summary>
/// 
/// </summary>
void DragFace::StartDrag(OwlInstance inst, int iConceptualFace, VECTOR3 const& startDragPoint)
{
    TRACE(__FUNCTION__ ": instance 0x%p, conceptual face %d\n", inst, iConceptualFace);
    TRACE("   start drag point: (%g, %g, %g)\n", startDragPoint.x, startDragPoint.y, startDragPoint.z);

    AssertIsClean();
    Cleanup();

    VECTOR3 normal;
    if (FindNormal(normal, startDragPoint, inst, iConceptualFace)) {
        m_instance = inst;
        m_startNormal.pt[0] = startDragPoint;
        m_startNormal.pt[1] = startDragPoint + normal;

        PrepareDynamicDraw();
    }
    else {
        TRACE(__FUNCTION__ ": failed to find normal at start drag point\n");
    }
}

/// <summary>
/// 
/// </summary>
void DragFace::Dragging(SEGMENT3 const& targetLine)
{
    if (!m_instance)
        return;

    TRACE(__FUNCTION__ ": targetLine: (%g, %g, %g) - (%g, %g, %g)\n",
        targetLine.pt[0].x, targetLine.pt[0].y, targetLine.pt[0].z,
        targetLine.pt[1].x, targetLine.pt[1].y, targetLine.pt[1].z
    );

    SEGMENT3 targetPoints;
    if (LineLineClosestPoints(targetPoints, m_startNormal, targetLine)) {
        UpdateDynamicDraw(targetPoints);
        RestoreInstance();
        ModifyInstance(targetPoints.pt[1]);
    }

}

/// <summary>
/// 
/// </summary>
OwlInstance DragFace::FinishDrag(bool apply)
{
    if (!m_instance)
        return NULL;

    if (!apply) {
        RestoreInstance();
    }

    ClearDynamicDraw();

    OwlInstance result = m_instance;
    Cleanup();
    
    return result;
}

/// <summary>
/// 
/// </summary>
void DragFace::PrepareDynamicDraw()
{
    if (!m_instance)
        return;

    double box[6] = { 0,0,0,0,0,0 };
    GetBoundingBox(m_instance, box, box + 3);

    double size = 0;
    for (int i = 0; i < 3; i++) {
        size = max(size, (box[i + 3] - box[i]) / 30);
    }

    auto model = GetModel(m_instance);

    m_drawStartPoint = DrawPoint(model, m_startNormal.pt[0], size, "start point");
    m_drawTargetPoints[0] = DrawPoint(model, m_startNormal.pt[1], size, "target point");
    m_drawTargetPoints[1] = DrawPoint(model, m_startNormal.pt[1], size, "target point on normal");

    OwlInstance collection[] = { m_drawStartPoint, m_drawTargetPoints[0], m_drawTargetPoints[1], m_instance };

    m_drawDynamic = GEOM::Collection::Create(model, "dragging drawing");
    m_drawDynamic.set_objects(collection, _countof(collection));
}

/// <summary>
/// 
/// </summary>
void DragFace::UpdateDynamicDraw(const SEGMENT3& targetPoints)
{
    for (int i = 0; i < 2; i++) {
        auto& pt = targetPoints.pt[i];

        GEOM::Matrix* M = const_cast<GEOM::Matrix*>(m_drawTargetPoints[i].get_matrix());
        ASSERT(M);
        if (M) {
            M[0].set__41(pt.x);
            M[0].set__42(pt.y);
            M[0].set__43(pt.z);
        }
    }
}


/// <summary>
/// 
/// </summary>
void DragFace::ClearDynamicDraw()
    {
#if 0
    RemoveInstance(m_drawDynamic);
    RemoveInstanceRecursive(m_drawStartPoint);
    RemoveInstanceRecursive(m_drawTargetPoints[0]);
    RemoveInstanceRecursivw(m_drawTargetPoints[1]);
#else
    OwlInstance collection[] = { m_drawStartPoint, m_drawTargetPoints[0], m_drawTargetPoints[1]};
    m_drawDynamic.set_objects(collection, _countof(collection));
#endif
    }

/// <summary>
/// 
/// </summary>
void DragFace::RestoreInstance()
{
    if (m_instance && m_changedProperty) {
        SetDatatypeProperty(m_instance, m_changedProperty, m_oldValue);
    }

    m_changedProperty = NULL;
    m_oldValue = NAN;
}

/// <summary>
/// 
/// </summary>
void DragFace::ModifyInstance(const VECTOR3& targetPoint)
{
    if (!m_instance)
        return;

    RAY3 directrix; //from target to start point
    directrix.org = targetPoint;
    directrix.dir = m_startNormal.pt[0] - targetPoint;
    double startDistance = Vec3Normalize(directrix.dir);

    std::map<double, PropertyResult> results; //property results sorted by distance

    RdfProperty prop = NULL;
    while (NULL != (prop = GetInstancePropertyByIterator(m_instance, prop))) {

        auto propType = GetPropertyType(prop);
        if (propType == DATATYPEPROPERTY_TYPE_DOUBLE) {

            double* values = NULL;
            int_t card = 0;
            GetDatatypeProperty(m_instance, prop, (void**)&values, &card);
            if (card == 1) {

                double oldValue = values[0];

                PropertyResult result;
                result.prop = prop;
                result.oldValue = oldValue;
                result.newValue = oldValue;
                result.distance = startDistance;

                if (TryProperty(result, m_instance, directrix)) {
                    results[fabs(result.distance)] = result;
                }

                SetDatatypeProperty(m_instance, prop, oldValue);
            }
        }
    }

    if (results.size() > 0) {
        //apply best result
        auto& best = *results.begin();
        if (best.first < startDistance / 2) {
            SetDatatypeProperty(m_instance, best.second.prop, best.second.newValue);
            m_changedProperty = best.second.prop;
            m_oldValue = best.second.oldValue;
        }
    }
}
