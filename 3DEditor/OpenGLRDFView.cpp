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

// ------------------------------------------------------------------------------------------------
#ifdef _LINUX
COpenGLRDFView::COpenGLRDFView(wxGLCanvas * pWnd)
#else
COpenGLRDFView::COpenGLRDFView(CWnd * pWnd)
#endif // _LINUX
	: _oglRenderer()
	, CRDFView()
	, m_pWnd(pWnd)
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
#endif // _DEBUG_OCTREE
	, m_bShowBoundingBoxes(FALSE)
	, m_bShowNormalVectors(FALSE)
	, m_bShowTangenVectors(FALSE)
	, m_bShowBiNormalVectors(FALSE)
	, m_bScaleVectors(FALSE)	
	, m_ptStartMousePosition(-1, -1)
	, m_ptPrevMousePosition(-1, -1)
	, m_pInstanceSelectionFrameBuffer(new _oglSelectionFramebuffer())	
	, m_pPointedInstance(NULL)
	, m_pSelectedInstance(NULL)
	, m_pFaceSelectionFrameBuffer(new _oglSelectionFramebuffer())
	, m_iPointedFace(-1)	
	, m_pSelectedInstanceMaterial(NULL)
	, m_pPointedInstanceMaterial(NULL)
{
	ASSERT(m_pWnd != NULL);

	_initialize(
		*(m_pWnd->GetDC()), 
		TEST_MODE ? 1 : 16,
		IDR_TEXTFILE_VERTEX_SHADER2, 
		IDR_TEXTFILE_FRAGMENT_SHADER2, 
		TEXTFILE,
		true);
	
	m_pSelectedInstanceMaterial = new _material();
	m_pSelectedInstanceMaterial->init(
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		1.f,
		nullptr);

	m_pPointedInstanceMaterial = new _material();
	m_pPointedInstanceMaterial->init(
		.33f, .33f, .33f,
		.33f, .33f, .33f,
		.33f, .33f, .33f,
		.33f, .33f, .33f,
		.66f,
		nullptr);
}

// ------------------------------------------------------------------------------------------------
COpenGLRDFView::~COpenGLRDFView()
{
	GetController()->UnRegisterView(this);	

	delete m_pInstanceSelectionFrameBuffer;
	delete m_pFaceSelectionFrameBuffer;	
	
	// PATCH: AMD 6700 XT - Access violation
	if (!TEST_MODE)
	{
		_destroy();
	}

	delete m_pSelectedInstanceMaterial;
	m_pSelectedInstanceMaterial = NULL;

	delete m_pPointedInstanceMaterial;
	m_pPointedInstanceMaterial = NULL;
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
    BOOL bResult = m_pOGLContext->makeCurrent();
	VERIFY(bResult);

#ifdef _ENABLE_OPENGL_DEBUG
	m_pOGLContext->enableDebug();
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

	m_pOGLProgram->use();

	glViewport(0, 0, iWidth, iHeight);

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up the parameters
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	m_pOGLProgram->setPointLightLocation(0.f, 0.f, 10000.f);
	m_pOGLProgram->setMaterialShininess(30.f);	

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

	glm::mat4 matProjection = glm::frustum<GLdouble>(-fW, fW, -fH, fH, zNear, zFar);
	m_pOGLProgram->setProjectionMatrix(matProjection);

	/*
	* Model-View Matrix
	*/
	m_matModelView = glm::identity<glm::mat4>();
	m_matModelView = glm::translate(m_matModelView, glm::vec3(m_fXTranslation, m_fYTranslation, m_fZTranslation));

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

	m_matModelView = glm::translate(m_matModelView, glm::vec3(-fXTranslation, -fYTranslation, -fZTranslation));
	m_matModelView = glm::rotate(m_matModelView, m_fXAngle, glm::vec3(1.0f, 0.0f, 0.0f));
	m_matModelView = glm::rotate(m_matModelView, m_fYAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	m_matModelView = glm::translate(m_matModelView, glm::vec3(fXTranslation, fYTranslation, fZTranslation));
	m_pOGLProgram->setModelViewMatrix(m_matModelView);

	/*
	* Normal Matrix
	*/
	glm::mat4 matNormal = m_matModelView;
	matNormal = glm::inverse(matNormal);
	matNormal = glm::transpose(matNormal);
	m_pOGLProgram->setNormalMatrix(matNormal);
	
	m_pOGLProgram->enableBinnPhongModel(true);
	m_pOGLProgram->enableTexture(false);

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

				m_pFaceSelectionFrameBuffer->encoding().clear();

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
    BOOL bResult = m_pOGLContext->makeCurrent();
	VERIFY(bResult);
#endif // _LINUX

	// OpenGL buffers
	m_oglBuffers.clear();

	m_pInstanceSelectionFrameBuffer->encoding().clear();
	m_pPointedInstance = NULL;
	m_pSelectedInstance = NULL;

	m_pFaceSelectionFrameBuffer->encoding().clear();
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

	// Limits
	GLsizei VERTICES_MAX_COUNT = _oglUtils::getVerticesCountLimit(GEOMETRY_VBO_VERTEX_LENGTH * sizeof(float));
	GLsizei INDICES_MAX_COUNT = _oglUtils::getIndicesCountLimit();

	const map<int64_t, CRDFInstance*>& mapRDFInstances = pModel->GetRDFInstances();

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

	for (auto itRDFInstances = mapRDFInstances.begin(); itRDFInstances != mapRDFInstances.end(); itRDFInstances++)
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
		* VBO - Conceptual faces, polygons, etc.
		*/
		if (((int_t)iVerticesCount + pRDFInstance->getVerticesCount()) > (int_t)VERTICES_MAX_COUNT)
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
		for (size_t iFacesCohort = 0; iFacesCohort < pRDFInstance->concFacesCohorts().size(); iFacesCohort++)
		{
			if ((int_t)(iConcFacesIndicesCount + pRDFInstance->concFacesCohorts()[iFacesCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
			{
				if (m_oglBuffers.createIBO(vecConcFacesCohorts) != iConcFacesIndicesCount)
				{
					assert(false);

					return;
				}

				iConcFacesIndicesCount = 0;
				vecConcFacesCohorts.clear();
			}

			iConcFacesIndicesCount += (GLsizei)pRDFInstance->concFacesCohorts()[iFacesCohort]->indices().size();
			vecConcFacesCohorts.push_back(pRDFInstance->concFacesCohorts()[iFacesCohort]);
		}

		/*
		* IBO - Conceptual face polygons
		*/
		for (size_t iConcFacePolygonsCohort = 0; iConcFacePolygonsCohort < pRDFInstance->concFacePolygonsCohorts().size(); iConcFacePolygonsCohort++)
		{
			if ((int_t)(iConcFacePolygonsIndicesCount + pRDFInstance->concFacePolygonsCohorts()[iConcFacePolygonsCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
			{
				if (m_oglBuffers.createIBO(vecConcFacePolygonsCohorts) != iConcFacePolygonsIndicesCount)
				{
					assert(false);

					return;
				}

				iConcFacePolygonsIndicesCount = 0;
				vecConcFacePolygonsCohorts.clear();
			}

			iConcFacePolygonsIndicesCount += (GLsizei)pRDFInstance->concFacePolygonsCohorts()[iConcFacePolygonsCohort]->indices().size();
			vecConcFacePolygonsCohorts.push_back(pRDFInstance->concFacePolygonsCohorts()[iConcFacePolygonsCohort]);
		}

		/*
		* IBO - Face polygons
		*/
		for (size_t iFacePolygonsCohort = 0; iFacePolygonsCohort < pRDFInstance->facePolygonsCohorts().size(); iFacePolygonsCohort++)
		{
			if ((int_t)(iFacePolygonsIndicesCount + pRDFInstance->facePolygonsCohorts()[iFacePolygonsCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
			{
				if (m_oglBuffers.createIBO(vecFacePolygonsCohorts) != iFacePolygonsIndicesCount)
				{
					assert(false);

					return;
				}

				iFacePolygonsIndicesCount = 0;
				vecFacePolygonsCohorts.clear();
			}

			iFacePolygonsIndicesCount += (GLsizei)pRDFInstance->facePolygonsCohorts()[iFacePolygonsCohort]->indices().size();
			vecFacePolygonsCohorts.push_back(pRDFInstance->facePolygonsCohorts()[iFacePolygonsCohort]);
		}

		/*
		* IBO - Lines
		*/
		for (size_t iLinesCohort = 0; iLinesCohort < pRDFInstance->linesCohorts().size(); iLinesCohort++)
		{
			if ((int_t)(iLinesIndicesCount + pRDFInstance->linesCohorts()[iLinesCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
			{
				if (m_oglBuffers.createIBO(vecLinesCohorts) != iLinesIndicesCount)
				{
					assert(false);

					return;
				}

				iLinesIndicesCount = 0;
				vecLinesCohorts.clear();
			}

			iLinesIndicesCount += (GLsizei)pRDFInstance->linesCohorts()[iLinesCohort]->indices().size();
			vecLinesCohorts.push_back(pRDFInstance->linesCohorts()[iLinesCohort]);
		}

		/*
		* IBO - Points
		*/
		for (size_t iPointsCohort = 0; iPointsCohort < pRDFInstance->pointsCohorts().size(); iPointsCohort++)
		{
			if ((int_t)(iPointsIndicesCount + pRDFInstance->pointsCohorts()[iPointsCohort]->indices().size()) > (int_t)INDICES_MAX_COUNT)
			{
				if (m_oglBuffers.createIBO(vecPointsCohorts) != iPointsIndicesCount)
				{
					assert(false);

					return;
				}

				iPointsIndicesCount = 0;
				vecPointsCohorts.clear();
			}

			iPointsIndicesCount += (GLsizei)pRDFInstance->pointsCohorts()[iPointsCohort]->indices().size();
			vecPointsCohorts.push_back(pRDFInstance->pointsCohorts()[iPointsCohort]);
		}

		iVerticesCount += (GLsizei)pRDFInstance->getVerticesCount();
		vecInstancesCohort.push_back(pRDFInstance);
	} // for (; itRDFInstances != ...

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
	_reset();

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

		m_pFaceSelectionFrameBuffer->encoding().clear();

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

		m_pFaceSelectionFrameBuffer->encoding().clear();

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

	_oglUtils::checkForErrors();
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
	
	m_pOGLProgram->enableBinnPhongModel(true);

	for (auto itCohort : m_oglBuffers.instancesCohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto itInstance : itCohort.second)
		{
			CRDFInstance* pRDFInstance = itInstance;

			if (!pRDFInstance->getEnable())
			{
				continue;
			}
			
			for (size_t iConcFacesCohort = 0; iConcFacesCohort < pRDFInstance->concFacesCohorts().size(); iConcFacesCohort++)
			{
				auto pConcFacesCohort = pRDFInstance->concFacesCohorts()[iConcFacesCohort];

				const _material* pMaterial =
					pRDFInstance == m_pSelectedInstance ? m_pSelectedInstanceMaterial :
					pRDFInstance == m_pPointedInstance ? m_pPointedInstanceMaterial :
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
					m_pOGLProgram->enableTexture(true);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, pModel->GetDefaultTexture()->TexName());

					m_pOGLProgram->setSampler(0);
				}
				else
				{			
					m_pOGLProgram->setMaterial(pMaterial);
				}

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pConcFacesCohort->ibo());
				glDrawElementsBaseVertex(GL_TRIANGLES,
					(GLsizei)pConcFacesCohort->indices().size(),
					GL_UNSIGNED_INT,
					(void*)(sizeof(GLuint) * pConcFacesCohort->iboOffset()),
					pRDFInstance->VBOOffset());

				if (pMaterial->hasTexture())
				{
					m_pOGLProgram->enableTexture(false);
				}
			} // for (size_t iConcFacesCohort ...
		} // for (auto itInstance ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

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

	_oglUtils::checkForErrors();

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

	m_pOGLProgram->enableBinnPhongModel(false);
	m_pOGLProgram->setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->setTransparency(1.f);

	for (auto itCohort : m_oglBuffers.instancesCohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto itInstance : itCohort.second)
		{
			CRDFInstance* pRDFInstance = itInstance;

			if (!pRDFInstance->getEnable())
			{
				continue;
			}

			for (size_t iCohort = 0; iCohort < pRDFInstance->facePolygonsCohorts().size(); iCohort++)
			{
				_cohort* pCohort = pRDFInstance->facePolygonsCohorts()[iCohort];

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->ibo());
				glDrawElementsBaseVertex(GL_LINES,
					(GLsizei)pCohort->indices().size(),
					GL_UNSIGNED_INT,
					(void*)(sizeof(GLuint) * pCohort->iboOffset()),
					pRDFInstance->VBOOffset());
			}
		} // for (auto itInstance ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	_oglUtils::checkForErrors();

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

	m_pOGLProgram->enableBinnPhongModel(false); 
	m_pOGLProgram->setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->setTransparency(1.f);

	for (auto itCohort : m_oglBuffers.instancesCohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto itInstance : itCohort.second)
		{
			CRDFInstance* pRDFInstance = itInstance;

			if (!pRDFInstance->getEnable())
			{
				continue;
			}

			for (size_t iCohort = 0; iCohort < pRDFInstance->concFacePolygonsCohorts().size(); iCohort++)
			{
				_cohort* pCohort = pRDFInstance->concFacePolygonsCohorts()[iCohort];

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->ibo());
				glDrawElementsBaseVertex(GL_LINES,
					(GLsizei)pCohort->indices().size(),
					GL_UNSIGNED_INT,
					(void*)(sizeof(GLuint) * pCohort->iboOffset()),
					pRDFInstance->VBOOffset());
			}
		} // for (auto itInstance ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	_oglUtils::checkForErrors();

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

	m_pOGLProgram->enableBinnPhongModel(false);
	m_pOGLProgram->setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->setTransparency(1.f);

	for (auto itCohort : m_oglBuffers.instancesCohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto itInstance : itCohort.second)
		{
			auto pRDFInstance = itInstance;
			if (!pRDFInstance->getEnable())
			{
				continue;
			}

			for (size_t iCohort = 0; iCohort < pRDFInstance->linesCohorts().size(); iCohort++)
			{
				auto pCohort = pRDFInstance->linesCohorts()[iCohort];

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->ibo());
				glDrawElementsBaseVertex(GL_LINES,
					(GLsizei)pCohort->indices().size(),
					GL_UNSIGNED_INT,
					(void*)(sizeof(GLuint) * pCohort->iboOffset()),
					pRDFInstance->VBOOffset());
			}
		} // for (auto itInstance ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	_oglUtils::checkForErrors();

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

	glEnable(GL_PROGRAM_POINT_SIZE);

	m_pOGLProgram->enableBinnPhongModel(false);	
	m_pOGLProgram->setTransparency(1.f);

	for (auto itCohort : m_oglBuffers.instancesCohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto itInstance : itCohort.second)
		{
			auto pRDFInstance = itInstance;
			if (!pRDFInstance->getEnable())
			{
				continue;
			}

			for (size_t iPointsCohort = 0; iPointsCohort < pRDFInstance->pointsCohorts().size(); iPointsCohort++)
			{
				auto pCohort = pRDFInstance->pointsCohorts()[iPointsCohort];

				const _material* pMaterial =
					pRDFInstance == m_pSelectedInstance ? m_pSelectedInstanceMaterial :
					pRDFInstance == m_pPointedInstance ? m_pPointedInstanceMaterial :
					pCohort->getMaterial();

				if (pMaterial->hasTexture())
				{
					ASSERT(FALSE); // TODO!?!?
					/*m_pOGLProgram->enableTexture(true);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, pModel->GetDefaultTexture()->TexName());

					m_pOGLProgram->setSampler(0);*/
				}
				/*else
				{
					ASSERT(FALSE); // TODO - Enable Binn-Phong!?!?
					m_pOGLProgram->setMaterial(pMaterial);
				}*/
				
				m_pOGLProgram->setAmbientColor(
					pMaterial->getDiffuseColor().r(),
					pMaterial->getDiffuseColor().g(),
					pMaterial->getDiffuseColor().b());

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->ibo());
				glDrawElementsBaseVertex(GL_POINTS,
					(GLsizei)pCohort->indices().size(),
					GL_UNSIGNED_INT,
					(void*)(sizeof(GLuint) * pCohort->iboOffset()),
					pRDFInstance->VBOOffset());

				/*if (pMaterial->hasTexture())
				{
					ASSERT(FALSE); // TODO!?!?
					m_pOGLProgram->enableTexture(false);
				}*/
			} // for (size_t iPointsCohort = ...		
		} // for (auto itInstance ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	glDisable(GL_PROGRAM_POINT_SIZE);

	_oglUtils::checkForErrors();

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

	// Save Model-View Matrix
	glm::mat4 matModelView = m_matModelView;

	float fXTranslation = 0.f;
	float fYTranslation = 0.f;
	float fZTranslation = 0.f;
	pModel->GetWorldTranslations(fXTranslation, fYTranslation, fZTranslation);	

	m_pOGLProgram->enableBinnPhongModel(false);
	m_pOGLProgram->setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->setTransparency(1.f);

	_oglUtils::checkForErrors();

	bool bIsNew = false;
	GLuint iVAO = m_oglBuffers.getVAOcreateNew(BOUNDING_BOX_VAO, bIsNew);

	if (iVAO == 0)
	{
		ASSERT(FALSE);

		return;
	}	
	
	GLuint iVBO = 0;

	if (bIsNew)
	{
		glBindVertexArray(iVAO);

		iVBO = m_oglBuffers.getBufferCreateNew(BOUNDING_BOX_VBO, bIsNew);
		if ((iVBO == 0) || !bIsNew)
		{
			ASSERT(FALSE);

			return;
		}

		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		m_oglBuffers.setVBOAttributes(m_pOGLProgram);

		GLuint iIBO = m_oglBuffers.getBufferCreateNew(BOUNDING_BOX_IBO, bIsNew);
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

	const map<int64_t, CRDFInstance*>& mapRDFInstances = pModel->GetRDFInstances();
	for (auto itRDFInstances = mapRDFInstances.begin(); 
		itRDFInstances != mapRDFInstances.end(); 
		itRDFInstances++)
	{
		CRDFInstance* pRDFInstance = itRDFInstances->second;
		if (!pRDFInstance->getEnable())
		{
			continue;
		}

		if ((pRDFInstance->getBoundingBoxTransformation() == NULL) || (pRDFInstance->getBoundingBoxMin() == NULL) || (pRDFInstance->getBoundingBoxMax() == NULL))
		{
			continue;
		}		

		float arBBTransformation[] =
			{
				(float)pRDFInstance->getBoundingBoxTransformation()->_11,
				(float)pRDFInstance->getBoundingBoxTransformation()->_12,
				(float)pRDFInstance->getBoundingBoxTransformation()->_13,
				0.f,
				(float)pRDFInstance->getBoundingBoxTransformation()->_21,
				(float)pRDFInstance->getBoundingBoxTransformation()->_22,
				(float)pRDFInstance->getBoundingBoxTransformation()->_23,
				0.f,
				(float)pRDFInstance->getBoundingBoxTransformation()->_31,
				(float)pRDFInstance->getBoundingBoxTransformation()->_32,
				(float)pRDFInstance->getBoundingBoxTransformation()->_33,
				0.f,
				(float)pRDFInstance->getBoundingBoxTransformation()->_41,
				(float)pRDFInstance->getBoundingBoxTransformation()->_42,
				(float)pRDFInstance->getBoundingBoxTransformation()->_43,
				1.f,
			};

		glm::mat4 matBBTransformation = glm::make_mat4(arBBTransformation);
		matModelView = glm::translate(matModelView, glm::vec3(fXTranslation, fYTranslation, fZTranslation));
		matModelView = matModelView * matBBTransformation;
		matModelView = glm::translate(matModelView, glm::vec3(-fXTranslation, -fYTranslation, -fZTranslation));

		m_pOGLProgram->setModelViewMatrix(matModelView);

		_vector3d vecBoundingBoxMin = { pRDFInstance->getBoundingBoxMin()->x, pRDFInstance->getBoundingBoxMin()->y, pRDFInstance->getBoundingBoxMin()->z };
		_vector3d vecBoundingBoxMax = { pRDFInstance->getBoundingBoxMax()->x, pRDFInstance->getBoundingBoxMax()->y, pRDFInstance->getBoundingBoxMax()->z };

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
	} // for (; itRDFInstances != ...

	// Restore Model-View Matrix
	m_pOGLProgram->setModelViewMatrix(m_matModelView);

	_oglUtils::checkForErrors();
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

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	const float SCALE_FACTOR = m_bScaleVectors ? sqrt(pow(fXmax - fXmin, 2.f) + pow(fYmax - fYmin, 2.f) + pow(fZmax - fZmin, 2.f)) * 0.1f : 1.f;

	m_pOGLProgram->enableBinnPhongModel(false);
	m_pOGLProgram->setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->setTransparency(1.f);

	_oglUtils::checkForErrors();

	bool bIsNew = false;
	GLuint iVAO = m_oglBuffers.getVAOcreateNew(NORMAL_VECS_VAO, bIsNew);

	if (iVAO == 0)
	{
		ASSERT(FALSE);

		return;
	}

	GLuint iVBO = 0;

	if (bIsNew)
	{
		glBindVertexArray(iVAO);

		iVBO = m_oglBuffers.getBufferCreateNew(NORMAL_VECS_VBO, bIsNew);
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

	if (m_pSelectedInstance == NULL)
	{
		const map<int64_t, CRDFInstance*>& mapRDFInstances = pModel->GetRDFInstances();

		map<int64_t, CRDFInstance*>::const_iterator itRDFInstances = mapRDFInstances.begin();
		for (; itRDFInstances != mapRDFInstances.end(); itRDFInstances++)
		{
			CRDFInstance* pRDFInstance = itRDFInstances->second;
			if (!pRDFInstance->getEnable())
			{
				continue;
			}

			auto vecTriangles = pRDFInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				for (int64_t iIndex = vecTriangles[iTriangle].startIndex(); 
					iIndex < vecTriangles[iTriangle].startIndex() + vecTriangles[iTriangle].indicesCount();
					iIndex++)
				{
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 3] * SCALE_FACTOR);
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 4] * SCALE_FACTOR);
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 5] * SCALE_FACTOR);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // for (; itRDFInstances != ...
	} // if (m_pSelectedInstance == NULL)
	else
	{
		auto vecTriangles = m_pSelectedInstance->getTriangles();
		ASSERT(!vecTriangles.empty());

		if (m_iPointedFace == -1)
		{
			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				for (int64_t iIndex = vecTriangles[iTriangle].startIndex(); 
					iIndex < vecTriangles[iTriangle].startIndex() + vecTriangles[iTriangle].indicesCount();
					iIndex++)
				{
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 3] * SCALE_FACTOR);
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 4] * SCALE_FACTOR);
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 5] * SCALE_FACTOR);

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

			for (int64_t iIndex = vecTriangles[m_iPointedFace].startIndex(); 
				iIndex < vecTriangles[m_iPointedFace].startIndex() + vecTriangles[m_iPointedFace].indicesCount();
				iIndex++)
			{
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

				vecVertices.push_back(0.f); // Nx
				vecVertices.push_back(0.f); // Ny
				vecVertices.push_back(0.f); // Nz
				vecVertices.push_back(0.f); // Tx
				vecVertices.push_back(0.f); // Ty

				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 3] * SCALE_FACTOR);
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 4] * SCALE_FACTOR);
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 5] * SCALE_FACTOR);

				vecVertices.push_back(0.f); // Nx
				vecVertices.push_back(0.f); // Ny
				vecVertices.push_back(0.f); // Nz
				vecVertices.push_back(0.f); // Tx
				vecVertices.push_back(0.f); // Ty
			} // for (size_t iIndex = ...
		} // else if (m_iPointedFace == -1)
	} // else if (m_pSelectedInstance == NULL)

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

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	const float SCALE_FACTOR = m_bScaleVectors ? sqrt(pow(fXmax - fXmin, 2.f) + pow(fYmax - fYmin, 2.f) + pow(fZmax - fZmin, 2.f)) * 0.1f : 1.f;

	m_pOGLProgram->enableBinnPhongModel(false);
	m_pOGLProgram->setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->setTransparency(1.f);

	_oglUtils::checkForErrors();

	bool bIsNew = false;
	GLuint iVAO = m_oglBuffers.getVAOcreateNew(TANGENT_VECS_VAO, bIsNew);

	if (iVAO == 0)
	{
		ASSERT(FALSE);

		return;
	}

	GLuint iVBO = 0;

	if (bIsNew)
	{
		glBindVertexArray(iVAO);

		iVBO = m_oglBuffers.getBufferCreateNew(TANGENT_VECS_VBO, bIsNew);
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

	if (m_pSelectedInstance == NULL)
	{
		const map<int64_t, CRDFInstance*>& mapRDFInstances = pModel->GetRDFInstances();

		map<int64_t, CRDFInstance*>::const_iterator itRDFInstances = mapRDFInstances.begin();
		for (; itRDFInstances != mapRDFInstances.end(); itRDFInstances++)
		{
			CRDFInstance* pRDFInstance = itRDFInstances->second;
			if (!pRDFInstance->getEnable())
			{
				continue;
			}

			auto vecTriangles = pRDFInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				for (int64_t iIndex = vecTriangles[iTriangle].startIndex(); 
					iIndex < vecTriangles[iTriangle].startIndex() + vecTriangles[iTriangle].indicesCount();
					iIndex++)
				{
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 12] * SCALE_FACTOR);
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 13] * SCALE_FACTOR);
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 14] * SCALE_FACTOR);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // for (; itRDFInstances != ...
	} // if (m_pSelectedInstance == NULL)
	else
	{
		auto vecTriangles = m_pSelectedInstance->getTriangles();
		ASSERT(!vecTriangles.empty());

		if (m_iPointedFace == -1)
		{
			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				for (int64_t iIndex = vecTriangles[iTriangle].startIndex(); 
					iIndex < vecTriangles[iTriangle].startIndex() + vecTriangles[iTriangle].indicesCount();
					iIndex++)
				{
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 12] * SCALE_FACTOR);
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 13] * SCALE_FACTOR);
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 14] * SCALE_FACTOR);

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

			for (int64_t iIndex = vecTriangles[m_iPointedFace].startIndex();
				iIndex < vecTriangles[m_iPointedFace].startIndex() + vecTriangles[m_iPointedFace].indicesCount();
				iIndex++)
			{
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

				vecVertices.push_back(0.f); // Nx
				vecVertices.push_back(0.f); // Ny
				vecVertices.push_back(0.f); // Nz
				vecVertices.push_back(0.f); // Tx
				vecVertices.push_back(0.f); // Ty

				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 12] * SCALE_FACTOR);
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 13] * SCALE_FACTOR);
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 14] * SCALE_FACTOR);

				vecVertices.push_back(0.f); // Nx
				vecVertices.push_back(0.f); // Ny
				vecVertices.push_back(0.f); // Nz
				vecVertices.push_back(0.f); // Tx
				vecVertices.push_back(0.f); // Ty
			} // for (size_t iIndex = ...
		} // else if (m_iPointedFace == -1)
	} // else if (m_pSelectedInstance == NULL)

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

	float fXmin = -1.f;
	float fXmax = 1.f;
	float fYmin = -1.f;
	float fYmax = 1.f;
	float fZmin = -1.f;
	float fZmax = 1.f;
	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	const float SCALE_FACTOR = m_bScaleVectors ? sqrt(pow(fXmax - fXmin, 2.f) + pow(fYmax - fYmin, 2.f) + pow(fZmax - fZmin, 2.f)) * 0.1f : 1.f;

	m_pOGLProgram->enableBinnPhongModel(false);
	m_pOGLProgram->setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->setTransparency(1.f);

	_oglUtils::checkForErrors();

	bool bIsNew = false;
	GLuint iVAO = m_oglBuffers.getVAOcreateNew(BINORMAL_VECS_VAO, bIsNew);

	if (iVAO == 0)
	{
		ASSERT(FALSE);

		return;
	}

	GLuint iVBO = 0;

	if (bIsNew)
	{
		glBindVertexArray(iVAO);

		iVBO = m_oglBuffers.getBufferCreateNew(BINORMAL_VECS_VBO, bIsNew);
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

	if (m_pSelectedInstance == NULL)
	{
		const map<int64_t, CRDFInstance*>& mapRDFInstances = pModel->GetRDFInstances();

		map<int64_t, CRDFInstance*>::const_iterator itRDFInstances = mapRDFInstances.begin();
		for (; itRDFInstances != mapRDFInstances.end(); itRDFInstances++)
		{
			CRDFInstance* pRDFInstance = itRDFInstances->second;
			if (!pRDFInstance->getEnable())
			{
				continue;
			}

			auto vecTriangles = pRDFInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				for (int64_t iIndex = vecTriangles[iTriangle].startIndex(); 
					iIndex < vecTriangles[iTriangle].startIndex() + vecTriangles[iTriangle].indicesCount();
					iIndex++)
				{
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 15] * SCALE_FACTOR);
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 16] * SCALE_FACTOR);
					vecVertices.push_back(pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						pRDFInstance->getVertices()[(pRDFInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 17] * SCALE_FACTOR);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // for (; itRDFInstances != ...
	} // if (m_pSelectedInstance == NULL)
	else
	{
		auto vecTriangles = m_pSelectedInstance->getTriangles();
		ASSERT(!vecTriangles.empty());

		if (m_iPointedFace == -1)
		{
			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				for (int64_t iIndex = vecTriangles[iTriangle].startIndex();
					iIndex < vecTriangles[iTriangle].startIndex() + vecTriangles[iTriangle].indicesCount();
					iIndex++)
				{
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 15] * SCALE_FACTOR);
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 16] * SCALE_FACTOR);
					vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 17] * SCALE_FACTOR);

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

			for (int64_t iIndex = vecTriangles[m_iPointedFace].startIndex();
				iIndex < vecTriangles[m_iPointedFace].startIndex() + vecTriangles[m_iPointedFace].indicesCount();
				iIndex++)
			{
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

				vecVertices.push_back(0.f); // Nx
				vecVertices.push_back(0.f); // Ny
				vecVertices.push_back(0.f); // Nz
				vecVertices.push_back(0.f); // Tx
				vecVertices.push_back(0.f); // Ty

				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 15] * SCALE_FACTOR);
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 16] * SCALE_FACTOR);
				vecVertices.push_back(m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
					m_pSelectedInstance->getVertices()[(m_pSelectedInstance->getIndices()[iIndex] * VERTEX_LENGTH) + 17] * SCALE_FACTOR);

				vecVertices.push_back(0.f); // Nx
				vecVertices.push_back(0.f); // Ny
				vecVertices.push_back(0.f); // Nz
				vecVertices.push_back(0.f); // Tx
				vecVertices.push_back(0.f); // Ty
			} // for (size_t iIndex = ...
		} // else if (m_iPointedFace == -1)
	} // else if (m_pSelectedInstance == NULL)

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

	BOOL bResult = m_pOGLContext->makeCurrent();
	VERIFY(bResult);

	m_pInstanceSelectionFrameBuffer->create();

	/*
	* Selection colors
	*/
	if (m_pInstanceSelectionFrameBuffer->encoding().empty())
	{
		const map<int64_t, CRDFInstance *> & mapRDFInstances = pModel->GetRDFInstances();
		for (auto itRDFInstances = mapRDFInstances.begin(); itRDFInstances != mapRDFInstances.end(); itRDFInstances++)
		{
			CRDFInstance * pRDFInstance = itRDFInstances->second;
			if (!pRDFInstance->getEnable())
			{
				continue;
			}

			auto vecTriangles = pRDFInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			float fR, fG, fB;
			_i64RGBCoder::encode(pRDFInstance->getID(), fR, fG, fB);

			m_pInstanceSelectionFrameBuffer->encoding()[pRDFInstance->getInstance()] = _color(fR, fG, fB);
		}
	} // if (m_pInstanceSelectionFrameBuffer->encoding().empty())

	/*
	* Draw
	*/

	m_pInstanceSelectionFrameBuffer->bind();

	glViewport(0, 0, BUFFER_SIZE, BUFFER_SIZE);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up the parameters
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	m_pOGLProgram->enableBinnPhongModel(false);
	m_pOGLProgram->setTransparency(1.f);

	for (auto itCohort : m_oglBuffers.instancesCohorts())
	{
		glBindVertexArray(itCohort.first);

		for (auto itInstance : itCohort.second)
		{
			auto pRDFInstance = itInstance;

			if (!pRDFInstance->getEnable())
			{
				continue;
			}

			auto vecTriangles = pRDFInstance->getTriangles();
			if (vecTriangles.empty())
			{
				continue;
			}

			auto itSelectionColor = m_pInstanceSelectionFrameBuffer->encoding().find(pRDFInstance->getInstance());
			ASSERT(itSelectionColor != m_pInstanceSelectionFrameBuffer->encoding().end());

			m_pOGLProgram->setAmbientColor(
				itSelectionColor->second.r(),
				itSelectionColor->second.g(),
				itSelectionColor->second.b());

			for (size_t iConcFacesCohort = 0; iConcFacesCohort < pRDFInstance->concFacesCohorts().size(); iConcFacesCohort++)
			{
				auto pConcFacesCohort = pRDFInstance->concFacesCohorts()[iConcFacesCohort];

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pConcFacesCohort->ibo());
				glDrawElementsBaseVertex(GL_TRIANGLES,
					(GLsizei)pConcFacesCohort->indices().size(),
					GL_UNSIGNED_INT,
					(void*)(sizeof(GLuint) * pConcFacesCohort->iboOffset()),
					pRDFInstance->VBOOffset());
			}
		} // for (auto itInstance ...

		glBindVertexArray(0);
	} // for (auto itCohort ...

	m_pInstanceSelectionFrameBuffer->unbind();

	_oglUtils::checkForErrors();
}

// ------------------------------------------------------------------------------------------------
void COpenGLRDFView::DrawFacesFrameBuffer()
{
	if ((m_pSelectedInstance == NULL) || !m_pSelectedInstance->getEnable())
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

	BOOL bResult = m_pOGLContext->makeCurrent();
	VERIFY(bResult);

	m_pFaceSelectionFrameBuffer->create();

	/*
	* Selection colors
	*/
	if (m_pFaceSelectionFrameBuffer->encoding().empty())
	{
		auto vecTriangles = m_pSelectedInstance->getTriangles();
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

	m_pOGLProgram->enableBinnPhongModel(false);
	m_pOGLProgram->setTransparency(1.f);

	_oglUtils::checkForErrors();	

	GLuint iVAO = m_oglBuffers.findVAO(m_pSelectedInstance);
	if (iVAO == 0)
	{
		ASSERT(FALSE);

		return;
	}
	bool bIsNew = false;
	GLuint iIBO = m_oglBuffers.getBufferCreateNew(FACE_SELECTION_IBO, bIsNew);

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

	auto vecTriangles = m_pSelectedInstance->getTriangles();
	ASSERT(!vecTriangles.empty());

	for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
	{
		vector<unsigned int> vecIndices;
		for (int64_t iIndex = vecTriangles[iTriangle].startIndex();
			iIndex < vecTriangles[iTriangle].startIndex() + vecTriangles[iTriangle].indicesCount();
			iIndex++)
		{
			vecIndices.push_back(m_pSelectedInstance->getIndices()[iIndex]);
		}

		if (!vecIndices.empty())
		{
			auto itSelectionColor = m_pFaceSelectionFrameBuffer->encoding().find(iTriangle);
			ASSERT(itSelectionColor != m_pFaceSelectionFrameBuffer->encoding().end());

			m_pOGLProgram->setAmbientColor(
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
	auto vecTriangles = m_pSelectedInstance->getTriangles();

	ASSERT(!vecTriangles.empty());
	ASSERT((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

	m_pOGLProgram->enableBinnPhongModel(false);
	m_pOGLProgram->setAmbientColor(0.f, 1.f, 0.f);
	m_pOGLProgram->setTransparency(1.f);

	_oglUtils::checkForErrors();	

	GLuint iVAO = m_oglBuffers.findVAO(m_pSelectedInstance);
	if (iVAO == 0)
	{
		ASSERT(FALSE);

		return;
	}

	bool bIsNew = false;
	GLuint iIBO = m_oglBuffers.getBufferCreateNew(FACE_SELECTION_IBO, bIsNew);

	if (iIBO == 0)
	{
		ASSERT(FALSE);

		return;
	}

	glBindVertexArray(iVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIBO);	

	vector<unsigned int> vecIndices;
	for (int64_t iIndex = vecTriangles[m_iPointedFace].startIndex();
		iIndex < vecTriangles[m_iPointedFace].startIndex() + vecTriangles[m_iPointedFace].indicesCount();
		iIndex++)
	{
		vecIndices.push_back(m_pSelectedInstance->getIndices()[iIndex]);
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

			CRDFInstance * pPointedInstance = NULL;
			if (arPixels[3] != 0)
			{
				int64_t iObjectID = _i64RGBCoder::decode(arPixels[0], arPixels[1], arPixels[2]);
				pPointedInstance = pModel->GetRDFInstanceByID(iObjectID);
				ASSERT(pPointedInstance != NULL);
			}

			if (m_pPointedInstance != pPointedInstance)
			{
				m_pPointedInstance = pPointedInstance;

#ifdef _LINUX
                m_pWnd->Refresh(false);
#else
                m_pWnd->RedrawWindow();
#endif // _LINUX
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

#ifdef _LINUX
                m_pWnd->Refresh(false);
#else
                m_pWnd->RedrawWindow();
#endif // _LINUX
			}
		} // if ((m_pFaceSelectionFrameBuffer->isInitialized() != 0) && ...
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