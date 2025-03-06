#include "_host.h"
#include "_oglUtils.h"
#include "_instance.h"

// ************************************************************************************************
static glm::vec3 directionToEulerAngles(const glm::vec3 & direction, const glm::vec3 & upVector)
{
	glm::vec3 dir = glm::normalize(direction);
	glm::vec3 up = glm::normalize(upVector);

	// Y1
	glm::vec3 right = glm::normalize(glm::cross(up, dir));

	// ortho up
	up = glm::cross(dir, right);

	glm::mat3 rotationMatrix = glm::mat3(right, up, dir);
	glm::vec3 eulerAngles = glm::eulerAngles(glm::quat_cast(rotationMatrix));

	return eulerAngles;
}

// ************************************************************************************************
_oglRendererSettings::_oglRendererSettings()
	: m_enProjection(enumProjection::Perspective)
	, m_enRotationMode(enumRotationMode::XYZ)
	, m_fXAngle(0.f)
	, m_fYAngle(0.f)
	, m_fZAngle(0.f)
	, m_rotation(_quaterniond::toQuaternion(0., 0., 0.))
	, m_bGhostView(FALSE)
	, m_fGhostViewTransparency(1.f)
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
	, m_pSelectedInstanceMaterial(new _material())
	, m_pPointedInstanceMaterial(new _material())
	, m_bMultiSelect(false)
{
	_reset();
}

/*virtual*/ _oglRendererSettings::~_oglRendererSettings()
{
	delete m_pSelectedInstanceMaterial;
	delete m_pPointedInstanceMaterial;
}

/*virtual*/ void _oglRendererSettings::_reset()
{
	// Projection
	m_enProjection = enumProjection::Perspective;

	// Rotation
	m_enRotationMode = enumRotationMode::XYZ;
	m_fXAngle = 0.f;
	m_fYAngle = 0.f;
	m_fZAngle = 0.f;
	m_rotation = _quaterniond::toQuaternion(glm::radians(-45.), 0., glm::radians(45.));

	// UI
	m_bGhostView = FALSE;
	m_fGhostViewTransparency = .50f;
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

	// Selection
	m_pSelectedInstanceMaterial->init(
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f,
		nullptr);

	m_pPointedInstanceMaterial->init(
		.0f, .0f, 1.f,
		.0f, .0f, 1.f,
		.0f, .0f, 1.f,
		.0f, .0f, 1.f,
		1.f,
		nullptr);
}

void _oglRendererSettings::_setView(enumView enView)
{
	// Note: OpenGL/Quaternions - CW/CCW

	m_fXAngle = 0.f;
	m_fYAngle = 0.f;
	m_fZAngle = 0.f;
	m_rotation = _quaterniond::toQuaternion(0., 0., 0.);

	switch (enView)
	{
		case enumView::Front:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fXAngle = 270.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(0., 0., glm::radians(90.));
			}
			else
			{
				assert(false);
			}
		}
		break;

		case enumView::Back:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fXAngle = 90.f;
				m_fYAngle = 180.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(glm::radians(180.), 0., glm::radians(90.));
			}
			else
			{
				assert(false);
			}
		}
		break;

		case enumView::Left:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fXAngle = 270.f;
				m_fZAngle = 90.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(glm::radians(270.), 0., glm::radians(90.));
			}
			else
			{
				assert(false);
			}
		}
		break;

		case enumView::Right:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fXAngle = 270.f;
				m_fZAngle = 270.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(glm::radians(90.), 0., glm::radians(90.));
			}
			else
			{
				assert(false);
			}
		}
		break;

		case enumView::Top:
		{
			m_fXAngle = 0.f;
			m_fYAngle = 0.f;
			m_fZAngle = 0.f;
		}
		break;

		case enumView::Bottom:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fYAngle = 180.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(0., glm::radians(180.), 0.);
			}
			else
			{
				assert(false);
			}
		}
		break;

		case enumView::FrontTopLeft:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fXAngle = 315.f;
				m_fZAngle = 45.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(glm::radians(-45.), 0., glm::radians(-315.));
			}
			else
			{
				assert(false);
			}
		}
		break;

		case enumView::FrontTopRight:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fXAngle = 315.f;
				m_fZAngle = 315.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(glm::radians(-315.), 0., glm::radians(-315.));
			}
			else
			{
				assert(false);
			}
		}
		break;

		case enumView::FrontBottomLeft:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fXAngle = 225.f;
				m_fZAngle = 45.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(glm::radians(-45.), 0., glm::radians(-225.));
			}
			else
			{
				assert(false);
			}
		}
		break;

		case enumView::FrontBottomRight:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fXAngle = 225.f;
				m_fZAngle = 315.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(glm::radians(-315.), 0., glm::radians(-225.));
			}
			else
			{
				assert(false);
			}
		}
		break;

		case enumView::BackTopLeft:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fXAngle = 315.f;
				m_fZAngle = 225.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(glm::radians(-225.), 0., glm::radians(-315.));
			}
			else
			{
				assert(false);
			}
		}
		break;

		case enumView::BackTopRight:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fXAngle = 315.f;
				m_fZAngle = 135.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(glm::radians(-135.), 0., glm::radians(-315.f));
			}
			else
			{
				assert(false);
			}
		}
		break;

		case enumView::BackBottomLeft:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fXAngle = 225.f;
				m_fZAngle = 225.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(glm::radians(-225.), 0., glm::radians(-225.));
			}
			else
			{
				assert(false);
			}
		}
		break;

		case enumView::BackBottomRight:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fXAngle = 225.f;
				m_fZAngle = 135.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(glm::radians(-135.), 0., glm::radians(-225.));
			}
			else
			{
				assert(false);
			}
		}
		break;

		case enumView::Isometric:
		{
			if (m_enRotationMode == enumRotationMode::XY)
			{
				m_fXAngle = 315.f;
				m_fYAngle = 0.f;
				m_fZAngle = 45.f;
			}
			else if (m_enRotationMode == enumRotationMode::XYZ)
			{
				m_rotation = _quaterniond::toQuaternion(glm::radians(-45.), 0., glm::radians(45.));
			}
			else
			{
				assert(false);
			}
		}
		break;

		default:
		{
			assert(false);
		}
		break;
	} // switch (enView)
}

/*virtual*/ void _oglRendererSettings::_oglRendererSettings::saveSetting(const string& strName, const string& strValue)
{
	auto pController = _getController();
	if (pController != nullptr)
	{
		pController->getSettingsStorage()->setSetting(strName, strValue);
	}
}

/*virtual*/ string _oglRendererSettings::loadSetting(const string& strName)
{
	auto pController = _getController();
	if (pController != nullptr)
	{
		return pController->getSettingsStorage()->getSetting(strName);
	}

	return "";
}

/*virtual*/ void _oglRendererSettings::loadSettings()
{
	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bGhostView);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_bGhostView = strValue == "TRUE";
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_fGhostViewTransparency);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_fGhostViewTransparency = (float)atof(strValue.c_str());
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowFaces);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_bShowFaces = strValue == "TRUE";
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_strCullFaces);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_strCullFaces = CA2W(strValue.c_str());
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowFacesPolygons);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_bShowFacesPolygons = strValue == "TRUE";
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowConceptualFacesPolygons);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_bShowConceptualFacesPolygons = strValue == "TRUE";
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowLines);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_bShowLines = strValue == "TRUE";
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowPoints);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_bShowPoints = strValue == "TRUE";
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowBoundingBoxes);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_bShowBoundingBoxes = strValue == "TRUE";
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowNormalVectors);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_bShowNormalVectors = strValue == "TRUE";
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowTangenVectors);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_bShowTangenVectors = strValue == "TRUE";
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowBiNormalVectors);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_bShowBiNormalVectors = strValue == "TRUE";
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bScaleVectors);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_bScaleVectors = strValue == "TRUE";
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowCoordinateSystem);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_bShowCoordinateSystem = strValue == "TRUE";
		}
	}

	{
		string strSettingName(typeid(this).raw_name());
		strSettingName += NAMEOFVAR(m_bShowNavigator);

		string strValue = loadSetting(strSettingName);
		if (!strValue.empty())
		{
			m_bShowNavigator = strValue == "TRUE";
		}
	}
}

enumProjection _oglRendererSettings::_getProjection() const
{
	return m_enProjection;
}

void _oglRendererSettings::_setProjection(enumProjection enProjection)
{
	m_enProjection = enProjection;

	_setView(enumView::Isometric);
}

enumRotationMode _oglRendererSettings::_getRotationMode() const
{
	return m_enRotationMode;
}

void _oglRendererSettings::_setRotationMode(enumRotationMode enRotationMode)
{
	m_enRotationMode = enRotationMode;

	_setView(enumView::Isometric);
}

void _oglRendererSettings::setShowFaces(BOOL bValue)
{
	m_bShowFaces = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowFaces);

	saveSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL _oglRendererSettings::getShowFaces()
{
	return m_bShowFaces;
}

void _oglRendererSettings::setGhostView(BOOL bValue)
{
	m_bGhostView = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bGhostView);

	saveSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL _oglRendererSettings::getGhostView() const
{
	return m_bGhostView;
}

void _oglRendererSettings::setGhostViewTransparency(GLfloat fTransparency)
{
	m_fGhostViewTransparency = fTransparency;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_fGhostViewTransparency);

	saveSetting(strSettingName, to_string(m_fGhostViewTransparency));
}

GLfloat _oglRendererSettings::getGhostViewTransparency() const
{
	return m_fGhostViewTransparency;
}

void _oglRendererSettings::setCullFacesMode(LPCTSTR szMode)
{
	m_strCullFaces = szMode;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_strCullFaces);

	saveSetting(strSettingName, (LPCSTR)CW2A(szMode));
}

LPCTSTR _oglRendererSettings::getCullFacesMode() const
{
	return m_strCullFaces;
}

void _oglRendererSettings::setShowFacesPolygons(BOOL bValue)
{
	m_bShowFacesPolygons = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowFacesPolygons);

	saveSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL _oglRendererSettings::getShowFacesPolygons() const
{
	return m_bShowFacesPolygons;
}

void _oglRendererSettings::setShowConceptualFacesPolygons(BOOL bValue)
{
	m_bShowConceptualFacesPolygons = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowConceptualFacesPolygons);

	saveSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL _oglRendererSettings::getShowConceptualFacesPolygons() const
{
	return m_bShowConceptualFacesPolygons;
}

void _oglRendererSettings::setShowLines(BOOL bValue)
{
	m_bShowLines = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowLines);

	saveSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL _oglRendererSettings::getShowLines() const
{
	return m_bShowLines;
}

void _oglRendererSettings::setLineWidth(GLfloat fWidth)
{
	m_fLineWidth = fWidth;
}

GLfloat _oglRendererSettings::getLineWidth() const
{
	return m_fLineWidth;
}

void _oglRendererSettings::setShowPoints(BOOL bValue)
{
	m_bShowPoints = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowPoints);

	saveSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL _oglRendererSettings::getShowPoints() const
{
	return m_bShowPoints;
}

void _oglRendererSettings::setPointSize(GLfloat fSize)
{
	m_fPointSize = fSize;
}

GLfloat _oglRendererSettings::getPointSize() const
{
	return m_fPointSize;
}

void _oglRendererSettings::setShowBoundingBoxes(BOOL bValue)
{
	m_bShowBoundingBoxes = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowBoundingBoxes);

	saveSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL _oglRendererSettings::getShowBoundingBoxes() const
{
	return m_bShowBoundingBoxes;
}

void _oglRendererSettings::setShowNormalVectors(BOOL bValue)
{
	m_bShowNormalVectors = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowNormalVectors);

	saveSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL _oglRendererSettings::getShowNormalVectors() const
{
	return m_bShowNormalVectors;
}

void _oglRendererSettings::setShowTangentVectors(BOOL bValue)
{
	m_bShowTangenVectors = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowTangenVectors);

	saveSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL _oglRendererSettings::getShowTangentVectors() const
{
	return m_bShowTangenVectors;
}

void _oglRendererSettings::setShowBiNormalVectors(BOOL bValue)
{
	m_bShowBiNormalVectors = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowBiNormalVectors);

	saveSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL _oglRendererSettings::getShowBiNormalVectors() const
{
	return m_bShowBiNormalVectors;
}

void _oglRendererSettings::setScaleVectors(BOOL bValue)
{
	m_bScaleVectors = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bScaleVectors);

	saveSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL _oglRendererSettings::getScaleVectors() const
{
	return m_bScaleVectors;
}

void _oglRendererSettings::setShowCoordinateSystem(BOOL bValue)
{
	m_bShowCoordinateSystem = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowCoordinateSystem);

	saveSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL _oglRendererSettings::getShowCoordinateSystem() const
{
	return m_bShowCoordinateSystem;
}

void _oglRendererSettings::setShowNavigator(BOOL bValue)
{
	m_bShowNavigator = bValue;

	string strSettingName(typeid(this).raw_name());
	strSettingName += NAMEOFVAR(m_bShowNavigator);

	saveSetting(strSettingName, bValue ? "TRUE" : "FALSE");
}

BOOL _oglRendererSettings::getShowNavigator() const
{
	return m_bShowNavigator;
}

void _oglRendererSettings::setSelectedInstanceMaterial(float fR, float fG, float fB, float fTransparency)
{
	m_pSelectedInstanceMaterial->init(
		fR, fG, fB,
		fR, fG, fB,
		fR, fG, fB,
		fR, fG, fB,
		fTransparency,
		nullptr);
}

// ************************************************************************************************
_oglRenderer::_oglRenderer()
	: _oglRendererSettings()
	, m_pWnd(nullptr)
	, m_toolTipCtrl()
	, m_pOGLContext(nullptr)
	, m_pOGLProgram(nullptr)
	, m_pVertexShader(nullptr)
	, m_pFragmentShader(nullptr)
	, m_matModelView()
	, m_oglBuffers()
	, m_fXmin(-1.f)
	, m_fXmax(1.f)
	, m_fYmin(-1.f)
	, m_fYmax(1.f)
	, m_fZmin(-1.f)
	, m_fZmax(1.f)
	, m_fZoomMin(-1.f)
	, m_fZoomMax(1.f)
	, m_fZoomInterval(2.f)
	, m_fPanXMin(-1.f)
	, m_fPanXMax(1.f)
	, m_fPanXInterval(2.f)
	, m_fPanYMin(-1.f)
	, m_fPanYMax(1.f)
	, m_fPanYInterval(2.f)
	, m_fXTranslation(.0f)
	, m_fYTranslation(.0f)
	, m_fZTranslation(DEFAULT_TRANSLATION)
	, m_fScaleFactor(2.f)
	, m_fScaleFactorMin(0.f)
	, m_fScaleFactorMax(2.f)
	, m_fScaleFactorInterval(2.f)
	, m_bCameraSettings(false)
	, m_vecViewPoint({ 0., 0, 0. })
	, m_vecDirection({ 0., 0, 0. })
	, m_vecUpVector({ 0., 0, 0. })
	, m_dFieldOfView(45.)
	, m_dAspectRatio(1.)
{
	_setView(enumView::Isometric);
}

/*virtual*/ _oglRenderer::~_oglRenderer()
{
}

void _oglRenderer::_initialize(CWnd* pWnd,
	int iSamples,
	int iVertexShader,
	int iFragmentShader,
	int iResourceType,
	bool bSupportsTexture)
{
	m_pWnd = pWnd;
	assert(m_pWnd != nullptr);

	m_toolTipCtrl.Create(m_pWnd, WS_POPUP | WS_CLIPSIBLINGS | TTS_NOANIMATE | TTS_NOFADE | TTS_ALWAYSTIP);
	m_toolTipCtrl.SetDelayTime(TTDT_INITIAL, 0);
	m_toolTipCtrl.SetDelayTime(TTDT_AUTOPOP, 30000);
	m_toolTipCtrl.SetDelayTime(TTDT_RESHOW, 30000);
	m_toolTipCtrl.Activate(TRUE);
	m_toolTipCtrl.AddTool(m_pWnd, _T(""));

	m_pOGLContext = new _oglContext(*(m_pWnd->GetDC()), iSamples);
	m_pOGLContext->makeCurrent();

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram = new _oglBlinnPhongProgram(bSupportsTexture);
#else
	m_pOGLProgram = new _oglPerPixelProgram(bSupportsTexture);
#endif
	m_pVertexShader = new _oglShader(GL_VERTEX_SHADER);
	m_pFragmentShader = new _oglShader(GL_FRAGMENT_SHADER);

	if (!m_pVertexShader->load(iVertexShader, iResourceType))
	{
		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), _T("Vertex shader loading error!"), _T("Error"), MB_ICONERROR | MB_OK);

		PostQuitMessage(0);
	}

	if (!m_pFragmentShader->load(iFragmentShader, iResourceType))
	{
		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), _T("Fragment shader loading error!"), _T("Error"), MB_ICONERROR | MB_OK);

		PostQuitMessage(0);
	}

	if (!m_pVertexShader->compile())
	{
		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), _T("Vertex shader compiling error!"), _T("Error"), MB_ICONERROR | MB_OK);

		PostQuitMessage(0);
	}

	if (!m_pFragmentShader->compile())
	{
		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), _T("Fragment shader compiling error!"), _T("Error"), MB_ICONERROR | MB_OK);

		PostQuitMessage(0);
	}

	m_pOGLProgram->_attachShader(m_pVertexShader);
	m_pOGLProgram->_attachShader(m_pFragmentShader);

	glBindFragDataLocation(m_pOGLProgram->_getID(), 0, "FragColor");

	if (!m_pOGLProgram->_link())
	{
		::MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), _T("Program linking error!"), _T("Error"), MB_ICONERROR | MB_OK);
	}

	m_matModelView = glm::identity<glm::mat4>();
}

void _oglRenderer::_destroy()
{
	m_oglBuffers.clear();

	if (m_pOGLContext != nullptr)
	{
		m_pOGLContext->makeCurrent();
	}

	if (m_pOGLProgram != nullptr)
	{
		m_pOGLProgram->_detachShader(m_pVertexShader);
		m_pOGLProgram->_detachShader(m_pFragmentShader);

		delete m_pOGLProgram;
		m_pOGLProgram = nullptr;
	}

	delete m_pVertexShader;
	m_pVertexShader = nullptr;

	delete m_pFragmentShader;
	m_pFragmentShader = nullptr;

	delete m_pOGLContext;
	m_pOGLContext = nullptr;
}

void _oglRenderer::_prepare(
	int iViewportX, int iViewportY,
	int iViewportWidth, int iViewportHeight,
	float fXmin, float fXmax,
	float fYmin, float fYmax,
	float fZmin, float fZmax,
	bool bClearScene,
	bool bApplyTranslations)
{
	m_fXmin = fXmin;
	m_fXmax = fXmax;
	m_fYmin = fYmin;
	m_fYmax = fYmax;
	m_fZmin = fZmin;
	m_fZmax = fZmax;

	float fBoundingSphereDiameter = m_fXmax - m_fXmin;
	fBoundingSphereDiameter = fmax(fBoundingSphereDiameter, m_fYmax - m_fYmin);
	fBoundingSphereDiameter = fmax(fBoundingSphereDiameter, m_fZmax - m_fZmin);

	// Zoom
	m_fZoomMin = -(m_fZmin + m_fZmax) / 2.f;
	m_fZoomMin -= (fBoundingSphereDiameter * 4.f);
	m_fZoomMax = ((m_fZmin + m_fZmax) / 2.f);
	m_fZoomInterval = m_fZoomMax - m_fZoomMin;

	// Pan X
	m_fPanXMin = -(m_fXmax - m_fXmin) / 2.f;
	m_fPanXMin -= fBoundingSphereDiameter * 1.25f;
	m_fPanXMax = (m_fXmax - m_fXmin) / 2.f;
	m_fPanXMax += fBoundingSphereDiameter * 1.25f;
	m_fPanXInterval = m_fPanXMax - m_fPanXMin;

	// Pan Y
	m_fPanYMin = -(m_fYmax - m_fYmin) / 2.f;
	m_fPanYMin -= fBoundingSphereDiameter * .75f;
	m_fPanYMax = (m_fYmax - m_fYmin) / 2.f;
	m_fPanYMax += fBoundingSphereDiameter * .75f;
	m_fPanYInterval = abs(m_fPanYMax - m_fPanYMin);

	// Scale (Orthographic)
	m_fScaleFactorMin = 0.f;
	m_fScaleFactorMax = fBoundingSphereDiameter;
	m_fScaleFactorInterval = abs(m_fScaleFactorMax - m_fScaleFactorMin);

	BOOL bResult = m_pOGLContext->makeCurrent();
	VERIFY(bResult);

#ifdef _ENABLE_OPENGL_DEBUG
	m_pOGLContext->enableDebug();
#endif

	m_pOGLProgram->_use();

	glViewport(iViewportX, iViewportY, iViewportWidth, iViewportHeight);

	if (bClearScene)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(iViewportX, iViewportY, iViewportWidth, iViewportHeight);

		glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_SCISSOR_TEST);
	}

	// Set up the parameters
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Projection Matrix
	// fovY     - Field of vision in degrees in the y direction
	// aspect   - Aspect ratio of the viewport
	// zNear    - The near clipping distance
	// zFar     - The far clipping distance
	GLdouble fovY = m_dFieldOfView;
	if (!m_bCameraSettings)
	{
		m_dAspectRatio = (GLdouble)iViewportWidth / (GLdouble)iViewportHeight;
	}
	GLdouble aspect = m_dAspectRatio;

	GLdouble zNear = min(abs((double)fXmin), abs((double)fYmin));
	zNear = min(zNear, abs((double)fZmin));
	if (zNear != 0.)
	{
		zNear /= 25.;
	}
	else
	{
		zNear = fBoundingSphereDiameter * .1;
	}

	GLdouble zFar = 100.;
	GLdouble fH = tan(fovY / 360 * M_PI) * zNear;
	GLdouble fW = fH * aspect;

	// Projection
	switch (m_enProjection)
	{
	case enumProjection::Perspective:
	{
		glm::mat4 matProjection = glm::frustum<GLdouble>(-fW, fW, -fH, fH, zNear, zFar);
		m_pOGLProgram->_setProjectionMatrix(matProjection);
	}
	break;

	case enumProjection::Orthographic:
	{
		glm::mat4 matProjection = glm::ortho<GLdouble>(-m_fScaleFactor, m_fScaleFactor, -m_fScaleFactor, m_fScaleFactor, zNear, zFar);
		m_pOGLProgram->_setProjectionMatrix(matProjection);
	}
	break;

	default:
	{
		assert(false);
	}
	break;
	} // switch (m_enProjection)

	// Model-View Matrix
	m_matModelView = glm::identity<glm::mat4>();

	if (bApplyTranslations)
	{
		m_matModelView = glm::translate(m_matModelView, glm::vec3(m_fXTranslation, m_fYTranslation, m_fZTranslation));
	}
	else
	{
		m_matModelView = glm::translate(m_matModelView, glm::vec3(0.f, 0.f, DEFAULT_TRANSLATION));
	}

	float fXTranslation = fXmin;
	fXTranslation += (fXmax - fXmin) / 2.f;
	fXTranslation = -fXTranslation;

	float fYTranslation = fYmin;
	fYTranslation += (fYmax - fYmin) / 2.f;
	fYTranslation = -fYTranslation;

	float fZTranslation = fZmin;
	fZTranslation += (fZmax - fZmin) / 2.f;
	fZTranslation = -fZTranslation;

	m_matModelView = glm::translate(m_matModelView, glm::vec3(-fXTranslation, -fYTranslation, -fZTranslation));

	if (m_enRotationMode == enumRotationMode::XY)
	{
		m_matModelView = glm::rotate(m_matModelView, glm::radians(m_fXAngle), glm::vec3(1.f, 0.f, 0.f));
		m_matModelView = glm::rotate(m_matModelView, glm::radians(m_fYAngle), glm::vec3(0.f, 1.f, 0.f));
		m_matModelView = glm::rotate(m_matModelView, glm::radians(m_fZAngle), glm::vec3(0.f, 0.f, 1.f));
	}
	else if (m_enRotationMode == enumRotationMode::XYZ)
	{
		// Apply rotation...
		_quaterniond rotation = _quaterniond::toQuaternion(glm::radians(m_fZAngle), glm::radians(m_fYAngle), glm::radians(m_fXAngle));
		m_rotation.cross(rotation);

		// ... and reset
		m_fXAngle = m_fYAngle = m_fZAngle = 0.f;

		const double* pRotationMatrix = m_rotation.toMatrix();
		glm::mat4 matTransformation = glm::make_mat4((GLdouble*)pRotationMatrix);
		delete pRotationMatrix;

		m_matModelView = m_matModelView * matTransformation;
	}
	else
	{
		assert(false);
	}

	m_matModelView = glm::translate(m_matModelView, glm::vec3(fXTranslation, fYTranslation, fZTranslation));

	m_pOGLProgram->_setModelViewMatrix(m_matModelView);
#ifdef _BLINN_PHONG_SHADERS
	glm::mat4 matNormal = m_matModelView;
	matNormal = glm::inverse(matNormal);
	matNormal = glm::transpose(matNormal);
	m_pOGLProgram->_setNormalMatrix(matNormal);

	// Model
	m_pOGLProgram->_enableBlinnPhongModel(true);
#else
	m_pOGLProgram->_setNormalMatrix(m_matModelView);

	// Model
	m_pOGLProgram->_enableLighting(true);
#endif
}

void _oglRenderer::_rotateMouseLButton(float fXAngle, float fYAngle)
{
	if (m_enRotationMode == enumRotationMode::XY)
	{
		if (abs(fXAngle) >= abs(fYAngle))
		{
			fYAngle = 0.;
		}
		else
		{
			if (abs(fYAngle) >= abs(fXAngle))
			{
				fXAngle = 0.;
			}
		}

		_rotate(
			fXAngle * ROTATION_SPEED,
			fYAngle * ROTATION_SPEED);
	}
	else if (m_enRotationMode == enumRotationMode::XYZ)
	{
		_rotate(
			-fXAngle * ROTATION_SPEED,
			-fYAngle * ROTATION_SPEED);
	}
	else
	{
		assert(false);
	}
}

void _oglRenderer::_zoomMouseMButton(LONG lDelta)
{
	if (lDelta == 0)
	{
		return;
	}

	switch (m_enProjection)
	{
		case enumProjection::Perspective:
		{
			_zoom(
				lDelta > 0 ?
				-abs(m_fZoomInterval * ZOOM_SPEED_MOUSE) :
				abs(m_fZoomInterval * ZOOM_SPEED_MOUSE));
		}
		break;

		case enumProjection::Orthographic:
		{
			_zoom(
				lDelta > 0 ?
				abs(m_fScaleFactorInterval * ZOOM_SPEED_MOUSE) :
				-abs(m_fScaleFactorInterval * ZOOM_SPEED_MOUSE));
		}
		break;

		default:
		{
			assert(false);
		}
		break;
	} // switch (m_enProjection)
}

void _oglRenderer::_panMouseRButton(float fX, float fY)
{
	_pan(
		m_fPanXInterval * fX,
		m_fPanYInterval * -fY);
}

void _oglRenderer::_setCameraSettings(
	bool bPerspective,
	double arViewPoint[3],
	double arDirection[3],
	double arUpVector[3],
	double dViewToWorldScale,
	double dFieldOfView,
	double dAspectRatio,
	double dLengthConversionFactor)
{
	_reset();

	auto pWorld = _getController()->getModel();
	_vector3d vecVertexBufferOffset;
	GetVertexBufferOffset(pWorld->getOwlModel(), (double*)&vecVertexBufferOffset);
	auto dScaleFactor = pWorld->getOriginalBoundingSphereDiameter() / 2.;

	m_bCameraSettings = true;

	m_enProjection = bPerspective ? enumProjection::Perspective : enumProjection::Orthographic;	

	m_vecViewPoint = { arViewPoint[0] / dLengthConversionFactor, arViewPoint[1] / dLengthConversionFactor, arViewPoint[2] / dLengthConversionFactor };
	m_vecViewPoint.x = (m_vecViewPoint.x + vecVertexBufferOffset.x) / dScaleFactor;
	m_vecViewPoint.y = (m_vecViewPoint.y + vecVertexBufferOffset.y) / dScaleFactor;
	m_vecViewPoint.z = (m_vecViewPoint.z + vecVertexBufferOffset.z) / dScaleFactor;

	m_vecDirection = { arDirection[0], arDirection[1], arDirection[2] };

	m_vecUpVector = { arUpVector[0], arUpVector[1], arUpVector[2] };

	m_fScaleFactor = (float)dViewToWorldScale;

	m_dFieldOfView = dFieldOfView;
	m_dAspectRatio = dAspectRatio;

	// I. Rotation and View point
	glm::mat4 matRotation = glm::lookAt(
		glm::vec3(m_vecViewPoint.x, m_vecViewPoint.y, -m_vecViewPoint.z),
		glm::vec3(m_vecDirection.x, m_vecDirection.y, m_vecDirection.z),
		glm::vec3(m_vecUpVector.x, m_vecUpVector.y, m_vecUpVector.z));
	glm::quat quatRotation = glm::quat_cast(matRotation);
	m_rotation = _quaterniond(quatRotation.w, quatRotation.x, quatRotation.y, quatRotation.z);

	// II. Only rotation; View point is not set
	/*glm::vec3 eulerAngles = directionToEulerAngles(
		glm::vec3(m_vecDirection.x, m_vecDirection.y, m_vecDirection.z),
		glm::vec3(m_vecUpVector.x, m_vecUpVector.y, m_vecUpVector.z));
	m_rotation = _quaterniond::toQuaternion(eulerAngles.x, eulerAngles.y, eulerAngles.z);*/

	_redraw();
}

void _oglRenderer::_getCameraSettings(
	bool& bPerspective,
	double arViewPoint[3],
	double arDirection[3],
	double arUpVector[3],
	double& dViewToWorldScale,
	double& dFieldOfView,
	double& dAspectRatio,
	double dLengthConversionFactor)
{
	auto pWorld = _getController()->getModel();
	_vector3d vecVertexBufferOffset;
	GetVertexBufferOffset(pWorld->getOwlModel(), (double*)&vecVertexBufferOffset);
	auto dScaleFactor = pWorld->getOriginalBoundingSphereDiameter() / 2.;	

	bPerspective = m_enProjection == enumProjection::Perspective;

	arViewPoint[0] = -m_matModelView[3][0] * dScaleFactor;
	arViewPoint[1] = -m_matModelView[3][1] * dScaleFactor;
	arViewPoint[2] = -m_matModelView[3][2] * dScaleFactor;
	arViewPoint[0] -= vecVertexBufferOffset.x;
	arViewPoint[1] -= vecVertexBufferOffset.y;
	arViewPoint[2] -= vecVertexBufferOffset.z;
	arViewPoint[0] *= dLengthConversionFactor;
	arViewPoint[1] *= dLengthConversionFactor;
	arViewPoint[2] *= dLengthConversionFactor;

	arDirection[0] = -m_matModelView[2][0];
	arDirection[1] = -m_matModelView[2][1];
	arDirection[2] = -m_matModelView[2][2];

	arUpVector[0] = m_matModelView[0][0];
	arUpVector[1] = m_matModelView[0][1];
	arUpVector[2] = m_matModelView[0][2];

	dViewToWorldScale = m_enProjection == enumProjection::Perspective ? 0. : m_fScaleFactor;

	dFieldOfView = 45.0;
	dAspectRatio = 1.;
}

void _oglRenderer::_rotate(float fXAngle, float fYAngle)
{
	m_fXAngle += fXAngle * (180.f / (float)M_PI);
	if (m_fXAngle > 360.f)
	{
		m_fXAngle -= 360.f;
	}
	else if (m_fXAngle < -360.f)
	{
		m_fXAngle += 360.f;
	}

	m_fYAngle += fYAngle * (180.f / (float)M_PI);
	if (m_fYAngle > 360.f)
	{
		m_fYAngle = m_fYAngle - 360.f;
	}
	else if (m_fYAngle < -360.f)
	{
		m_fYAngle += 360.f;
	}

	_redraw();
}

void _oglRenderer::_zoom(float fZTranslation)
{
	switch (m_enProjection)
	{
		case enumProjection::Perspective:
		{
			float fNewZTranslation = m_fZTranslation + fZTranslation;
			//#todo
			/*if ((fNewZTranslation >= m_fZoomMax) ||
				(fNewZTranslation <= m_fZoomMin))
			{
				return;
			}*/

			m_fZTranslation = fNewZTranslation;
		}
		break;

		case enumProjection::Orthographic:
		{
			float fNewScaleFactor = m_fScaleFactor + fZTranslation;
			if ((fNewScaleFactor >= m_fScaleFactorMax) ||
				(fNewScaleFactor <= m_fScaleFactorMin))
			{
				return;
			}

			m_fScaleFactor = fNewScaleFactor;
		}
		break;

		default:
		{
			assert(false);
		}
		break;
	} // switch (m_enProjection)

	_redraw();
}

void _oglRenderer::_pan(float fX, float fY)
{
	bool bRedraw = false;

	float fNewXTranslation = m_fXTranslation + fX;
	if ((fNewXTranslation < m_fPanXMax) &&
		(fNewXTranslation > m_fPanXMin))
	{
		m_fXTranslation += fX;

		bRedraw = true;
	}

	float fNewYTranslation = m_fYTranslation + fY;
	if ((fNewYTranslation < m_fPanYMax) &&
		(fNewYTranslation > m_fPanYMin))
	{
		m_fYTranslation += fY;

		bRedraw = true;
	}

	if (bRedraw)
	{
		_redraw();
	}
}

/*virtual*/ void _oglRenderer::_onMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/)
{
	switch (m_enProjection)
	{
		case enumProjection::Perspective:
		{
			_zoom(
				zDelta < 0 ?
				-abs(m_fZoomInterval * ZOOM_SPEED_MOUSE_WHEEL) :
				abs(m_fZoomInterval * ZOOM_SPEED_MOUSE_WHEEL));
		}
		break;

		case enumProjection::Orthographic:
		{
			_zoom(zDelta < 0 ?
				-abs(m_fScaleFactorInterval * ZOOM_SPEED_MOUSE_WHEEL) :
				abs(m_fScaleFactorInterval * ZOOM_SPEED_MOUSE_WHEEL));
		}
		break;

		default:
		{
			assert(false);
		}
		break;
	} // switch (m_enProjection)
}

/*virtual*/ void _oglRenderer::_onKeyUp(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{
	CRect rcClient;
	m_pWnd->GetClientRect(&rcClient);

	switch (nChar)
	{
		case VK_UP:
		{
			_pan(
				0.f,
				PAN_SPEED_KEYS * m_fPanYInterval);
		}
		break;

		case VK_DOWN:
		{
			_pan(
				0.f,
				-PAN_SPEED_KEYS * m_fPanYInterval);
		}
		break;

		case VK_LEFT:
		{
			_pan(
				-PAN_SPEED_KEYS * m_fPanXInterval,
				0.f);
		}
		break;

		case VK_RIGHT:
		{
			_pan(
				PAN_SPEED_KEYS * m_fPanXInterval,
				0.f);
		}
		break;

		case VK_PRIOR:
		{
			_zoom(abs(m_fZoomInterval * ZOOM_SPEED_KEYS));
		}
		break;

		case VK_NEXT:
		{
			_zoom(-abs(m_fZoomInterval * ZOOM_SPEED_KEYS));
		}
		break;
	} // switch (nChar)
}

void _oglRenderer::_reset()
{
	_oglRendererSettings::_reset();

	float fWorldXmin = FLT_MAX;
	float fWorldXmax = -FLT_MAX;
	float fWorldYmin = FLT_MAX;
	float fWorldYmax = -FLT_MAX;
	float fWorldZmin = FLT_MAX;
	float fWorldZmax = -FLT_MAX;
	_getController()->getWorldDimensions(fWorldXmin, fWorldXmax, fWorldYmin, fWorldYmax, fWorldZmin, fWorldZmax);

	float fWorldBoundingSphereDiameter = fWorldXmax - fWorldXmin;
	fWorldBoundingSphereDiameter = fmax(fWorldBoundingSphereDiameter, fWorldYmax - fWorldYmin);
	fWorldBoundingSphereDiameter = fmax(fWorldBoundingSphereDiameter, fWorldZmax - fWorldZmin);

	m_fXTranslation = fWorldXmin;
	m_fXTranslation += (fWorldXmax - fWorldXmin) / 2.f;
	m_fXTranslation = -m_fXTranslation;

	m_fYTranslation = fWorldYmin;
	m_fYTranslation += (fWorldYmax - fWorldYmin) / 2.f;
	m_fYTranslation = -m_fYTranslation;

	m_fZTranslation = fWorldZmin;
	m_fZTranslation += (fWorldZmax - fWorldZmin) / 2.f;
	m_fZTranslation = -m_fZTranslation;
	m_fZTranslation -= (fWorldBoundingSphereDiameter * 2.f);

	m_fScaleFactor = fWorldBoundingSphereDiameter;

	m_bCameraSettings = false;
	m_vecViewPoint = { 0., 0, 0. };
	m_vecDirection = { 0., 0, 0. };
	m_vecUpVector = { 0., 0, 0. };
	m_dFieldOfView = 45.;
	m_dAspectRatio = 1.;
}

void _oglRenderer::_showTooltip(LPCTSTR szTitle, LPCTSTR szText)
{
	assert(m_toolTipCtrl.GetToolCount() <= 1);

	if (m_toolTipCtrl.GetToolCount() == 1)
	{
		CToolInfo toolInfo;
		m_toolTipCtrl.GetToolInfo(toolInfo, m_pWnd);

		if (CString(toolInfo.lpszText) != szText)
		{
			m_toolTipCtrl.SetTitle(0, szTitle);

			toolInfo.lpszText = (LPWSTR)szText;
			m_toolTipCtrl.SetToolInfo(&toolInfo);
		}
		else
		{
			CPoint ptCursor;
			GetCursorPos(&ptCursor);

			m_toolTipCtrl.SetWindowPos(
				NULL,
				ptCursor.x + 10,
				ptCursor.y + 10,
				0,
				0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

			return;
		}
	} // if (m_toolTipCtrl.GetToolCount() == 1)
	else
	{
		m_toolTipCtrl.SetTitle(0, szTitle);
		m_toolTipCtrl.AddTool(m_pWnd, szText);
	}

	m_toolTipCtrl.Popup();
}

void _oglRenderer::_hideTooltip()
{
	assert(m_toolTipCtrl.GetToolCount() <= 1);

	if (m_toolTipCtrl.GetToolCount() == 1)
	{
		m_toolTipCtrl.DelTool(m_pWnd, 0);
	}
}

// ************************************************************************************************
_oglView::_oglView()
	: _oglRenderer()
	, _view()
	, m_mapUserDefinedMaterials()
	, m_ptStartMousePosition(-1, -1)
	, m_ptPrevMousePosition(-1, -1)
	, m_pSelectInstanceFrameBuffer(new _oglSelectionFramebuffer())
	, m_pPointedInstance(nullptr)	
	, m_tmShowTooltip(clock())
{	
}

/*virtual*/ _oglView::~_oglView()
{
	getController()->unRegisterView(this);

	_destroy();

	delete m_pSelectInstanceFrameBuffer;

	removeUserDefinedMaterials();
}

/*virtual*/ void _oglView::onWorldDimensionsChanged() /*override*/
{
	float fWorldXmin = FLT_MAX;
	float fWorldXmax = -FLT_MAX;
	float fWorldYmin = FLT_MAX;
	float fWorldYmax = -FLT_MAX;
	float fWorldZmin = FLT_MAX;
	float fWorldZmax = -FLT_MAX;
	getController()->getWorldDimensions(fWorldXmin, fWorldXmax, fWorldYmin, fWorldYmax, fWorldZmin, fWorldZmax);

	float fWorldBoundingSphereDiameter = fWorldXmax - fWorldXmin;
	fWorldBoundingSphereDiameter = fmax(fWorldBoundingSphereDiameter, fWorldYmax - fWorldYmin);
	fWorldBoundingSphereDiameter = fmax(fWorldBoundingSphereDiameter, fWorldZmax - fWorldZmin);

	m_fXTranslation = fWorldXmin;
	m_fXTranslation += (fWorldXmax - fWorldXmin) / 2.f;
	m_fXTranslation = -m_fXTranslation;

	m_fYTranslation = fWorldYmin;
	m_fYTranslation += (fWorldYmax - fWorldYmin) / 2.f;
	m_fYTranslation = -m_fYTranslation;

	m_fZTranslation = fWorldZmin;
	m_fZTranslation += (fWorldZmax - fWorldZmin) / 2.f;
	m_fZTranslation = -m_fZTranslation;
	m_fZTranslation -= (fWorldBoundingSphereDiameter * 2.f);

	m_fScaleFactor = fWorldBoundingSphereDiameter;

	_redraw();
}

/*virtual*/ void _oglView::onInstanceSelected(_view* pSender)  /*override*/
{
	if (pSender == this)
	{
		return;
	}

	_redraw();
}

/*virtual*/ void _oglView::onInstanceEnabledStateChanged(_view* pSender, _instance* /*pInstance*/, int /*iFlag*/) /*override*/
{
	if (pSender == this)
	{
		return;
	}

	_redraw();
}

/*virtual*/ void _oglView::onInstancesEnabledStateChanged(_view* pSender) /*override*/
{
	if (pSender == this)
	{
		return;
	}

	_redraw();
}

/*virtual*/ void _oglView::onInstancesShowStateChanged(_view* pSender) /*override*/
{
	if (pSender == this)
	{
		return;
	}

	_redraw();
}

/*virtual*/ void _oglView::onApplicationPropertyChanged(_view* pSender, enumApplicationProperty enApplicationProperty) /*override*/
{
	if (pSender == this)
	{
		return;
	}

	switch (enApplicationProperty)
	{
		case enumApplicationProperty::All:
		case enumApplicationProperty::Projection:
		case enumApplicationProperty::View:
		case enumApplicationProperty::GhostView:
		case enumApplicationProperty::GhostViewTransparency:
		case enumApplicationProperty::ShowFaces:
		case enumApplicationProperty::CullFaces:
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
		{
			_redraw();
		}
		break;

		default:
		{
			ASSERT(FALSE); // Internal error!
		}
		break;
	} // switch (enApplicationProperty)
}

/*virtual*/ void _oglView::onControllerChanged() /*override*/
{
	assert(getController() != nullptr);

	getController()->registerView(this);

	loadSettings();
}

/*virtual*/ void _oglView::_load()
{
	if (getController()->getModels().empty())
	{
		_redraw();

		return;
	}

	// Limits
	GLsizei VERTICES_MAX_COUNT = _oglUtils::getVerticesCountLimit(GEOMETRY_VBO_VERTEX_LENGTH * sizeof(float));
	GLsizei INDICES_MAX_COUNT = _oglUtils::getIndicesCountLimit();

	BOOL bResult = m_pOGLContext->makeCurrent();
	VERIFY(bResult);

	// OpenGL buffers
	m_oglBuffers.clear();

	m_pSelectInstanceFrameBuffer->encoding().clear();
	m_pPointedInstance = nullptr;

	float fWorldXmin = FLT_MAX;
	float fWorldXmax = -FLT_MAX;
	float fWorldYmin = FLT_MAX;
	float fWorldYmax = -FLT_MAX;
	float fWorldZmin = FLT_MAX;
	float fWorldZmax = -FLT_MAX;
	getController()->getWorldDimensions(fWorldXmin, fWorldXmax, fWorldYmin, fWorldYmax, fWorldZmin, fWorldZmax);

	float fWorldBoundingSphereDiameter = fWorldXmax - fWorldXmin;
	fWorldBoundingSphereDiameter = fmax(fWorldBoundingSphereDiameter, fWorldYmax - fWorldYmin);
	fWorldBoundingSphereDiameter = fmax(fWorldBoundingSphereDiameter, fWorldZmax - fWorldZmin);

	m_fXTranslation = fWorldXmin;
	m_fXTranslation += (fWorldXmax - fWorldXmin) / 2.f;
	m_fXTranslation = -m_fXTranslation;

	m_fYTranslation = fWorldYmin;
	m_fYTranslation += (fWorldYmax - fWorldYmin) / 2.f;
	m_fYTranslation = -m_fYTranslation;

	m_fZTranslation = fWorldZmin;
	m_fZTranslation += (fWorldZmax - fWorldZmin) / 2.f;
	m_fZTranslation = -m_fZTranslation;
	m_fZTranslation -= (fWorldBoundingSphereDiameter * 2.f);

	m_fScaleFactor = fWorldBoundingSphereDiameter;
		
	for (auto pModel : getController()->getModels())
	{
		if (!pModel->getEnable())
		{
			continue;
		}

		// VBO
		GLuint iVerticesCount = 0;
		vector<_geometry*> vecGeometriesCohort;

		// IBO - Conceptual faces
		GLuint iConcFacesIndicesCount = 0;
		vector<_cohort*> vecConcFacesCohorts;

		// IBO - Conceptual face polygons
		GLuint iConcFacePolygonsIndicesCount = 0;
		vector<_cohort*> vecConcFacePolygonsCohorts;

		// IBO - Lines
		GLuint iLinesIndicesCount = 0;
		vector<_cohort*> vecLinesCohorts;

		// IBO - Points
		GLuint iPointsIndicesCount = 0;
		vector<_cohort*> vecPointsCohorts;

		for (auto pGeometry : pModel->getGeometries())
		{
			if (pGeometry->getVerticesCount() == 0)
			{
				continue;
			}

			// VBO - Conceptual faces, polygons, etc.
			if (((int_t)iVerticesCount + pGeometry->getVerticesCount()) > (int_t)VERTICES_MAX_COUNT)
			{
				if (m_oglBuffers.createCohort(vecGeometriesCohort, m_pOGLProgram) != iVerticesCount)
				{
					ASSERT(FALSE);

					return;
				}

				iVerticesCount = 0;
				vecGeometriesCohort.clear();
			}

			// IBO - Conceptual faces
			for (size_t iCohort = 0; iCohort < pGeometry->concFacesCohorts().size(); iCohort++)
			{
				if ((int_t)(iConcFacesIndicesCount + pGeometry->concFacesCohorts()[iCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
				{
					if (m_oglBuffers.createIBO(vecConcFacesCohorts) != iConcFacesIndicesCount)
					{
						ASSERT(FALSE);

						return;
					}

					iConcFacesIndicesCount = 0;
					vecConcFacesCohorts.clear();
				}

				iConcFacesIndicesCount += (GLsizei)pGeometry->concFacesCohorts()[iCohort]->indices().size();
				vecConcFacesCohorts.push_back(pGeometry->concFacesCohorts()[iCohort]);
			}

			//  IBO - Conceptual face polygons
			for (size_t iCohort = 0; iCohort < pGeometry->concFacePolygonsCohorts().size(); iCohort++)
			{
				if ((int_t)(iConcFacePolygonsIndicesCount + pGeometry->concFacePolygonsCohorts()[iCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
				{
					if (m_oglBuffers.createIBO(vecConcFacePolygonsCohorts) != iConcFacePolygonsIndicesCount)
					{
						ASSERT(FALSE);

						return;
					}

					iConcFacePolygonsIndicesCount = 0;
					vecConcFacePolygonsCohorts.clear();
				}

				iConcFacePolygonsIndicesCount += (GLsizei)pGeometry->concFacePolygonsCohorts()[iCohort]->indices().size();
				vecConcFacePolygonsCohorts.push_back(pGeometry->concFacePolygonsCohorts()[iCohort]);
			}

			// IBO - Lines
			for (size_t iCohort = 0; iCohort < pGeometry->linesCohorts().size(); iCohort++)
			{
				if ((int_t)(iLinesIndicesCount + pGeometry->linesCohorts()[iCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
				{
					if (m_oglBuffers.createIBO(vecLinesCohorts) != iLinesIndicesCount)
					{
						ASSERT(FALSE);

						return;
					}

					iLinesIndicesCount = 0;
					vecLinesCohorts.clear();
				}

				iLinesIndicesCount += (GLsizei)pGeometry->linesCohorts()[iCohort]->indices().size();
				vecLinesCohorts.push_back(pGeometry->linesCohorts()[iCohort]);
			}

			//  IBO - Points
			for (size_t iCohort = 0; iCohort < pGeometry->pointsCohorts().size(); iCohort++)
			{
				if ((int_t)(iPointsIndicesCount + pGeometry->pointsCohorts()[iCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
				{
					if (m_oglBuffers.createIBO(vecPointsCohorts) != iPointsIndicesCount)
					{
						ASSERT(FALSE);

						return;
					}

					iPointsIndicesCount = 0;
					vecPointsCohorts.clear();
				}

				iPointsIndicesCount += (GLsizei)pGeometry->pointsCohorts()[iCohort]->indices().size();
				vecPointsCohorts.push_back(pGeometry->pointsCohorts()[iCohort]);
			}

			iVerticesCount += (GLsizei)pGeometry->getVerticesCount();
			vecGeometriesCohort.push_back(pGeometry);
		} // for (auto pGeometry : ...

		//  VBO - Conceptual faces, polygons, etc.
		if (iVerticesCount > 0)
		{
			if (m_oglBuffers.createCohort(vecGeometriesCohort, m_pOGLProgram) != iVerticesCount)
			{
				ASSERT(FALSE);

				return;
			}

			iVerticesCount = 0;
			vecGeometriesCohort.clear();
		} // if (iVerticesCount > 0)	

		//  IBO - Conceptual faces
		if (iConcFacesIndicesCount > 0)
		{
			if (m_oglBuffers.createIBO(vecConcFacesCohorts) != iConcFacesIndicesCount)
			{
				ASSERT(FALSE);

				return;
			}

			iConcFacesIndicesCount = 0;
			vecConcFacesCohorts.clear();
		}

		//  IBO - Conceptual face polygons
		if (iConcFacePolygonsIndicesCount > 0)
		{
			if (m_oglBuffers.createIBO(vecConcFacePolygonsCohorts) != iConcFacePolygonsIndicesCount)
			{
				ASSERT(FALSE);

				return;
			}

			iConcFacePolygonsIndicesCount = 0;
			vecConcFacePolygonsCohorts.clear();
		}

		//  IBO - Lines
		if (iLinesIndicesCount > 0)
		{
			if (m_oglBuffers.createIBO(vecLinesCohorts) != iLinesIndicesCount)
			{
				ASSERT(FALSE);

				return;
			}

			iLinesIndicesCount = 0;
			vecLinesCohorts.clear();
		}

		//  IBO - Points
		if (iPointsIndicesCount > 0)
		{
			if (m_oglBuffers.createIBO(vecPointsCohorts) != iPointsIndicesCount)
			{
				ASSERT(FALSE);

				return;
			}

			iPointsIndicesCount = 0;
			vecPointsCohorts.clear();
		}

	} // for (auto pModel : ...	
	
	_redraw();
}

/*virtual*/ bool _oglView::_prepareScene()
{
	CRect rcClient;
	m_pWnd->GetClientRect(&rcClient);

	int iWidth = rcClient.Width();
	int iHeight = rcClient.Height();

	if ((iWidth < MIN_VIEW_PORT_LENGTH) || (iHeight < MIN_VIEW_PORT_LENGTH))
	{
		return false;
	}

	float fWorldXmin = FLT_MAX;
	float fWorldXmax = -FLT_MAX;
	float fWorldYmin = FLT_MAX;
	float fWorldYmax = -FLT_MAX;
	float fWorldZmin = FLT_MAX;
	float fWorldZmax = -FLT_MAX;
	getController()->getWorldDimensions(fWorldXmin, fWorldXmax, fWorldYmin, fWorldYmax, fWorldZmin, fWorldZmax);

	_prepare(
		0, 0,
		iWidth, iHeight,
		fWorldXmin, fWorldXmax,
		fWorldYmin, fWorldYmax,
		fWorldZmin, fWorldZmax,
		true,
		true);

	return true;
}

/*virtual*/ void _oglView::_draw(CDC* pDC)
{
	// Initialize
	if (!_prepareScene())
	{
		return;
	}

	// Off-screen
	_drawBuffers();

	// Restore
	if (!_prepareScene())
	{
		return;
	}

	// Coordinate System, Navigation, etc.
	_preDraw();

	// Models
	_drawFaces();
	_drawConceptualFacesPolygons();
	_drawLines();
	_drawPoints();

	// Tangent, Normal, Bi-Normal Vectors, etc.
	_postDraw();

	// Update
	SwapBuffers(*pDC);	
}

/*virtual*/ void _oglView::_drawBuffers()
{
	_drawInstancesFrameBuffer();
}

void _oglView::_drawFaces()
{
	_drawFaces(false);
	_drawFaces(true);
}

/*virtual*/ void _oglView::_drawFaces(bool bTransparent)
{
#ifdef _DEBUG_DRAW_DURATION
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif

	CString strCullFaces = getCullFacesMode();

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

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(true);
#else
	m_pOGLProgram->_enableLighting(true);
#endif

	bool bGhostView = m_bGhostView && !getController()->getSelectedInstances().empty();

	for (auto itCohort : m_oglBuffers.cohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto pGeometry : itCohort.second)
		{
			auto pModel = getController()->getModelByInstance(pGeometry->getOwlModel());
			assert(pModel->getEnable());

			if (!pGeometry->getShow())
			{
				continue;
			}

			if (pGeometry->concFacesCohorts().empty())
			{
				continue;
			}

			for (auto pInstance : pGeometry->getInstances())
			{
				if (!pInstance->getEnable())
				{
					continue;
				}

				// Transformation Matrix
				glm::mat4 matTransformation = glm::make_mat4((GLdouble*)pInstance->getTransformationMatrix());
				glm::mat4 matModelView = m_matModelView;
				matModelView = matModelView * matTransformation;

				m_pOGLProgram->_setModelViewMatrix(matModelView);
#ifdef _BLINN_PHONG_SHADERS
				glm::mat4 matNormal = m_matModelView * matTransformation;
				matNormal = glm::inverse(matNormal);
				matNormal = glm::transpose(matNormal);
				m_pOGLProgram->_setNormalMatrix(matNormal);
#else
				m_pOGLProgram->_setNormalMatrix(matModelView);
#endif
				for (size_t iCohort = 0; iCohort < pGeometry->concFacesCohorts().size(); iCohort++)
				{
					auto pCohort = pGeometry->concFacesCohorts()[iCohort];

					auto pCohortMaterial = pCohort->getMaterial();
					auto itUserDefinedMaterial = m_mapUserDefinedMaterials.find(pInstance);
					if (itUserDefinedMaterial != m_mapUserDefinedMaterials.end())
					{
						pCohortMaterial = itUserDefinedMaterial->second;
					}

					bool bIsInstancePointed = m_pPointedInstance != nullptr ?
						pInstance->getOwner() != nullptr ? pInstance->getOwner() == m_pPointedInstance : pInstance == m_pPointedInstance :
						false;

					const _material* pMaterial =
						getController()->isInstanceSelected(pInstance) ? m_pSelectedInstanceMaterial :
						bIsInstancePointed ? m_pPointedInstanceMaterial :
						pCohortMaterial;

					float fTransparency = pMaterial->getA();
					if (bGhostView)
					{
						if ((pMaterial != m_pSelectedInstanceMaterial) &&
							(pMaterial != m_pPointedInstanceMaterial) &&
							(fTransparency > m_fGhostViewTransparency))
						{
							fTransparency = m_fGhostViewTransparency;
						}
					}
					else if (pMaterial == m_pPointedInstanceMaterial)
					{
						fTransparency = m_fGhostViewTransparency;
					}

					if (bTransparent)
					{
						if (fTransparency == 1.0f)
						{
							continue;
						}
					}
					else
					{
						if (fTransparency < 1.0f)
						{
							continue;
						}
					}

					_texture* pTexture = nullptr;
					if (pMaterial->hasTexture())
					{
						pTexture = pModel->getTexture(pMaterial->texture());
					}

					if (pTexture != nullptr)
					{
						m_pOGLProgram->_enableTexture(true);

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, pTexture->getName());

						m_pOGLProgram->_setSampler(0);
					}
					else
					{
						m_pOGLProgram->_setMaterial(pMaterial, fTransparency);
					}

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->IBO());
					glDrawElementsBaseVertex(GL_TRIANGLES,
						(GLsizei)pCohort->indices().size(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pCohort->IBOOffset()),
						pGeometry->VBOOffset());

					if (pTexture != nullptr)
					{
						m_pOGLProgram->_enableTexture(false);
					}
				}
			} // auto pInstance : ...			
		} // for (auto pGeometry : ...

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

	// Restore Model-View Matrix
	m_pOGLProgram->_setModelViewMatrix(m_matModelView);

	_oglUtils::checkForErrors();

#ifdef _DEBUG_DRAW_DURATION
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawFaces() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
#endif
}

void _oglView::_drawConceptualFacesPolygons()
{
#ifdef _DEBUG_DRAW_DURATION
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(false);
#else
	m_pOGLProgram->_enableLighting(false);
#endif
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	for (auto itCohort : m_oglBuffers.cohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto pGeometry : itCohort.second)
		{
			if (!pGeometry->getShow())
			{
				continue;
			}

			if (pGeometry->concFacePolygonsCohorts().empty())
			{
				continue;
			}

			for (auto pInstance : pGeometry->getInstances())
			{
				if (!pInstance->getEnable())
				{
					continue;
				}

				// Transformation Matrix
				glm::mat4 matTransformation = glm::make_mat4((GLdouble*)pInstance->getTransformationMatrix());
				glm::mat4 matModelView = m_matModelView;
				matModelView = matModelView * matTransformation;

				m_pOGLProgram->_setModelViewMatrix(matModelView);
#ifdef _BLINN_PHONG_SHADERS
				glm::mat4 matNormal = m_matModelView * matTransformation;
				matNormal = glm::inverse(matNormal);
				matNormal = glm::transpose(matNormal);
				m_pOGLProgram->_setNormalMatrix(matNormal);
#else
				m_pOGLProgram->_setNormalMatrix(matModelView);
#endif
				for (size_t iCohort = 0; iCohort < pGeometry->concFacePolygonsCohorts().size(); iCohort++)
				{
					_cohort* pCohort = pGeometry->concFacePolygonsCohorts()[iCohort];

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->IBO());
					glDrawElementsBaseVertex(GL_LINES,
						(GLsizei)pCohort->indices().size(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pCohort->IBOOffset()),
						pGeometry->VBOOffset());
				}
			} // for (size_t iInstance = ...			
		} // for (auto pGeometry ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	// Restore Model-View Matrix
	m_pOGLProgram->_setModelViewMatrix(m_matModelView);

	_oglUtils::checkForErrors();

#ifdef _DEBUG_DRAW_DURATION
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawConceptualFacesPolygons() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
#endif
}

void _oglView::_drawLines()
{
#ifdef _DEBUG_DRAW_DURATION
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(false);
#else
	m_pOGLProgram->_enableLighting(false);
#endif
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	for (auto itCohort : m_oglBuffers.cohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto pGeometry : itCohort.second)
		{
			if (!pGeometry->getShow())
			{
				continue;
			}

			if (pGeometry->linesCohorts().empty())
			{
				continue;
			}

			for (auto pInstance : pGeometry->getInstances())
			{
				if (!pInstance->getEnable())
				{
					continue;
				}

				// Transformation Matrix
				glm::mat4 matTransformation = glm::make_mat4((GLdouble*)pInstance->getTransformationMatrix());
				glm::mat4 matModelView = m_matModelView;
				matModelView = matModelView * matTransformation;

				m_pOGLProgram->_setModelViewMatrix(matModelView);
#ifdef _BLINN_PHONG_SHADERS
				glm::mat4 matNormal = m_matModelView * matTransformation;
				matNormal = glm::inverse(matNormal);
				matNormal = glm::transpose(matNormal);
				m_pOGLProgram->_setNormalMatrix(matNormal);
#else
				m_pOGLProgram->_setNormalMatrix(matModelView);
#endif
				for (size_t iCohort = 0; iCohort < pGeometry->linesCohorts().size(); iCohort++)
				{
					_cohort* pCohort = pGeometry->linesCohorts()[iCohort];

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->IBO());
					glDrawElementsBaseVertex(GL_LINES,
						(GLsizei)pCohort->indices().size(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pCohort->IBOOffset()),
						pGeometry->VBOOffset());
				}
			} // for (size_t iInstance = ...			
		} // for (auto pGeometry ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	// Restore Model-View Matrix
	m_pOGLProgram->_setModelViewMatrix(m_matModelView);

	_oglUtils::checkForErrors();

#ifdef _DEBUG_DRAW_DURATION
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawLines() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
#endif
}

void _oglView::_drawPoints()
{
#ifdef _DEBUG_DRAW_DURATION
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#endif

	glEnable(GL_PROGRAM_POINT_SIZE);

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(false);
#else
	m_pOGLProgram->_enableLighting(false);
#endif
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);

	for (auto itCohort : m_oglBuffers.cohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto pGeometry : itCohort.second)
		{
			if (!pGeometry->getShow())
			{
				continue;
			}

			if (pGeometry->pointsCohorts().empty())
			{
				continue;
			}

			for (auto pInstance : pGeometry->getInstances())
			{
				if (!pInstance->getEnable())
				{
					continue;
				}

				// Transformation Matrix
				glm::mat4 matTransformation = glm::make_mat4((GLdouble*)pInstance->getTransformationMatrix());
				glm::mat4 matModelView = m_matModelView;
				matModelView = matModelView * matTransformation;

				m_pOGLProgram->_setModelViewMatrix(matModelView);
#ifdef _BLINN_PHONG_SHADERS
				glm::mat4 matNormal = m_matModelView * matTransformation;
				matNormal = glm::inverse(matNormal);
				matNormal = glm::transpose(matNormal);
				m_pOGLProgram->_setNormalMatrix(matNormal);
#else
				m_pOGLProgram->_setNormalMatrix(matModelView);
#endif
				for (auto pCohort : pGeometry->pointsCohorts())
				{
					bool bIsInstancePointed = m_pPointedInstance != nullptr ?
						pInstance->getOwner() != nullptr ? pInstance->getOwner() == m_pPointedInstance : pInstance == m_pPointedInstance :
						false;

					const _material* pMaterial =
						getController()->isInstanceSelected(pInstance) ? m_pSelectedInstanceMaterial :
						bIsInstancePointed ? m_pPointedInstanceMaterial :
						pCohort->getMaterial();

					m_pOGLProgram->_setAmbientColor(
						pMaterial->getDiffuseColor().r(),
						pMaterial->getDiffuseColor().g(),
						pMaterial->getDiffuseColor().b());

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->IBO());
					glDrawElementsBaseVertex(GL_POINTS,
						(GLsizei)pCohort->indices().size(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pCohort->IBOOffset()),
						pGeometry->VBOOffset());
				}
			} // for (auto pInstance ...
		} // for (auto pGeometry ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	// Restore Model-View Matrix
	m_pOGLProgram->_setModelViewMatrix(m_matModelView);

	_oglUtils::checkForErrors();

#ifdef _DEBUG_DRAW_DURATION
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawPoints() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
#endif
}

void _oglView::_drawInstancesFrameBuffer()
{
	//
	// Create a frame buffer
	//

	BOOL bResult = m_pOGLContext->makeCurrent();
	VERIFY(bResult);

	m_pSelectInstanceFrameBuffer->create();

	// Selection colors
	if (m_pSelectInstanceFrameBuffer->encoding().empty())
	{
		for (auto pModel : getController()->getModels())
		{
			if (!pModel->getEnable())
			{
				continue;
			}

			for (auto pGeometry : pModel->getGeometries())
			{
				if (pGeometry->getTriangles().empty())
				{
					continue;
				}

				for (auto pInstance : pGeometry->getInstances())
				{
					float fR, fG, fB;
					_i64RGBCoder::encode(pInstance->getID(), fR, fG, fB);

					m_pSelectInstanceFrameBuffer->encoding()[pInstance->getID()] = _color(fR, fG, fB);
				}
			}
		} // for (auto pModel : ...
	} // if (m_pSelectInstanceFrameBuffer->encoding().empty())

	//
	// Draw
	//

	m_pSelectInstanceFrameBuffer->bind();

	glViewport(0, 0, BUFFER_SIZE, BUFFER_SIZE);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up the parameters
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(false);
#else
	m_pOGLProgram->_enableLighting(false);
#endif
	m_pOGLProgram->_setTransparency(1.f);

	for (auto itCohort : m_oglBuffers.cohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto pGeometry : itCohort.second)
		{
			if (!pGeometry->getShow())
			{
				continue;
			}

			if (pGeometry->getTriangles().empty())
			{
				continue;
			}

			for (auto pInstance : pGeometry->getInstances())
			{
				if (!pInstance->getEnable())
				{
					continue;
				}

				// Transformation Matrix
				glm::mat4 matTransformation = glm::make_mat4((GLdouble*)pInstance->getTransformationMatrix());
				glm::mat4 matModelView = m_matModelView;
				matModelView = matModelView * matTransformation;

				m_pOGLProgram->_setModelViewMatrix(matModelView);
#ifdef _BLINN_PHONG_SHADERS
				glm::mat4 matNormal = m_matModelView * matTransformation;
				matNormal = glm::inverse(matNormal);
				matNormal = glm::transpose(matNormal);
				m_pOGLProgram->_setNormalMatrix(matNormal);
#else
				m_pOGLProgram->_setNormalMatrix(matModelView);
#endif
				for (size_t iCohort = 0; iCohort < pGeometry->concFacesCohorts().size(); iCohort++)
				{
					auto pCohort = pGeometry->concFacesCohorts()[iCohort];

					auto itSelectionColor = m_pSelectInstanceFrameBuffer->encoding().find(pInstance->getID());
					ASSERT(itSelectionColor != m_pSelectInstanceFrameBuffer->encoding().end());

					m_pOGLProgram->_setAmbientColor(
						itSelectionColor->second.r(),
						itSelectionColor->second.g(),
						itSelectionColor->second.b());

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->IBO());
					glDrawElementsBaseVertex(GL_TRIANGLES,
						(GLsizei)pCohort->indices().size(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pCohort->IBOOffset()),
						pGeometry->VBOOffset());
				}
			} // for (size_t iInstance = ...			
		} // for (auto pGeometry ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	// Restore Model-View Matrix
	m_pOGLProgram->_setModelViewMatrix(m_matModelView);

	m_pSelectInstanceFrameBuffer->unbind();

	_oglUtils::checkForErrors();
}

bool _oglView::getOGLPos(int iX, int iY, float fDepth, GLdouble& dX, GLdouble& dY, GLdouble& dZ)
{
	CRect rcClient;
	m_pWnd->GetClientRect(&rcClient);

	GLfloat arModelViewMatrix[16];
	glGetUniformfv(m_pOGLProgram->_getID(), glGetUniformLocation(m_pOGLProgram->_getID(), "ModelViewMatrix"), arModelViewMatrix);

	GLfloat arProjectionMatrix[16];
	glGetUniformfv(m_pOGLProgram->_getID(), glGetUniformLocation(m_pOGLProgram->_getID(), "ProjectionMatrix"), arProjectionMatrix);

	GLint arViewport[4] = { 0, 0, rcClient.Width(), rcClient.Height() };

	GLdouble arModelView[16];
	GLdouble arProjection[16];
	for (int i = 0; i < 16; i++)
	{
		arModelView[i] = arModelViewMatrix[i];
		arProjection[i] = arProjectionMatrix[i];
	}

	GLdouble dWinX = (double)iX;
	GLdouble dWinY = (double)arViewport[3] - (double)iY;

	double dWinZ = 0.;
	if (fDepth == -FLT_MAX)
	{
		float fWinZ = 0.f;
		glReadPixels(iX, (int)dWinY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &fWinZ);

		dWinZ = fWinZ;

		_oglUtils::checkForErrors();
	}
	else
	{
		dWinZ = fDepth;
	}

	if (dWinZ >= 1.)
	{
		return false;
	}

	GLint iResult = gluUnProject(dWinX, dWinY, dWinZ, arModelView, arProjection, arViewport, &dX, &dY, &dZ);

	_oglUtils::checkForErrors();

	return iResult == GL_TRUE;
}

void _oglView::_onMouseMoveEvent(UINT nFlags, CPoint point)
{
	// Selection
	if (((nFlags & MK_LBUTTON) != MK_LBUTTON) &&
		((nFlags & MK_MBUTTON) != MK_MBUTTON) &&
		((nFlags & MK_RBUTTON) != MK_RBUTTON) &&
		m_pSelectInstanceFrameBuffer->isInitialized())
	{
		int iWidth = 0;
		int iHeight = 0;

		BOOL bResult = m_pOGLContext->makeCurrent();
		VERIFY(bResult);

		CRect rcClient;
		m_pWnd->GetClientRect(&rcClient);

		iWidth = rcClient.Width();
		iHeight = rcClient.Height();

		GLubyte arPixels[4];
		memset(arPixels, 0, sizeof(GLubyte) * 4);

		double dX = (double)point.x * ((double)BUFFER_SIZE / (double)iWidth);
		double dY = ((double)iHeight - (double)point.y) * ((double)BUFFER_SIZE / (double)iHeight);

		m_pSelectInstanceFrameBuffer->bind();
		glReadPixels(
			(GLint)dX,
			(GLint)dY,
			1, 1,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			arPixels);
		m_pSelectInstanceFrameBuffer->unbind();

		_instance* pPointedInstance = nullptr;
		if (arPixels[3] != 0)
		{
			int64_t iInstanceID = _i64RGBCoder::decode(arPixels[0], arPixels[1], arPixels[2]);
			pPointedInstance = getController()->getInstanceByID(iInstanceID);
			ASSERT(pPointedInstance != nullptr);
		}

		if ((pPointedInstance != nullptr) && (pPointedInstance->getOwner() != nullptr))
		{
			pPointedInstance = pPointedInstance->getOwner();
		}

		if (m_pPointedInstance != pPointedInstance)
		{
			m_pPointedInstance = pPointedInstance;			

			_redraw();
		}

		_onMouseMove(point);
	} // if (((nFlags & MK_LBUTTON) != MK_LBUTTON) && ...

	if (m_pPointedInstance != nullptr)
	{
		clock_t timeSpan = clock() - m_tmShowTooltip;
		if (timeSpan >= 200)
		{
			auto pModel = getController()->getModelByInstance(m_pPointedInstance->getOwlModel());
			assert(pModel != nullptr);

			wstring strInstanceMetaData = m_pPointedInstance->getName();

			GLdouble dX = 0.;
			GLdouble dY = 0.;
			GLdouble dZ = 0.;
			if (getOGLPos(point.x, point.y, -FLT_MAX, dX, dY, dZ))
			{
				_vector3d vecVertexBufferOffset;
				GetVertexBufferOffset(pModel->getOwlModel(), (double*)&vecVertexBufferOffset);

				auto dScaleFactor = pModel->getOriginalBoundingSphereDiameter() / 2.;

				GLdouble dWorldX = -vecVertexBufferOffset.x + (dX * dScaleFactor);
				GLdouble dWorldY = -vecVertexBufferOffset.y + (dY * dScaleFactor);
				GLdouble dWorldZ = -vecVertexBufferOffset.z + (dZ * dScaleFactor);

				strInstanceMetaData += L"\n\nX/Y/Z: ";
				strInstanceMetaData += to_wstring(dWorldX).c_str();
				strInstanceMetaData += L", ";
				strInstanceMetaData += to_wstring(dWorldY).c_str();
				strInstanceMetaData += L", ";
				strInstanceMetaData += to_wstring(dWorldZ).c_str();				
			} // if (getOGLPos( ...

			if (strInstanceMetaData.size() >= 250)
			{
				strInstanceMetaData = strInstanceMetaData.substr(0, 250);
				strInstanceMetaData += L"...";
			}

			m_tmShowTooltip = clock();

			_showTooltip(L"Information", strInstanceMetaData.c_str());
		} // if (timeSpan >= ...
	} // if (m_pPointedInstance != nullptr)
	else
	{
		_hideTooltip();
	}

	if (m_ptPrevMousePosition.x == -1)
	{
		return;
	}

	// Rotate
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON)
	{
		_rotateMouseLButton(
			(float)point.y - (float)m_ptPrevMousePosition.y,
			(float)point.x - (float)m_ptPrevMousePosition.x);

		m_ptPrevMousePosition = point;

		return;
	}

	// Zoom
	if ((nFlags & MK_MBUTTON) == MK_MBUTTON)
	{
		_zoomMouseMButton(point.y - m_ptPrevMousePosition.y);

		m_ptPrevMousePosition = point;

		return;
	}

	// Move
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

void _oglView::_onMouseEvent(enumMouseEvent enEvent, UINT nFlags, CPoint point)
{
	if (enEvent == enumMouseEvent::LBtnUp)
	{
		if (point == m_ptStartMousePosition)
		{
			getController()->selectInstance(
				nullptr/*update this view if needed*/,
				m_pPointedInstance,
				m_bMultiSelect ? GetKeyState(VK_CONTROL) & 0x8000 : false);
		}
	} // if (enEvent == meLBtnDown)

	switch (enEvent)
	{
	case enumMouseEvent::Move:
	{
		_onMouseMoveEvent(nFlags, point);
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
		assert(false);
		break;
	} // switch (enEvent)
}

void _oglView::addUserDefinedMaterial(const vector<_instance*>& vecInstances, float fR, float fG, float fB)
{
	assert(!vecInstances.empty());

	for (auto pInstance : vecInstances)
	{
		auto pMaterial = new _material();
		pMaterial->init(
			fR, fG, fB,
			fR, fG, fB,
			fR, fG, fB,
			fR, fG, fB,
			1.f,
			nullptr);

		auto itUserDefinedMaterial = m_mapUserDefinedMaterials.find(pInstance);
		if (itUserDefinedMaterial != m_mapUserDefinedMaterials.end())
		{
			delete itUserDefinedMaterial->second;
			itUserDefinedMaterial->second = pMaterial;
		}
		else
		{
			m_mapUserDefinedMaterials[pInstance] = pMaterial;
		}
	}

	_redraw();
}

void _oglView::removeUserDefinedMaterials()
{
	bool bRedraw = !m_mapUserDefinedMaterials.empty();

	for (auto itUserDefinedMaterial : m_mapUserDefinedMaterials)
	{
		delete itUserDefinedMaterial.second;
	}
	m_mapUserDefinedMaterials.clear();

	if (bRedraw)
	{
		_redraw();
	}	
}