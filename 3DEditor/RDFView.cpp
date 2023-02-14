#include "stdafx.h"
#include "RDFView.h"
#include "RDFController.h"

// ------------------------------------------------------------------------------------------------
CRDFView::CRDFView()
	: m_pController(nullptr)
{
}

// ------------------------------------------------------------------------------------------------
CRDFView::~CRDFView()
{
}

// ------------------------------------------------------------------------------------------------
void CRDFView::SetController(CRDFController * pController)
{
	ASSERT(pController != nullptr);

	m_pController = pController;

	OnControllerChanged();
}

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
/*virtual*/ void CRDFView::OnVisibleValuesCountLimitChanged()
{
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void CRDFView::OnApplicationPropertiesChanged()
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

