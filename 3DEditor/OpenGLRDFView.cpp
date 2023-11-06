#include "stdafx.h"

#include "OpenGLRDFView.h"
#include "RDFController.h"
#include "ProgressIndicator.h"

#include <chrono>

#include "Resource.h"

#ifdef _LINUX
#include <cfloat>
#include <GL/gl.h>
#include <GL/glext.h>
#endif // _LINUX

// ------------------------------------------------------------------------------------------------
extern BOOL TEST_MODE;

// ------------------------------------------------------------------------------------------------
wchar_t FACE_SELECTION_IBO[] = L"FACE_SELECTION_IBO";
wchar_t BOUNDING_BOX_VAO[] = L"BOUNDING_BOX_VAO";
wchar_t BOUNDING_BOX_VBO[] = L"BOUNDING_BOX_VBO";
wchar_t BOUNDING_BOX_IBO[] = L"BOUNDING_BOX_IBO";

wchar_t NORMAL_VECS_VAO[] = L"NORMAL_VECS_VAO";
wchar_t NORMAL_VECS_VBO[] = L"NORMAL_VECS_VBO";
wchar_t TANGENT_VECS_VAO[] = L"TANGENT_VECS_VAO";
wchar_t TANGENT_VECS_VBO[] = L"TANGENT_VECS_VBO";
wchar_t BINORMAL_VECS_VAO[] = L"BINORMAL_VECS_VAO";
wchar_t BINORMAL_VECS_VBO[] = L"BINORMAL_VECS_VBO";

wchar_t TOOLTIP_INFORMATION[] = L"Information";

int NAVIGATION_VIEW_LENGTH = 200;
int MIN_VIEW_PORT_LENGTH = 100;

// ------------------------------------------------------------------------------------------------
#ifdef _LINUX
COpenGLRDFView::COpenGLRDFView(wxGLCanvas * pWnd)
#else
COpenGLRDFView::COpenGLRDFView(CWnd * pWnd)
#endif // _LINUX
	: _oglRenderer()
	, CRDFView()
	, m_bShowFaces(TRUE)
	, m_strCullFaces(CULL_FACES_NONE)
	, m_bShowFacesPolygons(FALSE)
	, m_bShowConceptualFacesPolygons(TRUE)
	, m_bShowLines(TRUE)
	, m_fLineWidth(1.f)
	, m_bShowPoints(TRUE)
	, m_fPointSize(1.f)
	, m_bShowBoundingBoxes(FALSE)
	, m_bShowNormalVectors(FALSE)
	, m_bShowTangenVectors(FALSE)
	, m_bShowBiNormalVectors(FALSE)
	, m_bScaleVectors(FALSE)
	, m_bShowCoordinateSystem(TRUE)
	, m_bShowNavigator(TRUE)
	, m_ptStartMousePosition(-1, -1)
	, m_ptPrevMousePosition(-1, -1)
	, m_pInstanceSelectionFrameBuffer(new _oglSelectionFramebuffer())	
	, m_pPointedInstance(nullptr)
	, m_pSelectedInstance(nullptr)
	, m_pFaceSelectionFrameBuffer(new _oglSelectionFramebuffer())
	, m_iPointedFace(-1)
	, m_pNavigatorSelectionFrameBuffer(new _oglSelectionFramebuffer())
	, m_pNavigatorPointedInstance(nullptr)
	, m_pSelectedInstanceMaterial(new _material())
	, m_pPointedInstanceMaterial(new _material())
	, m_pNavigatorPointedInstanceMaterial(new _material())
{
	ASSERT(pWnd != nullptr);

	_initialize(
		pWnd,
		TEST_MODE ? 1 : 16,
		IDR_TEXTFILE_VERTEX_SHADER2, 
		IDR_TEXTFILE_FRAGMENT_SHADER2, 
		TEXTFILE,
		true);
	
	m_pSelectedInstanceMaterial->init(
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f,
		nullptr);

	m_pPointedInstanceMaterial->init(
		.33f, .33f, .33f,
		.33f, .33f, .33f,
		.33f, .33f, .33f,
		.33f, .33f, .33f,
		.66f,
		nullptr);
	
	m_pNavigatorPointedInstanceMaterial->init(
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f,
		nullptr);

	// OpenGL
	m_pOGLProgram->_setAmbientLightWeighting(
		0.4f,
		0.4f,
		0.4f);
}

// ------------------------------------------------------------------------------------------------
COpenGLRDFView::~COpenGLRDFView()
{
	GetController()->UnRegisterView(this);	

	delete m_pInstanceSelectionFrameBuffer;
	delete m_pFaceSelectionFrameBuffer;	
	delete m_pNavigatorSelectionFrameBuffer;
	
	// PATCH: AMD 6700 XT - Access violation
	if (!TEST_MODE)
	{
		_destroy();
	}

	delete m_pSelectedInstanceMaterial;
	m_pSelectedInstanceMaterial = nullptr;

	delete m_pPointedInstanceMaterial;
	m_pPointedInstanceMaterial = nullptr;
	
	delete m_pNavigatorPointedInstanceMaterial;
	m_pNavigatorPointedInstanceMaterial = nullptr;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetShowFaces(BOOL bValue)
{
	m_bShowFaces = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowFaces);
	GetController()->GetSettingsStorage()->SetSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL COpenGLRDFView::GetShowFaces(CRDFModel* pModel)
{
	if ((pModel == nullptr) || (pModel == GetController()->GetModel()))
	{
		return m_bShowFaces;
	}

	return TRUE;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetCullFacesMode(LPCTSTR szMode)
{
	m_strCullFaces = szMode;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_strCullFaces);
	GetController()->GetSettingsStorage()->SetSetting(strSettingName, (LPCSTR)CW2A(szMode));
}

// ------------------------------------------------------------------------------------------------
LPCTSTR COpenGLRDFView::GetCullFacesMode(CRDFModel* pModel) const
{
	if ((pModel == nullptr) || (pModel == GetController()->GetModel()))
	{
		return m_strCullFaces;
	}

	return CULL_FACES_NONE;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetShowFacesPolygons(BOOL bValue)
{
	m_bShowFacesPolygons = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowFacesPolygons);
	GetController()->GetSettingsStorage()->SetSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::GetShowFacesPolygons(CRDFModel* pModel) const
{
	if ((pModel == nullptr) || (pModel == GetController()->GetModel()))
	{
		return m_bShowFacesPolygons;
	}

	return FALSE;	
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetShowConceptualFacesPolygons(BOOL bValue)
{
	m_bShowConceptualFacesPolygons = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowConceptualFacesPolygons);
	GetController()->GetSettingsStorage()->SetSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::GetShowConceptualFacesPolygons(CRDFModel* pModel) const
{
	if ((pModel == nullptr) || (pModel == GetController()->GetModel()))
	{
		return m_bShowConceptualFacesPolygons;
	}

	return TRUE;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetShowLines(BOOL bValue)
{
	m_bShowLines = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowLines);
	GetController()->GetSettingsStorage()->SetSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::GetShowLines(CRDFModel* pModel) const
{
	if ((pModel == nullptr) || (pModel == GetController()->GetModel()))
	{
		return m_bShowLines;
	}

	return TRUE;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetLineWidth(GLfloat fWidth)
{
	m_fLineWidth = fWidth;
}

// ------------------------------------------------------------------------------------------------
GLfloat COpenGLRDFView::GetLineWidth() const
{
	return m_fLineWidth;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetShowPoints(BOOL bValue)
{
	m_bShowPoints = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowPoints);
	GetController()->GetSettingsStorage()->SetSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::GetShowPoints(CRDFModel* pModel) const
{
	if ((pModel == nullptr) || (pModel == GetController()->GetModel()))
	{
		return m_bShowPoints;
	}

	return TRUE;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetPointSize(GLfloat fSize)
{
	m_fPointSize = fSize;
}

// ------------------------------------------------------------------------------------------------
GLfloat COpenGLRDFView::GetPointSize() const
{
	return m_fPointSize;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetShowBoundingBoxes(BOOL bValue)
{
	m_bShowBoundingBoxes = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowBoundingBoxes);
	GetController()->GetSettingsStorage()->SetSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::GetShowBoundingBoxes(CRDFModel* pModel) const
{
	if ((pModel == nullptr) || (pModel == GetController()->GetModel()))
	{
		return m_bShowBoundingBoxes;
	}

	return FALSE;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetShowNormalVectors(BOOL bValue)
{
	m_bShowNormalVectors = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowNormalVectors);
	GetController()->GetSettingsStorage()->SetSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::GetShowNormalVectors(CRDFModel* pModel) const
{
	if ((pModel == nullptr) || (pModel == GetController()->GetModel()))
	{
		return m_bShowNormalVectors;
	}

	return FALSE;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetShowTangentVectors(BOOL bValue)
{
	m_bShowTangenVectors = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowTangenVectors);
	GetController()->GetSettingsStorage()->SetSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::GetShowTangentVectors(CRDFModel* pModel) const
{
	if ((pModel == nullptr) || (pModel == GetController()->GetModel()))
	{
		return m_bShowTangenVectors;
	}

	return FALSE;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetShowBiNormalVectors(BOOL bValue)
{
	m_bShowBiNormalVectors = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowBiNormalVectors);
	GetController()->GetSettingsStorage()->SetSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::GetShowBiNormalVectors(CRDFModel* pModel) const
{
	if ((pModel == nullptr) || (pModel == GetController()->GetModel()))
	{
		return m_bShowBiNormalVectors;
	}

	return FALSE;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetScaleVectors(BOOL bValue)
{
	m_bScaleVectors = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bScaleVectors);
	GetController()->GetSettingsStorage()->SetSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::GetScaleVectors(CRDFModel* pModel) const
{
	if ((pModel == nullptr) || (pModel == GetController()->GetModel()))
	{
		return m_bScaleVectors;
	}

	return FALSE;
}

void COpenGLRDFView::SetShowCoordinateSystem(BOOL bValue) 
{ 
	m_bShowCoordinateSystem = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowCoordinateSystem);
	GetController()->GetSettingsStorage()->SetSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL COpenGLRDFView::GetShowCoordinateSystem() const 
{ 
	return m_bShowCoordinateSystem;
}

void COpenGLRDFView::SetShowNavigator(BOOL bValue) 
{ 
	m_bShowNavigator = bValue; 

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowNavigator);
	GetController()->GetSettingsStorage()->SetSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL COpenGLRDFView::GetShowNavigator() const 
{ 
	return m_bShowNavigator;
}

void COpenGLRDFView::LoadSettings()
{
	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowFaces);
		string strValue = GetController()->GetSettingsStorage()->GetSetting(strSettingName);

		m_bShowFaces = !strValue.empty() ? strValue == "TRUE" : TRUE;
	}	

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_strCullFaces);
		string strValue = GetController()->GetSettingsStorage()->GetSetting(strSettingName);

		m_strCullFaces = !strValue.empty() ? CA2W(strValue.c_str()) : CULL_FACES_NONE;
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowFacesPolygons);
		string strValue = GetController()->GetSettingsStorage()->GetSetting(strSettingName);

		m_bShowFacesPolygons = !strValue.empty() ? strValue == "TRUE" : FALSE;
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowConceptualFacesPolygons);
		string strValue = GetController()->GetSettingsStorage()->GetSetting(strSettingName);

		m_bShowConceptualFacesPolygons = !strValue.empty() ? strValue == "TRUE" : TRUE;
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowLines);
		string strValue = GetController()->GetSettingsStorage()->GetSetting(strSettingName);

		m_bShowLines = !strValue.empty() ? strValue == "TRUE" : TRUE;
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowPoints);
		string strValue = GetController()->GetSettingsStorage()->GetSetting(strSettingName);

		m_bShowPoints = !strValue.empty() ? strValue == "TRUE" : TRUE;
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowBoundingBoxes);
		string strValue = GetController()->GetSettingsStorage()->GetSetting(strSettingName);

		m_bShowBoundingBoxes = !strValue.empty() ? strValue == "TRUE" : FALSE;
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowNormalVectors);
		string strValue = GetController()->GetSettingsStorage()->GetSetting(strSettingName);

		m_bShowNormalVectors = !strValue.empty() ? strValue == "TRUE" : FALSE;
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowTangenVectors);
		string strValue = GetController()->GetSettingsStorage()->GetSetting(strSettingName);

		m_bShowTangenVectors = !strValue.empty() ? strValue == "TRUE" : FALSE;
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowBiNormalVectors);
		string strValue = GetController()->GetSettingsStorage()->GetSetting(strSettingName);

		m_bShowBiNormalVectors = !strValue.empty() ? strValue == "TRUE" : FALSE;
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bScaleVectors);
		string strValue = GetController()->GetSettingsStorage()->GetSetting(strSettingName);

		m_bScaleVectors = !strValue.empty() ? strValue == "TRUE" : FALSE;
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowCoordinateSystem);
		string strValue = GetController()->GetSettingsStorage()->GetSetting(strSettingName);

		m_bShowCoordinateSystem = !strValue.empty() ? strValue == "TRUE" : TRUE;
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowNavigator);
		string strValue = GetController()->GetSettingsStorage()->GetSetting(strSettingName);

		m_bShowNavigator = !strValue.empty() ? strValue == "TRUE" : TRUE;
	}
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::Reset()
{
	// Rotation
	m_fXAngle = 0.f;
	m_fYAngle = 0.f;
	m_fZAngle = 0.f;
	m_rotation = _quaterniond::toQuaternion(0., 0., 0.);

	// Translation
	m_fXTranslation = 0.f;
	m_fYTranslation = 0.f;
	m_fZTranslation = -5.f;
	m_fScaleFactor = 2.f;

	// UI
	m_bShowFaces = TRUE;
	m_strCullFaces = CULL_FACES_NONE;
	m_bShowFacesPolygons = FALSE;
	m_bShowConceptualFacesPolygons = TRUE;
	m_bShowLines = TRUE;
	m_bShowPoints = TRUE;
	m_bShowBoundingBoxes = FALSE;
	m_bShowNormalVectors = FALSE;
	m_bShowTangenVectors = FALSE;
	m_bShowBiNormalVectors = FALSE;
	m_bScaleVectors = FALSE;
	m_bShowCoordinateSystem = TRUE;
	m_bShowNavigator = TRUE;

	_redraw();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetRotation(float fX, float fY, BOOL bRedraw)
{
	m_fXAngle = fX;
	m_fYAngle = fY;

	if (bRedraw)
	{
		_redraw();
	}
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::GetRotation(float& fX, float& fY)
{
	fX = m_fXAngle;
	fY = m_fYAngle;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetTranslation(float fX, float fY, float fZ, BOOL bRedraw)
{
	m_fXTranslation = fX;
	m_fYTranslation = fY;
	m_fZTranslation = fZ;

	if (bRedraw)
	{
		_redraw();
	}
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::GetTranslation(float& fX, float& fY, float& fZ)
{
	fX = m_fXTranslation;
	fY = m_fYTranslation;
	fZ = m_fZTranslation;
}

// ------------------------------------------------------------------------------------------------
#ifdef _LINUX
void COpenGLRDFView::Draw(wxPaintDC * pDC)
#else
void COpenGLRDFView::Draw(CDC* pDC)
#endif // _LINUX
{
	CRect rcClient;
	m_pWnd->GetClientRect(&rcClient);

	int iWidth = rcClient.Width();
	int iHeight = rcClient.Height();

	if ((iWidth < MIN_VIEW_PORT_LENGTH) || (iHeight < MIN_VIEW_PORT_LENGTH))
	{
		return;
	}

	auto pController = GetController();
	if (pController == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	DrawMainModel(
		pController->GetModel(),
		pController->GetSceneModel(),
		0, 0,
		iWidth, iHeight);

	if (m_bShowNavigator)
	{
		DrawNavigatorModel(
			pController->GetNavigatorModel(),
			0, 0,
			iWidth, iHeight);
	}

	/* End */
#ifdef _LINUX
	m_pWnd->SwapBuffers();
#else
	SwapBuffers(*pDC);
#endif // _LINUX

	/* Selection */

	DrawMainModelSelectionBuffers(
		pController->GetModel(),
		0, 0,
		iWidth, iHeight,
		m_pInstanceSelectionFrameBuffer);

	if (m_bShowNavigator)
	{
		DrawNavigatorModelSelectionBuffers(
			pController->GetNavigatorModel(),
			0, 0,
			iWidth, iHeight,
			m_pNavigatorSelectionFrameBuffer);
	}	
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::OnMouseEvent(enumMouseEvent enEvent, UINT nFlags, CPoint point)
{
	if (enEvent == enumMouseEvent::LBtnUp)
	{
		/*
		* OnSelectedItemChanged() notification
		*/
		if (point == m_ptStartMousePosition)
		{
			if (m_pSelectedInstance != m_pPointedInstance)
			{
				m_pSelectedInstance = m_pPointedInstance;
				m_iPointedFace = -1;

				m_pFaceSelectionFrameBuffer->encoding().clear();

				_redraw();

				ASSERT(GetController() != nullptr);
				GetController()->SelectInstance(this, m_pSelectedInstance);
			} // if (m_pSelectedInstance != ...

			if (m_pSelectedInstance == nullptr)
			{
				if (SelectNavigatorInstance())
				{
					_redraw();
				}
			}
		}
	} // if (enEvent == meLBtnDown)

	switch (enEvent)
	{
		case enumMouseEvent::Move:
		{
			OnMouseMoveEvent(nFlags, point);
		}
		break;

		case enumMouseEvent::LBtnDown:
		case enumMouseEvent::MBtnDown:
		case enumMouseEvent::RBtnDown:
		{
			m_ptStartMousePosition = point;
			m_ptPrevMousePosition = point;
		}
		break;

		case enumMouseEvent::LBtnUp:
		case enumMouseEvent::MBtnUp:
		case enumMouseEvent::RBtnUp:
		{
			m_ptStartMousePosition.x = -1;
			m_ptStartMousePosition.y = -1;
			m_ptPrevMousePosition.x = -1;
			m_ptPrevMousePosition.y = -1;
		}
		break;

		default:
			ASSERT(FALSE);
			break;
	} // switch (enEvent)
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	_onMouseWheel(nFlags, zDelta, pt);
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	_onKeyUp(nChar, nRepCnt, nFlags);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnModelChanged() /*override*/
{
	CWaitCursor waitCursor;	

#ifdef _LINUX
    m_pOGLContext->SetCurrent(*m_pWnd);
#else
    BOOL bResult = m_pOGLContext->makeCurrent();
	VERIFY(bResult);
#endif // _LINUX

	// OpenGL buffers
	m_oglBuffers.clear();

	m_pInstanceSelectionFrameBuffer->encoding().clear();
	m_pPointedInstance = nullptr;
	m_pSelectedInstance = nullptr;

	m_pFaceSelectionFrameBuffer->encoding().clear();
	m_iPointedFace = -1;

	m_pNavigatorSelectionFrameBuffer->encoding().clear();
	m_pNavigatorPointedInstance = nullptr;

	auto pController = GetController();
	if (pController == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto pModel = pController->GetModel();
	if (pModel == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	/*
	* Center the World
	*/
	m_fXTranslation = fXmin;
	m_fXTranslation += (fXmax - fXmin) / 2.f;
	m_fXTranslation = -m_fXTranslation;

	m_fYTranslation = fYmin;
	m_fYTranslation += (fYmax - fYmin) / 2.f;
	m_fYTranslation = -m_fYTranslation;

	m_fZTranslation = fZmin;
	m_fZTranslation += (fZmax - fZmin) / 2.f;
	m_fZTranslation = -m_fZTranslation;
	m_fZTranslation -= (pModel->GetBoundingSphereDiameter() * 2.f);

	m_fScaleFactor = pModel->GetBoundingSphereDiameter();

	LoadModel(pModel);
	LoadModel(pController->GetSceneModel());
	LoadModel(pController->GetNavigatorModel());
	
	_redraw();
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnWorldDimensionsChanged() /*override*/
{
	/*
	* Center
	*/
	auto pController = GetController();
	ASSERT(pController != nullptr);

	CRDFModel * pModel = pController->GetModel();
	ASSERT(pModel != nullptr);

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	m_fXTranslation = fXmin;
	m_fXTranslation += (fXmax - fXmin) / 2.f;
	m_fXTranslation = -m_fXTranslation;

	m_fYTranslation = fYmin;
	m_fYTranslation += (fYmax - fYmin) / 2.f;
	m_fYTranslation = -m_fYTranslation;

	m_fZTranslation = fZmin;
	m_fZTranslation += (fZmax - fZmin) / 2.f;
	m_fZTranslation = -m_fZTranslation;
	m_fZTranslation -= (pModel->GetBoundingSphereDiameter() * 2.f);

	m_fScaleFactor = pModel->GetBoundingSphereDiameter();

	/*
	* Reload model
	*/
	OnModelChanged();

	/*
	* Restore the selection
	*/
	OnInstanceSelected(nullptr);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnInstancePropertyEdited(CRDFInstance* /*pInstance*/, CRDFProperty* /*pProperty*/) /*override*/
{
	/*
	* Reload model
	*/
	OnModelChanged();

	/*
	* Restore the selection
	*/
	OnInstanceSelected(nullptr);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnNewInstanceCreated(CRDFView* /*pSender*/, CRDFInstance* /*pInstance*/) /*override*/
{
	/*
	* Reload model
	*/
	OnModelChanged();

	/*
	* Restore the selection
	*/
	OnInstanceSelected(nullptr);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnInstanceDeleted(CRDFView* /*pSender*/, int64_t /*iInstance*/) /*override*/
{
	/*
	* Reload model
	*/
	OnModelChanged();

	/*
	* Restore the selection
	*/
	OnInstanceSelected(nullptr);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnInstancesDeleted(CRDFView* /*pSender*/) /*override*/
{
	/*
	* Reload model
	*/
	OnModelChanged();

	/*
	* Restore the selection
	*/
	OnInstanceSelected(nullptr);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnMeasurementsAdded(CRDFView* /*pSender*/, CRDFInstance* /*pInstance*/) /*override*/
{
	/*
	* Reload model
	*/
	OnModelChanged();

	/*
	* Restore the selection
	*/
	OnInstanceSelected(nullptr);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnInstanceSelected(CRDFView* pSender) /*override*/
{
	if (pSender == this)
	{
		return;
	}

	if (GetController() == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto pSelectedInstance = GetController()->GetSelectedInstance();
	if ((pSelectedInstance != nullptr) && (!pSelectedInstance->HasGeometry() || pSelectedInstance->getTriangles().empty()))
	{
		pSelectedInstance = nullptr;
	}

	if (m_pSelectedInstance != pSelectedInstance)
	{
		m_iPointedFace = -1;

		m_pSelectedInstance = pSelectedInstance;

		m_pFaceSelectionFrameBuffer->encoding().clear();

		_redraw();
	}
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnInstancePropertySelected() /*override*/
{
	ASSERT(GetController() != nullptr);

	pair<CRDFInstance *, CRDFProperty *> prSelectedInstanceProperty = GetController()->GetSelectedInstanceProperty();

	CRDFInstance * pSelectedInstance = prSelectedInstanceProperty.first;

	if ((pSelectedInstance != nullptr) && (!pSelectedInstance->HasGeometry() || pSelectedInstance->getTriangles().empty()))
	{
		pSelectedInstance = nullptr;
	}

	if (m_pSelectedInstance != pSelectedInstance)
	{
		m_iPointedFace = -1;

		m_pSelectedInstance = pSelectedInstance;

		m_pFaceSelectionFrameBuffer->encoding().clear();

		_redraw();
	}
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnInstancesEnabledStateChanged() /*override*/
{
	/*
	* Reload model
	*/
	OnModelChanged();

	/*
	* Restore the selection
	*/
	OnInstanceSelected(nullptr);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnApplicationPropertyChanged(CRDFView* pSender, enumApplicationProperty enApplicationProperty) /*override*/
{
	if (pSender == this)
	{
		return;
	}

	switch (enApplicationProperty)
	{
		case enumApplicationProperty::Projection:
		case enumApplicationProperty::View:
		case enumApplicationProperty::ShowFaces:
		case enumApplicationProperty::CullFaces:
		case enumApplicationProperty::ShowFacesWireframes:
		case enumApplicationProperty::ShowConceptualFacesWireframes:
		case enumApplicationProperty::ShowLines:
		case enumApplicationProperty::ShowPoints:
		case enumApplicationProperty::ShowNormalVectors:
		case enumApplicationProperty::ShowTangenVectors:
		case enumApplicationProperty::ShowBiNormalVectors:
		case enumApplicationProperty::ScaleVectors:
		case enumApplicationProperty::ShowBoundingBoxes:
		case enumApplicationProperty::RotationMode:
		case enumApplicationProperty::PointLightingLocation:
		case enumApplicationProperty::AmbientLightWeighting:
		case enumApplicationProperty::SpecularLightWeighting:
		case enumApplicationProperty::DiffuseLightWeighting:
		case enumApplicationProperty::MaterialShininess:
		case enumApplicationProperty::Contrast:
		case enumApplicationProperty::Brightness:
		case enumApplicationProperty::Gamma:
		case enumApplicationProperty::ShowCoordinateSystem:		
		case enumApplicationProperty::ShowNavigator:
		{
			_redraw();
		}
		break;

		case enumApplicationProperty::VisibleValuesCountLimit:
		case enumApplicationProperty::ScalelAndCenter:
		case enumApplicationProperty::CoordinateSystemType:
		{
			// Not supported
		}
		break;

		default:
		{
			ASSERT(FALSE); // Internal error!
		}
		break;
	} // switch (enApplicationProperty)
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnControllerChanged()
{
	ASSERT(GetController() != nullptr);

	GetController()->RegisterView(this);

	LoadSettings();
}

void COpenGLRDFView::LoadModel(CRDFModel* pModel)
{
	if (pModel == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	ProgressStatus prgs(L"Prepare rendering");

	// Limits
	GLsizei VERTICES_MAX_COUNT = _oglUtils::getVerticesCountLimit(GEOMETRY_VBO_VERTEX_LENGTH * sizeof(float));
	GLsizei INDICES_MAX_COUNT = _oglUtils::getIndicesCountLimit();

	auto& mapInstances = pModel->GetInstances();

	// VBO
	GLuint iVerticesCount = 0;
	vector<CRDFInstance*> vecInstancesCohort;

	// IBO - Conceptual faces
	GLuint iConcFacesIndicesCount = 0;
	vector<_cohort*> vecConcFacesCohorts;

	// IBO - Conceptual face polygons
	GLuint iConcFacePolygonsIndicesCount = 0;
	vector<_cohort*> vecConcFacePolygonsCohorts;

	// IBO - Face polygons
	GLuint iFacePolygonsIndicesCount = 0;
	vector<_cohort*> vecFacePolygonsCohorts;

	// IBO - Lines
	GLuint iLinesIndicesCount = 0;
	vector<_cohort*> vecLinesCohorts;

	// IBO - Points
	GLuint iPointsIndicesCount = 0;
	vector<_cohort*> vecPointsCohorts;

	prgs.Start(mapInstances.size());
	for (auto itInstance = mapInstances.begin(); itInstance != mapInstances.end(); itInstance++)
	{
		prgs.Step();

		auto pInstance = itInstance->second;
		if (pInstance->GetVerticesCount() == 0)
		{
			continue;
		}

		/******************************************************************************************
		* Geometry
		*/

		/**
		* VBO - Conceptual faces, polygons, etc.
		*/
		if (((int_t)iVerticesCount + pInstance->GetVerticesCount()) > (int_t)VERTICES_MAX_COUNT)
		{
			if (m_oglBuffers.createInstancesCohort(vecInstancesCohort, m_pOGLProgram) != iVerticesCount)
			{
				assert(false);

				return;
			}

			iVerticesCount = 0;
			vecInstancesCohort.clear();
		}

		/*
		* IBO - Conceptual faces
		*/
		for (size_t iFacesCohort = 0; iFacesCohort < pInstance->concFacesCohorts().size(); iFacesCohort++)
		{
			if ((int_t)(iConcFacesIndicesCount + pInstance->concFacesCohorts()[iFacesCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
			{
				if (m_oglBuffers.createIBO(vecConcFacesCohorts) != iConcFacesIndicesCount)
				{
					assert(false);

					return;
				}

				iConcFacesIndicesCount = 0;
				vecConcFacesCohorts.clear();
			}

			iConcFacesIndicesCount += (GLsizei)pInstance->concFacesCohorts()[iFacesCohort]->indices().size();
			vecConcFacesCohorts.push_back(pInstance->concFacesCohorts()[iFacesCohort]);
		}

		/*
		* IBO - Conceptual face polygons
		*/
		for (size_t iConcFacePolygonsCohort = 0; iConcFacePolygonsCohort < pInstance->concFacePolygonsCohorts().size(); iConcFacePolygonsCohort++)
		{
			if ((int_t)(iConcFacePolygonsIndicesCount + pInstance->concFacePolygonsCohorts()[iConcFacePolygonsCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
			{
				if (m_oglBuffers.createIBO(vecConcFacePolygonsCohorts) != iConcFacePolygonsIndicesCount)
				{
					assert(false);

					return;
				}

				iConcFacePolygonsIndicesCount = 0;
				vecConcFacePolygonsCohorts.clear();
			}

			iConcFacePolygonsIndicesCount += (GLsizei)pInstance->concFacePolygonsCohorts()[iConcFacePolygonsCohort]->indices().size();
			vecConcFacePolygonsCohorts.push_back(pInstance->concFacePolygonsCohorts()[iConcFacePolygonsCohort]);
		}

		/*
		* IBO - Face polygons
		*/
		for (size_t iFacePolygonsCohort = 0; iFacePolygonsCohort < pInstance->facePolygonsCohorts().size(); iFacePolygonsCohort++)
		{
			if ((int_t)(iFacePolygonsIndicesCount + pInstance->facePolygonsCohorts()[iFacePolygonsCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
			{
				if (m_oglBuffers.createIBO(vecFacePolygonsCohorts) != iFacePolygonsIndicesCount)
				{
					assert(false);

					return;
				}

				iFacePolygonsIndicesCount = 0;
				vecFacePolygonsCohorts.clear();
			}

			iFacePolygonsIndicesCount += (GLsizei)pInstance->facePolygonsCohorts()[iFacePolygonsCohort]->indices().size();
			vecFacePolygonsCohorts.push_back(pInstance->facePolygonsCohorts()[iFacePolygonsCohort]);
		}

		/*
		* IBO - Lines
		*/
		for (size_t iLinesCohort = 0; iLinesCohort < pInstance->linesCohorts().size(); iLinesCohort++)
		{
			if ((int_t)(iLinesIndicesCount + pInstance->linesCohorts()[iLinesCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
			{
				if (m_oglBuffers.createIBO(vecLinesCohorts) != iLinesIndicesCount)
				{
					assert(false);

					return;
				}

				iLinesIndicesCount = 0;
				vecLinesCohorts.clear();
			}

			iLinesIndicesCount += (GLsizei)pInstance->linesCohorts()[iLinesCohort]->indices().size();
			vecLinesCohorts.push_back(pInstance->linesCohorts()[iLinesCohort]);
		}

		/*
		* IBO - Points
		*/
		for (size_t iPointsCohort = 0; iPointsCohort < pInstance->pointsCohorts().size(); iPointsCohort++)
		{
			if ((int_t)(iPointsIndicesCount + pInstance->pointsCohorts()[iPointsCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
			{
				if (m_oglBuffers.createIBO(vecPointsCohorts) != iPointsIndicesCount)
				{
					assert(false);

					return;
				}

				iPointsIndicesCount = 0;
				vecPointsCohorts.clear();
			}

			iPointsIndicesCount += (GLsizei)pInstance->pointsCohorts()[iPointsCohort]->indices().size();
			vecPointsCohorts.push_back(pInstance->pointsCohorts()[iPointsCohort]);
		}

		iVerticesCount += (GLsizei)pInstance->GetVerticesCount();
		vecInstancesCohort.push_back(pInstance);
	} // for (; itInstance != ...

	prgs.Finish();

	/******************************************************************************************
	* Geometry
	*/

	/*
	* VBO - Conceptual faces, polygons, etc.
	*/
	if (iVerticesCount > 0)
	{
		if (m_oglBuffers.createInstancesCohort(vecInstancesCohort, m_pOGLProgram) != iVerticesCount)
		{
			assert(false);

			return;
		}

		iVerticesCount = 0;
		vecInstancesCohort.clear();
	}

	/*
	* IBO - Conceptual faces
	*/
	if (iConcFacesIndicesCount > 0)
	{
		if (m_oglBuffers.createIBO(vecConcFacesCohorts) != iConcFacesIndicesCount)
		{
			assert(false);

			return;
		}

		iConcFacesIndicesCount = 0;
		vecConcFacesCohorts.clear();
	}

	/*
	* IBO - Conceptual face polygons
	*/
	if (iConcFacePolygonsIndicesCount > 0)
	{
		if (m_oglBuffers.createIBO(vecConcFacePolygonsCohorts) != iConcFacePolygonsIndicesCount)
		{
			assert(false);

			return;
		}

		iConcFacePolygonsIndicesCount = 0;
		vecConcFacePolygonsCohorts.clear();
	}

	/*
	* IBO - Face polygons
	*/
	if (iFacePolygonsIndicesCount > 0)
	{
		if (m_oglBuffers.createIBO(vecFacePolygonsCohorts) != iFacePolygonsIndicesCount)
		{
			assert(false);

			return;
		}

		iFacePolygonsIndicesCount = 0;
		vecFacePolygonsCohorts.clear();
	}

	/*
	* IBO - Lines
	*/
	if (iLinesIndicesCount > 0)
	{
		if (m_oglBuffers.createIBO(vecLinesCohorts) != iLinesIndicesCount)
		{
			assert(false);

			return;
		}

		iLinesIndicesCount = 0;
		vecLinesCohorts.clear();
	}

	/*
	* IBO - Points
	*/
	if (iPointsIndicesCount > 0)
	{
		if (m_oglBuffers.createIBO(vecPointsCohorts) != iPointsIndicesCount)
		{
			assert(false);

			return;
		}

		iPointsIndicesCount = 0;
		vecPointsCohorts.clear();
	}
}

void COpenGLRDFView::DrawMainModel(
	CRDFModel* pMainModel,
	CRDFModel* pSceneModel,
	int iViewportX, int iViewportY,
	int iViewportWidth, int iViewportHeight)
{
	if (pMainModel == nullptr)
	{
		return;
	}

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pMainModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	_prepare(
		iViewportX, iViewportY,
		iViewportWidth, iViewportHeight,
		fXmin, fXmax,
		fYmin, fYmax,
		fZmin, fZmax,
		true,
		true);

	/* Model */
	DrawModel(pMainModel);

	/* Scene */
	if (GetShowCoordinateSystem())
	{
		DrawModel(pSceneModel);
	}
}

void COpenGLRDFView::DrawNavigatorModel(
	CRDFModel* pNavigatorModel,
	int /*iViewportX*/, int iViewportY,
	int iViewportWidth, int /*iViewportHeight*/)
{
	if (pNavigatorModel == nullptr)
	{
		return;
	}

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pNavigatorModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	_prepare(
		iViewportWidth - NAVIGATION_VIEW_LENGTH, iViewportY,
		NAVIGATION_VIEW_LENGTH, NAVIGATION_VIEW_LENGTH,
		fXmin, fXmax,
		fYmin, fYmax,
		fZmin, fZmax,
		false,
		false);

	/* Model */
	DrawModel(pNavigatorModel);
}

void COpenGLRDFView::DrawModel(CRDFModel* pModel)
{
	m_pOGLProgram->_enableTexture(false);

	/* Non-transparent faces */
	DrawFaces(pModel, false);

	/* Transparent faces */
	DrawFaces(pModel, true);

	/* Pointed face */
	DrawPointedFace(pModel);

	/* Faces polygons */
	DrawFacesPolygons(pModel);

	/* Conceptual faces polygons */
	DrawConceptualFacesPolygons(pModel);

	/* Lines */
	DrawLines(pModel);

	/* Points */
	DrawPoints(pModel);

	/* Bounding boxes */
	DrawBoundingBoxes(pModel);

	/* Normal vectors */
	DrawNormalVectors(pModel);

	/* Tangent vectors */
	DrawTangentVectors(pModel);

	/* Bi-Normal vectors */
	DrawBiNormalVectors(pModel);
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawFaces(CRDFModel* pModel, bool bTransparent)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (!GetShowFaces(pModel))
	{
		return;
	}

	auto pController = GetController();
	if (pController == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto begin = std::chrono::steady_clock::now();

	CString strCullFaces = GetCullFacesMode(pModel);

	if (bTransparent)
	{
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		if ((strCullFaces == CULL_FACES_FRONT) || (strCullFaces == CULL_FACES_BACK))
		{
			glEnable(GL_CULL_FACE);
			glCullFace(strCullFaces == CULL_FACES_FRONT ? GL_FRONT : GL_BACK);
		}
	}
	
	m_pOGLProgram->_enableBlinnPhongModel(true);

	const auto pPointedInstance = pModel == pController->GetModel() ?
		m_pPointedInstance : m_pNavigatorPointedInstance;
	const auto pPointedInstanceMaterial = pModel == pController->GetModel() ? 
		m_pPointedInstanceMaterial : m_pNavigatorPointedInstanceMaterial;

	for (auto itCohort : m_oglBuffers.instancesCohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto pInstance : itCohort.second)
		{
			if ((pInstance->GetModel() != pModel->GetModel()) || !pInstance->getEnable())
			{
				continue;
			}
			
			for (auto pConcFacesCohort : pInstance->concFacesCohorts())
			{
				const _material* pMaterial =
					pInstance == m_pSelectedInstance ? m_pSelectedInstanceMaterial :
					pInstance == pPointedInstance ? pPointedInstanceMaterial :
					pConcFacesCohort->getMaterial();

				if (bTransparent)
				{
					if (pMaterial->getA() == 1.0)
					{
						continue;
					}
				}
				else
				{
					if (pMaterial->getA() < 1.0)
					{
						continue;
					}
				}
				
				if (pMaterial->hasTexture())
				{
					auto pOGLTexture = pModel->GetTexture(pMaterial->texture());

					m_pOGLProgram->_enableTexture(true);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, pOGLTexture->getOGLName());

					m_pOGLProgram->_setSampler(0);
				}
				else
				{			
					m_pOGLProgram->_setMaterial(pMaterial);
				}

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pConcFacesCohort->ibo());
				glDrawElementsBaseVertex(GL_TRIANGLES,
					(GLsizei)pConcFacesCohort->indices().size(),
					GL_UNSIGNED_INT,
					(void*)(sizeof(GLuint) * pConcFacesCohort->iboOffset()),
					pInstance->VBOOffset());

				if (pMaterial->hasTexture())
				{
					m_pOGLProgram->_enableTexture(false);
				}
			} // for (auto pConcFacesCohort ...
		} // for (auto pInstance ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	if (bTransparent)
	{
		glDisable(GL_BLEND);
	}
	else
	{
		if ((strCullFaces == CULL_FACES_FRONT) || (strCullFaces == CULL_FACES_BACK))
		{
			glDisable(GL_CULL_FACE);
		}
	}

	_oglUtils::checkForErrors();

	auto end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawFaces() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());	
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawFacesPolygons(CRDFModel* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (!GetShowFacesPolygons(pModel))
	{
		return;
	}

	auto begin = std::chrono::steady_clock::now();

	m_pOGLProgram->_enableBlinnPhongModel(false);
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	for (auto itCohort : m_oglBuffers.instancesCohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto itInstance : itCohort.second)
		{
			CRDFInstance* pInstance = itInstance;

			if ((pInstance->GetModel() != pModel->GetModel()) || !pInstance->getEnable())
			{
				continue;
			}

			for (size_t iCohort = 0; iCohort < pInstance->facePolygonsCohorts().size(); iCohort++)
			{
				_cohort* pCohort = pInstance->facePolygonsCohorts()[iCohort];

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->ibo());
				glDrawElementsBaseVertex(GL_LINES,
					(GLsizei)pCohort->indices().size(),
					GL_UNSIGNED_INT,
					(void*)(sizeof(GLuint) * pCohort->iboOffset()),
					pInstance->VBOOffset());
			}
		} // for (auto itInstance ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	_oglUtils::checkForErrors();

	auto end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawFacesPolygons() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawConceptualFacesPolygons(CRDFModel* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (!GetShowConceptualFacesPolygons(pModel))
	{
		return;
	}

	auto begin = std::chrono::steady_clock::now();

	m_pOGLProgram->_enableBlinnPhongModel(false);
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	for (auto itCohort : m_oglBuffers.instancesCohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto pInstance : itCohort.second)
		{
			if ((pInstance->GetModel() != pModel->GetModel()) || !pInstance->getEnable())
			{
				continue;
			}

			for (auto pCohort : pInstance->concFacePolygonsCohorts())
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->ibo());
				glDrawElementsBaseVertex(GL_LINES,
					(GLsizei)pCohort->indices().size(),
					GL_UNSIGNED_INT,
					(void*)(sizeof(GLuint) * pCohort->iboOffset()),
					pInstance->VBOOffset());
			}
		} // for (auto itInstance ...

		glBindVertexArray(0);
	} // for (auto pInstance ...

	_oglUtils::checkForErrors();

	auto end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawConceptualFacesPolygons() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());	
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawLines(CRDFModel* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (!GetShowLines(pModel))
	{
		return;
	}

	auto begin = std::chrono::steady_clock::now();

	m_pOGLProgram->_enableBlinnPhongModel(false);
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	for (auto itCohort : m_oglBuffers.instancesCohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto pInstance : itCohort.second)
		{
			if ((pInstance->GetModel() != pModel->GetModel()) || !pInstance->getEnable())
			{
				continue;
			}

			for (auto pCohort : pInstance->linesCohorts())
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->ibo());
				glDrawElementsBaseVertex(GL_LINES,
					(GLsizei)pCohort->indices().size(),
					GL_UNSIGNED_INT,
					(void*)(sizeof(GLuint) * pCohort->iboOffset()),
					pInstance->VBOOffset());
			}
		} // for (auto pInstance ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	_oglUtils::checkForErrors();

	auto end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawLines() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawPoints(CRDFModel* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (!GetShowPoints(pModel))
	{
		return;
	}

	auto pController = GetController();
	if (pController == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto begin = std::chrono::steady_clock::now();

	glEnable(GL_PROGRAM_POINT_SIZE);

	m_pOGLProgram->_enableBlinnPhongModel(false);
	m_pOGLProgram->_setTransparency(1.f);

	const auto pPointedInstance = pModel == pController->GetModel() ?
		m_pPointedInstance : m_pNavigatorPointedInstance;
	const auto pPointedInstanceMaterial = pModel == pController->GetModel() ?
		m_pPointedInstanceMaterial : m_pNavigatorPointedInstanceMaterial;

	for (auto itCohort : m_oglBuffers.instancesCohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto pInstance : itCohort.second)
		{
			if ((pInstance->GetModel() != pModel->GetModel()) || !pInstance->getEnable())
			{
				continue;
			}

			for (auto pCohort : pInstance->pointsCohorts())
			{
				const _material* pMaterial =
					pInstance == m_pSelectedInstance ? m_pSelectedInstanceMaterial :
					pInstance == pPointedInstance ? pPointedInstanceMaterial :
					pCohort->getMaterial();				
				
				m_pOGLProgram->_setAmbientColor(
					pMaterial->getDiffuseColor().r(),
					pMaterial->getDiffuseColor().g(),
					pMaterial->getDiffuseColor().b());

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->ibo());
				glDrawElementsBaseVertex(GL_POINTS,
					(GLsizei)pCohort->indices().size(),
					GL_UNSIGNED_INT,
					(void*)(sizeof(GLuint) * pCohort->iboOffset()),
					pInstance->VBOOffset());
			} // for (auto pCohort ...		
		} // for (auto pInstance ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	glDisable(GL_PROGRAM_POINT_SIZE);

	_oglUtils::checkForErrors();

	auto end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawPoints() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());	
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawBoundingBoxes(CRDFModel* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (!GetShowBoundingBoxes(pModel))
	{
		return;
	}

	m_pOGLProgram->_enableBlinnPhongModel(false);
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	_oglUtils::checkForErrors();

	bool bIsNew = false;
	GLuint iVAO = m_oglBuffers.getVAOcreateNewIfNeeded(BOUNDING_BOX_VAO, bIsNew);

	if (iVAO == 0)
	{
		ASSERT(FALSE);

		return;
	}	
	
	GLuint iVBO = 0;

	if (bIsNew)
	{
		glBindVertexArray(iVAO);

		iVBO = m_oglBuffers.getBufferCreateNewIfNeeded(BOUNDING_BOX_VBO, bIsNew);
		if ((iVBO == 0) || !bIsNew)
		{
			ASSERT(FALSE);

			return;
		}

		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		m_oglBuffers.setVBOAttributes(m_pOGLProgram);

		GLuint iIBO = m_oglBuffers.getBufferCreateNewIfNeeded(BOUNDING_BOX_IBO, bIsNew);
		if ((iIBO == 0) || !bIsNew)
		{
			ASSERT(FALSE);

			return;
		}

		vector<unsigned int> vecIndices =
		{
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			4, 5,
			5, 6,
			6, 7,
			7, 4,
			0, 6,
			3, 5,
			1, 7,
			2, 4,
		};

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vecIndices.size(), vecIndices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);

		_oglUtils::checkForErrors();
	} // if (bIsNew)
	else
	{
		iVBO = m_oglBuffers.getBuffer(BOUNDING_BOX_VBO);
		if (iVBO == 0)
		{
			ASSERT(FALSE);

			return;
		}
	}

	auto& mapInstances = pModel->GetInstances();
	for (auto itInstance = mapInstances.begin(); 
		itInstance != mapInstances.end(); 
		itInstance++)
	{
		CRDFInstance* pInstance = itInstance->second;

		if ((pInstance->GetModel() != pModel->GetModel()) || !pInstance->getEnable())
		{
			continue;
		}

		if ((pInstance->getBBMin() == nullptr) || (pInstance->getBBMax() == nullptr))
		{
			continue;
		}

		_vector3d vecBoundingBoxMin = { pInstance->getBBMin()->x, pInstance->getBBMin()->y, pInstance->getBBMin()->z };
		_vector3d vecBoundingBoxMax = { pInstance->getBBMax()->x, pInstance->getBBMax()->y, pInstance->getBBMax()->z };

		// Bottom face
		/*
		Min1						Min2
		>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		|								|
		|								|
		|								|
		|								|
		|								|
		<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		Min4						Min3
		*/

		_vector3d vecMin1 = { vecBoundingBoxMin.x, vecBoundingBoxMin.y, vecBoundingBoxMin.z };
		_vector3d vecMin2 = { vecBoundingBoxMax.x, vecBoundingBoxMin.y, vecBoundingBoxMin.z };
		_vector3d vecMin3 = { vecBoundingBoxMax.x, vecBoundingBoxMin.y, vecBoundingBoxMax.z };
		_vector3d vecMin4 = { vecBoundingBoxMin.x, vecBoundingBoxMin.y, vecBoundingBoxMax.z };

		// Top face
		/*
		Max3						Max4
		>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		|								|
		|								|
		|								|
		|								|
		|								|
		<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		Max2						Max1
		*/

		_vector3d vecMax1 = { vecBoundingBoxMax.x, vecBoundingBoxMax.y, vecBoundingBoxMax.z };
		_vector3d vecMax2 = { vecBoundingBoxMin.x, vecBoundingBoxMax.y, vecBoundingBoxMax.z };
		_vector3d vecMax3 = { vecBoundingBoxMin.x, vecBoundingBoxMax.y, vecBoundingBoxMin.z };
		_vector3d vecMax4 = { vecBoundingBoxMax.x, vecBoundingBoxMax.y, vecBoundingBoxMin.z };

		// X, Y, Z, Nx, Ny, Nz, Tx, Ty
		vector<float> vecVertices = 
		{
			(GLfloat)vecMin1.x, (GLfloat)vecMin1.y, (GLfloat)vecMin1.z, 0.f, 0.f, 0.f, 0.f, 0.f,
			(GLfloat)vecMin2.x, (GLfloat)vecMin2.y, (GLfloat)vecMin2.z, 0.f, 0.f, 0.f, 0.f, 0.f,
			(GLfloat)vecMin3.x, (GLfloat)vecMin3.y, (GLfloat)vecMin3.z, 0.f, 0.f, 0.f, 0.f, 0.f,
			(GLfloat)vecMin4.x, (GLfloat)vecMin4.y, (GLfloat)vecMin4.z, 0.f, 0.f, 0.f, 0.f, 0.f,
			(GLfloat)vecMax1.x, (GLfloat)vecMax1.y, (GLfloat)vecMax1.z, 0.f, 0.f, 0.f, 0.f, 0.f,
			(GLfloat)vecMax2.x, (GLfloat)vecMax2.y, (GLfloat)vecMax2.z, 0.f, 0.f, 0.f, 0.f, 0.f,
			(GLfloat)vecMax3.x, (GLfloat)vecMax3.y, (GLfloat)vecMax3.z, 0.f, 0.f, 0.f, 0.f, 0.f,
			(GLfloat)vecMax4.x, (GLfloat)vecMax4.y, (GLfloat)vecMax4.z, 0.f, 0.f, 0.f, 0.f, 0.f,
		};

		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vecVertices.size(), vecVertices.data(), GL_DYNAMIC_DRAW);

		glBindVertexArray(iVAO);

		glDrawElementsBaseVertex(GL_LINES,
			(GLsizei)24,
			GL_UNSIGNED_INT,
			(void*)0,
			0);

		glBindVertexArray(0);
	} // for (; itInstance != ...

	_oglUtils::checkForErrors();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawNormalVectors(CRDFModel* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (!GetShowNormalVectors(pModel))
	{
		return;
	}

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	const float SCALE_FACTOR = GetScaleVectors(pModel) ? sqrt(pow(fXmax - fXmin, 2.f) + pow(fYmax - fYmin, 2.f) + pow(fZmax - fZmin, 2.f)) * 0.1f : 1.f;

	m_pOGLProgram->_enableBlinnPhongModel(false);
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	_oglUtils::checkForErrors();

	bool bIsNew = false;
	GLuint iVAO = m_oglBuffers.getVAOcreateNewIfNeeded(NORMAL_VECS_VAO, bIsNew);

	if (iVAO == 0)
	{
		ASSERT(FALSE);

		return;
	}

	GLuint iVBO = 0;

	if (bIsNew)
	{
		glBindVertexArray(iVAO);

		iVBO = m_oglBuffers.getBufferCreateNewIfNeeded(NORMAL_VECS_VBO, bIsNew);
		if ((iVBO == 0) || !bIsNew)
		{
			ASSERT(FALSE);

			return;
		}

		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		m_oglBuffers.setVBOAttributes(m_pOGLProgram);

		glBindVertexArray(0);

		_oglUtils::checkForErrors();
	} // if (bIsNew)
	else
	{
		iVBO = m_oglBuffers.getBuffer(NORMAL_VECS_VBO);
		if (iVBO == 0)
		{
			ASSERT(FALSE);

			return;
		}
	}

	// X, Y, Z, Nx, Ny, Nz, Tx, Ty
	vector<float> vecVertices;

	if (m_pSelectedInstance == nullptr)
	{
		auto& mapInstances = pModel->GetInstances();

		auto itInstance = mapInstances.begin();
		for (; itInstance != mapInstances.end(); itInstance++)
		{
			CRDFInstance* pInstance = itInstance->second;

			if ((pInstance->GetModel() != pModel->GetModel()) || !pInstance->getEnable())
			{
				continue;
			}

			auto& vecTriangles = pInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				auto pTriangle = const_cast<_primitives*>(&vecTriangles[iTriangle]);

				for (int64_t iIndex = pTriangle->startIndex();
					iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
					iIndex++)
				{
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 3] * SCALE_FACTOR);
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 4] * SCALE_FACTOR);
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 5] * SCALE_FACTOR);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // for (; itInstance != ...
	} // if (m_pSelectedInstance == nullptr)
	else
	{
		if (m_pSelectedInstance->GetModel() == pModel->GetModel())
		{
			auto& vecTriangles = m_pSelectedInstance->getTriangles();
			ASSERT(!vecTriangles.empty());

			if (m_iPointedFace == -1)
			{
				for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
				{
					auto pTriangle = const_cast<_primitives*>(&vecTriangles[iTriangle]);

					for (int64_t iIndex = pTriangle->startIndex();
						iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
						iIndex++)
					{
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0]);
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);

						vecVertices.push_back(0.f); // Nx
						vecVertices.push_back(0.f); // Ny
						vecVertices.push_back(0.f); // Nz
						vecVertices.push_back(0.f); // Tx
						vecVertices.push_back(0.f); // Ty

						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0] +
							m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 3] * SCALE_FACTOR);
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1] +
							m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 4] * SCALE_FACTOR);
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2] +
							m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 5] * SCALE_FACTOR);

						vecVertices.push_back(0.f); // Nx
						vecVertices.push_back(0.f); // Ny
						vecVertices.push_back(0.f); // Nz
						vecVertices.push_back(0.f); // Tx
						vecVertices.push_back(0.f); // Ty
					} // for (size_t iIndex = ...
				} // for (size_t iTriangle = ...
			} // if (m_iPointedFace == -1)
			else
			{
				ASSERT((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

				auto pTriangle = const_cast<_primitives*>(&vecTriangles[m_iPointedFace]);

				for (int64_t iIndex = pTriangle->startIndex();
					iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
					iIndex++)
				{
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 3] * SCALE_FACTOR);
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 4] * SCALE_FACTOR);
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 5] * SCALE_FACTOR);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty
				} // for (size_t iIndex = ...
			} // else if (m_iPointedFace == -1)
		} // if (m_pSelectedInstance->GetModel() == pModel->GetModel())		
	} // else if (m_pSelectedInstance == nullptr)

	if (!vecVertices.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* vecVertices.size(), vecVertices.data(), GL_DYNAMIC_DRAW);

		glBindVertexArray(iVAO);

		glDrawArrays(GL_LINES, 0, (GLsizei)vecVertices.size() / GEOMETRY_VBO_VERTEX_LENGTH);

		glBindVertexArray(0);
	} // if (!vecVertices.empty())

	_oglUtils::checkForErrors();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawTangentVectors(CRDFModel* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (!GetShowTangentVectors(pModel))
	{
		return;
	}

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	const float SCALE_FACTOR = GetScaleVectors(pModel) ? sqrt(pow(fXmax - fXmin, 2.f) + pow(fYmax - fYmin, 2.f) + pow(fZmax - fZmin, 2.f)) * 0.1f : 1.f;

	m_pOGLProgram->_enableBlinnPhongModel(false);
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	_oglUtils::checkForErrors();

	bool bIsNew = false;
	GLuint iVAO = m_oglBuffers.getVAOcreateNewIfNeeded(TANGENT_VECS_VAO, bIsNew);

	if (iVAO == 0)
	{
		ASSERT(FALSE);

		return;
	}

	GLuint iVBO = 0;

	if (bIsNew)
	{
		glBindVertexArray(iVAO);

		iVBO = m_oglBuffers.getBufferCreateNewIfNeeded(TANGENT_VECS_VBO, bIsNew);
		if ((iVBO == 0) || !bIsNew)
		{
			ASSERT(FALSE);

			return;
		}

		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		m_oglBuffers.setVBOAttributes(m_pOGLProgram);

		glBindVertexArray(0);

		_oglUtils::checkForErrors();
	} // if (bIsNew)
	else
	{
		iVBO = m_oglBuffers.getBuffer(TANGENT_VECS_VBO);
		if (iVBO == 0)
		{
			ASSERT(FALSE);

			return;
		}
	}

	// X, Y, Z, Nx, Ny, Nz, Tx, Ty
	vector<float> vecVertices;

	if (m_pSelectedInstance == nullptr)
	{
		auto& mapInstances = pModel->GetInstances();

		map<int64_t, CRDFInstance*>::const_iterator itInstance = mapInstances.begin();
		for (; itInstance != mapInstances.end(); itInstance++)
		{
			CRDFInstance* pInstance = itInstance->second;

			if ((pInstance->GetModel() != pModel->GetModel()) || !pInstance->getEnable())
			{
				continue;
			}

			auto& vecTriangles = pInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				auto pTriangle = const_cast<_primitives*>(&vecTriangles[iTriangle]);

				for (int64_t iIndex = pTriangle->startIndex();
					iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
					iIndex++)
				{
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 12] * SCALE_FACTOR);
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 13] * SCALE_FACTOR);
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 14] * SCALE_FACTOR);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // for (; itInstance != ...
	} // if (m_pSelectedInstance == nullptr)
	else
	{
		if (m_pSelectedInstance->GetModel() == pModel->GetModel())
		{
			auto& vecTriangles = m_pSelectedInstance->getTriangles();
			ASSERT(!vecTriangles.empty());

			if (m_iPointedFace == -1)
			{
				for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
				{
					auto pTriangle = const_cast<_primitives*>(&vecTriangles[iTriangle]);

					for (int64_t iIndex = pTriangle->startIndex();
						iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
						iIndex++)
					{
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0]);
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);

						vecVertices.push_back(0.f); // Nx
						vecVertices.push_back(0.f); // Ny
						vecVertices.push_back(0.f); // Nz
						vecVertices.push_back(0.f); // Tx
						vecVertices.push_back(0.f); // Ty

						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0] +
							m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 12] * SCALE_FACTOR);
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1] +
							m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 13] * SCALE_FACTOR);
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2] +
							m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 14] * SCALE_FACTOR);

						vecVertices.push_back(0.f); // Nx
						vecVertices.push_back(0.f); // Ny
						vecVertices.push_back(0.f); // Nz
						vecVertices.push_back(0.f); // Tx
						vecVertices.push_back(0.f); // Ty
					} // for (size_t iIndex = ...
				} // for (size_t iTriangle = ...
			} // if (m_iPointedFace == -1)
			else
			{
				ASSERT((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

				auto pTriangle = const_cast<_primitives*>(&vecTriangles[m_iPointedFace]);

				for (int64_t iIndex = pTriangle->startIndex();
					iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
					iIndex++)
				{
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 12] * SCALE_FACTOR);
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 13] * SCALE_FACTOR);
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 14] * SCALE_FACTOR);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty
				} // for (size_t iIndex = ...
			} // else if (m_iPointedFace == -1)
		} // if (m_pSelectedInstance->GetModel() == pModel->GetModel())		
	} // else if (m_pSelectedInstance == nullptr)

	if (!vecVertices.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vecVertices.size(), vecVertices.data(), GL_DYNAMIC_DRAW);

		glBindVertexArray(iVAO);

		glDrawArrays(GL_LINES, 0, (GLsizei)vecVertices.size() / GEOMETRY_VBO_VERTEX_LENGTH);

		glBindVertexArray(0);
	} // if (!vecVertices.empty())

	_oglUtils::checkForErrors();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawBiNormalVectors(CRDFModel* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (!GetShowBiNormalVectors(pModel))
	{
		return;
	}

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	const float SCALE_FACTOR = GetScaleVectors(pModel) ? sqrt(pow(fXmax - fXmin, 2.f) + pow(fYmax - fYmin, 2.f) + pow(fZmax - fZmin, 2.f)) * 0.1f : 1.f;

	m_pOGLProgram->_enableBlinnPhongModel(false);
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	_oglUtils::checkForErrors();

	bool bIsNew = false;
	GLuint iVAO = m_oglBuffers.getVAOcreateNewIfNeeded(BINORMAL_VECS_VAO, bIsNew);

	if (iVAO == 0)
	{
		ASSERT(FALSE);

		return;
	}

	GLuint iVBO = 0;

	if (bIsNew)
	{
		glBindVertexArray(iVAO);

		iVBO = m_oglBuffers.getBufferCreateNewIfNeeded(BINORMAL_VECS_VBO, bIsNew);
		if ((iVBO == 0) || !bIsNew)
		{
			ASSERT(FALSE);

			return;
		}

		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		m_oglBuffers.setVBOAttributes(m_pOGLProgram);

		glBindVertexArray(0);

		_oglUtils::checkForErrors();
	} // if (bIsNew)
	else
	{
		iVBO = m_oglBuffers.getBuffer(BINORMAL_VECS_VBO);
		if (iVBO == 0)
		{
			ASSERT(FALSE);

			return;
		}
	}

	// X, Y, Z, Nx, Ny, Nz, Tx, Ty
	vector<float> vecVertices;

	if (m_pSelectedInstance == nullptr)
	{
		auto& mapInstances = pModel->GetInstances();

		map<int64_t, CRDFInstance*>::const_iterator itInstance = mapInstances.begin();
		for (; itInstance != mapInstances.end(); itInstance++)
		{
			CRDFInstance* pInstance = itInstance->second;

			if ((pInstance->GetModel() != pModel->GetModel()) || !pInstance->getEnable())
			{
				continue;
			}

			auto& vecTriangles = pInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				auto pTriangle = const_cast<_primitives*>(&vecTriangles[iTriangle]);

				for (int64_t iIndex = pTriangle->startIndex();
					iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
					iIndex++)
				{
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 15] * SCALE_FACTOR);
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 16] * SCALE_FACTOR);
					vecVertices.push_back(pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						pInstance->GetVertices()[(pInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 17] * SCALE_FACTOR);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // for (; itInstance != ...
	} // if (m_pSelectedInstance == nullptr)
	else
	{
		if (m_pSelectedInstance->GetModel() == pModel->GetModel())
		{
			auto& vecTriangles = m_pSelectedInstance->getTriangles();
			ASSERT(!vecTriangles.empty());

			if (m_iPointedFace == -1)
			{
				for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
				{
					auto pTriangle = const_cast<_primitives*>(&vecTriangles[iTriangle]);

					for (int64_t iIndex = pTriangle->startIndex();
						iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
						iIndex++)
					{
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0]);
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);

						vecVertices.push_back(0.f); // Nx
						vecVertices.push_back(0.f); // Ny
						vecVertices.push_back(0.f); // Nz
						vecVertices.push_back(0.f); // Tx
						vecVertices.push_back(0.f); // Ty

						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0] +
							m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 15] * SCALE_FACTOR);
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1] +
							m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 16] * SCALE_FACTOR);
						vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2] +
							m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 17] * SCALE_FACTOR);

						vecVertices.push_back(0.f); // Nx
						vecVertices.push_back(0.f); // Ny
						vecVertices.push_back(0.f); // Nz
						vecVertices.push_back(0.f); // Tx
						vecVertices.push_back(0.f); // Ty
					} // for (size_t iIndex = ...
				} // for (size_t iTriangle = ...
			} // if (m_iPointedFace == -1)
			else
			{
				ASSERT((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

				auto pTriangle = const_cast<_primitives*>(&vecTriangles[m_iPointedFace]);

				for (int64_t iIndex = pTriangle->startIndex();
					iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
					iIndex++)
				{
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 15] * SCALE_FACTOR);
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 16] * SCALE_FACTOR);
					vecVertices.push_back(m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						m_pSelectedInstance->GetVertices()[(m_pSelectedInstance->GetIndices()[iIndex] * VERTEX_LENGTH) + 17] * SCALE_FACTOR);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty
				} // for (size_t iIndex = ...
			} // else if (m_iPointedFace == -1)
		} // if (m_pSelectedInstance->GetModel() == pModel->GetModel())		
	} // else if (m_pSelectedInstance == nullptr)

	if (!vecVertices.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vecVertices.size(), vecVertices.data(), GL_DYNAMIC_DRAW);

		glBindVertexArray(iVAO);

		glDrawArrays(GL_LINES, 0, (GLsizei)vecVertices.size() / GEOMETRY_VBO_VERTEX_LENGTH);

		glBindVertexArray(0);
	} // if (!vecVertices.empty())

	_oglUtils::checkForErrors();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawInstancesFrameBuffer(CRDFModel* pModel, _oglSelectionFramebuffer* pInstanceSelectionFrameBuffer)
{
	if ((pModel == nullptr) || (pInstanceSelectionFrameBuffer == nullptr))
	{
		return;
	}

	BOOL bResult = m_pOGLContext->makeCurrent();
	VERIFY(bResult);

	/* Create a frame buffer */
	pInstanceSelectionFrameBuffer->create();

	/* Selection colors */
	if (pInstanceSelectionFrameBuffer->encoding().empty())
	{
		auto& mapInstances = pModel->GetInstances();
		for (auto itInstance = mapInstances.begin(); itInstance != mapInstances.end(); itInstance++)
		{
			auto pInstance = itInstance->second;
			if (!pInstance->getEnable())
			{
				continue;
			}

			auto& vecTriangles = pInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			float fR, fG, fB;
			_i64RGBCoder::encode(pInstance->GetID(), fR, fG, fB);

			pInstanceSelectionFrameBuffer->encoding()[pInstance->GetInstance()] = _color(fR, fG, fB);
		}
	} // if (pInstanceSelectionFrameBuffer->encoding().empty())

	/* Draw */

	pInstanceSelectionFrameBuffer->bind();

	glViewport(0, 0, BUFFER_SIZE, BUFFER_SIZE);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up the parameters
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	m_pOGLProgram->_enableBlinnPhongModel(false);
	m_pOGLProgram->_setTransparency(1.f);

	for (auto itCohort : m_oglBuffers.instancesCohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto pInstance : itCohort.second)
		{
			if ((pInstance->GetModel() != pModel->GetModel()) || !pInstance->getEnable())
			{
				continue;
			}

			auto& vecTriangles = pInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			auto itSelectionColor = pInstanceSelectionFrameBuffer->encoding().find(pInstance->GetInstance());
			ASSERT(itSelectionColor != pInstanceSelectionFrameBuffer->encoding().end());

			m_pOGLProgram->_setAmbientColor(
				itSelectionColor->second.r(),
				itSelectionColor->second.g(),
				itSelectionColor->second.b());

			for (auto pConcFacesCohort : pInstance->concFacesCohorts())
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pConcFacesCohort->ibo());
				glDrawElementsBaseVertex(GL_TRIANGLES,
					(GLsizei)pConcFacesCohort->indices().size(),
					GL_UNSIGNED_INT,
					(void*)(sizeof(GLuint) * pConcFacesCohort->iboOffset()),
					pInstance->VBOOffset());
			}
		} // for (auto pInstance ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	pInstanceSelectionFrameBuffer->unbind();

	_oglUtils::checkForErrors();
}

void COpenGLRDFView::DrawMainModelSelectionBuffers(
	CRDFModel* pModel,
	int iViewportX, int iViewportY,
	int iViewportWidth, int iViewportHeight,
	_oglSelectionFramebuffer* pInstanceSelectionFrameBuffer)
{
	if (pModel == nullptr)
	{
		return;
	}

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	_prepare(
		iViewportX, iViewportY,
		iViewportWidth, iViewportHeight,
		fXmin, fXmax,
		fYmin, fYmax,
		fZmin, fZmax,
		true,
		true);

	DrawInstancesFrameBuffer(pModel, pInstanceSelectionFrameBuffer);
	DrawFacesFrameBuffer(pModel);
}

void COpenGLRDFView::DrawNavigatorModelSelectionBuffers(
	CRDFModel* pNavigatorModel,
	int /*iViewportX*/, int iViewportY,
	int iViewportWidth, int /*iViewportHeight*/,
	_oglSelectionFramebuffer* pInstanceSelectionFrameBuffer)
{
	if (pNavigatorModel == nullptr)
	{
		return;
	}

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pNavigatorModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	_prepare(
		iViewportWidth - NAVIGATION_VIEW_LENGTH, iViewportY,
		NAVIGATION_VIEW_LENGTH, NAVIGATION_VIEW_LENGTH,
		fXmin, fXmax,
		fYmin, fYmax,
		fZmin, fZmax,
		true,
		false);

	DrawInstancesFrameBuffer(pNavigatorModel, pInstanceSelectionFrameBuffer);
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawFacesFrameBuffer(CRDFModel* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (m_pSelectedInstance == nullptr)
	{
		return;
	}
	
	if ((m_pSelectedInstance->GetModel() != pModel->GetModel()) || !m_pSelectedInstance->getEnable())
	{
		return;
	}

	/*
	* Create a frame buffer
	*/
	int iWidth = 0;
	int iHeight = 0;

#ifdef _LINUX
	const wxSize szClient = m_pWnd->GetClientSize();

	iWidth = szClient.GetWidth();
	iHeight = szClient.GetHeight();
#else
	CRect rcClient;
	m_pWnd->GetClientRect(&rcClient);

	iWidth = rcClient.Width();
	iHeight = rcClient.Height();
#endif // _LINUX

	if ((iWidth < MIN_VIEW_PORT_LENGTH) || (iHeight < MIN_VIEW_PORT_LENGTH))
	{
		return;
	}

	BOOL bResult = m_pOGLContext->makeCurrent();
	VERIFY(bResult);

	m_pFaceSelectionFrameBuffer->create();

	/*
	* Selection colors
	*/
	if (m_pFaceSelectionFrameBuffer->encoding().empty())
	{
		auto& vecTriangles = m_pSelectedInstance->getTriangles();
		ASSERT(!vecTriangles.empty());

		for (int64_t iTriangle = 0; iTriangle < (int64_t)vecTriangles.size(); iTriangle++)
		{
			float fR, fG, fB;
			_i64RGBCoder::encode(iTriangle, fR, fG, fB);

			m_pFaceSelectionFrameBuffer->encoding()[iTriangle] = _color(fR, fG, fB);
		}
	}

	/*
	* Draw
	*/

	m_pFaceSelectionFrameBuffer->bind();

	glViewport(0, 0, BUFFER_SIZE, BUFFER_SIZE);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up the parameters
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	m_pOGLProgram->_enableBlinnPhongModel(false);
	m_pOGLProgram->_setTransparency(1.f);

	_oglUtils::checkForErrors();	

	GLuint iVAO = m_oglBuffers.findVAO(m_pSelectedInstance);
	if (iVAO == 0)
	{
		ASSERT(FALSE);

		return;
	}
	bool bIsNew = false;
	GLuint iIBO = m_oglBuffers.getBufferCreateNewIfNeeded(FACE_SELECTION_IBO, bIsNew);

	if (iIBO == 0)
	{
		ASSERT(FALSE);

		return;
	}

	if (bIsNew)
	{
		vector<unsigned int> vecIndices(64, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vecIndices.size(), vecIndices.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		_oglUtils::checkForErrors();
	}

	glBindVertexArray(iVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);

	auto& vecTriangles = m_pSelectedInstance->getTriangles();
	ASSERT(!vecTriangles.empty());

	for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
	{
		auto pTriangle = const_cast<_primitives*>(&vecTriangles[iTriangle]);

		vector<unsigned int> vecIndices;
		for (int64_t iIndex = pTriangle->startIndex();
			iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
			iIndex++)
		{
			vecIndices.push_back(m_pSelectedInstance->GetIndices()[iIndex]);
		}

		if (!vecIndices.empty())
		{
			auto itSelectionColor = m_pFaceSelectionFrameBuffer->encoding().find(iTriangle);
			ASSERT(itSelectionColor != m_pFaceSelectionFrameBuffer->encoding().end());

			m_pOGLProgram->_setAmbientColor(
				itSelectionColor->second.r(),
				itSelectionColor->second.g(),
				itSelectionColor->second.b());
			
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* vecIndices.size(), vecIndices.data(), GL_DYNAMIC_DRAW);			

			glDrawElementsBaseVertex(GL_TRIANGLES,
				(GLsizei)vecIndices.size(),
				GL_UNSIGNED_INT,
				(void*)(sizeof(GLuint) * 0),
				m_pSelectedInstance->VBOOffset());
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_pFaceSelectionFrameBuffer->unbind();

	_oglUtils::checkForErrors();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawPointedFace(CRDFModel* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (m_pSelectedInstance == nullptr)
	{
		return;
	}

	if ((m_pSelectedInstance->GetModel() != pModel->GetModel()) || !m_pSelectedInstance->getEnable())
	{
		return;
	}

	if (m_iPointedFace == -1)
	{
		return;
	}

	/*
	* Triangles
	*/
	auto& vecTriangles = m_pSelectedInstance->getTriangles();

	ASSERT(!vecTriangles.empty());
	ASSERT((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

	m_pOGLProgram->_enableBlinnPhongModel(false);
	m_pOGLProgram->_setAmbientColor(0.f, 1.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	_oglUtils::checkForErrors();	

	GLuint iVAO = m_oglBuffers.findVAO(m_pSelectedInstance);
	if (iVAO == 0)
	{
		ASSERT(FALSE);

		return;
	}

	bool bIsNew = false;
	GLuint iIBO = m_oglBuffers.getBufferCreateNewIfNeeded(FACE_SELECTION_IBO, bIsNew);

	if (iIBO == 0)
	{
		ASSERT(FALSE);

		return;
	}

	glBindVertexArray(iVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);	

	auto pTriangle = const_cast<_primitives*>(&vecTriangles[m_iPointedFace]);

	vector<unsigned int> vecIndices;
	for (int64_t iIndex = pTriangle->startIndex();
		iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
		iIndex++)
	{
		vecIndices.push_back(m_pSelectedInstance->GetIndices()[iIndex]);
	}

	if (!vecIndices.empty())
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vecIndices.size(), vecIndices.data(), GL_DYNAMIC_DRAW);

		glDrawElementsBaseVertex(GL_TRIANGLES,
			(GLsizei)vecIndices.size(),
			GL_UNSIGNED_INT,
			(void*)(sizeof(GLuint) * 0),
			m_pSelectedInstance->VBOOffset());
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	_oglUtils::checkForErrors();
}

void COpenGLRDFView::PointNavigatorInstance(const CPoint& point)
{
	auto pController = GetController();
	if (pController == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto pModel = pController->GetNavigatorModel();
	if (pModel == nullptr)
	{
		return;
	}

	if (m_pNavigatorSelectionFrameBuffer->isInitialized())
	{
		int iWidth = 0;
		int iHeight = 0;

#ifdef _LINUX
		m_pOGLContext->SetCurrent(*m_pWnd);

		const wxSize szClient = m_pWnd->GetClientSize();

		iWidth = szClient.GetWidth();
		iHeight = szClient.GetHeight();
#else
		BOOL bResult = m_pOGLContext->makeCurrent();
		VERIFY(bResult);

		CRect rcClient;
		m_pWnd->GetClientRect(&rcClient);

		iWidth = rcClient.Width();
		iHeight = rcClient.Height();
#endif // _LINUX

		if ((point.x > (iWidth - NAVIGATION_VIEW_LENGTH)) && (point.x < iWidth) &&
			(point.y > (iHeight - NAVIGATION_VIEW_LENGTH)) && (point.y < iHeight))
		{
			GLubyte arPixels[4];
			memset(arPixels, 0, sizeof(GLubyte) * 4);

			double dX = (double)((double)point.x - (double)(iWidth - NAVIGATION_VIEW_LENGTH)) * ((double)BUFFER_SIZE / (double)NAVIGATION_VIEW_LENGTH);
			double dY = ((double)(iHeight - (double)point.y)) * ((double)BUFFER_SIZE / (double)NAVIGATION_VIEW_LENGTH);

			m_pNavigatorSelectionFrameBuffer->bind();

			glReadPixels(
				(GLint)dX,
				(GLint)dY,
				1, 1,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				arPixels);

			m_pNavigatorSelectionFrameBuffer->unbind();

			CRDFInstance* pPointedInstance = 0;
			if (arPixels[3] != 0)
			{
				int64_t iInstanceID = _i64RGBCoder::decode(arPixels[0], arPixels[1], arPixels[2]);
				pPointedInstance = pModel->GetInstanceByID(iInstanceID);
				ASSERT(pPointedInstance != nullptr);
			}

			if (m_pNavigatorPointedInstance != pPointedInstance)
			{
				m_pNavigatorPointedInstance = pPointedInstance;

				_redraw();
			}
		}
	}
}

bool COpenGLRDFView::SelectNavigatorInstance()
{
	if (m_pNavigatorPointedInstance == nullptr)
	{
		return false;
	}

	wstring strInstanceName = m_pNavigatorPointedInstance->GetName();
	if ((strInstanceName == L"#front") || (strInstanceName == L"#front-label"))
	{
		_setView(enumView::Front); 
		
		return true;
	}
	else if ((strInstanceName == L"#back") || (strInstanceName == L"#back-label"))
	{
		_setView(enumView::Back);

		return true;
	}
	else if ((strInstanceName == L"#top") || (strInstanceName == L"#top-label"))
	{
		_setView(enumView::Top);

		return true;
	}
	else if ((strInstanceName == L"#bottom") || (strInstanceName == L"#bottom-label"))
	{
		_setView(enumView::Bottom);

		return true;
	}
	else if ((strInstanceName == L"#left") || (strInstanceName == L"#left-label"))
	{
		_setView(enumView::Left);

		return true;
	}
	else if ((strInstanceName == L"#right") || (strInstanceName == L"#right-label"))
	{
		_setView(enumView::Right);

		return true;
	}
	else if (strInstanceName == L"#front-top-left")
	{
		_setView(enumView::FrontTopLeft);

		return true;
	}
	else if (strInstanceName == L"#front-top-right")
	{
		_setView(enumView::FrontTopRight);

		return true;
	}
	else if (strInstanceName == L"#front-bottom-left")
	{
		_setView(enumView::FrontBottomLeft);

		return true;
	}
	else if (strInstanceName == L"#front-bottom-right")
	{
		_setView(enumView::FrontBottomRight);

		return true;
	}
	else if (strInstanceName == L"#back-top-left")
	{
		_setView(enumView::BackTopLeft);

		return true;
	}
	else if (strInstanceName == L"#back-top-right")
	{
		_setView(enumView::BackTopRight);

		return true;
	}
	else if (strInstanceName == L"#back-bottom-left")
	{
		_setView(enumView::BackBottomLeft);

		return true;
	}
	else if (strInstanceName == L"#back-bottom-right")
	{
		_setView(enumView::BackBottomRight);

		return true;
	}

	return false;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::OnMouseMoveEvent(UINT nFlags, const CPoint& point)
{
	auto pController = GetController();
	if (pController == nullptr)
	{
		ASSERT(FALSE);

		return;
	}

	auto pModel = pController->GetModel();
	if (pModel == nullptr)
	{
		return;
	}

	/*
	* Selection
	*/
	if (((nFlags & MK_LBUTTON) != MK_LBUTTON) && ((nFlags & MK_MBUTTON) != MK_MBUTTON) && ((nFlags & MK_RBUTTON) != MK_RBUTTON))
	{
		/*
		* Select an instance
		*/
		if (m_pInstanceSelectionFrameBuffer->isInitialized())
		{
			int iWidth = 0;
			int iHeight = 0;

#ifdef _LINUX
			m_pOGLContext->SetCurrent(*m_pWnd);

			const wxSize szClient = m_pWnd->GetClientSize();

			iWidth = szClient.GetWidth();
			iHeight = szClient.GetHeight();
#else
			BOOL bResult = m_pOGLContext->makeCurrent();
			VERIFY(bResult);

			CRect rcClient;
			m_pWnd->GetClientRect(&rcClient);

			iWidth = rcClient.Width();
			iHeight = rcClient.Height();
#endif // _LINUX

			GLubyte arPixels[4];
			memset(arPixels, 0, sizeof(GLubyte) * 4);

			double dX = (double)point.x * ((double)BUFFER_SIZE / (double)iWidth);
			double dY = ((double)iHeight - (double)point.y) * ((double)BUFFER_SIZE / (double)iHeight);

			m_pInstanceSelectionFrameBuffer->bind();

			glReadPixels(
				(GLint)dX,
				(GLint)dY,
				1, 1,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				arPixels);

			m_pInstanceSelectionFrameBuffer->unbind();

			CRDFInstance* pPointedInstance = nullptr;
			if (arPixels[3] != 0)
			{
				int64_t iInstanceID = _i64RGBCoder::decode(arPixels[0], arPixels[1], arPixels[2]);
				pPointedInstance = pModel->GetInstanceByID(iInstanceID);
				ASSERT(pPointedInstance != nullptr);
			}

			if (m_pPointedInstance != pPointedInstance)
			{
				m_pPointedInstance = pPointedInstance;

				_redraw();
			}
			
			if (m_pPointedInstance == nullptr)
			{
				PointNavigatorInstance(point);
			}
		} // if (m_pInstanceSelectionFrameBuffer->isInitialized())

		/*
		* Select a face
		*/
		if ((m_pFaceSelectionFrameBuffer->isInitialized() != 0) && 
			(m_pSelectedInstance != nullptr) && 
			m_pSelectedInstance->getEnable())
		{
			int iWidth = 0;
			int iHeight = 0;

#ifdef _LINUX
			m_pOGLContext->SetCurrent(*m_pWnd);

			const wxSize szClient = m_pWnd->GetClientSize();

			iWidth = szClient.GetWidth();
			iHeight = szClient.GetHeight();
#else
			BOOL bResult = m_pOGLContext->makeCurrent();
			VERIFY(bResult);

			CRect rcClient;
			m_pWnd->GetClientRect(&rcClient);

			iWidth = rcClient.Width();
			iHeight = rcClient.Height();
#endif // _LINUX

			GLubyte arPixels[4];
			memset(arPixels, 0, sizeof(GLubyte) * 4);

			double dX = (double)point.x * ((double)BUFFER_SIZE / (double)iWidth);
			double dY = ((double)iHeight - (double)point.y) * ((double)BUFFER_SIZE / (double)iHeight);

			m_pFaceSelectionFrameBuffer->bind();

			glReadPixels(
				(GLint)dX,
				(GLint)dY,
				1, 1,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				arPixels);

			m_pFaceSelectionFrameBuffer->unbind();

			int64_t iPointedFace = -1;
			if (arPixels[3] != 0)
			{
				iPointedFace = _i64RGBCoder::decode(arPixels[0], arPixels[1], arPixels[2]);
				ASSERT(m_pFaceSelectionFrameBuffer->encoding().find(iPointedFace) != m_pFaceSelectionFrameBuffer->encoding().end());
			}

			if (m_iPointedFace != iPointedFace)
			{
				m_iPointedFace = iPointedFace;

				_redraw();
			}
		} // if ((m_pFaceSelectionFrameBuffer->isInitialized() != 0) && ...
	} // if (((nFlags & MK_LBUTTON) != MK_LBUTTON) && ...

	if (m_pPointedInstance != nullptr)
	{		
		_showTooltip(TOOLTIP_INFORMATION, pModel->GetInstanceMetaData(m_pPointedInstance));
	}
	else
	{
		_hideTooltip();
	}

	if (m_ptPrevMousePosition.x == -1)
	{
		return;
	}

	/*
	* Rotate
	*/
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON)
	{
		_rotateMouseLButton(
			(float)point.y - (float)m_ptPrevMousePosition.y,
			(float)point.x - (float)m_ptPrevMousePosition.x);

		m_ptPrevMousePosition = point;

		return;
	}

	/*
	* Zoom
	*/
	if ((nFlags & MK_MBUTTON) == MK_MBUTTON)
	{
		_zoomMouseMButton(point.y - m_ptPrevMousePosition.y);

		m_ptPrevMousePosition = point;

		return;
	}

	/*
	* Pan
	*/
	if ((nFlags & MK_RBUTTON) == MK_RBUTTON)
	{
		CRect rcClient;
		m_pWnd->GetClientRect(&rcClient);

		_panMouseRButton(
			((float)point.x - (float)m_ptPrevMousePosition.x) / rcClient.Width(),
			((float)point.y - (float)m_ptPrevMousePosition.y) / rcClient.Height());

		m_ptPrevMousePosition = point;

		return;
	}
}

// ------------------------------------------------------------------------------------------------
// http://nehe.gamedev.net/article/using_gluunproject/16013/
void COpenGLRDFView::GetOGLPos(int iX, int iY, float fDepth, GLfloat & fX, GLfloat & fY, GLfloat & fZ) const
{
	GLint arViewport[4];
	GLdouble arModelView[16];
	GLdouble arProjection[16];
	GLfloat fWinX, fWinY, fWinZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, arModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, arProjection);
	glGetIntegerv(GL_VIEWPORT, arViewport);

	fWinX = (float)iX;
	fWinY = (float)arViewport[3] - (float)iY;

	if (fDepth == -FLT_MAX)
	{
		glReadPixels(iX, int(fWinY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &fWinZ);
	}
	else
	{
		fWinZ = fDepth;
	}

	GLdouble dX, dY, dZ;
	GLint iResult = gluUnProject(fWinX, fWinY, fWinZ, arModelView, arProjection, arViewport, &dX, &dY, &dZ);
#ifdef _LINUX
    ASSERT(iResult == GL_TRUE);
#else
    VERIFY(iResult == GL_TRUE);
#endif // _LINUX

	fX = (GLfloat)dX;
	fY = (GLfloat)dY;
	fZ = (GLfloat)dZ;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::OGLProject(GLdouble dInX, GLdouble dInY, GLdouble dInZ, GLdouble & dOutX, GLdouble & dOutY, GLdouble & dOutZ) const
{
	GLint arViewport[4];
	GLdouble arModelView[16];
	GLdouble arProjection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, arModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, arProjection);
	glGetIntegerv(GL_VIEWPORT, arViewport);

	GLint iResult = gluProject(dInX, dInY, dInZ, arModelView, arProjection, arViewport, &dOutX, &dOutY, &dOutZ);
#ifdef _LINUX
    ASSERT(iResult == GL_TRUE);
#else
    VERIFY(iResult == GL_TRUE);
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
// https://community.khronos.org/t/taking-screenshots-how-to/19154/3
void COpenGLRDFView::TakeScreenshot(unsigned char*& arPixels, unsigned int& iWidth, unsigned int& iHeight)
{
	CRect rcClient;
	m_pWnd->GetClientRect(&rcClient);

	iWidth = rcClient.Width();
	iHeight = rcClient.Height();	

	arPixels = (unsigned char*)malloc(iWidth * iHeight * 3);

	_redraw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glReadPixels(0, 0, iWidth, iHeight, GL_RGB, GL_UNSIGNED_BYTE, arPixels);

	unsigned char temp;
	for (unsigned int i = 0; i < iWidth * iHeight * 3; i += 3)
	{
		temp = arPixels[i];
		arPixels[i] = arPixels[i + 2];
		arPixels[i + 2] = temp;
	}
}

// ------------------------------------------------------------------------------------------------
// https://community.khronos.org/t/taking-screenshots-how-to/19154/3
bool COpenGLRDFView::SaveScreenshot(const wchar_t* szFilePath)
{
	unsigned char* arPixels;
	unsigned int iWidth;
	unsigned int iHeight;
	TakeScreenshot(arPixels, iWidth, iHeight);

	bool bResult = ::SaveScreenshot(arPixels, iWidth, iHeight, szFilePath);

	free(arPixels);

	return bResult;
}