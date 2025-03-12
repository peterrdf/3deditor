#include "stdafx.h"
#include "RDFView.h"
#include "RDFController.h"

// ------------------------------------------------------------------------------------------------
CRDFView::CRDFView()
	: _rdf_view()
	, m_pController(nullptr)
{
}

// ------------------------------------------------------------------------------------------------
CRDFView::~CRDFView()
{
}

// ------------------------------------------------------------------------------------------------
//void CRDFView::SetController(CRDFController * pController)
//{
//	assert(pController != nullptr);
//
//	m_pController = pController;
//
//	OnControllerChanged();
//}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnModelChanged()
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnWorldDimensionsChanged()
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnShowBaseInformation()
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnShowMetaInformation()
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnInstanceSelected(CRDFView * /*pSender*/)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnInstancePropertySelected()
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnInstanceNameEdited(CRDFView* /*pSender*/, CRDFInstance* /*pInstance*/)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnInstancePropertyEdited(CRDFInstance * /*pInstance*/, CRDFProperty * /*pProperty*/)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnNewInstanceCreated(CRDFView * /*pSender*/, CRDFInstance * /*pInstance*/)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnInstanceDeleted(CRDFView * /*pSender*/, int64_t /*iInstance*/)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnInstancesDeleted(CRDFView * /*pSender*/)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnMeasurementsAdded(CRDFView * /*pSender*/, CRDFInstance * /*pInstance*/)
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnInstancesEnabledStateChanged()
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnApplicationPropertyChanged(CRDFView* /*pSender*/, enumApplicationProperty /*enApplicationProperty*/)
{
}

// ------------------------------------------------------------------------------------------------
CRDFController * CRDFView::GetController() const
{
	return m_pController;
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnControllerChanged()
{
}

