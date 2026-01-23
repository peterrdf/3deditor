#include "pch.h"
#include "DragFace.h"
#include "DragFaceXYZ.h"

/// <summary>
/// 
/// </summary>
DragFace::DragFace()
{
    m_pImpl = NULL;
}


/// <summary>
/// 
/// </summary>
DragFace::~DragFace()
{
    if (m_pImpl) {
        delete m_pImpl;
    }
    m_pImpl = NULL;
}


/// <summary>
/// 
/// </summary>
bool DragFace::StartDrag(OwlInstance inst, int iConceptualFace, VECTOR3 const& startDragPoint, RDFGEOM_CALLBACK_LOG logger, void* hostData)
{
    if (m_pImpl) {
        delete m_pImpl;
    }

    m_pImpl = new DragFaceXYZ();
    
    if (!m_pImpl->StartDrag(inst, iConceptualFace, startDragPoint, logger, hostData)){
        delete m_pImpl;
        m_pImpl = NULL;
    }

    return m_pImpl != NULL;
}

/// <summary>
/// 
/// </summary>
RdfProperty DragFace::GetActivePropertyByIterator(RdfProperty prev, double& effect)
{
    if (m_pImpl) {
        return m_pImpl->GetActivePropertyByIterator(prev, effect);
    }
    return NULL;
}

/// <summary>
/// 
/// </summary>
void DragFace::RemoveActiveProperty(RdfProperty prop)
{
    if (m_pImpl) {
        m_pImpl->RemoveActiveProperty(prop);
    }
}

/// <summary>
/// 
/// </summary>
void DragFace::Dragging(SEGMENT3 const& targetLine)
{
    if (m_pImpl) {
        m_pImpl->Dragging(targetLine);
    }
}

/// <summary>
/// 
/// </summary>
OwlInstance DragFace::GetDynamicDraw()
{
    if (m_pImpl) {
        return m_pImpl->GetDynamicDraw();
    }
    return NULL;
}

/// <summary>
/// 
/// </summary>
OwlInstance DragFace::FinishDrag(bool apply)
{
    OwlInstance inst = NULL;

    if (m_pImpl) {
        inst = m_pImpl->FinishDrag(apply);
        delete m_pImpl;
        m_pImpl = NULL;
    }

    return inst;
}

