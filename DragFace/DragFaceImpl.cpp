#include "pch.h"
#include "DragFaceImpl.h"

/// <summary>
/// 
/// </summary>
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

/// <summary>
/// 
/// </summary>
DragFaceImpl::PropertyState::PropertyState()
{
    value = formula_NewValue();
}

/// <summary>
/// 
/// </summary>
DragFaceImpl::PropertyState::~PropertyState()
{
    formula_DeleteValue(value);
    value = NULL;
}

/// <summary>
/// 
/// </summary>
DragFaceImpl::DragFaceImpl()
{
    m_instance = NULL;
    m_iConceptualFace = 0;
    m_logger = NULL;
    m_hostData = nullptr;
 
    Vec3Init(m_dragRay.dir);
    Vec3Init(m_dragRay.org);

    m_activeProperties.clear();
    m_savedState.clear();
    m_changed = false;

    m_drawDynamic = NULL;
    m_drawStartPoint = NULL;
    m_drawTargetPoints[0] = NULL;
    m_drawTargetPoints[1] = NULL;
}

/// <summary>
/// 
/// </summary>
bool DragFaceImpl::StartDrag(OwlInstance inst, int iConceptualFace, VECTOR3 const& startPoint, RDFGEOM_CALLBACK_LOG logger, void* hostData)
{
    Log(RDFGEOM_LOG_LEVEL::INFO, __FUNCTION__ ": instance 0x%p, conceptual face %d\n", inst, iConceptualFace);
    Log(RDFGEOM_LOG_LEVEL::INFO, "   start drag point: (%g, %g, %g)\n", startPoint.x, startPoint.y, startPoint.z);

    if (!IsUpToDate(inst)) {
        assert(false);
        CalculateInstance(inst);
    }

    m_instance = inst;
    m_iConceptualFace = iConceptualFace;
    m_logger = logger;
    m_hostData = hostData;

    if (OnStartDrag(startPoint)) {
        CollectEffectiveProperties();

        if (m_activeProperties.size()) {
            PrepareDynamicDraw();
            return true;
        }
        else {
            AfxMessageBox(L"No effective properties found for dragging", MB_ICONSTOP | MB_TOPMOST);
        }
    }

    CalculateInstance(m_instance);
    return false;
}

/// <summary>
/// 
/// </summary>
RdfProperty DragFaceImpl::GetActivePropertyByIterator(RdfProperty prev, double& effect)
{
    if (m_activeProperties.empty()) {
        return NULL;
    }

    auto it = m_activeProperties.begin();

    if (prev) {
        while (it != m_activeProperties.end() && it->prop != prev) {
            it++;
        }

        if (it != m_activeProperties.end()) {
            it++;
        }
        else Log(RDFGEOM_LOG_LEVEL::ERR, __FUNCTION__ ": property is not in list");
    }

    if (it != m_activeProperties.end()) {
        effect = Vec3LengthSqr(&it->effect);
        return it->prop;
    }
    else {
        return NULL;
    }
}
    
/// <summary>
/// 
/// </summary>
void DragFaceImpl::RemoveActiveProperty(RdfProperty prop)
{
    for (auto it = m_activeProperties.begin(); it != m_activeProperties.end(); ++it) {
        if (it->prop == prop) {
            m_activeProperties.erase(it);
            return;
        }
    }
    Log(RDFGEOM_LOG_LEVEL::ERR, __FUNCTION__ ": property not found");
}

/// <summary>
/// 
/// </summary>
void DragFaceImpl::Dragging(SEGMENT3 const& targetLine)
{
    TRACE(__FUNCTION__ ": targetLine: (%g, %g, %g) - (%g, %g, %g)\n",
        targetLine.pt[0].x, targetLine.pt[0].y, targetLine.pt[0].z,
        targetLine.pt[1].x, targetLine.pt[1].y, targetLine.pt[1].z
    );

    if (!m_instance)
        return;

    OnDragging(targetLine);
}

/// <summary>
/// 
/// </summary>
OwlInstance DragFaceImpl::FinishDrag(bool apply)
{
    if (!m_instance)
        return NULL;

    if (!apply) {
        RestoreInstance(true);
    }

    ClearDynamicDraw();

    CalculateInstance(m_instance);

    return m_instance;
}

/// <summary>
/// 
/// </summary>
void DragFaceImpl::Log(RDFGEOM_LOG_LEVEL level, const char* msgFormat, ...)
{
    va_list args;
    va_start(args, msgFormat);

    CStringA msg;
    msg.FormatV(msgFormat, args);

    va_end(args);

    if (m_logger) {
        m_logger(level, msg, m_hostData);
    }

    OutputDebugStringA(msg);
}


/// <summary>
/// 
/// </summary>
void DragFaceImpl::RestoreInstance(bool cleanSavedState)
{
    if (m_instance && m_changed) {
        for (auto& propState : m_savedState) {
            SetPropertyDerived(m_instance, propState.first, propState.second.derived);
            if (!propState.second.derived) {
                formula_SetPropertyValue(m_instance, propState.first, *propState.second.value);
            }
        }
        m_changed = false;
    }

    if (cleanSavedState)
        m_savedState.clear();
}

/// <summary>
/// 
/// </summary>
void DragFaceImpl::CollectEffectiveProperties()
{
    m_activeProperties.clear();

    //save initial state
    m_savedState.clear();
    RdfProperty prop = NULL;
    while (NULL != (prop = GetInstancePropertyByIterator(m_instance, prop))) {
        auto& state = m_savedState[prop];
        state.derived = GetPropertyDerived(m_instance, prop);
        if (!state.derived) {
            formula_GetPropertyValue(m_instance, prop, *state.value);
        }
    }
    m_changed = false;

    //effects of properties
    while (NULL != (prop = GetInstancePropertyByIterator(m_instance, prop))) {
        auto propType = GetPropertyType(prop);
        if (!GetPropertyDerived(m_instance, prop) && (propType == DATATYPEPROPERTY_TYPE_DOUBLE)) {

            double* values = NULL;
            int64_t card = 0;
            GetDatatypeProperty(m_instance, prop, (void**)&values, &card);
            if (card == 1) {

                double oldValue = values[0];

                double newValue = StandardStep(oldValue);
                SetDatatypeProperty(m_instance, prop, newValue);

                PropertyEffect propEffect;
                propEffect.prop = prop;
                propEffect.initialValue = oldValue;

                CalculateEffect(propEffect.effect);

                SetDatatypeProperty(m_instance, prop, oldValue);

                auto effect = Vec3LengthSqr(&propEffect.effect);
                if (fabs(effect) > 1e-5 && fabs(effect) < FLT_MAX - 1) {
                    m_activeProperties.push_back(propEffect);
                }
            }
        }
    }
}

/// <summary>
/// 
/// </summary>
double DragFaceImpl::StandardStep(double oldValue)
{
    return fabs(oldValue) > 0.1 ? oldValue * 1.1 : 1;
}

/// <summary>
/// 
/// </summary>
void DragFaceImpl::PrepareDynamicDraw()
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

    m_drawStartPoint = DrawPoint(model, m_dragRay.org, size, "start point");
    m_drawTargetPoints[0] = DrawPoint(model, m_dragRay.org, size, "target point");
    m_drawTargetPoints[1] = DrawPoint(model, m_dragRay.org, size, "target point on normal");

    OwlInstance collection[] = { m_drawStartPoint, m_drawTargetPoints[0], m_drawTargetPoints[1], m_instance };

    m_drawDynamic = GEOM::Collection::Create(model, "dragging drawing");
    m_drawDynamic.set_objects(collection, _countof(collection));
}

/// <summary>
/// 
/// </summary>
void DragFaceImpl::UpdateDynamicDraw(const SEGMENT3& targetPoints)
{
    for (int i = 0; i < 2; i++) {
        auto& pt = targetPoints.pt[i];

        GEOM::Matrix* M = const_cast<GEOM::Matrix*>(m_drawTargetPoints[i].get_matrix());
        assert(M);
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
void DragFaceImpl::ClearDynamicDraw()
{
#if 1
    auto res = RemoveInstance(m_drawDynamic);
    ASSERT(res == 0);
    res += RemoveInstanceRecursively(m_drawStartPoint);
    res += RemoveInstanceRecursively(m_drawTargetPoints[0]);
    res += RemoveInstanceRecursively(m_drawTargetPoints[1]);
    ASSERT(res == 9);
#else
    //left for debugging
    OwlInstance collection[] = { m_drawStartPoint, m_drawTargetPoints[0], m_drawTargetPoints[1] };
    m_drawDynamic.set_objects(collection, _countof(collection));
#endif
}
