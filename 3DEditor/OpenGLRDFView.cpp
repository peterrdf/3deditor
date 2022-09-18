#include "stdafx.h"

#include "OpenGLRDFView.h"
#include "RDFController.h"
#include "RDFModel.h"

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
COpenGLLight::COpenGLLight(GLenum enLight)
{
	m_bIsEnabled = false;

	m_enLight = enLight;

	m_arAmbient[0] = 0.f;
	m_arAmbient[1] = 0.f;
	m_arAmbient[2] = 0.f;
	m_arAmbient[3] = 1.f;

	m_arDiffuse[0] = 1.f;
	m_arDiffuse[1] = 1.f;
	m_arDiffuse[2] = 1.f;
	m_arDiffuse[3] = 1.f;

	m_arSpecular[0] = 1.f;
	m_arSpecular[1] = 1.f;
	m_arSpecular[2] = 1.f;
	m_arSpecular[3] = 1.f;

	m_arPosition[0] = 1.f;
	m_arPosition[1] = 1.f;
	m_arPosition[2] = 1.f;
	m_arPosition[3] = 0.f; // directional light
}

// ------------------------------------------------------------------------------------------------
COpenGLLight::COpenGLLight(const COpenGLLight & light)
{
	m_bIsEnabled = light.m_bIsEnabled;

	m_enLight = light.m_enLight;

	m_arAmbient[0] = light.m_arAmbient[0];
	m_arAmbient[1] = light.m_arAmbient[1];
	m_arAmbient[2] = light.m_arAmbient[2];
	m_arAmbient[3] = light.m_arAmbient[3];

	m_arDiffuse[0] = light.m_arDiffuse[0];
	m_arDiffuse[1] = light.m_arDiffuse[1];
	m_arDiffuse[2] = light.m_arDiffuse[2];
	m_arDiffuse[3] = light.m_arDiffuse[3];

	m_arSpecular[0] = light.m_arSpecular[0];
	m_arSpecular[1] = light.m_arSpecular[1];
	m_arSpecular[2] = light.m_arSpecular[2];
	m_arSpecular[3] = light.m_arSpecular[3];

	m_arPosition[0] = light.m_arPosition[0];
	m_arPosition[1] = light.m_arPosition[1];
	m_arPosition[2] = light.m_arPosition[2];
	m_arPosition[3] = light.m_arPosition[3];
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ COpenGLLight::~COpenGLLight()
{
}

// ------------------------------------------------------------------------------------------------
bool COpenGLLight::isEnabled() const
{
	return m_bIsEnabled;
}

// ------------------------------------------------------------------------------------------------
void COpenGLLight::enable(bool bEnable)
{
	m_bIsEnabled = bEnable;
}

// ------------------------------------------------------------------------------------------------
GLenum COpenGLLight::getLight() const
{
	return m_enLight;
}

// ------------------------------------------------------------------------------------------------
GLfloat * COpenGLLight::getAmbient() const
{
	return (GLfloat *)m_arAmbient;
}

// ------------------------------------------------------------------------------------------------
void COpenGLLight::setAmbient(GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fW/* = 1.f*/)
{
	m_arAmbient[0] = fR;
	m_arAmbient[1] = fG;
	m_arAmbient[2] = fB;
	m_arAmbient[3] = fW;
}

// ------------------------------------------------------------------------------------------------
GLfloat * COpenGLLight::getDiffuse() const
{
	return (GLfloat *)m_arDiffuse;
}

// ------------------------------------------------------------------------------------------------
void COpenGLLight::setDiffuse(GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fW/* = 1.f*/)
{
	m_arDiffuse[0] = fR;
	m_arDiffuse[1] = fG;
	m_arDiffuse[2] = fB;
	m_arDiffuse[3] = fW;
}

// ------------------------------------------------------------------------------------------------
GLfloat * COpenGLLight::getSpecular() const
{
	return (GLfloat *)m_arSpecular;
}

// ------------------------------------------------------------------------------------------------
void COpenGLLight::setSpecular(GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fW/* = 1.f*/)
{
	m_arSpecular[0] = fR;
	m_arSpecular[1] = fG;
	m_arSpecular[2] = fB;
	m_arSpecular[3] = fW;
}

// ------------------------------------------------------------------------------------------------
GLfloat * COpenGLLight::getPosition() const
{
	return (GLfloat *)m_arPosition;
}

// ------------------------------------------------------------------------------------------------
void COpenGLLight::setPosition(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fW)
{
	m_arPosition[0] = fX;
	m_arPosition[1] = fY;
	m_arPosition[2] = fZ;
	m_arPosition[3] = fW;
}

// ------------------------------------------------------------------------------------------------
#define SELECTION_BUFFER_SIZE 512

// ------------------------------------------------------------------------------------------------
#ifdef _LINUX
COpenGLRDFView::COpenGLRDFView(wxGLCanvas * pWnd)
#else
COpenGLRDFView::COpenGLRDFView(CWnd * pWnd)
#endif // _LINUX
	: CRDFView()
	, m_pWnd(pWnd)
	, m_bShowReferencedInstances(TRUE)
	, m_bShowCoordinateSystem(TRUE)
	, m_bShowFaces(TRUE)
	, m_strCullFaces(CULL_FACES_NONE)
	, m_bShowFacesPolygons(FALSE)
	, m_bShowConceptualFacesPolygons(TRUE)
	, m_bShowLines(TRUE)
	, m_fLineWidth(1.f)
	, m_bShowPoints(TRUE)
#ifdef	_DEBUG_OCTREE
	, m_fPointSize(10.f)
#else
	, m_fPointSize(1.f)
#endif //	_DEBUG_OCTREE
	, m_bShowBoundingBoxes(FALSE)
	, m_bShowNormalVectors(FALSE)
	, m_bShowTangenVectors(FALSE)
	, m_bShowBiNormalVectors(FALSE)
	, m_bScaleVectors(FALSE)
	, m_pOGLContext(NULL)
	, m_vecOGLLights()
	//, m_vecFacesVBOs()
	//, m_vecFacesIBOs()
	//, m_vecWireframesVBOs()
	//, m_vecWireframesIBOs()
	, m_fXAngle(30.0f)
	, m_fYAngle(30.0f)
	, m_fXTranslation(0.0f)
	, m_fYTranslation(0.0f)
	, m_fZTranslation(-5.0f)
	, m_ptStartMousePosition(-1, -1)
	, m_ptPrevMousePosition(-1, -1)
	, m_iInstanceSelectionFrameBuffer(0)
	, m_iInstanceSelectionTextureBuffer(0)
	, m_iInstanceSelectionDepthRenderBuffer(0)
	, m_iFaceSelectionFrameBuffer(0)
	, m_iFaceSelectionTextureBuffer(0)
	, m_iFaceSelectionDepthRenderBuffer(0)
	, m_mapInstancesSelectionColors()
	, m_pPointedInstance(NULL)
	, m_pSelectedInstance(NULL)
	, m_ptSelectedPoint(-1, -1)
	, m_mapFacesSelectionColors()
	, m_iPointedFace(-1)
	, m_vecDrawMetaData()
	, m_vecIBOs()
	, m_pSelectedInstanceMaterial(NULL)
	, m_pPointedInstanceMaterial(NULL)
	, m_iBoundingBoxesVBO(0)
	, m_iBoundingBoxesVAO(0)
	, m_iBoundingBoxesIBO(0)
{
	ASSERT(m_pWnd != NULL);

#ifdef _LINUX
    m_pOGLContext = new wxGLContext(m_pWnd);
#else
    m_pOGLContext = new COpenGLContext(*(m_pWnd->GetDC()));
#endif // _LINUX

	/*
	* Light model
	*/
	// http://www.glprogramming.com/red/chapter05.html#name4
	m_arLightModelAmbient[0] = .2f;
	m_arLightModelAmbient[1] = .2f;
	m_arLightModelAmbient[2] = .2f;
	m_arLightModelAmbient[3] = 1.f;
	m_bLightModelLocalViewer = true;
	m_bLightModel2Sided = false;

	/*
	* Lights: GL_LIGHT0 - GL_LIGHT7
	*/
	for (int iLight = 0; iLight < 8; iLight++)
	{
		m_vecOGLLights.push_back(COpenGLLight(GL_LIGHT0 + iLight));
	}

	/*
	* Enable GL_LIGHT0
	*/
	m_vecOGLLights[0].enable(true);

	m_arSelectedPoint[0] = -FLT_MAX;
	m_arSelectedPoint[1] = -FLT_MAX;
	m_arSelectedPoint[2] = -FLT_MAX;

	m_arCamera[0] = -FLT_MAX;
	m_arCamera[1] = -FLT_MAX;
	m_arCamera[2] = -FLT_MAX;

	/*
	* Default
	*/
	m_pSelectedInstanceMaterial = new CRDFMaterial();
	m_pSelectedInstanceMaterial->set(
		1, 0, 0,  // ambient
		1, 0, 0,  // diffuse
		1, 0, 0,  // emissive
		1, 0, 0,  //specular
		1,        // transparency	
		NULL);	  // texture

	/*
	* Default
	*/
	m_pPointedInstanceMaterial = new CRDFMaterial();
	m_pPointedInstanceMaterial->set(
		.33f, .33f, .33f,  // ambient
		.33f, .33f, .33f,  // diffuse
		.33f, .33f, .33f,  // emissive
		.33f, .33f, .33f,  //specular
		1,                 // transparency	
		NULL);	           // texture

#ifdef _USE_SHADERS
	m_pOGLContext->MakeCurrent();

	m_pProgram = new CBinnPhongGLProgram();
	m_pVertSh = new CGLShader(GL_VERTEX_SHADER);
	m_pFragSh = new CGLShader(GL_FRAGMENT_SHADER);

	if (!m_pVertSh->Load(IDR_TEXTFILE_VERTEX_SHADER2))
		AfxMessageBox(_T("Vertex shader loading error!"));

	if (!m_pFragSh->Load(IDR_TEXTFILE_FRAGMENT_SHADER2))
		AfxMessageBox(_T("Fragment shader loading error!"));

	if (!m_pVertSh->Compile())
		AfxMessageBox(_T("Vertex shader compiling error!"));

	if (!m_pFragSh->Compile())
		AfxMessageBox(_T("Fragment shader compiling error!"));

	m_pProgram->AttachShader(m_pVertSh);
	m_pProgram->AttachShader(m_pFragSh);

	glBindFragDataLocation(m_pProgram->GetID(), 0, "FragColor");

	if (!m_pProgram->Link())
		AfxMessageBox(_T("Program linking error!"));

	m_modelViewMatrix = glm::identity<glm::mat4>();
#endif // _USE_SHADERS
}

// ------------------------------------------------------------------------------------------------
COpenGLRDFView::~COpenGLRDFView()
{
	GetController()->UnRegisterView(this);

	if (m_iInstanceSelectionFrameBuffer != 0)
	{
		glDeleteFramebuffers(1, &m_iInstanceSelectionFrameBuffer);
		m_iInstanceSelectionFrameBuffer = 0;
	}

	if (m_iInstanceSelectionTextureBuffer != 0)
	{
		glDeleteTextures(1, &m_iInstanceSelectionTextureBuffer);
		m_iInstanceSelectionTextureBuffer = 0;
	}

	if (m_iInstanceSelectionDepthRenderBuffer != 0)
	{
		glDeleteRenderbuffers(1, &m_iInstanceSelectionDepthRenderBuffer);
		m_iInstanceSelectionDepthRenderBuffer = 0;
	}

	if (m_iFaceSelectionFrameBuffer != 0)
	{
		glDeleteFramebuffers(1, &m_iFaceSelectionFrameBuffer);
		m_iFaceSelectionFrameBuffer = 0;
	}

	if (m_iFaceSelectionTextureBuffer != 0)
	{
		glDeleteTextures(1, &m_iFaceSelectionTextureBuffer);
		m_iFaceSelectionTextureBuffer = 0;
	}

	if (m_iFaceSelectionDepthRenderBuffer != 0)
	{
		glDeleteRenderbuffers(1, &m_iFaceSelectionDepthRenderBuffer);
		m_iFaceSelectionDepthRenderBuffer = 0;
	}	

	/*
	* VBO
	*/
	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		delete m_vecDrawMetaData[iDrawMetaData];
	}
	m_vecDrawMetaData.clear();

	/*
	* IBO
	*/
	for (size_t iIBO = 0; iIBO < m_vecIBOs.size(); iIBO++)
	{
		glDeleteBuffers(1, &(m_vecIBOs[iIBO]));
	}
	m_vecIBOs.clear();	


#ifdef _USE_SHADERS
	m_pOGLContext->MakeCurrent();

	m_pProgram->DetachShader(m_pVertSh);
	m_pProgram->DetachShader(m_pFragSh);

	delete m_pProgram;
	m_pProgram = NULL;

	delete m_pVertSh;
	m_pVertSh = NULL;
	delete m_pFragSh;
	m_pFragSh = NULL;
#endif // _USE_SHADERS

	if (m_pOGLContext != NULL)
	{
		delete m_pOGLContext;
		m_pOGLContext = NULL;
	}

	ReleaseBuffers();

	delete m_pSelectedInstanceMaterial;
	m_pSelectedInstanceMaterial = NULL;

	delete m_pPointedInstanceMaterial;
	m_pPointedInstanceMaterial = NULL;

	// Bounding boxes
	if (m_iBoundingBoxesVAO != 0)
	{
		glDeleteVertexArrays(1, &m_iBoundingBoxesVAO);
		m_iBoundingBoxesVAO = 0;
	}

	if (m_iBoundingBoxesVBO != 0)
	{
		glDeleteRenderbuffers(1, &m_iBoundingBoxesVBO);
		m_iBoundingBoxesVBO = 0;
	}

	if (m_iBoundingBoxesIBO != 0)
	{
		glDeleteRenderbuffers(1, &m_iBoundingBoxesIBO);
		m_iBoundingBoxesIBO = 0;
	}
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetRotation(float fX, float fY, BOOL bRedraw)
{
	m_fXAngle = fX;
	m_fYAngle = fY;

	if (bRedraw)
	{
		m_pWnd->RedrawWindow();
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
		m_pWnd->RedrawWindow();
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
void COpenGLRDFView::ShowReferencedInstances(BOOL bShow)
{
	m_bShowReferencedInstances = bShow;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::AreReferencedInstancesShown() const
{
	return m_bShowReferencedInstances;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::ShowCoordinateSystem(BOOL bShow)
{
	m_bShowCoordinateSystem = bShow;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::IsCoordinateSystemShown() const
{
	return m_bShowCoordinateSystem;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::ShowFaces(BOOL bShow)
{
	m_bShowFaces = bShow;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::AreFacesShown() const
{
	return m_bShowFaces;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetCullFacesMode(LPCTSTR szMode)
{
	m_strCullFaces = szMode;

#ifdef _LINUX
	m_pWnd->Refresh(false);
#else
	m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
LPCTSTR COpenGLRDFView::GetCullFacesMode() const
{
	return m_strCullFaces;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::ShowFacesPolygons(BOOL bShow)
{
	m_bShowFacesPolygons = bShow;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::AreFacesPolygonsShown() const
{
	return m_bShowFacesPolygons;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::ShowConceptualFacesPolygons(BOOL bShow)
{
	m_bShowConceptualFacesPolygons = bShow;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::AreConceptualFacesPolygonsShown() const
{
	return m_bShowConceptualFacesPolygons;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::ShowLines(BOOL bShow)
{
	m_bShowLines = bShow;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::AreLinesShown() const
{
	return m_bShowLines;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetLineWidth(GLfloat fWidth)
{
	m_fLineWidth = fWidth;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
GLfloat COpenGLRDFView::GetLineWidth() const
{
	return m_fLineWidth;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::ShowPoints(BOOL bShow)
{
	m_bShowPoints = bShow;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::ArePointsShown() const
{
	return m_bShowPoints;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetPointSize(GLfloat fSize)
{
	m_fPointSize = fSize;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
GLfloat COpenGLRDFView::GetPointSize() const
{
	return m_fPointSize;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::ShowBoundingBoxes(BOOL bShow)
{
	m_bShowBoundingBoxes = bShow;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::AreBoundingBoxesShown() const
{
	return m_bShowBoundingBoxes;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::ShowNormalVectors(BOOL bShow)
{
	m_bShowNormalVectors = bShow;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::AreNormalVectorsShown() const
{
	return m_bShowNormalVectors;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::ShowTangentVectors(BOOL bShow)
{
	m_bShowTangenVectors = bShow;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::AreTangentVectorsShown() const
{
	return m_bShowTangenVectors;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::ShowBiNormalVectors(BOOL bShow)
{
	m_bShowBiNormalVectors = bShow;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::AreBiNormalVectorsShown() const
{
	return m_bShowBiNormalVectors;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::ScaleVectors(BOOL bShow)
{
	m_bScaleVectors = bShow;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
BOOL COpenGLRDFView::AreVectorsScaled() const
{
	return m_bScaleVectors;
}

// ------------------------------------------------------------------------------------------------
GLfloat * COpenGLRDFView::GetLightModelAmbient() const
{
	return (GLfloat *)m_arLightModelAmbient;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetLightModelAmbient(GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fW/* = 1.f*/)
{
	m_arLightModelAmbient[0] = fR;
	m_arLightModelAmbient[1] = fG;
	m_arLightModelAmbient[2] = fB;
	m_arLightModelAmbient[3] = fW;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
bool COpenGLRDFView::GetLightModelLocalViewer() const
{
	return m_bLightModelLocalViewer;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetLightModelLocalViewer(bool bLocalViewer)
{
	m_bLightModelLocalViewer = bLocalViewer;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
bool COpenGLRDFView::GetLightModel2Sided() const
{
	return m_bLightModel2Sided;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetLightModel2Sided(bool b2Sided)
{
	m_bLightModel2Sided = b2Sided;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
const vector<COpenGLLight> & COpenGLRDFView::GetOGLLights() const
{
	return m_vecOGLLights;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::SetOGLLight(int iLight, const COpenGLLight & light)
{
	ASSERT((iLight >= 0)& (iLight < (int)m_vecOGLLights.size()));

	m_vecOGLLights[iLight] = light;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

bool gluInvertMatrix(const double m[16], double invOut[16])
{
	double inv[16], det;
	int i;

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return false;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * det;

	return true;
}

// ------------------------------------------------------------------------------------------------
#ifdef _LINUX
void COpenGLRDFView::Draw(wxPaintDC * pDC)
#else
void COpenGLRDFView::Draw(CDC * pDC)
#endif // _LINUX
{
	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	if (pModel == NULL)
	{
		return;
	}

	int iWidth = 0;
	int iHeight = 0;

#ifdef _LINUX
    m_pOGLContext->SetCurrent(*m_pWnd);

    const wxSize szClient = m_pWnd->GetClientSize();

    iWidth = szClient.GetWidth();
    iHeight = szClient.GetHeight();
#else
    BOOL bResult = m_pOGLContext->MakeCurrent();
	VERIFY(bResult);

#ifdef _ENABLE_OPENGL_DEBUG
	m_pOGLContext->EnableDebug();
#endif

	CRect rcClient;
	m_pWnd->GetClientRect(&rcClient);

	iWidth = rcClient.Width();
	iHeight = rcClient.Height();
#endif // _LINUX

	if ((iWidth < 20) || (iHeight < 20))
	{
		return;
	}

#ifdef _USE_SHADERS
	m_pProgram->Use();

	glViewport(0, 0, iWidth, iHeight);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up the parameters
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	

	glShadeModel(TEST_MODE ? GL_FLAT : GL_SMOOTH);

	/*
	* Light
	*/
	glProgramUniform3f(
		m_pProgram->GetID(),
		m_pProgram->getPointLightingLocation(),
		0.f,
		0.f,
		10000.f);

	/*
	* Shininess
	*/
	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->getMaterialShininess(),
		30.f);

	/*
	* Projection Matrix
	*/
	// fovY     - Field of vision in degrees in the y direction
	// aspect   - Aspect ratio of the viewport
	// zNear    - The near clipping distance
	// zFar     - The far clipping distance
	GLdouble fovY = 45.0;
	GLdouble aspect = (GLdouble)iWidth / (GLdouble)iHeight;
	GLdouble zNear = 0.001;
	GLdouble zFar = 1000000.0;

	GLdouble fH = tan(fovY / 360 * M_PI) * zNear;
	GLdouble fW = fH * aspect;

	glm::mat4 projectionMatrix = glm::frustum<GLdouble>(-fW, fW, -fH, fH, zNear, zFar);

	glProgramUniformMatrix4fv(
		m_pProgram->GetID(),
		m_pProgram->getPMatrix(),
		1,
		false,
		value_ptr(projectionMatrix));

	/*
	* Model-View Matrix
	*/
	m_modelViewMatrix = glm::identity<glm::mat4>();
	m_modelViewMatrix = glm::translate(m_modelViewMatrix, glm::vec3(m_fXTranslation, m_fYTranslation, m_fZTranslation));

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	float fXTranslation = fXmin;
	fXTranslation += (fXmax - fXmin) / 2.f;
	fXTranslation = -fXTranslation;

	float fYTranslation = fYmin;
	fYTranslation += (fYmax - fYmin) / 2.f;
	fYTranslation = -fYTranslation;

	float fZTranslation = fZmin;
	fZTranslation += (fZmax - fZmin) / 2.f;
	fZTranslation = -fZTranslation;

	m_modelViewMatrix = glm::translate(m_modelViewMatrix, glm::vec3(-fXTranslation, -fYTranslation, -fZTranslation));

	m_modelViewMatrix = glm::rotate(m_modelViewMatrix, m_fXAngle, glm::vec3(1.0f, 0.0f, 0.0f));
	m_modelViewMatrix = glm::rotate(m_modelViewMatrix, m_fYAngle, glm::vec3(0.0f, 1.0f, 0.0f));

	m_modelViewMatrix = glm::translate(m_modelViewMatrix, glm::vec3(fXTranslation, fYTranslation, fZTranslation));

	glProgramUniformMatrix4fv(
		m_pProgram->GetID(),
		m_pProgram->getMVMatrix(),
		1,
		false,
		glm::value_ptr(m_modelViewMatrix));

	/*
	* Normal Matrix
	*/
	glm::mat4 normalMatrix = m_modelViewMatrix;
	normalMatrix = glm::inverse(normalMatrix);
	normalMatrix = glm::transpose(normalMatrix);

	glProgramUniformMatrix4fv(
		m_pProgram->GetID(),
		m_pProgram->getNMatrix(),
		1,
		false,
		value_ptr(normalMatrix));

	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->geUseBinnPhongModel(),
		1.f);

	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->geUseTexture(),
		0.f);

	/*
	Non-transparent faces
	*/
	DrawFaces(false);

	/*
	Transparent faces
	*/
	DrawFaces(true);

	/*
	Pointed face
	*/
	//#todoDrawPointedFace();

	/*
	Faces polygons
	*/
	DrawFacesPolygons();

	/*
	Conceptual faces polygons
	*/
	DrawConceptualFacesPolygons();

	/*
	Lines
	*/
	DrawLines();

	/*
	Points
	*/
	//DrawPoints();//#todo

	/*
	Bounding boxes
	*/
	DrawBoundingBoxes();

	/*
	Normal vectors
	*/
	//#todoDrawNormalVectors();

	/*
	Tangent vectors
	*/
	//#todoDrawTangentVectors();

	/*
	Bi-Normal vectors
	*/
	//#todoDrawBiNormalVectors();	

	/*
	End
	*/
#ifdef _LINUX
	m_pWnd->SwapBuffers();
#else
	SwapBuffers(*pDC);
#endif // _LINUX

	/*
	Selection support
	*/
	//DrawInstancesFrameBuffer();
	//#todoDrawFacesFrameBuffer();
#else
	glViewport(0, 0, iWidth, iHeight);

	/*
	* Convert the selected point in Open GL coordinates
	*/
	//if (m_ptSelectedPoint != CPoint(-1, -1))
	//{
	//	GetOGLPos(m_ptSelectedPoint.x, m_ptSelectedPoint.y, -FLT_MAX, m_arSelectedPoint[0], m_arSelectedPoint[1], m_arSelectedPoint[2]);

	//	/*
	//	* Project/unproject the camera
	//	*/
	//	GLdouble dOutX, dOutY, dOutZ;
	//	OGLProject(m_fXTranslation, m_fYTranslation, m_fZTranslation, dOutX, dOutY, dOutZ);

	//	GetOGLPos((int)dOutX, (int)dOutY, (float)dOutZ, m_arCamera[0], m_arCamera[1], m_arCamera[2]);

	//	m_ptSelectedPoint = CPoint(-1, -1);
	//}

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up the parameters
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_FRAMEBUFFER_SRGB);

	glShadeModel(TEST_MODE ? GL_FLAT : GL_SMOOTH);

	glEnable(GL_LIGHTING);

	/*
	* Light model, http://www.glprogramming.com/red/chapter05.html
	*/
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_arLightModelAmbient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, m_bLightModelLocalViewer ? GL_TRUE : GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, m_bLightModel2Sided ? GL_TRUE : GL_FALSE);

	/*
	* Lights
	*/
	for (size_t iLight = 0; iLight < m_vecOGLLights.size(); iLight++)
	{
		if (!m_vecOGLLights[iLight].isEnabled())
		{
			glDisable(m_vecOGLLights[iLight].getLight());

			continue;
		}

		glEnable(m_vecOGLLights[iLight].getLight());
		glLightfv(m_vecOGLLights[iLight].getLight(), GL_AMBIENT, m_vecOGLLights[iLight].getAmbient());
		glLightfv(m_vecOGLLights[iLight].getLight(), GL_DIFFUSE, m_vecOGLLights[iLight].getDiffuse());
		glLightfv(m_vecOGLLights[iLight].getLight(), GL_SPECULAR, m_vecOGLLights[iLight].getSpecular());
		glLightfv(m_vecOGLLights[iLight].getLight(), GL_POSITION, m_vecOGLLights[iLight].getPosition());
	}

	// Shininess between 0 and 128
	float fShininess = 64.f;
	glMaterialfv(GL_FRONT, GL_SHININESS, &fShininess);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// fovY     - Field of vision in degrees in the y direction
	// aspect   - Aspect ratio of the viewport
	// zNear    - The near clipping distance
	// zFar     - The far clipping distance
	GLdouble fovY = 45.0;
	GLdouble aspect = (GLdouble)iWidth / (GLdouble)iHeight;
	GLdouble zNear = 0.0001;
	GLdouble zFar = 1000000.0;

	GLdouble fH = tan(fovY / 360 * M_PI) * zNear;
	GLdouble fW = fH * aspect;

	glFrustum(-fW, fW, -fH, fH, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(m_fXTranslation, m_fYTranslation, m_fZTranslation);

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	float fXTranslation = fXmin;
	fXTranslation += (fXmax - fXmin) / 2.f;
	fXTranslation = -fXTranslation;

	float fYTranslation = fYmin;
	fYTranslation += (fYmax - fYmin) / 2.f;
	fYTranslation = -fYTranslation;

	float fZTranslation = fZmin;
	fZTranslation += (fZmax - fZmin) / 2.f;
	fZTranslation = -fZTranslation;

	glTranslatef(-fXTranslation, -fYTranslation, -fZTranslation);

	glRotatef(m_fXAngle, 1.0f, 0.0f, 0.0f);
	glRotatef(m_fYAngle, 0.0f, 1.0f, 0.0f);

	glTranslatef(fXTranslation, fYTranslation, fZTranslation);

	/*
	Non-transparent faces
	*/
	DrawFaces(false);

	/*
	Transparent faces
	*/
	DrawFaces(true);

	/*
	Pointed instance
	*/
	//DrawPointedInstance();

	/*
	Pointed face
	*/
	DrawPointedFace();

	/*
	Faces polygons
	*/
	DrawFacesPolygons();

	/*
	Conceptual faces polygons
	*/
	DrawConceptualFacesPolygons();

	/*
	Lines
	*/
	DrawLines();

	/*
	Points
	*/
	DrawPoints();

	/*
	Bounding boxes
	*/
	DrawBoundingBoxes();

	/*
	Normal vectors
	*/
	DrawNormalVectors();

	/*
	Tangent vectors
	*/
	DrawTangentVectors();

	/*
	Bi-Normal vectors
	*/
	DrawBiNormalVectors();

	/*
	* Selected point plane/view plane
	*/
	/*if ((m_arSelectedPoint[0] != -FLT_MAX) && (m_arSelectedPoint[1] != -FLT_MAX) && (m_arSelectedPoint[2] != -FLT_MAX) &&
		(m_arCamera[0] != -FLT_MAX) && (m_arCamera[1] != -FLT_MAX) && (m_arCamera[2] != -FLT_MAX))
	{
		glDisable(GL_LIGHTING);

		glLineWidth(m_fLineWidth);
		glColor3f(.0f, .0f, .0f);

		glBegin(GL_LINES);

		const float fStep = (fXmax - fXmin) / 10.f;

		for (int i = 0; i < 5; i++)
		{
			glVertex3f(m_arSelectedPoint[0] + (i * fStep), m_arSelectedPoint[1], m_arSelectedPoint[2]);
			glVertex3f(m_arCamera[0] + (i * fStep), m_arCamera[1], -m_arCamera[2]);
		}

		for (int i = 0; i < 5; i++)
		{
			glVertex3f(m_arSelectedPoint[0] - (i * fStep), m_arSelectedPoint[1], m_arSelectedPoint[2]);
			glVertex3f(m_arCamera[0] - (i * fStep), m_arCamera[1], -m_arCamera[2]);
		}

		glEnd();

		glEnable(GL_LIGHTING);

		COpenGL::Check4Errors();
	}*/

	/*
	Clip space
	*/
	// DISABLED
	//DrawClipSpace();

	/*
	End
	*/
#ifdef _LINUX
	m_pWnd->SwapBuffers();
#else
	SwapBuffers(*pDC);
#endif // _LINUX

	/*
	Selection support
	*/
	DrawInstancesFrameBuffer();
	DrawFacesFrameBuffer();
#endif // #ifdef _USE_SHADERS
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::OnMouseEvent(enumMouseEvent enEvent, UINT nFlags, CPoint point)
{
	if (enEvent == meLBtnUp)
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

				m_ptSelectedPoint = m_pSelectedInstance != NULL ? point : CPoint(-1, -1);

				m_arSelectedPoint[0] = -FLT_MAX;
				m_arSelectedPoint[1] = -FLT_MAX;
				m_arSelectedPoint[2] = -FLT_MAX;

				m_mapFacesSelectionColors.clear();

#ifdef _LINUX
                m_pWnd->Refresh(false);
#else
                m_pWnd->RedrawWindow();
#endif // _LINUX

				ASSERT(GetController() != NULL);

				GetController()->SelectInstance(this, m_pSelectedInstance);
			} // if (m_pSelectedInstance != ...
		}
	} // if (enEvent == meLBtnDown)

	switch (enEvent)
	{
		case meMove:
		{
			OnMouseMoveEvent(nFlags, point);
		}
		break;

		case meLBtnDown:
		case meMBtnDown:
		case meRBtnDown:
		{
			m_ptStartMousePosition = point;
			m_ptPrevMousePosition = point;
		}
		break;

		case meLBtnUp:
		case meMBtnUp:
		case meRBtnUp:
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
/*virtual*/ void COpenGLRDFView::OnModelChanged()
{
#ifdef _LINUX
    m_pOGLContext->SetCurrent(*m_pWnd);
#else
    BOOL bResult = m_pOGLContext->MakeCurrent();
	VERIFY(bResult);
#endif // _LINUX

	ReleaseBuffers();

	m_mapInstancesSelectionColors.clear();
	m_pPointedInstance = NULL;
	m_pSelectedInstance = NULL;
	m_arSelectedPoint[0] = -FLT_MAX;
	m_arSelectedPoint[1] = -FLT_MAX;
	m_arSelectedPoint[2] = -FLT_MAX;
	m_mapFacesSelectionColors.clear();
	m_iPointedFace = -1;

	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	ASSERT(pModel != NULL);

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	/*
	* Bounding sphere diameter
	*/
	float fBoundingSphereDiameter = fXmax - fXmin;
	fBoundingSphereDiameter = fmax(fBoundingSphereDiameter, fYmax - fYmin);
	fBoundingSphereDiameter = fmax(fBoundingSphereDiameter, fZmax - fZmin);

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

	/*
	* VBOs
	*/
	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		delete m_vecDrawMetaData[iDrawMetaData];
	}
	m_vecDrawMetaData.clear();

	/*
	* IBO
	*/
	for (size_t iIBO = 0; iIBO < m_vecIBOs.size(); iIBO++)
	{
		glDeleteBuffers(1, &(m_vecIBOs[iIBO]));
	}
	m_vecIBOs.clear();

	// Limits
	GLsizei GEOMETRY_VERTICES_MAX_COUNT = COpenGL::GetGeometryVerticesCountLimit();
	GLsizei INDICES_MAX_COUNT = COpenGL::GetIndicesCountLimit();

	const map<int64_t, CRDFInstance*>& mapRDFInstances = pModel->GetRDFInstances();

	// VBO
	GLuint iVerticesCount = 0;	
	vector<CRDFInstance*> vecRDFInstancesGroup;

	// VBO
	/*GLuint iVectorsVerticesCount = 0;
	vector<CRDFInstance*> vecRDFInstancesVectorsGroup;*/

	// IBO - Materials
	GLuint iMaterialsIndicesCount = 0;
	vector<CRDFGeometryWithMaterial*> vecRDFMaterialsGroup;

	// IBO - Lines
	GLuint iLinesIndicesCount = 0;
	vector<CLinesCohort*> vecLinesCohorts;

	// IBO - Points
	GLuint iPointsIndicesCount = 0;
	vector<CPointsCohort*> vecPointsCohorts;

	// IBO - Conceptual Faces
	GLuint iConceptualFacesIndicesCount = 0;
	vector<CWireframesCohort*> vecConceptualFacesCohorts;

	// IBO - Faces
	GLuint iFacesIndicesCount = 0;
	vector<CWireframesCohort*> vecFacesCohorts;

	// IBO - Normal Vectors
	//GLuint iNormalVectorsIndicesCount = 0;
	//vector<CLinesCohort*> vecNormalVectorsCohorts;

	// IBO - Tangent Vectors
	//GLuint iTangentVectorsIndicesCount = 0;
	//vector<CLinesCohort*> vecTangentVectorsCohorts;

	// IBO - Bi-Normal Vectors
	//GLuint iBiNormalVectorsIndicesCount = 0;
	//vector<CLinesCohort*> vecBiNormalVectorsCohorts;

	map<int64_t, CRDFInstance*>::const_iterator itRDFInstances = mapRDFInstances.begin();
	for (; itRDFInstances != mapRDFInstances.end(); itRDFInstances++)
	{
		CRDFInstance* pRDFInstance = itRDFInstances->second;
		if (pRDFInstance->getVerticesCount() == 0)
		{
			continue;
		}

		/******************************************************************************************
		* Geometry
		*/

		/**
		* VBO - Conceptual faces, wireframes, etc.
		*/
		if (((int_t)iVerticesCount + pRDFInstance->getVerticesCount()) > (int_t)GEOMETRY_VERTICES_MAX_COUNT)
		{
			ASSERT(!vecRDFInstancesGroup.empty());

			int_t iCohortVerticesCount = 0;
			float* pVertices = GetVertices(vecRDFInstancesGroup, iCohortVerticesCount);
			if ((iCohortVerticesCount == 0) || (pVertices == nullptr))
			{
				ASSERT(0);

				return;
			}

			ASSERT(iCohortVerticesCount == iVerticesCount);

			GLuint iVAO = 0;
			glGenVertexArrays(1, &iVAO);
			glBindVertexArray(iVAO);

			COpenGL::Check4Errors();

			GLuint iVBO = 0;
			glGenBuffers(1, &iVBO);

			ASSERT(iVBO != 0);

			COpenGL::Check4Errors();

			glBindBuffer(GL_ARRAY_BUFFER, iVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * iVerticesCount * GEOMETRY_VBO_VERTEX_LENGTH, pVertices, GL_STATIC_DRAW);

			COpenGL::Check4Errors();

			glVertexAttribPointer(m_pProgram->getVertexPosition(), 3, GL_FLOAT, false, sizeof(GLfloat) * GEOMETRY_VBO_VERTEX_LENGTH, 0);
			glVertexAttribPointer(m_pProgram->getTextureCoord(), 2, GL_FLOAT, false, sizeof(GLfloat) * GEOMETRY_VBO_VERTEX_LENGTH, (void*)(sizeof(GLfloat) * 6));
			glVertexAttribPointer(m_pProgram->getVertexNormal(), 3, GL_FLOAT, false, sizeof(GLfloat) * GEOMETRY_VBO_VERTEX_LENGTH, (void*)(sizeof(GLfloat) * 3));

			COpenGL::Check4Errors();

			glEnableVertexAttribArray(m_pProgram->getVertexPosition());
			glEnableVertexAttribArray(m_pProgram->getTextureCoord());
			glEnableVertexAttribArray(m_pProgram->getVertexNormal());

			COpenGL::Check4Errors();

			TRACE(L"\nVBO VERTICES: %d", iVerticesCount);

			/*
			* Store VBO/offset
			*/
			GLsizei iVBOOffset = 0;
			for (size_t iRDFInstance = 0; iRDFInstance < vecRDFInstancesGroup.size(); iRDFInstance++)
			{
				vecRDFInstancesGroup[iRDFInstance]->VBO() = iVBO;
				vecRDFInstancesGroup[iRDFInstance]->VBOOffset() = iVBOOffset;				

				iVBOOffset += (GLsizei)vecRDFInstancesGroup[iRDFInstance]->getVerticesCount();
			} // for (size_t iRDFInstance = ...

			delete[] pVertices;

			glBindVertexArray(0);

			COpenGL::Check4Errors();

			CDrawMetaData* pDrawMetaData = new CDrawMetaData(mdtGeometry);
			pDrawMetaData->AddGroup(iVAO, iVBO, vecRDFInstancesGroup);

			m_vecDrawMetaData.push_back(pDrawMetaData);

			iVerticesCount = 0;
			vecRDFInstancesGroup.clear();
		} // if (((int_t)iVerticesCount + pRDFInstance->getVerticesCount()) > ...		

		/*
		* IBO - Materials
		*/
		for (size_t iMaterial = 0; iMaterial < pRDFInstance->conceptualFacesMaterials().size(); iMaterial++)
		{
			if ((int_t)(iMaterialsIndicesCount + pRDFInstance->conceptualFacesMaterials()[iMaterial]->getIndicesCount()) > (int_t)INDICES_MAX_COUNT)
			{
				ASSERT(!vecRDFMaterialsGroup.empty());

				GLuint iIBO = 0;
				glGenBuffers(1, &iIBO);

				ASSERT(iIBO != 0);

				m_vecIBOs.push_back(iIBO);

				int_t iGroupIndicesCount = 0;
				unsigned int* pIndices = GetMaterialsIndices(vecRDFMaterialsGroup, iGroupIndicesCount);
				if ((iGroupIndicesCount == 0) || (pIndices == nullptr))
				{
					ASSERT(0);

					return;
				}

				ASSERT(iMaterialsIndicesCount == iGroupIndicesCount);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iMaterialsIndicesCount, pIndices, GL_STATIC_DRAW);

				delete[] pIndices;

				/*
				* Store IBO/offset
				*/
				GLsizei iIBOOffset = 0;
				for (size_t iMaterial2 = 0; iMaterial2 < vecRDFMaterialsGroup.size(); iMaterial2++)
				{
					vecRDFMaterialsGroup[iMaterial2]->IBO() = iIBO;
					vecRDFMaterialsGroup[iMaterial2]->IBOOffset() = iIBOOffset;

					iIBOOffset += (GLsizei)vecRDFMaterialsGroup[iMaterial2]->getIndicesCount();
				}

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				COpenGL::Check4Errors();

				iMaterialsIndicesCount = 0;
				vecRDFMaterialsGroup.clear();
			} // if ((int_t)(iMaterialsIndicesCount + ...	

			iMaterialsIndicesCount += (GLsizei)pRDFInstance->conceptualFacesMaterials()[iMaterial]->getIndicesCount();
			vecRDFMaterialsGroup.push_back(pRDFInstance->conceptualFacesMaterials()[iMaterial]);
		} // for (size_t iMaterial = ...	

		/*
		* IBO - Lines
		*/
		for (size_t iLinesCohort = 0; iLinesCohort < pRDFInstance->linesCohorts().size(); iLinesCohort++)
		{
			if ((int_t)(iLinesIndicesCount + pRDFInstance->linesCohorts()[iLinesCohort]->getIndicesCount()) > (int_t)INDICES_MAX_COUNT)
			{
				ASSERT(!vecLinesCohorts.empty());

				GLuint iIBO = 0;
				glGenBuffers(1, &iIBO);

				ASSERT(iIBO != 0);

				m_vecIBOs.push_back(iIBO);

				int_t iCohortIndicesCount = 0;
				unsigned int* pIndices = GetLinesCohortsIndices(vecLinesCohorts, iCohortIndicesCount);
				if ((iCohortIndicesCount == 0) || (pIndices == NULL))
				{
					ASSERT(0);

					return;
				}

				ASSERT(iLinesIndicesCount == iCohortIndicesCount);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iLinesIndicesCount, pIndices, GL_STATIC_DRAW);

				delete[] pIndices;

				/*
				* Store IBO/offset
				*/
				GLsizei iIBOOffset = 0;
				for (size_t iLinesCohort2 = 0; iLinesCohort2 < vecLinesCohorts.size(); iLinesCohort2++)
				{
					vecLinesCohorts[iLinesCohort2]->IBO() = iIBO;
					vecLinesCohorts[iLinesCohort2]->IBOOffset() = iIBOOffset;

					iIBOOffset += (GLsizei)vecLinesCohorts[iLinesCohort2]->getIndicesCount();
				} // for (size_t iLinesCohort2 = ...				

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				COpenGL::Check4Errors();

				iLinesIndicesCount = 0;
				vecLinesCohorts.clear();
			} // if ((int_t)(iLinesIndicesCount + ...	

			iLinesIndicesCount += (GLsizei)pRDFInstance->linesCohorts()[iLinesCohort]->getIndicesCount();
			vecLinesCohorts.push_back(pRDFInstance->linesCohorts()[iLinesCohort]);
		} // for (size_t iLinesCohort = ...	

		/*
		* IBO - Points
		*/
		for (size_t iPointsCohort = 0; iPointsCohort < pRDFInstance->pointsCohorts().size(); iPointsCohort++)
		{
			if ((int_t)(iPointsIndicesCount + pRDFInstance->pointsCohorts()[iPointsCohort]->getIndicesCount()) > (int_t)INDICES_MAX_COUNT)
			{
				ASSERT(!vecPointsCohorts.empty());

				GLuint iIBO = 0;
				glGenBuffers(1, &iIBO);

				ASSERT(iIBO != 0);

				m_vecIBOs.push_back(iIBO);

				int_t iCohortIndicesCount = 0;
				unsigned int* pIndices = GetPointsCohortsIndices(vecPointsCohorts, iCohortIndicesCount);
				if ((iCohortIndicesCount == 0) || (pIndices == NULL))
				{
					ASSERT(0);

					return;
				}

				ASSERT(iPointsIndicesCount == iCohortIndicesCount);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iPointsIndicesCount, pIndices, GL_STATIC_DRAW);

				delete[] pIndices;

				/*
				* Store IBO/offset
				*/
				GLsizei iIBOOffset = 0;
				for (size_t iPointsCohort2 = 0; iPointsCohort2 < vecPointsCohorts.size(); iPointsCohort2++)
				{
					vecPointsCohorts[iPointsCohort2]->IBO() = iIBO;
					vecPointsCohorts[iPointsCohort2]->IBOOffset() = iIBOOffset;

					iIBOOffset += (GLsizei)vecPointsCohorts[iPointsCohort2]->getIndicesCount();
				} // for (size_t iPointsCohort2 = ...				

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				COpenGL::Check4Errors();

				iPointsIndicesCount = 0;
				vecPointsCohorts.clear();
			} // if ((int_t)(iPointsIndicesCount + ...	

			iPointsIndicesCount += (GLsizei)pRDFInstance->pointsCohorts()[iPointsCohort]->getIndicesCount();
			vecPointsCohorts.push_back(pRDFInstance->pointsCohorts()[iPointsCohort]);
		} // for (size_t iPointsCohort = ...	

		/*
		* IBO - Conceptual Faces
		*/
		for (size_t iConceptualFacesCohort = 0; iConceptualFacesCohort < pRDFInstance->conceptualFacesCohorts().size(); iConceptualFacesCohort++)
		{
			if ((int_t)(iConceptualFacesIndicesCount + pRDFInstance->conceptualFacesCohorts()[iConceptualFacesCohort]->getIndicesCount()) > (int_t)INDICES_MAX_COUNT)
			{
				ASSERT(!vecConceptualFacesCohorts.empty());

				GLuint iIBO = 0;
				glGenBuffers(1, &iIBO);

				ASSERT(iIBO != 0);

				m_vecIBOs.push_back(iIBO);

				int_t iCohortIndicesCount = 0;
				unsigned int* pIndices = GetWireframesCohortsIndices(vecConceptualFacesCohorts, iCohortIndicesCount);
				if ((iCohortIndicesCount == 0) || (pIndices == NULL))
				{
					ASSERT(0);

					return;
				}

				ASSERT(iConceptualFacesIndicesCount == iCohortIndicesCount);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iConceptualFacesIndicesCount, pIndices, GL_STATIC_DRAW);

				delete[] pIndices;

				/*
				* Store IBO/offset
				*/
				GLsizei iIBOOffset = 0;
				for (size_t iConceptualFacesCohort2 = 0; iConceptualFacesCohort2 < vecConceptualFacesCohorts.size(); iConceptualFacesCohort2++)
				{
					vecConceptualFacesCohorts[iConceptualFacesCohort2]->IBO() = iIBO;
					vecConceptualFacesCohorts[iConceptualFacesCohort2]->IBOOffset() = iIBOOffset;

					iIBOOffset += (GLsizei)vecConceptualFacesCohorts[iConceptualFacesCohort2]->getIndicesCount();
				} // for (size_t iConceptualFacesCohort2 = ...				

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				COpenGL::Check4Errors();

				iConceptualFacesIndicesCount = 0;
				vecConceptualFacesCohorts.clear();
			} // if ((int_t)(iConceptualFacesIndicesCount + ...	

			iConceptualFacesIndicesCount += (GLsizei)pRDFInstance->conceptualFacesCohorts()[iConceptualFacesCohort]->getIndicesCount();
			vecConceptualFacesCohorts.push_back(pRDFInstance->conceptualFacesCohorts()[iConceptualFacesCohort]);
		} // for (size_t iConceptualFacesCohort = ...	

		/*
		* IBO - Faces
		*/
		for (size_t iFacesCohort = 0; iFacesCohort < pRDFInstance->facesCohorts().size(); iFacesCohort++)
		{
			if ((int_t)(iFacesIndicesCount + pRDFInstance->facesCohorts()[iFacesCohort]->getIndicesCount()) > (int_t)INDICES_MAX_COUNT)
			{
				ASSERT(!vecFacesCohorts.empty());

				GLuint iIBO = 0;
				glGenBuffers(1, &iIBO);

				ASSERT(iIBO != 0);

				m_vecIBOs.push_back(iIBO);

				int_t iCohortIndicesCount = 0;
				unsigned int* pIndices = GetWireframesCohortsIndices(vecFacesCohorts, iCohortIndicesCount);
				if ((iCohortIndicesCount == 0) || (pIndices == NULL))
				{
					ASSERT(0);

					return;
				}

				ASSERT(iFacesIndicesCount == iCohortIndicesCount);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iFacesIndicesCount, pIndices, GL_STATIC_DRAW);

				delete[] pIndices;

				/*
				* Store IBO/offset
				*/
				GLsizei iIBOOffset = 0;
				for (size_t iFacesCohort2 = 0; iFacesCohort2 < vecFacesCohorts.size(); iFacesCohort2++)
				{
					vecFacesCohorts[iFacesCohort2]->IBO() = iIBO;
					vecFacesCohorts[iFacesCohort2]->IBOOffset() = iIBOOffset;

					iIBOOffset += (GLsizei)vecFacesCohorts[iFacesCohort2]->getIndicesCount();
				} // for (size_t iFacesCohort2 = ...				

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				COpenGL::Check4Errors();

				iFacesIndicesCount = 0;
				vecFacesCohorts.clear();
			} // if ((int_t)(iFacesIndicesCount + ...	

			iFacesIndicesCount += (GLsizei)pRDFInstance->facesCohorts()[iFacesCohort]->getIndicesCount();
			vecFacesCohorts.push_back(pRDFInstance->facesCohorts()[iFacesCohort]);
		} // for (size_t iConceptualFacesCohort = ...			

		iVerticesCount += (GLsizei)pRDFInstance->getVerticesCount();
		vecRDFInstancesGroup.push_back(pRDFInstance);
	} // for (; itRDFInstances != ...

	/******************************************************************************************
	* Geometry
	*/

	/*
	* VBO - Conceptual faces, wireframes, etc.
	*/
	if (iVerticesCount > 0)
	{
		ASSERT(!vecRDFInstancesGroup.empty());

		int_t iCohortVerticesCount = 0;
		float* pVertices = GetVertices(vecRDFInstancesGroup, iCohortVerticesCount);
		if ((iCohortVerticesCount == 0) || (pVertices == nullptr))
		{
			ASSERT(0);

			return;
		}

		ASSERT(iCohortVerticesCount == iVerticesCount);

		GLuint iVAO = 0;
		glGenVertexArrays(1, &iVAO);
		glBindVertexArray(iVAO);

		COpenGL::Check4Errors();

		GLuint iVBO = 0;
		glGenBuffers(1, &iVBO);

		ASSERT(iVBO != 0);

		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * iVerticesCount * GEOMETRY_VBO_VERTEX_LENGTH, pVertices, GL_STATIC_DRAW);

		COpenGL::Check4Errors();

		glVertexAttribPointer(m_pProgram->getVertexPosition(), 3, GL_FLOAT, false, sizeof(GLfloat)* GEOMETRY_VBO_VERTEX_LENGTH, 0);
		glVertexAttribPointer(m_pProgram->getTextureCoord(), 2, GL_FLOAT, false, sizeof(GLfloat)* GEOMETRY_VBO_VERTEX_LENGTH, (void*)(sizeof(GLfloat) * 6));
		glVertexAttribPointer(m_pProgram->getVertexNormal(), 3, GL_FLOAT, false, sizeof(GLfloat)* GEOMETRY_VBO_VERTEX_LENGTH, (void*)(sizeof(GLfloat) * 3));

		COpenGL::Check4Errors();

		glEnableVertexAttribArray(m_pProgram->getVertexPosition());
		glEnableVertexAttribArray(m_pProgram->getTextureCoord());
		glEnableVertexAttribArray(m_pProgram->getVertexNormal());

		COpenGL::Check4Errors();

		TRACE(L"\nVBO VERTICES: %d", iVerticesCount);

		/*
		* Store VBO/offset
		*/
		GLsizei iVBOOffset = 0;
		for (size_t iRDFInstance = 0; iRDFInstance < vecRDFInstancesGroup.size(); iRDFInstance++)
		{
			vecRDFInstancesGroup[iRDFInstance]->VBO() = iVBO;
			vecRDFInstancesGroup[iRDFInstance]->VBOOffset() = iVBOOffset;

			iVBOOffset += (GLsizei)vecRDFInstancesGroup[iRDFInstance]->getVerticesCount();
		} // for (size_t iRDFInstance = ...

		delete[] pVertices;

		glBindVertexArray(0);

		COpenGL::Check4Errors();

		CDrawMetaData* pDrawMetaData = new CDrawMetaData(mdtGeometry);
		pDrawMetaData->AddGroup(iVAO, iVBO, vecRDFInstancesGroup);

		m_vecDrawMetaData.push_back(pDrawMetaData);

		iVerticesCount = 0;
		vecRDFInstancesGroup.clear();
	} // if (iVerticesCount > 0)	

	/*
	* IBO - Materials
	*/
	if (iMaterialsIndicesCount > 0)
	{
		ASSERT(!vecRDFMaterialsGroup.empty());

		GLuint iIBO = 0;
		glGenBuffers(1, &iIBO);

		ASSERT(iIBO != 0);

		m_vecIBOs.push_back(iIBO);

		int_t iGroupIndicesCount = 0;
		unsigned int* pIndices = GetMaterialsIndices(vecRDFMaterialsGroup, iGroupIndicesCount);
		if ((iGroupIndicesCount == 0) || (pIndices == nullptr))
		{
			ASSERT(0);

			return;
		}

		ASSERT(iMaterialsIndicesCount == iGroupIndicesCount);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iMaterialsIndicesCount, pIndices, GL_STATIC_DRAW);

		delete[] pIndices;

		/*
		* Store IBO/offset
		*/
		GLsizei iIBOOffset = 0;
		for (size_t iMaterial2 = 0; iMaterial2 < vecRDFMaterialsGroup.size(); iMaterial2++)
		{
			vecRDFMaterialsGroup[iMaterial2]->IBO() = iIBO;
			vecRDFMaterialsGroup[iMaterial2]->IBOOffset() = iIBOOffset;

			iIBOOffset += (GLsizei)vecRDFMaterialsGroup[iMaterial2]->getIndicesCount();
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		COpenGL::Check4Errors();

		iMaterialsIndicesCount = 0;
		vecRDFMaterialsGroup.clear();
	} // if (iMaterialsIndicesCount > 0)

	/*
	* IBO - Lines
	*/
	if (iLinesIndicesCount > 0)
	{
		ASSERT(!vecLinesCohorts.empty());

		GLuint iIBO = 0;
		glGenBuffers(1, &iIBO);

		ASSERT(iIBO != 0);

		m_vecIBOs.push_back(iIBO);

		int_t iCohortIndicesCount = 0;
		unsigned int* pIndices = GetLinesCohortsIndices(vecLinesCohorts, iCohortIndicesCount);
		if ((iCohortIndicesCount == 0) || (pIndices == NULL))
		{
			ASSERT(0);

			return;
		}

		ASSERT(iLinesIndicesCount == iCohortIndicesCount);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iLinesIndicesCount, pIndices, GL_STATIC_DRAW);

		delete[] pIndices;

		/*
		* Store IBO/offset
		*/
		GLsizei iIBOOffset = 0;
		for (size_t iLinesCohort2 = 0; iLinesCohort2 < vecLinesCohorts.size(); iLinesCohort2++)
		{
			vecLinesCohorts[iLinesCohort2]->IBO() = iIBO;
			vecLinesCohorts[iLinesCohort2]->IBOOffset() = iIBOOffset;

			iIBOOffset += (GLsizei)vecLinesCohorts[iLinesCohort2]->getIndicesCount();
		} // for (size_t iLinesCohort2 = ...				

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		COpenGL::Check4Errors();

		iLinesIndicesCount = 0;
		vecLinesCohorts.clear();
	} // if (iLinesIndicesCount > 0)		

	if (iPointsIndicesCount > 0)
	{
		ASSERT(!vecPointsCohorts.empty());

		GLuint iIBO = 0;
		glGenBuffers(1, &iIBO);

		ASSERT(iIBO != 0);

		m_vecIBOs.push_back(iIBO);

		int_t iCohortIndicesCount = 0;
		unsigned int* pIndices = GetPointsCohortsIndices(vecPointsCohorts, iCohortIndicesCount);
		if ((iCohortIndicesCount == 0) || (pIndices == NULL))
		{
			ASSERT(0);

			return;
		}

		ASSERT(iPointsIndicesCount == iCohortIndicesCount);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iPointsIndicesCount, pIndices, GL_STATIC_DRAW);

		delete[] pIndices;

		/*
		* Store IBO/offset
		*/
		GLsizei iIBOOffset = 0;
		for (size_t iPointsCohort2 = 0; iPointsCohort2 < vecPointsCohorts.size(); iPointsCohort2++)
		{
			vecPointsCohorts[iPointsCohort2]->IBO() = iIBO;
			vecPointsCohorts[iPointsCohort2]->IBOOffset() = iIBOOffset;

			iIBOOffset += (GLsizei)vecPointsCohorts[iPointsCohort2]->getIndicesCount();
		} // for (size_t iPointsCohort2 = ...				

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		COpenGL::Check4Errors();

		iPointsIndicesCount = 0;
		vecPointsCohorts.clear();
	} // if (iPointsIndicesCount > 0)

	/*
	* IBO - Conceptual Faces
	*/
	if (iConceptualFacesIndicesCount > 0)
	{
		ASSERT(!vecConceptualFacesCohorts.empty());

		GLuint iIBO = 0;
		glGenBuffers(1, &iIBO);

		ASSERT(iIBO != 0);

		m_vecIBOs.push_back(iIBO);

		int_t iCohortIndicesCount = 0;
		unsigned int* pIndices = GetWireframesCohortsIndices(vecConceptualFacesCohorts, iCohortIndicesCount);
		if ((iCohortIndicesCount == 0) || (pIndices == NULL))
		{
			ASSERT(0);

			return;
		}

		ASSERT(iConceptualFacesIndicesCount == iCohortIndicesCount);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iConceptualFacesIndicesCount, pIndices, GL_STATIC_DRAW);

		delete[] pIndices;

		/*
		* Store IBO/offset
		*/
		GLsizei iIBOOffset = 0;
		for (size_t iConceptualFacesCohort2 = 0; iConceptualFacesCohort2 < vecConceptualFacesCohorts.size(); iConceptualFacesCohort2++)
		{
			vecConceptualFacesCohorts[iConceptualFacesCohort2]->IBO() = iIBO;
			vecConceptualFacesCohorts[iConceptualFacesCohort2]->IBOOffset() = iIBOOffset;

			iIBOOffset += (GLsizei)vecConceptualFacesCohorts[iConceptualFacesCohort2]->getIndicesCount();
		} // for (size_t iConceptualFacesCohort2 = ...				

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		COpenGL::Check4Errors();

		iConceptualFacesIndicesCount = 0;
		vecConceptualFacesCohorts.clear();
	} // if (iConceptualFacesIndicesCount > 0)

	/*
	* IBO - Faces
	*/
	if (iFacesIndicesCount > 0)
	{
		ASSERT(!vecFacesCohorts.empty());

		GLuint iIBO = 0;
		glGenBuffers(1, &iIBO);

		ASSERT(iIBO != 0);

		m_vecIBOs.push_back(iIBO);

		int_t iCohortIndicesCount = 0;
		unsigned int* pIndices = GetWireframesCohortsIndices(vecFacesCohorts, iCohortIndicesCount);
		if ((iCohortIndicesCount == 0) || (pIndices == NULL))
		{
			ASSERT(0);

			return;
		}

		ASSERT(iFacesIndicesCount == iCohortIndicesCount);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iFacesIndicesCount, pIndices, GL_STATIC_DRAW);

		delete[] pIndices;

		/*
		* Store IBO/offset
		*/
		GLsizei iIBOOffset = 0;
		for (size_t iFacesCohort2 = 0; iFacesCohort2 < vecFacesCohorts.size(); iFacesCohort2++)
		{
			vecFacesCohorts[iFacesCohort2]->IBO() = iIBO;
			vecFacesCohorts[iFacesCohort2]->IBOOffset() = iIBOOffset;

			iIBOOffset += (GLsizei)vecFacesCohorts[iFacesCohort2]->getIndicesCount();
		} // for (size_t iFacesCohort2 = ...				

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		COpenGL::Check4Errors();

		iFacesIndicesCount = 0;
		vecFacesCohorts.clear();
	} // if (iFacesIndicesCount > 0)
	
#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnWorldDimensionsChanged()
{
	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	ASSERT(pModel != NULL);

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

	// TODO: UPDATE THE BUFFERS
	OnModelChanged();

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnInstancePropertyEdited(CRDFInstance * /*pInstance*/, CRDFProperty * /*pProperty*/)
{
	/*
	* Reload model
	*/
	OnModelChanged();

	/*
	* Restore the selection
	*/
	OnInstanceSelected(NULL);
}

// ------------------------------------------------------------------------------------------------
#ifdef _DEBUG_OCTREE
/*virtual*/ void COpenGLRDFView::OnNewInstanceCreated(CRDFView * /*pSender*/, CRDFInstance * pInstance)
#else
/*virtual*/ void COpenGLRDFView::OnNewInstanceCreated(CRDFView* /*pSender*/, CRDFInstance* /*pInstance*/)
#endif // _DEBUG_OCTREE
{
	/*
	* Reload model
	*/
	OnModelChanged();

#ifdef _DEBUG_OCTREE 
	if (COctree::s_bStepByStepMode)
	{
		ASSERT(GetController() != NULL);
		GetController()->ZoomToInstance(pInstance->getInstance());

		while (true)
		{
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0))
			{
				if (msg.message == WM_KEYDOWN)
				{
					switch (msg.wParam)
					{
					case VK_RETURN:
						return;

					default:
						break;
					}
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	} // if (COcttree::s_bStepByStepMode)
#else
	/*
	* Restore the selection
	*/
	OnInstanceSelected(NULL);
#endif // _DEBUG_OCTREE 	
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnInstanceDeleted(CRDFView * /*pSender*/, int64_t /*iInstance*/)
{
	/*
	* Reload model
	*/
	OnModelChanged();

	/*
	* Restore the selection
	*/
	OnInstanceSelected(NULL);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnInstancesDeleted(CRDFView * /*pSender*/)
{
	/*
	* Reload model
	*/
	OnModelChanged();

	/*
	* Restore the selection
	*/
	OnInstanceSelected(NULL);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnMeasurementsAdded(CRDFView * /*pSender*/, CRDFInstance * /*pInstance*/)
{
	/*
	* Reload model
	*/
	OnModelChanged();

	/*
	* Restore the selection
	*/
	OnInstanceSelected(NULL);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::Reset()
{
	m_fXAngle = 30.0f;
	m_fYAngle = 30.0f;
	m_fXTranslation = 0.0f;
	m_fYTranslation = 0.0f;
	m_fZTranslation = -5.0f;

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

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnInstanceSelected(CRDFView * pSender)
{
	if (pSender == this)
	{
		return;
	}

	ASSERT(GetController() != NULL);

	CRDFInstance * pSelectedInstance = GetController()->GetSelectedInstance();

	if ((pSelectedInstance != NULL) && (!pSelectedInstance->hasGeometry() || pSelectedInstance->getTriangles().empty()))
	{
		pSelectedInstance = NULL;
	}

	if (m_pSelectedInstance != pSelectedInstance)
	{
		m_iPointedFace = -1;

		m_pSelectedInstance = pSelectedInstance;

		m_arSelectedPoint[0] = -FLT_MAX;
		m_arSelectedPoint[1] = -FLT_MAX;
		m_arSelectedPoint[2] = -FLT_MAX;

		m_mapFacesSelectionColors.clear();

#ifdef _LINUX
        m_pWnd->Refresh(false);
#else
        m_pWnd->RedrawWindow();
#endif // _LINUX
	}
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnInstancePropertySelected()
{
	ASSERT(GetController() != NULL);

	pair<CRDFInstance *, CRDFProperty *> prSelectedInstanceProperty = GetController()->GetSelectedInstanceProperty();

	CRDFInstance * pSelectedInstance = prSelectedInstanceProperty.first;

	if ((pSelectedInstance != NULL) && (!pSelectedInstance->hasGeometry() || pSelectedInstance->getTriangles().empty()))
	{
		pSelectedInstance = NULL;
	}

	if (m_pSelectedInstance != pSelectedInstance)
	{
		m_iPointedFace = -1;

		m_pSelectedInstance = pSelectedInstance;

		m_arSelectedPoint[0] = -FLT_MAX;
		m_arSelectedPoint[1] = -FLT_MAX;
		m_arSelectedPoint[2] = -FLT_MAX;

		m_mapFacesSelectionColors.clear();

#ifdef _LINUX
        m_pWnd->Refresh(false);
#else
        m_pWnd->RedrawWindow();
#endif // _LINUX
	}
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnInstancesEnabledStateChanged()
{
	/*
	* Reload model
	*/
	OnModelChanged();

	/*
	* Restore the selection
	*/
	OnInstanceSelected(NULL);
}

// ------------------------------------------------------------------------------------------------
/*virtual*/ void COpenGLRDFView::OnControllerChanged()
{
	ASSERT(GetController() != NULL);

	GetController()->RegisterView(this);
}

// ------------------------------------------------------------------------------------------------
float* COpenGLRDFView::GetVertices(const vector<CRDFInstance*>& vecRDFInstances, int_t& iVerticesCount)
{
	iVerticesCount = 0;
	for (size_t iRDFInstance = 0; iRDFInstance < vecRDFInstances.size(); iRDFInstance++)
	{
		iVerticesCount += vecRDFInstances[iRDFInstance]->getVerticesCount();
	}

	float* pVertices = new float[iVerticesCount * GEOMETRY_VBO_VERTEX_LENGTH];

	int_t iOffset = 0;
	for (size_t iRDFInstance = 0; iRDFInstance < vecRDFInstances.size(); iRDFInstance++)
	{
		float* pFacesVertices = vecRDFInstances[iRDFInstance]->BuildFacesVertices();

		memcpy((float*)pVertices + iOffset, pFacesVertices,
			vecRDFInstances[iRDFInstance]->getVerticesCount() * GEOMETRY_VBO_VERTEX_LENGTH * sizeof(float));

		delete[] pFacesVertices;

		iOffset += vecRDFInstances[iRDFInstance]->getVerticesCount() * GEOMETRY_VBO_VERTEX_LENGTH;
	}

	return pVertices;
}

// ------------------------------------------------------------------------------------------------
unsigned int* COpenGLRDFView::GetMaterialsIndices(const vector<CRDFGeometryWithMaterial*> & vecRDFMaterials, int_t & iIndicesCount)
{
	iIndicesCount = 0;
	for (size_t iMaterial = 0; iMaterial < vecRDFMaterials.size(); iMaterial++)
	{
		iIndicesCount += vecRDFMaterials[iMaterial]->getIndicesCount();
	}

	unsigned int* pIndices = new unsigned int[iIndicesCount];

	int_t iOffset = 0;
	for (size_t iMaterial = 0; iMaterial < vecRDFMaterials.size(); iMaterial++)
	{
		memcpy((unsigned int*)pIndices + iOffset, vecRDFMaterials[iMaterial]->getIndices(),
			vecRDFMaterials[iMaterial]->getIndicesCount() * sizeof(unsigned int));

		iOffset += vecRDFMaterials[iMaterial]->getIndicesCount();
	}

	return pIndices;
}

// ------------------------------------------------------------------------------------------------
unsigned int* COpenGLRDFView::GetLinesCohortsIndices(const vector<CLinesCohort*>& vecLinesCohorts, int_t& iIndicesCount)
{
	iIndicesCount = 0;
	for (size_t iCohort = 0; iCohort < vecLinesCohorts.size(); iCohort++)
	{
		iIndicesCount += vecLinesCohorts[iCohort]->getIndicesCount();
	}

	unsigned int* pIndices = new unsigned int[iIndicesCount];

	int_t iOffset = 0;
	for (size_t iCohort = 0; iCohort < vecLinesCohorts.size(); iCohort++)
	{
		memcpy((unsigned int*)pIndices + iOffset, vecLinesCohorts[iCohort]->getIndices(),
			vecLinesCohorts[iCohort]->getIndicesCount() * sizeof(unsigned int));

		iOffset += vecLinesCohorts[iCohort]->getIndicesCount();
	}

	return pIndices;
}

// ------------------------------------------------------------------------------------------------
unsigned int* COpenGLRDFView::GetPointsCohortsIndices(const vector<CPointsCohort*>& vecPointsCohorts, int_t& iIndicesCount)
{
	iIndicesCount = 0;
	for (size_t iCohort = 0; iCohort < vecPointsCohorts.size(); iCohort++)
	{
		iIndicesCount += vecPointsCohorts[iCohort]->getIndicesCount();
	}

	unsigned int* pIndices = new unsigned int[iIndicesCount];

	int_t iOffset = 0;
	for (size_t iCohort = 0; iCohort < vecPointsCohorts.size(); iCohort++)
	{
		memcpy((unsigned int*)pIndices + iOffset, vecPointsCohorts[iCohort]->getIndices(),
			vecPointsCohorts[iCohort]->getIndicesCount() * sizeof(unsigned int));

		iOffset += vecPointsCohorts[iCohort]->getIndicesCount();
	}

	return pIndices;
}

// ------------------------------------------------------------------------------------------------
unsigned int* COpenGLRDFView::GetWireframesCohortsIndices(const vector<CWireframesCohort*>& vecWireframesCohorts, int_t& iIndicesCount)
{
	iIndicesCount = 0;
	for (size_t iCohort = 0; iCohort < vecWireframesCohorts.size(); iCohort++)
	{
		iIndicesCount += vecWireframesCohorts[iCohort]->getIndicesCount();
	}

	unsigned int* pIndices = new unsigned int[iIndicesCount];

	int_t iOffset = 0;
	for (size_t iCohort = 0; iCohort < vecWireframesCohorts.size(); iCohort++)
	{
		if (vecWireframesCohorts[iCohort]->getIndicesCount() == 0)
		{
			continue;
		}

		memcpy((unsigned int*)pIndices + iOffset, vecWireframesCohorts[iCohort]->getIndices(),
			vecWireframesCohorts[iCohort]->getIndicesCount() * sizeof(unsigned int));

		iOffset += vecWireframesCohorts[iCohort]->getIndicesCount();
	}

	return pIndices;
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::ReleaseBuffers()
{
	/*for (size_t iBuffers = 0; iBuffers < m_vecFacesVBOs.size(); iBuffers++)
	{
		glDeleteBuffers(1, &m_vecFacesVBOs[iBuffers]);
	}
	m_vecFacesVBOs.clear();

	for (size_t iBuffers = 0; iBuffers < m_vecFacesIBOs.size(); iBuffers++)
	{
		delete m_vecFacesIBOs[iBuffers];
	}
	m_vecFacesIBOs.clear();

	for (size_t iBuffers = 0; iBuffers < m_vecWireframesVBOs.size(); iBuffers++)
	{
		glDeleteBuffers(1, &m_vecWireframesVBOs[iBuffers]);
	}
	m_vecWireframesVBOs.clear();

	for (size_t iBuffers = 0; iBuffers < m_vecWireframesIBOs.size(); iBuffers++)
	{
		delete m_vecWireframesIBOs[iBuffers];
	}
	m_vecWireframesIBOs.clear();*/
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawClipSpace()
{
	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();

	glDisable(GL_LIGHTING);

	glLineWidth(m_fLineWidth);
	glColor3f(0.0f, 0.0f, 0.0f);

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	if (pModel != NULL)
	{
		pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);
	}

	glBegin(GL_LINE_LOOP);
	glVertex3f(fXmax, fYmax, fZmax);
	glVertex3f(fXmin, fYmax, fZmax);
	glVertex3f(fXmin, fYmin, fZmax);
	glVertex3f(fXmax, fYmin, fZmax);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(fXmin, fYmin, fZmin);
	glVertex3f(fXmin, fYmax, fZmin);
	glVertex3f(fXmax, fYmax, fZmin);
	glVertex3f(fXmax, fYmin, fZmin);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(fXmax, fYmax, fZmax);
	glVertex3f(fXmax, fYmax, fZmin);
	glVertex3f(fXmin, fYmax, fZmin);
	glVertex3f(fXmin, fYmax, fZmax);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(fXmin, fYmin, fZmin);
	glVertex3f(fXmax, fYmin, fZmin);
	glVertex3f(fXmax, fYmin, fZmax);
	glVertex3f(fXmin, fYmin, fZmax);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(fXmax, fYmax, fZmax);
	glVertex3f(fXmax, fYmin, fZmax);
	glVertex3f(fXmax, fYmin, fZmin);
	glVertex3f(fXmax, fYmax, fZmin);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(fXmin, fYmin, fZmin);
	glVertex3f(fXmin, fYmin, fZmax);
	glVertex3f(fXmin, fYmax, fZmax);
	glVertex3f(fXmin, fYmax, fZmin);
	glEnd();

	glEnable(GL_LIGHTING);

	COpenGL::Check4Errors();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawFaces(bool bTransparent)
{
	if (!m_bShowFaces)
	{
		return;
	}

	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	if (pModel == NULL)
	{
		return;
	}

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

#ifdef _USE_SHADERS
	if (bTransparent)
	{
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		if ((m_strCullFaces == CULL_FACES_FRONT) || (m_strCullFaces == CULL_FACES_BACK))
		{
			glEnable(GL_CULL_FACE);
			glCullFace(m_strCullFaces == CULL_FACES_FRONT ? GL_FRONT : GL_BACK);
		}
	}

	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->geUseBinnPhongModel(),
		1.f);

	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtGeometry)
		{
			continue;
		}

		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
		for (; itGroups != mapGroups.end(); itGroups++)
		{
			glBindVertexArray(itGroups->first);		

			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
			{
				CRDFInstance* pRDFInstance = itGroups->second[iObject];

				if (!pRDFInstance->getEnable())
				{
					continue;
				}

				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
				{
					continue;
				}

				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
				{
					continue;
				}

				/*
				* Conceptual faces
				*/
				for (size_t iGeometryWithMaterial = 0; iGeometryWithMaterial < pRDFInstance->conceptualFacesMaterials().size(); iGeometryWithMaterial++)
				{
					CRDFGeometryWithMaterial* pGeometryWithMaterial = pRDFInstance->conceptualFacesMaterials()[iGeometryWithMaterial];

					const CRDFMaterial* pMaterial =
						pRDFInstance == m_pSelectedInstance ? m_pSelectedInstanceMaterial :
						pRDFInstance == m_pPointedInstance ? m_pPointedInstanceMaterial :
						pGeometryWithMaterial->getMaterial();

					if (bTransparent)
					{
						if (pMaterial->A() == 1.0)
						{
							continue;
						}
					}
					else
					{
						if (pMaterial->A() < 1.0)
						{
							continue;
						}
					}

					/*
					* Material - Texture
					*/
					if (pMaterial->hasTexture())
					{
						glProgramUniform1f(
							m_pProgram->GetID(),
							m_pProgram->geUseTexture(),
							1.f);

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, pModel->GetDefaultTexture()->TexName());

						glProgramUniform1i(
							m_pProgram->GetID(),
							m_pProgram->getSampler(),
							0);
					} // if (pMaterial->hasTexture())
					else
					{
						/*
						* Material - Ambient color
						*/
						glProgramUniform3f(m_pProgram->GetID(),
							m_pProgram->getMaterialAmbientColor(),
							pMaterial->getAmbientColor().R(),
							pMaterial->getAmbientColor().G(),
							pMaterial->getAmbientColor().B());

						/*
						* Material - Transparency
						*/
						glProgramUniform1f(
							m_pProgram->GetID(),
							m_pProgram->getTransparency(),
							pMaterial->A());

						/*
						* Material - Diffuse color
						*/
						glProgramUniform3f(m_pProgram->GetID(),
							m_pProgram->getMaterialDiffuseColor(),
							pMaterial->getDiffuseColor().R() / 2.f,
							pMaterial->getDiffuseColor().G() / 2.f,
							pMaterial->getDiffuseColor().B() / 2.f);

						/*
						* Material - Specular color
						*/
						glProgramUniform3f(m_pProgram->GetID(),
							m_pProgram->getMaterialSpecularColor(),
							pMaterial->getSpecularColor().R() / 2.f,
							pMaterial->getSpecularColor().G() / 2.f,
							pMaterial->getSpecularColor().B() / 2.f);

						/*
						* Material - Emissive color
						*/
						glProgramUniform3f(m_pProgram->GetID(),
							m_pProgram->getMaterialEmissiveColor(),
							pMaterial->getEmissiveColor().R() / 3.f,
							pMaterial->getEmissiveColor().G() / 3.f,
							pMaterial->getEmissiveColor().B() / 3.f);
					} // else if (pMaterial->hasTexture())					

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pGeometryWithMaterial->IBO());
					glDrawElementsBaseVertex(GL_TRIANGLES,
						(GLsizei)pGeometryWithMaterial->getIndicesCount(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pGeometryWithMaterial->IBOOffset()),
						pRDFInstance->VBOOffset());

					if (pMaterial->hasTexture())
					{
						glProgramUniform1f(
							m_pProgram->GetID(),
							m_pProgram->geUseTexture(),
							0.f);
					}
				} // for (size_t iMaterial = ...
			} // for (size_t iObject = ...

			glBindVertexArray(0);
		} // for (; itGroups != ...
	} // for (size_t iDrawMetaData = ...

	if (bTransparent)
	{
		glDisable(GL_BLEND);
	}
	else
	{
		if ((m_strCullFaces == CULL_FACES_FRONT) || (m_strCullFaces == CULL_FACES_BACK))
		{
			glDisable(GL_CULL_FACE);
		}
	}
#else 
	if (bTransparent)
	{
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		if ((m_strCullFaces == CULL_FACES_FRONT) || (m_strCullFaces == CULL_FACES_BACK))
		{
			glEnable(GL_CULL_FACE);
			glCullFace(m_strCullFaces == CULL_FACES_FRONT ? GL_FRONT : GL_BACK);
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtGeometry)
		{
			continue;
		}

		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
		for (; itGroups != mapGroups.end(); itGroups++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, itGroups->first);
			glVertexPointer(3, GL_FLOAT, sizeof(GLfloat) * GEOMETRY_VBO_VERTEX_LENGTH, NULL);
			glNormalPointer(GL_FLOAT, sizeof(GLfloat) * GEOMETRY_VBO_VERTEX_LENGTH, (float*)(sizeof(GLfloat) * 3));

			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
			{
				CRDFInstance* pRDFInstance = itGroups->second[iObject];

				if (!pRDFInstance->getEnable())
				{
					continue;
				}

				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
				{
					continue;
				}

				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
				{
					continue;
				}

				/*
				* Conceptual faces
				*/
				for (size_t iGeometryWithMaterial = 0; iGeometryWithMaterial < pRDFInstance->conceptualFacesMaterials().size(); iGeometryWithMaterial++)
				{
					CRDFGeometryWithMaterial* pGeometryWithMaterial = pRDFInstance->conceptualFacesMaterials()[iGeometryWithMaterial];

					const CRDFMaterial* pMaterial =
						pRDFInstance == m_pSelectedInstance ? m_pSelectedInstanceMaterial :
						pRDFInstance == m_pPointedInstance ? m_pPointedInstanceMaterial :
						pGeometryWithMaterial->getMaterial();

					if (bTransparent)
					{
						if (pMaterial->A() == 1.0)
						{
							continue;
						}
					}
					else
					{
						if (pMaterial->A() < 1.0)
						{
							continue;
						}
					}

					/*
					* Material - Texture
					*/
					if (pMaterial->hasTexture())
					{
						glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
						glColor4f(
							1.f,
							1.f,
							1.f,
							1.f);

						glEnable(GL_TEXTURE_2D);
						glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

						glBindTexture(GL_TEXTURE_2D, pModel->GetDefaultTexture()->TexName());

						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
						glTexCoordPointer(2, GL_FLOAT, sizeof(GLfloat) * GEOMETRY_VBO_VERTEX_LENGTH, (float*)(sizeof(GLfloat) * 6));
					} // if (pMaterial->hasTexture())
					else
					{
						/*
						* Material - Ambient color
						*/
						glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
						glColor4f(
							pMaterial->getAmbientColor().R(),
							pMaterial->getAmbientColor().G(),
							pMaterial->getAmbientColor().B(),
							pMaterial->A());

						/*
						* Material - Diffuse color
						*/
						glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
						glColor4f(
							pMaterial->getDiffuseColor().R(),
							pMaterial->getDiffuseColor().G(),
							pMaterial->getDiffuseColor().B(),
							pMaterial->A());

						/*
						* Material - Specular color
						*/
						glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
						glColor4f(
							pMaterial->getSpecularColor().R(),
							pMaterial->getSpecularColor().G(),
							pMaterial->getSpecularColor().B(),
							pMaterial->A());

						/*
						* Material - Emissive color
						*/
						glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
						glColor4f(
							pMaterial->getEmissiveColor().R(),
							pMaterial->getEmissiveColor().G(),
							pMaterial->getEmissiveColor().B(),
							pMaterial->A());
					} // else if (pMaterial->hasTexture())					

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pGeometryWithMaterial->IBO());

					glDrawElementsBaseVertex(GL_TRIANGLES,
						(GLsizei)pGeometryWithMaterial->getIndicesCount(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pGeometryWithMaterial->IBOOffset()),
						pRDFInstance->VBOOffset());

					//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

					if (pMaterial->hasTexture())
					{
						glDisable(GL_TEXTURE_2D);
						glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					}
				} // for (size_t iMaterial = ...
			} // for (size_t iObject = ...

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		} // for (; itGroups != ...
	} // for (size_t iDrawMetaData = ...

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	if (bTransparent)
	{
		glDisable(GL_BLEND);
	}
	else
	{
		if ((m_strCullFaces == CULL_FACES_FRONT) || (m_strCullFaces == CULL_FACES_BACK))
		{
			glDisable(GL_CULL_FACE);
		}
	}
#endif // _USE_SHADERS	

	COpenGL::Check4Errors();

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawFaces() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());	
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawFacesPolygons()
{
	if (!m_bShowFacesPolygons)
	{
		return;
	}

	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	if (pModel == NULL)
	{
		return;
	}

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

#ifdef _USE_SHADERS
	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->geUseBinnPhongModel(),
		0.f);

	glProgramUniform3f(
		m_pProgram->GetID(),
		m_pProgram->getMaterialAmbientColor(),
		0.f,
		0.f,
		0.f);

	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->getTransparency(),
		1.f);

	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtGeometry)
		{
			continue;
		}

		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
		for (; itGroups != mapGroups.end(); itGroups++)
		{
			glBindVertexArray(itGroups->first);

			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
			{
				CRDFInstance* pRDFInstance = itGroups->second[iObject];

				if (!pRDFInstance->getEnable())
				{
					continue;
				}

				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
				{
					continue;
				}

				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
				{
					continue;
				}

				/*
				* Wireframes
				*/
				for (size_t iWireframesCohort = 0; iWireframesCohort < pRDFInstance->facesCohorts().size(); iWireframesCohort++)
				{
					CWireframesCohort* pWireframesCohort = pRDFInstance->facesCohorts()[iWireframesCohort];

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pWireframesCohort->IBO());
					glDrawElementsBaseVertex(GL_LINES,
						(GLsizei)pWireframesCohort->getIndicesCount(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pWireframesCohort->IBOOffset()),
						pRDFInstance->VBOOffset());
				} // for (size_t iWireframesCohort = ...
			} // for (size_t iObject = ...

			glBindVertexArray(0);
		} // for (; itGroups != ...
	} // for (size_t iDrawMetaData = ...
#else
	glDisable(GL_LIGHTING);

	glLineWidth(1.0);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0);

	glEnableClientState(GL_VERTEX_ARRAY);

	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtGeometry)
		{
			continue;
		}

		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
		for (; itGroups != mapGroups.end(); itGroups++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, itGroups->first);
			glVertexPointer(3, GL_FLOAT, sizeof(GLfloat) * GEOMETRY_VBO_VERTEX_LENGTH, NULL);

			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
			{
				CRDFInstance* pRDFInstance = itGroups->second[iObject];

				if (!pRDFInstance->getEnable())
				{
					continue;
				}

				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
				{
					continue;
				}

				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
				{
					continue;
				}

				/*
				* Wireframes
				*/
				for (size_t iWireframesCohort = 0; iWireframesCohort < pRDFInstance->facesCohorts().size(); iWireframesCohort++)
				{
					CWireframesCohort* pWireframesCohort = pRDFInstance->facesCohorts()[iWireframesCohort];

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pWireframesCohort->IBO());

					glDrawElementsBaseVertex(GL_LINES,
						(GLsizei)pWireframesCohort->getIndicesCount(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pWireframesCohort->IBOOffset()),
						pRDFInstance->VBOOffset());

					//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				} // for (size_t iWireframesCohort = ...
			} // for (size_t iObject = ...

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		} // for (; itGroups != ...
	} // for (size_t iDrawMetaData = ...

	glDisableClientState(GL_VERTEX_ARRAY);

	glEnable(GL_LIGHTING);
#endif // #ifdef _USE_SHADERS

	COpenGL::Check4Errors();

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawFacesPolygons() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawConceptualFacesPolygons()
{
	if (!m_bShowConceptualFacesPolygons)
	{
		return;
	}

	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	if (pModel == NULL)
	{
		return;
	}

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

#ifdef _USE_SHADERS
	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->geUseBinnPhongModel(),
		0.f);

	glProgramUniform3f(
		m_pProgram->GetID(),
		m_pProgram->getMaterialAmbientColor(),
		0.f,
		0.f,
		0.f);

	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->getTransparency(),
		1.f);

	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtGeometry)
		{
			continue;
		}

		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
		for (; itGroups != mapGroups.end(); itGroups++)
		{
			glBindVertexArray(itGroups->first);

			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
			{
				CRDFInstance* pRDFInstance = itGroups->second[iObject];

				if (!pRDFInstance->getEnable())
				{
					continue;
				}

				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
				{
					continue;
				}

				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
				{
					continue;
				}

				/*
				* Wireframes
				*/
				for (size_t iWireframesCohort = 0; iWireframesCohort < pRDFInstance->conceptualFacesCohorts().size(); iWireframesCohort++)
				{
					CWireframesCohort* pWireframesCohort = pRDFInstance->conceptualFacesCohorts()[iWireframesCohort];

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pWireframesCohort->IBO());
					glDrawElementsBaseVertex(GL_LINES,
						(GLsizei)pWireframesCohort->getIndicesCount(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pWireframesCohort->IBOOffset()),
						pRDFInstance->VBOOffset());
				} // for (size_t iWireframesCohort = ...
			} // for (size_t iObject = ...

			glBindVertexArray(0);
		} // for (; itGroups != ...
	} // for (size_t iDrawMetaData = ...
#else
	glDisable(GL_LIGHTING);

	glLineWidth(1.0);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0);

	glEnableClientState(GL_VERTEX_ARRAY);

	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtGeometry)
		{
			continue;
		}

		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
		for (; itGroups != mapGroups.end(); itGroups++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, itGroups->first);
			glVertexPointer(3, GL_FLOAT, sizeof(GLfloat) * GEOMETRY_VBO_VERTEX_LENGTH, NULL);

			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
			{
				CRDFInstance* pRDFInstance = itGroups->second[iObject];

				if (!pRDFInstance->getEnable())
				{
					continue;
				}

				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
				{
					continue;
				}

				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
				{
					continue;
				}

				/*
				* Wireframes
				*/
				for (size_t iWireframesCohort = 0; iWireframesCohort < pRDFInstance->conceptualFacesCohorts().size(); iWireframesCohort++)
				{
					CWireframesCohort* pWireframesCohort = pRDFInstance->conceptualFacesCohorts()[iWireframesCohort];

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pWireframesCohort->IBO());

					glDrawElementsBaseVertex(GL_LINES,
						(GLsizei)pWireframesCohort->getIndicesCount(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pWireframesCohort->IBOOffset()),
						pRDFInstance->VBOOffset());

					//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				} // for (size_t iWireframesCohort = ...
			} // for (size_t iObject = ...

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		} // for (; itGroups != ...
	} // for (size_t iDrawMetaData = ...

	glDisableClientState(GL_VERTEX_ARRAY);

	glEnable(GL_LIGHTING);
#endif // _USE_SHADERS

	COpenGL::Check4Errors();

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawConceptualFacesPolygons() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());	
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawLines()
{
	if (!m_bShowLines)
	{
		return;
	}

	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	if (pModel == NULL)
	{
		return;
	}

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

#ifdef _USE_SHADERS
	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->geUseBinnPhongModel(),
		0.f);

	glProgramUniform3f(
		m_pProgram->GetID(),
		m_pProgram->getMaterialAmbientColor(),
		0.f,
		0.f,
		0.f);

	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->getTransparency(),
		1.f);

	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtGeometry)
		{
			continue;
		}

		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
		for (; itGroups != mapGroups.end(); itGroups++)
		{
			glBindVertexArray(itGroups->first);

			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
			{
				CRDFInstance* pRDFInstance = itGroups->second[iObject];

				if (!pRDFInstance->getEnable())
				{
					continue;
				}

				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
				{
					continue;
				}

				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
				{
					continue;
				}

				/*
				* Lines
				*/
				for (size_t iLinesCohort = 0; iLinesCohort < pRDFInstance->linesCohorts().size(); iLinesCohort++)
				{
					CLinesCohort* pLinesCohort = pRDFInstance->linesCohorts()[iLinesCohort];

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pLinesCohort->IBO());
					glDrawElementsBaseVertex(GL_LINES,
						(GLsizei)pLinesCohort->getIndicesCount(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pLinesCohort->IBOOffset()),
						pRDFInstance->VBOOffset());
				} // for (size_t iLinesCohort = ...
			} // for (size_t iObject = ...

			glBindVertexArray(0);
		} // for (; itGroups != ...
	} // for (size_t iDrawMetaData = ...
#else
	glDisable(GL_LIGHTING);

	glLineWidth(m_fLineWidth);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0);

	glEnableClientState(GL_VERTEX_ARRAY);

	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtGeometry)
		{
			continue;
		}

		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
		for (; itGroups != mapGroups.end(); itGroups++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, itGroups->first);
			glVertexPointer(3, GL_FLOAT, sizeof(GLfloat) * GEOMETRY_VBO_VERTEX_LENGTH, NULL);

			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
			{
				CRDFInstance* pRDFInstance = itGroups->second[iObject];

				if (!pRDFInstance->getEnable())
				{
					continue;
				}

				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
				{
					continue;
				}

				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
				{
					continue;
				}

				/*
				* Lines
				*/
				for (size_t iLinesCohort = 0; iLinesCohort < pRDFInstance->linesCohorts().size(); iLinesCohort++)
				{
					CLinesCohort* pLinesCohort = pRDFInstance->linesCohorts()[iLinesCohort];

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pLinesCohort->IBO());

					glDrawElementsBaseVertex(GL_LINES,
						(GLsizei)pLinesCohort->getIndicesCount(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pLinesCohort->IBOOffset()),
						pRDFInstance->VBOOffset());

					//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				} // for (size_t iLinesCohort = ...
			} // for (size_t iObject = ...

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		} // for (; itGroups != ...
	} // for (size_t iDrawMetaData = ...

	glDisableClientState(GL_VERTEX_ARRAY);

	glEnable(GL_LIGHTING);
#endif // _USE_SHADERS	

	COpenGL::Check4Errors();

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawLines() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawPoints()
{
	if (!m_bShowPoints)
	{
		return;
	}

	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	if (pModel == NULL)
	{
		return;
	}

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

#ifdef _USE_SHADERS
	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->geUseBinnPhongModel(),
		0.f);

	glProgramUniform3f(
		m_pProgram->GetID(),
		m_pProgram->getMaterialAmbientColor(),
		0.f,
		0.f,
		0.f);

	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->getTransparency(),
		1.f);

	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtGeometry)
		{
			continue;
		}

		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
		for (; itGroups != mapGroups.end(); itGroups++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, itGroups->first);
			glVertexAttribPointer(m_pProgram->getVertexPosition(), 3, GL_FLOAT, false, sizeof(GLfloat) * GEOMETRY_VBO_VERTEX_LENGTH, 0);
			glEnableVertexAttribArray(m_pProgram->getVertexPosition());

			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
			{
				CRDFInstance* pRDFInstance = itGroups->second[iObject];

				if (!pRDFInstance->getEnable())
				{
					continue;
				}

				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
				{
					continue;
				}

				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
				{
					continue;
				}

				/*
				* Points
				*/
				for (size_t iPointsCohort = 0; iPointsCohort < pRDFInstance->pointsCohorts().size(); iPointsCohort++)
				{
					CPointsCohort* pPointsCohort = pRDFInstance->pointsCohorts()[iPointsCohort];

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pPointsCohort->IBO());
					glDrawElementsBaseVertex(GL_POINTS,
						(GLsizei)pPointsCohort->getIndicesCount(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pPointsCohort->IBOOffset()),
						pRDFInstance->VBOOffset());
				} // for (size_t iPointsCohort = ...
			} // for (size_t iObject = ...

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		} // for (; itGroups != ...
	} // for (size_t iDrawMetaData = ...
#else
	glDisable(GL_LIGHTING);

	glPointSize(m_fPointSize);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0);

	glEnableClientState(GL_VERTEX_ARRAY);

	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtGeometry)
		{
			continue;
		}

		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
		for (; itGroups != mapGroups.end(); itGroups++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, itGroups->first);
			glVertexPointer(3, GL_FLOAT, sizeof(GLfloat) * GEOMETRY_VBO_VERTEX_LENGTH, NULL);

			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
			{
				CRDFInstance* pRDFInstance = itGroups->second[iObject];

				if (!pRDFInstance->getEnable())
				{
					continue;
				}

				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
				{
					continue;
				}

				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
				{
					continue;
				}

				/*
				* Points
				*/
				for (size_t iPointsCohort = 0; iPointsCohort < pRDFInstance->pointsCohorts().size(); iPointsCohort++)
				{
					CPointsCohort* pPointsCohort = pRDFInstance->pointsCohorts()[iPointsCohort];

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pPointsCohort->IBO());

					glDrawElementsBaseVertex(GL_POINTS,
						(GLsizei)pPointsCohort->getIndicesCount(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pPointsCohort->IBOOffset()),
						pRDFInstance->VBOOffset());

					//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				} // for (size_t iPointsCohort = ...
			} // for (size_t iObject = ...

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		} // for (; itGroups != ...
	} // for (size_t iDrawMetaData = ...

	glDisableClientState(GL_VERTEX_ARRAY);

	glEnable(GL_LIGHTING);
#endif // _USE_SHADERS

	COpenGL::Check4Errors();

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	TRACE(L"\n*** DrawPoints() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());	
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawBoundingBoxes()
{
	if (!m_bShowBoundingBoxes)
	{
		return;
	}

	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	if (pModel == NULL)
	{
		return;
	}

	float fXTranslation = 0.f;
	float fYTranslation = 0.f;
	float fZTranslation = 0.f;
	pModel->GetWorldTranslations(fXTranslation, fYTranslation, fZTranslation);	

#ifdef _USE_SHADERS
	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->geUseBinnPhongModel(),
		0.f);

	glProgramUniform3f(
		m_pProgram->GetID(),
		m_pProgram->getMaterialAmbientColor(),
		0.f,
		0.f,
		0.f);

	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->getTransparency(),
		1.f);

	COpenGL::Check4Errors();

	if (m_iBoundingBoxesVAO == 0)
	{
		glGenVertexArrays(1, &m_iBoundingBoxesVAO);
		ASSERT(m_iBoundingBoxesVAO != 0);

		COpenGL::Check4Errors();

		ASSERT(m_iBoundingBoxesVBO == 0);

		glGenBuffers(1, &m_iBoundingBoxesVBO);
		ASSERT(m_iBoundingBoxesVBO != 0);

		COpenGL::Check4Errors();
	}

	if (m_iBoundingBoxesIBO == 0)
	{
		glGenBuffers(1, &m_iBoundingBoxesIBO);
		ASSERT(m_iBoundingBoxesIBO != 0);

		COpenGL::Check4Errors();
	}

	GLint iMatrixMode = 0;
	glGetIntegerv(GL_MATRIX_MODE, &iMatrixMode);

	glMatrixMode(GL_MODELVIEW);

	const map<int64_t, CRDFInstance*>& mapRDFInstances = pModel->GetRDFInstances();

	map<int64_t, CRDFInstance*>::const_iterator itRDFInstances = mapRDFInstances.begin();
	for (; itRDFInstances != mapRDFInstances.end(); itRDFInstances++)
	{
		CRDFInstance* pRDFInstance = itRDFInstances->second;
		if (!pRDFInstance->getEnable())
		{
			continue;
		}

		if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
		{
			continue;
		}

		if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
		{
			continue;
		}

		if ((pRDFInstance->getBoundingBoxTransformation() == NULL) || (pRDFInstance->getBoundingBoxMin() == NULL) || (pRDFInstance->getBoundingBoxMax() == NULL))
		{
			continue;
		}

		OGLMATRIX transformation;
		OGLMatrixIdentity(&transformation);

		transformation._11 = pRDFInstance->getBoundingBoxTransformation()->_11;
		transformation._12 = pRDFInstance->getBoundingBoxTransformation()->_12;
		transformation._13 = pRDFInstance->getBoundingBoxTransformation()->_13;
		transformation._21 = pRDFInstance->getBoundingBoxTransformation()->_21;
		transformation._22 = pRDFInstance->getBoundingBoxTransformation()->_22;
		transformation._23 = pRDFInstance->getBoundingBoxTransformation()->_23;
		transformation._31 = pRDFInstance->getBoundingBoxTransformation()->_31;
		transformation._32 = pRDFInstance->getBoundingBoxTransformation()->_32;
		transformation._33 = pRDFInstance->getBoundingBoxTransformation()->_33;
		transformation._41 = pRDFInstance->getBoundingBoxTransformation()->_41;
		transformation._42 = pRDFInstance->getBoundingBoxTransformation()->_42;
		transformation._43 = pRDFInstance->getBoundingBoxTransformation()->_43;

		glPushMatrix();
		glTranslatef(fXTranslation, fYTranslation, fZTranslation);
		glMultMatrixd((GLdouble*)&transformation);
		glTranslatef(-fXTranslation, -fYTranslation, -fZTranslation);

		VECTOR3 vecBoundingBoxMin = { pRDFInstance->getBoundingBoxMin()->x, pRDFInstance->getBoundingBoxMin()->y, pRDFInstance->getBoundingBoxMin()->z };
		VECTOR3 vecBoundingBoxMax = { pRDFInstance->getBoundingBoxMax()->x, pRDFInstance->getBoundingBoxMax()->y, pRDFInstance->getBoundingBoxMax()->z };

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

		VECTOR3 vecMin1 = { vecBoundingBoxMin.x, vecBoundingBoxMin.y, vecBoundingBoxMin.z };
		VECTOR3 vecMin2 = { vecBoundingBoxMax.x, vecBoundingBoxMin.y, vecBoundingBoxMin.z };
		VECTOR3 vecMin3 = { vecBoundingBoxMax.x, vecBoundingBoxMin.y, vecBoundingBoxMax.z };
		VECTOR3 vecMin4 = { vecBoundingBoxMin.x, vecBoundingBoxMin.y, vecBoundingBoxMax.z };

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

		VECTOR3 vecMax1 = { vecBoundingBoxMax.x, vecBoundingBoxMax.y, vecBoundingBoxMax.z };
		VECTOR3 vecMax2 = { vecBoundingBoxMin.x, vecBoundingBoxMax.y, vecBoundingBoxMax.z };
		VECTOR3 vecMax3 = { vecBoundingBoxMin.x, vecBoundingBoxMax.y, vecBoundingBoxMin.z };
		VECTOR3 vecMax4 = { vecBoundingBoxMax.x, vecBoundingBoxMax.y, vecBoundingBoxMin.z };		

		vector<float> vecVertices = 
		{
			(GLfloat)vecMin1.x, (GLfloat)vecMin1.y, (GLfloat)vecMin1.z,
			(GLfloat)vecMin2.x, (GLfloat)vecMin2.y, (GLfloat)vecMin2.z,
			(GLfloat)vecMin3.x, (GLfloat)vecMin3.y, (GLfloat)vecMin3.z,
			(GLfloat)vecMin4.x, (GLfloat)vecMin4.y, (GLfloat)vecMin4.z,
			(GLfloat)vecMax1.x, (GLfloat)vecMax1.y, (GLfloat)vecMax1.z,
			(GLfloat)vecMax2.x, (GLfloat)vecMax2.y, (GLfloat)vecMax2.z,
			(GLfloat)vecMax3.x, (GLfloat)vecMax3.y, (GLfloat)vecMax3.z,
			(GLfloat)vecMax4.x, (GLfloat)vecMax4.y, (GLfloat)vecMax4.z,
		};		
		
		glBindVertexArray(m_iBoundingBoxesVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_iBoundingBoxesVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vecVertices.size(), vecVertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(m_pProgram->getVertexPosition(), 3, GL_FLOAT, false, sizeof(GLfloat) * 3, 0);

		glEnableVertexAttribArray(m_pProgram->getVertexPosition());

		COpenGL::Check4Errors();

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

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBoundingBoxesIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vecIndices.size(), vecIndices.data(), GL_STATIC_DRAW);

		glDrawElementsBaseVertex(GL_LINES,
			(GLsizei)vecIndices.size(),
			GL_UNSIGNED_INT,
			(void*)0,
			0);

		glPopMatrix();

		glBindVertexArray(0);
	} // for (; itRDFInstances != ...

	glMatrixMode(iMatrixMode);
#else
	glDisable(GL_LIGHTING);

	glLineWidth(m_fLineWidth);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0);

	/*
	* Transformations
	*/
	GLint iMatrixMode = 0;
	glGetIntegerv(GL_MATRIX_MODE, &iMatrixMode);

	glMatrixMode(GL_MODELVIEW);

	OGLMATRIX transformation;

	const map<int64_t, CRDFInstance*>& mapRDFInstances = pModel->GetRDFInstances();

	map<int64_t, CRDFInstance*>::const_iterator itRDFInstances = mapRDFInstances.begin();
	for (; itRDFInstances != mapRDFInstances.end(); itRDFInstances++)
	{
		CRDFInstance* pRDFInstance = itRDFInstances->second;
		if (!pRDFInstance->getEnable())
		{
			continue;
		}

		if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
		{
			continue;
		}

		if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
		{
			continue;
		}

		if ((pRDFInstance->getBoundingBoxTransformation() == NULL) || (pRDFInstance->getBoundingBoxMin() == NULL) || (pRDFInstance->getBoundingBoxMax() == NULL))
		{
			continue;
		}

		OGLMatrixIdentity(&transformation);
		transformation._11 = pRDFInstance->getBoundingBoxTransformation()->_11;
		transformation._12 = pRDFInstance->getBoundingBoxTransformation()->_12;
		transformation._13 = pRDFInstance->getBoundingBoxTransformation()->_13;
		transformation._21 = pRDFInstance->getBoundingBoxTransformation()->_21;
		transformation._22 = pRDFInstance->getBoundingBoxTransformation()->_22;
		transformation._23 = pRDFInstance->getBoundingBoxTransformation()->_23;
		transformation._31 = pRDFInstance->getBoundingBoxTransformation()->_31;
		transformation._32 = pRDFInstance->getBoundingBoxTransformation()->_32;
		transformation._33 = pRDFInstance->getBoundingBoxTransformation()->_33;
		transformation._41 = pRDFInstance->getBoundingBoxTransformation()->_41;
		transformation._42 = pRDFInstance->getBoundingBoxTransformation()->_42;
		transformation._43 = pRDFInstance->getBoundingBoxTransformation()->_43;

		glPushMatrix();
		glTranslatef(fXTranslation, fYTranslation, fZTranslation);
		glMultMatrixd((GLdouble*)&transformation);
		glTranslatef(-fXTranslation, -fYTranslation, -fZTranslation);


		VECTOR3 vecBoundingBoxMin = { pRDFInstance->getBoundingBoxMin()->x, pRDFInstance->getBoundingBoxMin()->y, pRDFInstance->getBoundingBoxMin()->z };
		VECTOR3 vecBoundingBoxMax = { pRDFInstance->getBoundingBoxMax()->x, pRDFInstance->getBoundingBoxMax()->y, pRDFInstance->getBoundingBoxMax()->z };

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

		VECTOR3 vecMin1 = { vecBoundingBoxMin.x, vecBoundingBoxMin.y, vecBoundingBoxMin.z };

		VECTOR3 vecMin2 = { vecBoundingBoxMax.x, vecBoundingBoxMin.y, vecBoundingBoxMin.z };

		VECTOR3 vecMin3 = { vecBoundingBoxMax.x, vecBoundingBoxMin.y, vecBoundingBoxMax.z };

		VECTOR3 vecMin4 = { vecBoundingBoxMin.x, vecBoundingBoxMin.y, vecBoundingBoxMax.z };

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

		VECTOR3 vecMax1 = { vecBoundingBoxMax.x, vecBoundingBoxMax.y, vecBoundingBoxMax.z };

		VECTOR3 vecMax2 = { vecBoundingBoxMin.x, vecBoundingBoxMax.y, vecBoundingBoxMax.z };

		VECTOR3 vecMax3 = { vecBoundingBoxMin.x, vecBoundingBoxMax.y, vecBoundingBoxMin.z };

		VECTOR3 vecMax4 = { vecBoundingBoxMax.x, vecBoundingBoxMax.y, vecBoundingBoxMin.z };

		glBegin(GL_LINE_LOOP);
		glVertex3f((GLfloat)vecMin1.x, (GLfloat)vecMin1.y, (GLfloat)vecMin1.z);
		glVertex3f((GLfloat)vecMin2.x, (GLfloat)vecMin2.y, (GLfloat)vecMin2.z);
		glVertex3f((GLfloat)vecMin3.x, (GLfloat)vecMin3.y, (GLfloat)vecMin3.z);
		glVertex3f((GLfloat)vecMin4.x, (GLfloat)vecMin4.y, (GLfloat)vecMin4.z);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f((GLfloat)vecMax1.x, (GLfloat)vecMax1.y, (GLfloat)vecMax1.z);
		glVertex3f((GLfloat)vecMax2.x, (GLfloat)vecMax2.y, (GLfloat)vecMax2.z);
		glVertex3f((GLfloat)vecMax3.x, (GLfloat)vecMax3.y, (GLfloat)vecMax3.z);
		glVertex3f((GLfloat)vecMax4.x, (GLfloat)vecMax4.y, (GLfloat)vecMax4.z);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f((GLfloat)vecMax1.x, (GLfloat)vecMax1.y, (GLfloat)vecMax1.z);
		glVertex3f((GLfloat)vecMax2.x, (GLfloat)vecMax2.y, (GLfloat)vecMax2.z);
		glVertex3f((GLfloat)vecMin4.x, (GLfloat)vecMin4.y, (GLfloat)vecMin4.z);
		glVertex3f((GLfloat)vecMin3.x, (GLfloat)vecMin3.y, (GLfloat)vecMin3.z);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f((GLfloat)vecMax4.x, (GLfloat)vecMax4.y, (GLfloat)vecMax4.z);
		glVertex3f((GLfloat)vecMax3.x, (GLfloat)vecMax3.y, (GLfloat)vecMax3.z);
		glVertex3f((GLfloat)vecMin1.x, (GLfloat)vecMin1.y, (GLfloat)vecMin1.z);
		glVertex3f((GLfloat)vecMin2.x, (GLfloat)vecMin2.y, (GLfloat)vecMin2.z);
		glEnd();

		glPopMatrix();
	} // for (; itRDFInstances != ...

	glMatrixMode(iMatrixMode);

	glEnable(GL_LIGHTING);
#endif // _USE_SHADERS	

	COpenGL::Check4Errors();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawNormalVectors()
{
	if (!m_bShowNormalVectors)
	{
		return;
	}

	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	if (pModel == NULL)
	{
		return;
	}

	//glDisable(GL_LIGHTING);

	//glLineWidth(m_fLineWidth);
	//glColor4f(0.0f, 0.0f, 0.0f, 1.0);

	//if ((m_pSelectedInstance != NULL) && (m_iPointedFace != -1))
	//{
	//	float fXmin = -1.f;
	//	float fXmax = 1.f;
	//	float fYmin = -1.f;
	//	float fYmax = 1.f;
	//	float fZmin = -1.f;
	//	float fZmax = 1.f;
	//	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	//	const float SCALE_FACTOR = m_bScaleVectors ? sqrt(pow(fXmax - fXmin, 2.f) + pow(fYmax - fYmin, 2.f) + pow(fZmax - fZmin, 2.f)) * 0.1f : 1.f;

	//	const vector<pair<int64_t, int64_t> >& vecTriangles = m_pSelectedInstance->getTriangles();
	//	ASSERT(!vecTriangles.empty());

	//	ASSERT((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

	//	glBegin(GL_LINES);

	//	for (int64_t iIndex = vecTriangles[m_iPointedFace].first; iIndex < vecTriangles[m_iPointedFace].first + vecTriangles[m_iPointedFace].second; iIndex++)
	//	{
	//		glVertex3f(
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

	//		glVertex3f(
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 3] * SCALE_FACTOR,
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 4] * SCALE_FACTOR,
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 5] * SCALE_FACTOR);
	//	} // for (size_t iIndex = ...

	//	glEnd();
	//} // if ((m_pSelectedInstance != NULL) && ...
	//else
	//{
	//	glEnableClientState(GL_VERTEX_ARRAY);

	//	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	//	{
	//		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtVectors)
	//		{
	//			continue;
	//		}

	//		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

	//		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
	//		for (; itGroups != mapGroups.end(); itGroups++)
	//		{
	//			glBindBuffer(GL_ARRAY_BUFFER, itGroups->first);
	//			glVertexPointer(3, GL_FLOAT, sizeof(GLfloat) * VECTORS_VBO_VERTEX_LENGTH, NULL);

	//			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
	//			{
	//				CRDFInstance* pRDFInstance = itGroups->second[iObject];

	//				if (!pRDFInstance->getEnable())
	//				{
	//					continue;
	//				}

	//				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
	//				{
	//					continue;
	//				}

	//				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
	//				{
	//					continue;
	//				}

	//				if ((m_pSelectedInstance != NULL) && (m_pSelectedInstance != pRDFInstance))
	//				{
	//					continue;
	//				}

	//				/*
	//				* Lines
	//				*/
	//				for (size_t iLinesCohort = 0; iLinesCohort < pRDFInstance->normalVectorsCohorts().size(); iLinesCohort++)
	//				{
	//					CLinesCohort* pLinesCohort = pRDFInstance->normalVectorsCohorts()[iLinesCohort];

	//					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pLinesCohort->IBO());

	//					glDrawElementsBaseVertex(GL_LINES,
	//						(GLsizei)pLinesCohort->getIndicesCount(),
	//						GL_UNSIGNED_INT,
	//						(void*)(sizeof(GLuint) * pLinesCohort->IBOOffset()),
	//						pRDFInstance->VBOOffset());

	//					//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//				} // for (size_t iLinesCohort = ...
	//			} // for (size_t iObject = ...

	//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//			glBindBuffer(GL_ARRAY_BUFFER, 0);
	//		} // for (; itGroups != ...
	//	} // for (size_t iDrawMetaData = ...

	//	glDisableClientState(GL_VERTEX_ARRAY);
	//}	

	//glEnable(GL_LIGHTING);

	//COpenGL::Check4Errors();

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	const float SCALE_FACTOR = m_bScaleVectors ? sqrt(pow(fXmax - fXmin, 2.f) + pow(fYmax - fYmin, 2.f) + pow(fZmax - fZmin, 2.f)) * 0.1f : 1.f;

	glDisable(GL_LIGHTING);

	glLineWidth(m_fLineWidth);
	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_LINES);

	if (m_pSelectedInstance == NULL)
	{
		const map<int64_t, CRDFInstance *> & mapRDFInstances = pModel->GetRDFInstances();

		map<int64_t, CRDFInstance *>::const_iterator itRDFInstances = mapRDFInstances.begin();
		for (; itRDFInstances != mapRDFInstances.end(); itRDFInstances++)
		{
			CRDFInstance * pRDFInstance = itRDFInstances->second;
			if (!pRDFInstance->getEnable())
			{
				continue;
			}

			if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
			{
				continue;
			}

			if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
			{
				continue;
			}

			const vector<pair<int64_t, int64_t> > & vecTriangles = pRDFInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				for (int64_t iIndex = vecTriangles[iTriangle].first; iIndex < vecTriangles[iTriangle].first + vecTriangles[iTriangle].second; iIndex++)
				{
					glVertex3f(
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					glVertex3f(
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH)] + pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 3] * SCALE_FACTOR,
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] + pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 4] * SCALE_FACTOR,
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] + pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 5] * SCALE_FACTOR);
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // for (; itRDFInstances != ...
	} // if (m_pSelectedInstance == NULL)
	else
	{
		const vector<pair<int64_t, int64_t> > & vecTriangles = m_pSelectedInstance->getTriangles();
		ASSERT(!vecTriangles.empty());

		if (m_iPointedFace == -1)
		{
			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				for (int64_t iIndex = vecTriangles[iTriangle].first; iIndex < vecTriangles[iTriangle].first + vecTriangles[iTriangle].second; iIndex++)
				{
					glVertex3f(
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					glVertex3f(
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 3] * SCALE_FACTOR,
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 4] * SCALE_FACTOR,
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 5] * SCALE_FACTOR);
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // if (m_iPointedFace == -1)
		else
		{
			ASSERT((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

			for (int64_t iIndex = vecTriangles[m_iPointedFace].first; iIndex < vecTriangles[m_iPointedFace].first + vecTriangles[m_iPointedFace].second; iIndex++)
			{
				glVertex3f(
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

				glVertex3f(
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 3] * SCALE_FACTOR,
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 4] * SCALE_FACTOR,
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 5] * SCALE_FACTOR);
			} // for (size_t iIndex = ...
		} // else if (m_iPointedFace == -1)
	} // else if (m_pSelectedInstance == NULL)

	glEnd();

	glEnable(GL_LIGHTING);

	COpenGL::Check4Errors();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawTangentVectors()
{
	if (!m_bShowTangenVectors)
	{
		return;
	}

	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	if (pModel == NULL)
	{
		return;
	}

	//glDisable(GL_LIGHTING);

	//glLineWidth(m_fLineWidth);
	//glColor4f(0.0f, 0.0f, 0.0f, 1.0);

	//if ((m_pSelectedInstance != NULL) && (m_iPointedFace != -1))
	//{
	//	float fXmin = -1.f;
	//	float fXmax = 1.f;
	//	float fYmin = -1.f;
	//	float fYmax = 1.f;
	//	float fZmin = -1.f;
	//	float fZmax = 1.f;
	//	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	//	const float SCALE_FACTOR = m_bScaleVectors ? sqrt(pow(fXmax - fXmin, 2.f) + pow(fYmax - fYmin, 2.f) + pow(fZmax - fZmin, 2.f)) * 0.1f : 1.f;

	//	const vector<pair<int64_t, int64_t> >& vecTriangles = m_pSelectedInstance->getTriangles();
	//	ASSERT(!vecTriangles.empty());

	//	ASSERT((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

	//	glBegin(GL_LINES);

	//	for (int64_t iIndex = vecTriangles[m_iPointedFace].first; iIndex < vecTriangles[m_iPointedFace].first + vecTriangles[m_iPointedFace].second; iIndex++)
	//	{
	//		glVertex3f(
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

	//		glVertex3f(
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 12] * SCALE_FACTOR,
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 13] * SCALE_FACTOR,
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 14] * SCALE_FACTOR);
	//	} // for (size_t iIndex = ...

	//	glEnd();
	//} // if ((m_pSelectedInstance != NULL) && ...
	//else
	//{
	//	glEnableClientState(GL_VERTEX_ARRAY);

	//	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	//	{
	//		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtVectors)
	//		{
	//			continue;
	//		}

	//		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

	//		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
	//		for (; itGroups != mapGroups.end(); itGroups++)
	//		{
	//			glBindBuffer(GL_ARRAY_BUFFER, itGroups->first);
	//			glVertexPointer(3, GL_FLOAT, sizeof(GLfloat) * VECTORS_VBO_VERTEX_LENGTH, NULL);

	//			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
	//			{
	//				CRDFInstance* pRDFInstance = itGroups->second[iObject];

	//				if (!pRDFInstance->getEnable())
	//				{
	//					continue;
	//				}

	//				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
	//				{
	//					continue;
	//				}

	//				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
	//				{
	//					continue;
	//				}

	//				if ((m_pSelectedInstance != NULL) && (m_pSelectedInstance != pRDFInstance))
	//				{
	//					continue;
	//				}

	//				/*
	//				* Lines
	//				*/
	//				for (size_t iLinesCohort = 0; iLinesCohort < pRDFInstance->tangentVectorsCohorts().size(); iLinesCohort++)
	//				{
	//					CLinesCohort* pLinesCohort = pRDFInstance->tangentVectorsCohorts()[iLinesCohort];

	//					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pLinesCohort->IBO());

	//					glDrawElementsBaseVertex(GL_LINES,
	//						(GLsizei)pLinesCohort->getIndicesCount(),
	//						GL_UNSIGNED_INT,
	//						(void*)(sizeof(GLuint) * pLinesCohort->IBOOffset()),
	//						pRDFInstance->VBOOffset());

	//					//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//				} // for (size_t iLinesCohort = ...
	//			} // for (size_t iObject = ...

	//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//			glBindBuffer(GL_ARRAY_BUFFER, 0);
	//		} // for (; itGroups != ...
	//	} // for (size_t iDrawMetaData = ...

	//	glDisableClientState(GL_VERTEX_ARRAY);
	//}

	//glEnable(GL_LIGHTING);

	//COpenGL::Check4Errors();

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	const float SCALE_FACTOR = m_bScaleVectors ? sqrt(pow(fXmax - fXmin, 2.f) + pow(fYmax - fYmin, 2.f) + pow(fZmax - fZmin, 2.f)) * 0.1f : 1.f;

	glDisable(GL_LIGHTING);

	glLineWidth(m_fLineWidth);
	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_LINES);

	if (m_pSelectedInstance == NULL)
	{
		const map<int64_t, CRDFInstance *> & mapRDFInstances = pModel->GetRDFInstances();

		map<int64_t, CRDFInstance *>::const_iterator itRDFInstances = mapRDFInstances.begin();
		for (; itRDFInstances != mapRDFInstances.end(); itRDFInstances++)
		{
			CRDFInstance * pRDFInstance = itRDFInstances->second;
			if (!pRDFInstance->getEnable())
			{
				continue;
			}

			if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
			{
				continue;
			}

			if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
			{
				continue;
			}

			const vector<pair<int64_t, int64_t> > & vecTriangles = pRDFInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				for (int64_t iIndex = vecTriangles[iTriangle].first; iIndex < vecTriangles[iTriangle].first + vecTriangles[iTriangle].second; iIndex++)
				{
					glVertex3f(
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					glVertex3f(
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH)] + pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 12] * SCALE_FACTOR,
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] + pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 13] * SCALE_FACTOR,
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] + pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 14] * SCALE_FACTOR);
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // for (; itRDFInstances != ...
	} // if (m_pSelectedInstance == NULL)
	else
	{
		const vector<pair<int64_t, int64_t> > & vecTriangles = m_pSelectedInstance->getTriangles();
		ASSERT(!vecTriangles.empty());

		if (m_iPointedFace == -1)
		{
			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				for (int64_t iIndex = vecTriangles[iTriangle].first; iIndex < vecTriangles[iTriangle].first + vecTriangles[iTriangle].second; iIndex++)
				{
					glVertex3f(
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					glVertex3f(
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 12] * SCALE_FACTOR,
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 13] * SCALE_FACTOR,
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 14] * SCALE_FACTOR);
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // if (m_iPointedFace == -1)
		else
		{
			ASSERT((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

			for (int64_t iIndex = vecTriangles[m_iPointedFace].first; iIndex < vecTriangles[m_iPointedFace].first + vecTriangles[m_iPointedFace].second; iIndex++)
			{
				glVertex3f(
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

				glVertex3f(
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 12] * SCALE_FACTOR,
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 13] * SCALE_FACTOR,
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 14] * SCALE_FACTOR);
			} // for (size_t iIndex = ...
		} // else if (m_iPointedFace == -1)
	} // else if (m_pSelectedInstance == NULL)

	glEnd();

	glEnable(GL_LIGHTING);

	COpenGL::Check4Errors();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawBiNormalVectors()
{
	if (!m_bShowBiNormalVectors)
	{
		return;
	}

	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	if (pModel == NULL)
	{
		return;
	}

	//glDisable(GL_LIGHTING);

	//glLineWidth(m_fLineWidth);
	//glColor4f(0.0f, 0.0f, 0.0f, 1.0);

	//if ((m_pSelectedInstance != NULL) && (m_iPointedFace != -1))
	//{
	//	float fXmin = -1.f;
	//	float fXmax = 1.f;
	//	float fYmin = -1.f;
	//	float fYmax = 1.f;
	//	float fZmin = -1.f;
	//	float fZmax = 1.f;
	//	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	//	const float SCALE_FACTOR = m_bScaleVectors ? sqrt(pow(fXmax - fXmin, 2.f) + pow(fYmax - fYmin, 2.f) + pow(fZmax - fZmin, 2.f)) * 0.1f : 1.f;

	//	const vector<pair<int64_t, int64_t> >& vecTriangles = m_pSelectedInstance->getTriangles();
	//	ASSERT(!vecTriangles.empty());

	//	ASSERT((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

	//	glBegin(GL_LINES);

	//	for (int64_t iIndex = vecTriangles[m_iPointedFace].first; iIndex < vecTriangles[m_iPointedFace].first + vecTriangles[m_iPointedFace].second; iIndex++)
	//	{
	//		glVertex3f(
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

	//		glVertex3f(
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 15] * SCALE_FACTOR,
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 16] * SCALE_FACTOR,
	//			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 17] * SCALE_FACTOR);
	//	} // for (size_t iIndex = ...

	//	glEnd();
	//} // if ((m_pSelectedInstance != NULL) && ...
	//else
	//{
	//	glEnableClientState(GL_VERTEX_ARRAY);

	//	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	//	{
	//		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtVectors)
	//		{
	//			continue;
	//		}

	//		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

	//		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
	//		for (; itGroups != mapGroups.end(); itGroups++)
	//		{
	//			glBindBuffer(GL_ARRAY_BUFFER, itGroups->first);
	//			glVertexPointer(3, GL_FLOAT, sizeof(GLfloat) * VECTORS_VBO_VERTEX_LENGTH, NULL);

	//			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
	//			{
	//				CRDFInstance* pRDFInstance = itGroups->second[iObject];

	//				if (!pRDFInstance->getEnable())
	//				{
	//					continue;
	//				}

	//				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
	//				{
	//					continue;
	//				}

	//				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
	//				{
	//					continue;
	//				}

	//				if ((m_pSelectedInstance != NULL) && (m_pSelectedInstance != pRDFInstance))
	//				{
	//					continue;
	//				}

	//				/*
	//				* Lines
	//				*/
	//				for (size_t iLinesCohort = 0; iLinesCohort < pRDFInstance->biNormalVectorsCohorts().size(); iLinesCohort++)
	//				{
	//					CLinesCohort* pLinesCohort = pRDFInstance->biNormalVectorsCohorts()[iLinesCohort];

	//					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pLinesCohort->IBO());

	//					glDrawElementsBaseVertex(GL_LINES,
	//						(GLsizei)pLinesCohort->getIndicesCount(),
	//						GL_UNSIGNED_INT,
	//						(void*)(sizeof(GLuint) * pLinesCohort->IBOOffset()),
	//						pRDFInstance->VBOOffset());

	//					//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//				} // for (size_t iLinesCohort = ...
	//			} // for (size_t iObject = ...

	//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//			glBindBuffer(GL_ARRAY_BUFFER, 0);
	//		} // for (; itGroups != ...
	//	} // for (size_t iDrawMetaData = ...

	//	glDisableClientState(GL_VERTEX_ARRAY);
	//}

	//glEnable(GL_LIGHTING);

	//COpenGL::Check4Errors();

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	const float SCALE_FACTOR = m_bScaleVectors ? sqrt(pow(fXmax - fXmin, 2.f) + pow(fYmax - fYmin, 2.f) + pow(fZmax - fZmin, 2.f)) * 0.1f : 1.f;

	glDisable(GL_LIGHTING);

	glLineWidth(m_fLineWidth);
	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_LINES);

	if (m_pSelectedInstance == NULL)
	{
		const map<int64_t, CRDFInstance *> & mapRDFInstances = pModel->GetRDFInstances();

		map<int64_t, CRDFInstance *>::const_iterator itRDFInstances = mapRDFInstances.begin();
		for (; itRDFInstances != mapRDFInstances.end(); itRDFInstances++)
		{
			CRDFInstance * pRDFInstance = itRDFInstances->second;
			if (!pRDFInstance->getEnable())
			{
				continue;
			}

			if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
			{
				continue;
			}

			if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
			{
				continue;
			}

			const vector<pair<int64_t, int64_t> > & vecTriangles = pRDFInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				for (int64_t iIndex = vecTriangles[iTriangle].first; iIndex < vecTriangles[iTriangle].first + vecTriangles[iTriangle].second; iIndex++)
				{
					glVertex3f(
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					glVertex3f(
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH)] + pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 15] * SCALE_FACTOR,
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] + pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 16] * SCALE_FACTOR,
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] + pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 17] * SCALE_FACTOR);
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // for (; itRDFInstances != ...
	} // if (m_pSelectedInstance == NULL)
	else
	{
		const vector<pair<int64_t, int64_t> > & vecTriangles = m_pSelectedInstance->getTriangles();
		ASSERT(!vecTriangles.empty());

		if (m_iPointedFace == -1)
		{
			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				for (int64_t iIndex = vecTriangles[iTriangle].first; iIndex < vecTriangles[iTriangle].first + vecTriangles[iTriangle].second; iIndex++)
				{
					glVertex3f(
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					glVertex3f(
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 15] * SCALE_FACTOR,
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 16] * SCALE_FACTOR,
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 17] * SCALE_FACTOR);
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // if (m_iPointedFace == -1)
		else
		{
			ASSERT((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

			for (int64_t iIndex = vecTriangles[m_iPointedFace].first; iIndex < vecTriangles[m_iPointedFace].first + vecTriangles[m_iPointedFace].second; iIndex++)
			{
				glVertex3f(
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

				glVertex3f(
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 15] * SCALE_FACTOR,
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 16] * SCALE_FACTOR,
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] + m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 17] * SCALE_FACTOR);
			} // for (size_t iIndex = ...
		} // else if (m_iPointedFace == -1)
	} // else if (m_pSelectedInstance == NULL)

	glEnd();

	glEnable(GL_LIGHTING);

	COpenGL::Check4Errors();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawInstancesFrameBuffer()
{
	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	if (pModel == NULL)
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

	BOOL bResult = m_pOGLContext->MakeCurrent();
	VERIFY(bResult);

	if (m_iInstanceSelectionFrameBuffer == 0)
	{
		ASSERT(m_iInstanceSelectionTextureBuffer == 0);
		ASSERT(m_iInstanceSelectionDepthRenderBuffer == 0);

		/*
		* Frame buffer
		*/
		glGenFramebuffers(1, &m_iInstanceSelectionFrameBuffer);
		ASSERT(m_iInstanceSelectionFrameBuffer != 0);

		glBindFramebuffer(GL_FRAMEBUFFER, m_iInstanceSelectionFrameBuffer);

		/*
		* Texture buffer
		*/
		glGenTextures(1, &m_iInstanceSelectionTextureBuffer);
		ASSERT(m_iInstanceSelectionTextureBuffer != 0);

		glBindTexture(GL_TEXTURE_2D, m_iInstanceSelectionTextureBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SELECTION_BUFFER_SIZE, SELECTION_BUFFER_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_iInstanceSelectionTextureBuffer, 0);

		/*
		* Depth buffer
		*/
		glGenRenderbuffers(1, &m_iInstanceSelectionDepthRenderBuffer);
		ASSERT(m_iInstanceSelectionDepthRenderBuffer != 0);

		glBindRenderbuffer(GL_RENDERBUFFER, m_iInstanceSelectionDepthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SELECTION_BUFFER_SIZE, SELECTION_BUFFER_SIZE);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iInstanceSelectionDepthRenderBuffer);

		GLenum arDrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, arDrawBuffers);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		COpenGL::Check4Errors();
	} // if (m_iInstanceSelectionFrameBuffer == 0)

	/*
	* Selection colors
	*/
	if (m_mapInstancesSelectionColors.empty())
	{
		//const float STEP = 1.0f / 255.0f;

		const map<int64_t, CRDFInstance *> & mapRDFInstances = pModel->GetRDFInstances();

		map<int64_t, CRDFInstance *>::const_iterator itRDFInstances = mapRDFInstances.begin();
		for (; itRDFInstances != mapRDFInstances.end(); itRDFInstances++)
		{
			CRDFInstance * pRDFInstance = itRDFInstances->second;
			if (!pRDFInstance->getEnable())
			{
				continue;
			}

			if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
			{
				continue;
			}

			if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
			{
				continue;
			}

			const vector<pair<int64_t, int64_t> > & vecTriangles = pRDFInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			/*float fR = floor((float)pRDFInstance->getID() / (255.0f * 255.0f));
			if (fR >= 1.0f)
			{
				fR *= STEP;
			}

			float fG = floor((float)pRDFInstance->getID() / 255.0f);
			if (fG >= 1.0f)
			{
				fG *= STEP;
			}

			float fB = (float)(pRDFInstance->getID() % 255);
			fB *= STEP;*/

			// https://math.stackexchange.com/questions/1635999/algorithm-to-convert-integer-to-3-variables-rgb
			unsigned int C = (unsigned int)pRDFInstance->getID();
			unsigned int B = C % 256;
			unsigned int G = ((C - B) / 256) % 256;
			unsigned int R = ((C - B) / (256 * 256)) - (G / 256);

			m_mapInstancesSelectionColors[pRDFInstance->getInstance()] = CRDFColor(R / 256.f, G / 256.f, B / 256.f);
		} // for (; itRDFInstances != ...
	} // if (m_mapInstancesSelectionColors.empty())

	/*
	* Draw
	*/

	glBindFramebuffer(GL_FRAMEBUFFER, m_iInstanceSelectionFrameBuffer);

	glViewport(0, 0, SELECTION_BUFFER_SIZE, SELECTION_BUFFER_SIZE);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up the parameters
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

#ifdef _USE_SHADERS
	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->geUseBinnPhongModel(),
		0.f);

	glProgramUniform1f(
		m_pProgram->GetID(),
		m_pProgram->getTransparency(),
		1.f);	

	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtGeometry)
		{
			continue;
		}

		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
		for (; itGroups != mapGroups.end(); itGroups++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, itGroups->first);
			glVertexAttribPointer(m_pProgram->getVertexPosition(), 3, GL_FLOAT, false, sizeof(GLfloat) * GEOMETRY_VBO_VERTEX_LENGTH, 0);
			glEnableVertexAttribArray(m_pProgram->getVertexPosition());

			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
			{
				CRDFInstance* pRDFInstance = itGroups->second[iObject];

				if (!pRDFInstance->getEnable())
				{
					continue;
				}

				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
				{
					continue;
				}

				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
				{
					continue;
				}

				/*
				* Ambient color
				*/
				map<int64_t, CRDFColor>::iterator itSelectionColor = m_mapInstancesSelectionColors.find(pRDFInstance->getInstance());
				ASSERT(itSelectionColor != m_mapInstancesSelectionColors.end());

				/*
				* Material - Ambient color
				*/
				glProgramUniform3f(
					m_pProgram->GetID(),
					m_pProgram->getMaterialAmbientColor(),
					itSelectionColor->second.R(),
					itSelectionColor->second.G(),
					itSelectionColor->second.B());

				/*
				* Conceptual faces
				*/
				for (size_t iMaterial = 0; iMaterial < pRDFInstance->conceptualFacesMaterials().size(); iMaterial++)
				{
					CRDFGeometryWithMaterial* pGeometryWithMaterial = pRDFInstance->conceptualFacesMaterials()[iMaterial];					

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pGeometryWithMaterial->IBO());
					glDrawElementsBaseVertex(GL_TRIANGLES,
						(GLsizei)pGeometryWithMaterial->getIndicesCount(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pGeometryWithMaterial->IBOOffset()),
						pRDFInstance->VBOOffset());
				} // for (size_t iMaterial = ...
			} // for (size_t iObject = ...

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		} // for (; itGroups != ...
	} // for (size_t iDrawMetaData = ...
#else
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_FLAT);

	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// fovY     - Field of vision in degrees in the y direction
	// aspect   - Aspect ratio of the viewport
	// zNear    - The near clipping distance
	// zFar     - The far clipping distance
	GLdouble fovY = 45.0;
	GLdouble aspect = (GLdouble)iWidth / (GLdouble)iHeight;
	GLdouble zNear = 0.0001;
	GLdouble zFar = 1000000.0;

	GLdouble fH = tan(fovY / 360 * M_PI) * zNear;
	GLdouble fW = fH * aspect;

	glFrustum(-fW, fW, -fH, fH, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(m_fXTranslation, m_fYTranslation, m_fZTranslation);

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	float fXTranslation = fXmin;
	fXTranslation += (fXmax - fXmin) / 2.f;
	fXTranslation = -fXTranslation;

	float fYTranslation = fYmin;
	fYTranslation += (fYmax - fYmin) / 2.f;
	fYTranslation = -fYTranslation;

	float fZTranslation = fZmin;
	fZTranslation += (fZmax - fZmin) / 2.f;
	fZTranslation = -fZTranslation;

	glTranslatef(-fXTranslation, -fYTranslation, -fZTranslation);

	glRotatef(m_fXAngle, 1.0f, 0.0f, 0.0f);
	glRotatef(m_fYAngle, 0.0f, 1.0f, 0.0f);

	glTranslatef(fXTranslation, fYTranslation, fZTranslation);

	/*
	* Draw
	*/
	glEnableClientState(GL_VERTEX_ARRAY);

	for (size_t iDrawMetaData = 0; iDrawMetaData < m_vecDrawMetaData.size(); iDrawMetaData++)
	{
		if (m_vecDrawMetaData[iDrawMetaData]->GetType() != mdtGeometry)
		{
			continue;
		}

		const map<GLuint, vector<CRDFInstance*>>& mapGroups = m_vecDrawMetaData[iDrawMetaData]->getVBOGroups();

		map<GLuint, vector<CRDFInstance*>>::const_iterator itGroups = mapGroups.begin();
		for (; itGroups != mapGroups.end(); itGroups++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, itGroups->first);
			glVertexPointer(3, GL_FLOAT, sizeof(GLfloat) * GEOMETRY_VBO_VERTEX_LENGTH, NULL);

			for (size_t iObject = 0; iObject < itGroups->second.size(); iObject++)
			{
				CRDFInstance* pRDFInstance = itGroups->second[iObject];

				if (!pRDFInstance->getEnable())
				{
					continue;
				}

				if (!m_bShowReferencedInstances && pRDFInstance->isReferenced())
				{
					continue;
				}

				if (!m_bShowCoordinateSystem && (pRDFInstance->GetModel() == pModel->GetCoordinateSystemModel()))
				{
					continue;
				}

				/*
				* Conceptual faces
				*/
				for (size_t iMaterial = 0; iMaterial < pRDFInstance->conceptualFacesMaterials().size(); iMaterial++)
				{
					CRDFGeometryWithMaterial* pGeometryWithMaterial = pRDFInstance->conceptualFacesMaterials()[iMaterial];

					/*
					* Ambient color
					*/
					map<int64_t, CRDFColor>::iterator itSelectionColor = m_mapInstancesSelectionColors.find(pRDFInstance->getInstance());
					ASSERT(itSelectionColor != m_mapInstancesSelectionColors.end());

					glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
					glColor4f(
						itSelectionColor->second.R(),
						itSelectionColor->second.G(),
						itSelectionColor->second.B(),
						1.f);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pGeometryWithMaterial->IBO());

					glDrawElementsBaseVertex(GL_TRIANGLES,
						(GLsizei)pGeometryWithMaterial->getIndicesCount(),
						GL_UNSIGNED_INT,
						(void*)(sizeof(GLuint) * pGeometryWithMaterial->IBOOffset()),
						pRDFInstance->VBOOffset());

					//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				} // for (size_t iMaterial = ...
			} // for (size_t iObject = ...

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		} // for (; itGroups != ...
	} // for (size_t iDrawMetaData = ...

	glDisableClientState(GL_VERTEX_ARRAY);

	glEnable(GL_LIGHTING);	
#endif // _USE_SHADERS	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	COpenGL::Check4Errors();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawFacesFrameBuffer()
{
	if (m_pSelectedInstance == NULL)
	{
		return;
	}

	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	if (pModel == NULL)
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

	if (m_iFaceSelectionFrameBuffer == 0)
	{
		ASSERT(m_iFaceSelectionTextureBuffer == 0);
		ASSERT(m_iFaceSelectionDepthRenderBuffer == 0);

		/*
		* Frame buffer
		*/
		glGenFramebuffers(1, &m_iFaceSelectionFrameBuffer);
		ASSERT(m_iFaceSelectionFrameBuffer != 0);

		glBindFramebuffer(GL_FRAMEBUFFER, m_iFaceSelectionFrameBuffer);

		/*
		* Texture buffer
		*/
		glGenTextures(1, &m_iFaceSelectionTextureBuffer);
		ASSERT(m_iFaceSelectionTextureBuffer != 0);

		glBindTexture(GL_TEXTURE_2D, m_iFaceSelectionTextureBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SELECTION_BUFFER_SIZE, SELECTION_BUFFER_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_iFaceSelectionTextureBuffer, 0);

		/*
		* Depth buffer
		*/
		glGenRenderbuffers(1, &m_iFaceSelectionDepthRenderBuffer);
		ASSERT(m_iFaceSelectionDepthRenderBuffer != 0);

		glBindRenderbuffer(GL_RENDERBUFFER, m_iFaceSelectionDepthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SELECTION_BUFFER_SIZE, SELECTION_BUFFER_SIZE);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iFaceSelectionDepthRenderBuffer);

		GLenum arDrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, arDrawBuffers);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		COpenGL::Check4Errors();
	} // if (m_iFaceSelectionFrameBuffer == 0)

	/*
	* Selection colors
	*/
	if (m_mapFacesSelectionColors.empty())
	{
		//const float STEP = 1.0f / 255.0f;

		const vector<pair<int64_t, int64_t> > & vecTriangles = m_pSelectedInstance->getTriangles();
		ASSERT(!vecTriangles.empty());

		for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
		{
			/*float fR = floor((float)iTriangle / (255.0f * 255.0f));
			if (fR >= 1.0f)
			{
				fR *= STEP;
			}

			float fG = floor((float)iTriangle / 255.0f);
			if (fG >= 1.0f)
			{
				fG *= STEP;
			}

			float fB = (float)(iTriangle % 255);
			fB *= STEP;*/

			// https://math.stackexchange.com/questions/1635999/algorithm-to-convert-integer-to-3-variables-rgb
			unsigned int C = (unsigned int)iTriangle;
			unsigned int B = C % 256;
			unsigned int G = ((C - B) / 256) % 256;
			unsigned int R = ((C - B) / (256 * 256)) - (G / 256);

			m_mapFacesSelectionColors[iTriangle] = CRDFColor(R / 256.f, G / 256.f, B / 256.f);
		} // for (size_t iTriangle = ...
	} // if (m_mapFacesSelectionColors.empty())

	/*
	* Draw
	*/

	glBindFramebuffer(GL_FRAMEBUFFER, m_iFaceSelectionFrameBuffer);

	glViewport(0, 0, SELECTION_BUFFER_SIZE, SELECTION_BUFFER_SIZE);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up the parameters
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_FLAT);

	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// fovY     - Field of vision in degrees in the y direction
	// aspect   - Aspect ratio of the viewport
	// zNear    - The near clipping distance
	// zFar     - The far clipping distance
	GLdouble fovY = 45.0;
	GLdouble aspect = (GLdouble)iWidth / (GLdouble)iHeight;
	GLdouble zNear = 0.0001;
	GLdouble zFar = 1000000.0;

	GLdouble fH = tan(fovY / 360 * M_PI) * zNear;
	GLdouble fW = fH * aspect;

	glFrustum(-fW, fW, -fH, fH, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(m_fXTranslation, m_fYTranslation, m_fZTranslation);

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	float fXTranslation = fXmin;
	fXTranslation += (fXmax - fXmin) / 2.f;
	fXTranslation = -fXTranslation;

	float fYTranslation = fYmin;
	fYTranslation += (fYmax - fYmin) / 2.f;
	fYTranslation = -fYTranslation;

	float fZTranslation = fZmin;
	fZTranslation += (fZmax - fZmin) / 2.f;
	fZTranslation = -fZTranslation;

	glTranslatef(-fXTranslation, -fYTranslation, -fZTranslation);

	glRotatef(m_fXAngle, 1.0f, 0.0f, 0.0f);
	glRotatef(m_fYAngle, 0.0f, 1.0f, 0.0f);

	glTranslatef(fXTranslation, fYTranslation, fZTranslation);

	const vector<pair<int64_t, int64_t> > & vecTriangles = m_pSelectedInstance->getTriangles();
	ASSERT(!vecTriangles.empty());

	for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
	{
		map<int64_t, CRDFColor>::iterator itSelectionColor = m_mapFacesSelectionColors.find(iTriangle);
		ASSERT(itSelectionColor != m_mapFacesSelectionColors.end());

		// Ambient color
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
		glColor4f(
			itSelectionColor->second.R(),
			itSelectionColor->second.G(),
			itSelectionColor->second.B(),
			1.f);

		glBegin(GL_TRIANGLES);

		for (int64_t iIndex = vecTriangles[iTriangle].first; iIndex < vecTriangles[iTriangle].first + vecTriangles[iTriangle].second; iIndex++)
		{
			glNormal3f(
				m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 3],
				m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 4],
				m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 5]);

			glVertex3f(
				m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
				m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
				m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
		} // for (size_t iIndex = ...

		glEnd();
	} // for (size_t iTriangle = ...

	glEnable(GL_LIGHTING);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	COpenGL::Check4Errors();
}

// ------------------------------------------------------------------------------------------------
//void COpenGLRDFView::DrawPointedInstance()
//{
//	if (m_pPointedInstance == NULL)
//	{
//		return;
//	}
//
//	if (m_pSelectedInstance != NULL)
//	{
//		return;
//	}
//
//	const vector<pair<int64_t, int64_t> > & vecTriangles = m_pPointedInstance->getTriangles();
//	ASSERT(!vecTriangles.empty());
//
//	// Ambient color
//	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
//	glColor4f(
//		.33f,
//		.33f,
//		.33f,
//		1.f);
//
//	// Diffuse color
//	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
//	glColor4f(
//		.33f,
//		.33f,
//		.33f,
//		1.f);
//
//	// Specular color
//	glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
//	glColor4f(
//		.33f,
//		.33f,
//		.33f,
//		1.f);
//
//	// Emissive color
//	glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
//	glColor4f(
//		.33f,
//		.33f,
//		.33f,
//		1.f);
//
//	glBegin(GL_TRIANGLES);
//
//	for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
//	{
//		for (int64_t iIndex = vecTriangles[iTriangle].first; iIndex < vecTriangles[iTriangle].first + vecTriangles[iTriangle].second; iIndex++)
//		{
//			glNormal3f(
//				m_pPointedInstance->getVertices()[(m_pPointedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 3],
//				m_pPointedInstance->getVertices()[(m_pPointedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 4],
//				m_pPointedInstance->getVertices()[(m_pPointedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 5]);
//
//			glVertex3f(
//				m_pPointedInstance->getVertices()[(m_pPointedInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
//				m_pPointedInstance->getVertices()[(m_pPointedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
//				m_pPointedInstance->getVertices()[(m_pPointedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
//		} // for (size_t iIndex = ...
//	} // for (size_t iTriangle = ...
//
//	glEnd();
//
//	COpenGL::Check4Errors();
//}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawPointedFace()
{
	if (m_pSelectedInstance == NULL)
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
	const vector<pair<int64_t, int64_t> >& vecTriangles = m_pSelectedInstance->getTriangles();
	ASSERT(!vecTriangles.empty());
	ASSERT((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

	// Ambient color
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
	glColor4f(
		0.f,
		1.f,
		0.f,
		1.f);

	// Diffuse color
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glColor4f(
		0.f,
		1.f,
		0.f,
		1.f);

	// Specular color
	glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
	glColor4f(
		0.f,
		1.f,
		0.f,
		1.f);

	// Emissive color
	glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
	glColor4f(
		0.f,
		1.f,
		0.f,
		1.f);

	glBegin(GL_TRIANGLES);

	for (int64_t iIndex = vecTriangles[m_iPointedFace].first; iIndex < vecTriangles[m_iPointedFace].first + vecTriangles[m_iPointedFace].second; iIndex++)
	{
		glNormal3f(
			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 3],
			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 4],
			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 5]);

		glVertex3f(
			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
			m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
	} // for (size_t iIndex = ...

	glEnd();

	/*
	* Lines
	*/
	if (GetKeyState(VK_CONTROL) & 0x8000)
	{
		glDisable(GL_LIGHTING);

		glLineWidth(1.0);
		glColor4f(0.0f, 0.0f, 0.0f, 1.0);

		glBegin(GL_LINES);

		for (int64_t iIndex = vecTriangles[m_iPointedFace].first; iIndex < vecTriangles[m_iPointedFace].first + vecTriangles[m_iPointedFace].second; iIndex++)
		{
			glVertex3f(
				m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH)],
				m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1],
				m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);
		} // for (size_t iIndex = ...

		glEnd();

		glEnable(GL_LIGHTING);
	} // if (GetKeyState(VK_CONTROL) & 0x8000)

	COpenGL::Check4Errors();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::OnMouseMoveEvent(UINT nFlags, CPoint point)
{
	CRDFController * pController = GetController();
	ASSERT(pController != NULL);

	CRDFModel * pModel = pController->GetModel();
	if (pModel == NULL)
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
		if (m_iInstanceSelectionFrameBuffer != 0)
		{
			int iWidth = 0;
			int iHeight = 0;

#ifdef _LINUX
			m_pOGLContext->SetCurrent(*m_pWnd);

			const wxSize szClient = m_pWnd->GetClientSize();

			iWidth = szClient.GetWidth();
			iHeight = szClient.GetHeight();
#else
			BOOL bResult = m_pOGLContext->MakeCurrent();
			VERIFY(bResult);

			CRect rcClient;
			m_pWnd->GetClientRect(&rcClient);

			iWidth = rcClient.Width();
			iHeight = rcClient.Height();
#endif // _LINUX

			GLubyte arPixels[4];
			memset(arPixels, 0, sizeof(GLubyte) * 4);

			double dX = (double)point.x * ((double)SELECTION_BUFFER_SIZE / (double)iWidth);
			double dY = ((double)iHeight - (double)point.y) * ((double)SELECTION_BUFFER_SIZE / (double)iHeight);

			glBindFramebuffer(GL_FRAMEBUFFER, m_iInstanceSelectionFrameBuffer);

			glReadPixels(
				(GLint)dX,
				(GLint)dY,
				1, 1,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				arPixels);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			CRDFInstance * pPointedInstance = NULL;

			if (arPixels[3] != 0)
			{
				// https://math.stackexchange.com/questions/1635999/algorithm-to-convert-integer-to-3-variables-rgb
				int64_t iObjectID =
					(arPixels[0/*R*/] * (256 * 256)) +
					(arPixels[1/*G*/] * 256) +
					 arPixels[2/*B*/];

				pPointedInstance = pModel->GetRDFInstanceByID(iObjectID);
				ASSERT(pPointedInstance != NULL);
			} // if (arPixels[3] != 0)

			if (m_pPointedInstance != pPointedInstance)
			{
				m_pPointedInstance = pPointedInstance;

#ifdef _LINUX
                m_pWnd->Refresh(false);
#else
                m_pWnd->RedrawWindow();
#endif // _LINUX
			}
		} // if (m_iInstanceSelectionFrameBuffer != 0)

		/*
		* Select a face
		*/
		if ((m_iFaceSelectionFrameBuffer != 0) && (m_pSelectedInstance != NULL))
		{
			int iWidth = 0;
			int iHeight = 0;

#ifdef _LINUX
			m_pOGLContext->SetCurrent(*m_pWnd);

			const wxSize szClient = m_pWnd->GetClientSize();

			iWidth = szClient.GetWidth();
			iHeight = szClient.GetHeight();
#else
			BOOL bResult = m_pOGLContext->MakeCurrent();
			VERIFY(bResult);

			CRect rcClient;
			m_pWnd->GetClientRect(&rcClient);

			iWidth = rcClient.Width();
			iHeight = rcClient.Height();
#endif // _LINUX

			GLubyte arPixels[4];
			memset(arPixels, 0, sizeof(GLubyte) * 4);

			double dX = (double)point.x * ((double)SELECTION_BUFFER_SIZE / (double)iWidth);
			double dY = ((double)iHeight - (double)point.y) * ((double)SELECTION_BUFFER_SIZE / (double)iHeight);

			glBindFramebuffer(GL_FRAMEBUFFER, m_iFaceSelectionFrameBuffer);

			glReadPixels(
				(GLint)dX,
				(GLint)dY,
				1, 1,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				arPixels);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			int64_t iPointedFace = -1;

			if (arPixels[3] != 0)
			{
				// https://math.stackexchange.com/questions/1635999/algorithm-to-convert-integer-to-3-variables-rgb
				int64_t iObjectID =
					(arPixels[0/*R*/] * (256 * 256)) +
					(arPixels[1/*G*/] * 256) +
					 arPixels[2/*B*/];

				iPointedFace = iObjectID;
				ASSERT(m_mapFacesSelectionColors.find(iPointedFace) != m_mapFacesSelectionColors.end());
			} // if (arPixels[3] != 0)

			if (m_iPointedFace != iPointedFace)
			{
				m_iPointedFace = iPointedFace;

#ifdef _LINUX
                m_pWnd->Refresh(false);
#else
                m_pWnd->RedrawWindow();
#endif // _LINUX
			}
		} // if ((m_iFaceSelectionFrameBuffer != 0) && ...
	} // if (((nFlags & MK_LBUTTON) != MK_LBUTTON) && ...

	if (m_ptPrevMousePosition.x == -1)
	{
		return;
	}

	float fBoundingSphereDiameter = pModel->GetBoundingSphereDiameter();

	/*
	* Rotate
	*/
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON)
	{
		float fXAngle = ((float)point.y - (float)m_ptPrevMousePosition.y);
		float fYAngle = ((float)point.x - (float)m_ptPrevMousePosition.x);

		const float ROTATE_SPEED = 0.075f;

		Rotate(fXAngle* ROTATE_SPEED, fYAngle* ROTATE_SPEED);

		m_ptPrevMousePosition = point;

		m_ptPrevMousePosition = point;

		return;
	}

	/*
	* Zoom
	*/
	if ((nFlags & MK_MBUTTON) == MK_MBUTTON)
	{
		Zoom(point.y - m_ptPrevMousePosition.y > 0 ? -(fBoundingSphereDiameter * 0.05f) : (fBoundingSphereDiameter * 0.05f));

		m_ptPrevMousePosition = point;

		return;
	}

	/*
	* Move
	*/
	if ((nFlags & MK_RBUTTON) == MK_RBUTTON)
	{
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

		m_fXTranslation += (((float)point.x - (float)m_ptPrevMousePosition.x) / iWidth) * (2.f * fBoundingSphereDiameter);
		m_fYTranslation -= (((float)point.y - (float)m_ptPrevMousePosition.y) / iHeight) * (2.f * fBoundingSphereDiameter);

#ifdef _LINUX
        m_pWnd->Refresh(false);
#else
        m_pWnd->RedrawWindow();
#endif // _LINUX

		m_ptPrevMousePosition = point;

		return;
	}
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::Rotate(float fXSpin, float fYSpin)
{
	m_fXAngle += fXSpin;
	if (m_fXAngle > 360.0)
	{
		m_fXAngle = m_fXAngle - 360.0f;
	}
	else
	{
		if (m_fXAngle < 0.0)
		{
			m_fXAngle = m_fXAngle + 360.0f;
		}
	}

	m_fYAngle += fYSpin;
	if (m_fYAngle > 360.0)
	{
		m_fYAngle = m_fYAngle - 360.0f;
	}
	else
	{
		if (m_fYAngle < 0.0)
		{
			m_fYAngle = m_fYAngle + 360.0f;
		}
	}

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::Zoom(float fZTranslation)
{
	m_fZTranslation += fZTranslation;

#ifdef _LINUX
    m_pWnd->Refresh(false);
#else
    m_pWnd->RedrawWindow();
#endif // _LINUX
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

	m_pWnd->RedrawWindow();

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