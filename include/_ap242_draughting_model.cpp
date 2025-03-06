#include "_host.h"
#include "_ap242_draughting_model.h"
#include "_instance.h"

// ************************************************************************************************
_ap242_draughting_model::_ap242_draughting_model(SdaiInstance sdaiInstance)
	: m_sdaiInstance(sdaiInstance)
	, m_strName(L"")
	, m_vecAnnotationPlanes()
	, m_vecDraughtingCallouts()
{
	assert(m_sdaiInstance != 0);

	SdaiAttr sdaiNameAttr = sdaiGetAttrDefinition(sdaiGetEntity(sdaiGetInstanceModel(m_sdaiInstance), "REPRESENTATION"), "name");
	assert(sdaiNameAttr != nullptr);

	wchar_t* szName = nullptr;
	sdaiGetAttr(m_sdaiInstance, sdaiNameAttr, sdaiUNICODE, &szName);
	m_strName = szName != nullptr ? szName : L"NA";
}

/*virtual*/ _ap242_draughting_model::~_ap242_draughting_model()
{
}

void _ap242_draughting_model::enableInstances(bool bEnable)
{
	for (auto pAnnotationPlane : m_vecAnnotationPlanes)
	{
		assert(pAnnotationPlane->getInstances().size() == 1);
		pAnnotationPlane->getInstances()[0]->setEnable(bEnable);
	}

	for (auto pDraughtingCallout : m_vecDraughtingCallouts)
	{
		assert(pDraughtingCallout->getInstances().size() == 1);
		pDraughtingCallout->getInstances()[0]->setEnable(bEnable);
	}
}

// ************************************************************************************************
_ap242_annotation_plane::_ap242_annotation_plane(OwlInstance owlInstance, SdaiInstance sdaiInstance)
	: _ap242_geometry(owlInstance, sdaiInstance)
	, m_strName(L"")
{
	wchar_t* szName = 0;
	sdaiGetAttrBN(getSdaiInstance(), "name", sdaiUNICODE, &szName);
	m_strName = szName != nullptr ? szName : L"NA";
}

/*virtual*/ _ap242_annotation_plane::~_ap242_annotation_plane()
{
}

// ************************************************************************************************
_ap242_draughting_callout::_ap242_draughting_callout(OwlInstance owlInstance, SdaiInstance sdaiInstance)
	: _ap242_geometry(owlInstance, sdaiInstance)
	, m_strName(L"")
{
	wchar_t* szName = 0;
	sdaiGetAttrBN(getSdaiInstance(), "name", sdaiUNICODE, &szName);
	m_strName = szName != nullptr ? szName : L"NA";
}

/*virtual*/ _ap242_draughting_callout::~_ap242_draughting_callout()
{
}