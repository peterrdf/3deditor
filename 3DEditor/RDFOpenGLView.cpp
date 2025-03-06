#include "stdafx.h"

#include "RDFOpenGLView.h"
#include "RDFController.h"
#include "ProgressIndicator.h"

#include <chrono>

#include "Resource.h"

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
//int MIN_VIEW_PORT_LENGTH = 100;

// ------------------------------------------------------------------------------------------------
CRDFOpenGLView::CRDFOpenGLView(CWnd* pWnd)
	: _oglView()
	, m_pPointFaceFrameBuffer(new _oglSelectionFramebuffer())
	, m_pPointedFaceInstance(nullptr)
	, m_iPointedFace(-1)
	, m_iNearestVertex(-1)
	, m_pNavigatorSelectionFrameBuffer(new _oglSelectionFramebuffer())
	, m_pNavigatorPointedInstance(nullptr)
	, m_pNavigatorPointedInstanceMaterial(new _material())
{
	assert(pWnd != nullptr);

	_initialize(
		pWnd,
		TEST_MODE ? 1 : 16,
#ifdef _BLINN_PHONG_SHADERS
		IDR_TEXTFILE_VERTEX_SHADER2,
		IDR_TEXTFILE_FRAGMENT_SHADER2,
#else
		IDR_TEXTFILE_VERTEX_SHADER3,
		IDR_TEXTFILE_FRAGMENT_SHADER3,
#endif
		TEXTFILE,
		true);
}

// ------------------------------------------------------------------------------------------------
CRDFOpenGLView::~CRDFOpenGLView()
{
}

/*virtual*/ void CRDFOpenGLView::_postDraw() /*override*/
{
	auto pController = dynamic_cast<CRDFController*>(getController());
	auto pCoordinateSystemModel = pController->GetSceneModel();

	_drawFaces(pCoordinateSystemModel, false);
	_drawFaces(pCoordinateSystemModel, true);

	for (auto pModel : getController()->getModels())
	{
		DrawBoundingBoxes(pModel);
		DrawNormalVectors(pModel);
		DrawTangentVectors(pModel);
		DrawBiNormalVectors(pModel);
	}

	DrawPointedFace();
}

/*virtual*/ void CRDFOpenGLView::_drawBuffers() /*override*/
{
	if (TEST_MODE)
	{
		return;
	}

	_oglView::_drawBuffers();

	DrawFacesFrameBuffer();
}

/*virtual*/ void CRDFOpenGLView::_onMouseMove(const CPoint& point) /*override*/
{
	if (m_pPointedFaceInstance != m_pPointedInstance)
	{
		m_pPointFaceFrameBuffer->encoding().clear();
		m_pPointedFaceInstance = m_pPointedInstance;
		m_iPointedFace = -1;
		m_iNearestVertex = -1;

		_redraw();

		return;
	}

	if (m_pPointFaceFrameBuffer->isInitialized())
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
		
		m_pPointFaceFrameBuffer->bind();		
		glReadPixels(
			(GLint)dX,
			(GLint)dY,
			1, 1,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			arPixels);		
		m_pPointFaceFrameBuffer->unbind();
		
		int64_t iPointedFace = -1;
		if (arPixels[3] != 0)
		{
			iPointedFace = _i64RGBCoder::decode(arPixels[0], arPixels[1], arPixels[2]);
			assert(m_pPointFaceFrameBuffer->encoding().find(iPointedFace) != m_pPointFaceFrameBuffer->encoding().end());
		}
		
		if (m_iPointedFace != iPointedFace)
		{
			m_iPointedFace = iPointedFace;
			m_iNearestVertex = -1;
		}
	} // if (m_pPointFaceFrameBuffer->isInitialized())
}

// ------------------------------------------------------------------------------------------------
void CRDFOpenGLView::SetRotation(float fX, float fY, BOOL bRedraw)
{
	m_fXAngle = fX;
	m_fYAngle = fY;

	if (bRedraw)
	{
		_redraw();
	}
}

// ------------------------------------------------------------------------------------------------
void CRDFOpenGLView::GetRotation(float& fX, float& fY)
{
	fX = m_fXAngle;
	fY = m_fYAngle;
}

// ------------------------------------------------------------------------------------------------
void CRDFOpenGLView::SetTranslation(float fX, float fY, float fZ, BOOL bRedraw)
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
void CRDFOpenGLView::GetTranslation(float& fX, float& fY, float& fZ)
{
	fX = m_fXTranslation;
	fY = m_fYTranslation;
	fZ = m_fZTranslation;
}

// ------------------------------------------------------------------------------------------------
void CRDFOpenGLView::OnMouseEvent(enumMouseEvent enEvent, UINT nFlags, CPoint point)
{
	//if (enEvent == enumMouseEvent::LBtnUp)
	//{
	//	/*
	//	* OnSelectedItemChanged() notification
	//	*/
	//	if (point == m_ptStartMousePosition)
	//	{
	//		if (m_pSelectedInstance != m_pPointedInstance)
	//		{
	//			m_pSelectedInstance = m_pPointedInstance;

	//			m_iPointedFace = -1;
	//			m_iNearestVertex = -1;
	//			m_pPointFaceFrameBuffer->encoding().clear();

	//			_redraw();

	//			assert(GetController() != nullptr);
	//			GetController()->SelectInstance(this, m_pSelectedInstance);
	//		} // if (m_pSelectedInstance != ...

	//		if (m_pSelectedInstance == nullptr)
	//		{
	//			if (SelectNavigatorInstance())
	//			{
	//				_redraw();
	//			}
	//		}
	//	}
	//} // if (enEvent == meLBtnDown)

	//switch (enEvent)
	//{
	//	case enumMouseEvent::Move:
	//	{
	//		OnMouseMoveEvent(nFlags, point);
	//	}
	//	break;

	//	case enumMouseEvent::LBtnDown:
	//	case enumMouseEvent::MBtnDown:
	//	case enumMouseEvent::RBtnDown:
	//	{
	//		m_ptStartMousePosition = point;
	//		m_ptPrevMousePosition = point;
	//	}
	//	break;

	//	case enumMouseEvent::LBtnUp:
	//	case enumMouseEvent::MBtnUp:
	//	case enumMouseEvent::RBtnUp:
	//	{
	//		m_ptStartMousePosition.x = -1;
	//		m_ptStartMousePosition.y = -1;
	//		m_ptPrevMousePosition.x = -1;
	//		m_ptPrevMousePosition.y = -1;
	//	}
	//	break;

	//	default:
	//		assert(false);
	//		break;
	//} // switch (enEvent)
}

// ------------------------------------------------------------------------------------------------
void CRDFOpenGLView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	_onMouseWheel(nFlags, zDelta, pt);
}

// ------------------------------------------------------------------------------------------------
void CRDFOpenGLView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	_onKeyUp(nChar, nRepCnt, nFlags);
}

// ------------------------------------------------------------------------------------------------
///*virtual*/ void CRDFOpenGLView::OnModelChanged() /*override*/
//{
//	CWaitCursor waitCursor;	
//
//	BOOL bResult = m_pOGLContext->makeCurrent();
//	VERIFY(bResult);
//
//	// OpenGL buffers
//	m_oglBuffers.clear();
//
//	m_pSelectInstanceFrameBuffer->encoding().clear();
//	m_pPointedInstance = nullptr;
//	m_pSelectedInstance = nullptr;
//
//	m_iPointedFace = -1;
//	m_iNearestVertex = -1;
//	m_pPointFaceFrameBuffer->encoding().clear();
//
//	m_pNavigatorSelectionFrameBuffer->encoding().clear();
//	m_pNavigatorPointedInstance = nullptr;
//
//	auto pController = GetController();
//	if (pController == nullptr)
//	{
//		assert(false);
//
//		return;
//	}
//
//	auto pModel = pController->GetModel();
//	if (pModel == nullptr)
//	{
//		assert(false);
//
//		return;
//	}
//
//	float fXmin = -1.f;
//	float fXmax = 1.f;
//	float fYmin = -1.f;
//	float fYmax = 1.f;
//	float fZmin = -1.f;
//	float fZmax = 1.f;
//	pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);
//
//	/*
//	* Center the World
//	*/
//	m_fXTranslation = fXmin;
//	m_fXTranslation += (fXmax - fXmin) / 2.f;
//	m_fXTranslation = -m_fXTranslation;
//
//	m_fYTranslation = fYmin;
//	m_fYTranslation += (fYmax - fYmin) / 2.f;
//	m_fYTranslation = -m_fYTranslation;
//
//	m_fZTranslation = fZmin;
//	m_fZTranslation += (fZmax - fZmin) / 2.f;
//	m_fZTranslation = -m_fZTranslation;
//	m_fZTranslation -= (pModel->GetBoundingSphereDiameter() * 2.f);
//
//	m_fScaleFactor = pModel->GetBoundingSphereDiameter();
//
//	LoadModel(pModel);
//	LoadModel(pController->GetSceneModel());
//	LoadModel(pController->GetNavigatorModel());
//	
//	_redraw();
//}

// ------------------------------------------------------------------------------------------------
/*virtual void CRDFOpenGLView::OnWorldDimensionsChanged() /*override*/
//{
	///*
	//* Center
	//*/
	//auto pController = GetController();
	//assert(pController != nullptr);

	//CRDFModel * pModel = pController->GetModel();
	//assert(pModel != nullptr);

	//float fXmin = -1.f;
	//float fXmax = 1.f;
	//float fYmin = -1.f;
	//float fYmax = 1.f;
	//float fZmin = -1.f;
	//float fZmax = 1.f;
	//pModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	//m_fXTranslation = fXmin;
	//m_fXTranslation += (fXmax - fXmin) / 2.f;
	//m_fXTranslation = -m_fXTranslation;

	//m_fYTranslation = fYmin;
	//m_fYTranslation += (fYmax - fYmin) / 2.f;
	//m_fYTranslation = -m_fYTranslation;

	//m_fZTranslation = fZmin;
	//m_fZTranslation += (fZmax - fZmin) / 2.f;
	//m_fZTranslation = -m_fZTranslation;
	//m_fZTranslation -= (pModel->GetBoundingSphereDiameter() * 2.f);

	//m_fScaleFactor = pModel->GetBoundingSphereDiameter();

	///*
	//* Reload model
	//*/
	//OnModelChanged();

	///*
	//* Restore the selection
	//*/
	//OnInstanceSelected(nullptr);
//}

// ------------------------------------------------------------------------------------------------
///*virtual*/ void CRDFOpenGLView::OnInstancePropertyEdited(CRDFInstance* /*pInstance*/, CRDFProperty* /*pProperty*/) /*override*/
//{
//	/*
//	* Reload model
//	*/
//	//OnModelChanged();
//
//	///*
//	//* Restore the selection
//	//*/
//	//OnInstanceSelected(nullptr);
//}

// ------------------------------------------------------------------------------------------------
///*virtual*/ void CRDFOpenGLView::OnNewInstanceCreated(CRDFView* /*pSender*/, CRDFInstance* /*pInstance*/) /*override*/
//{
//	/*
//	* Reload model
//	*/
//	//OnModelChanged();
//
//	///*
//	//* Restore the selection
//	//*/
//	//OnInstanceSelected(nullptr);
//}

// ------------------------------------------------------------------------------------------------
///*virtual*/ void CRDFOpenGLView::OnInstanceDeleted(CRDFView* /*pSender*/, int64_t /*iInstance*/) /*override*/
//{
//	/*
//	* Reload model
//	*/
//	//OnModelChanged();
//
//	///*
//	//* Restore the selection
//	//*/
//	//OnInstanceSelected(nullptr);
//}

// ------------------------------------------------------------------------------------------------
///*virtual*/ void CRDFOpenGLView::OnInstancesDeleted(CRDFView* /*pSender*/) /*override*/
//{
//	/*
//	* Reload model
//	*/
//	//OnModelChanged();
//
//	///*
//	//* Restore the selection
//	//*/
//	//OnInstanceSelected(nullptr);
//}

// ------------------------------------------------------------------------------------------------
///*virtual*/ void CRDFOpenGLView::OnMeasurementsAdded(CRDFView* /*pSender*/, CRDFInstance* /*pInstance*/) /*override*/
//{
//	/*
//	* Reload model
//	*/
//	OnModelChanged();
//
//	/*
//	* Restore the selection
//	*/
//	OnInstanceSelected(nullptr);
//}

// ------------------------------------------------------------------------------------------------
///*virtual*/ void CRDFOpenGLView::OnInstanceSelected(CRDFView* pSender) /*override*/
//{
//	if (pSender == this)
//	{
//		return;
//	}
//
//	if (GetController() == nullptr)
//	{
//		assert(false);
//
//		return;
//	}
//
//	auto pSelectedInstance = GetController()->GetSelectedInstance();
//	if ((pSelectedInstance != nullptr) && (!pSelectedInstance->_instance::hasGeometry() || pSelectedInstance->getTriangles().empty()))
//	{
//		pSelectedInstance = nullptr;
//	}
//
//	if (m_pSelectedInstance != pSelectedInstance)
//	{
//		m_pSelectedInstance = pSelectedInstance;
//
//		m_iPointedFace = -1;
//		m_iNearestVertex = -1;
//		m_pPointFaceFrameBuffer->encoding().clear();
//
//		_redraw();
//	}
//}

// ------------------------------------------------------------------------------------------------
///*virtual*/ void CRDFOpenGLView::OnInstancePropertySelected() /*override*/
//{
//	assert(GetController() != nullptr);
//
//	pair<CRDFInstance *, CRDFProperty *> prSelectedInstanceProperty = GetController()->GetSelectedInstanceProperty();
//
//	CRDFInstance * pSelectedInstance = prSelectedInstanceProperty.first;
//
//	if ((pSelectedInstance != nullptr) && (!pSelectedInstance->_instance::hasGeometry() || pSelectedInstance->getTriangles().empty()))
//	{
//		pSelectedInstance = nullptr;
//	}
//
//	if (m_pSelectedInstance != pSelectedInstance)
//	{
//		m_pSelectedInstance = pSelectedInstance;
//
//		m_iPointedFace = -1;
//		m_iNearestVertex = -1;
//		m_pPointFaceFrameBuffer->encoding().clear();
//
//		_redraw();
//	}
//}

// ------------------------------------------------------------------------------------------------
///*virtual*/ void CRDFOpenGLView::OnInstancesEnabledStateChanged() /*override*/
//{
//	/*
//	* Reload model
//	*/
//	OnModelChanged();
//
//	/*
//	* Restore the selection
//	*/
//	OnInstanceSelected(nullptr);
//}

//void CRDFOpenGLView::DrawMainModel(
//	CRDFModel* pMainModel,
//	CRDFModel* pSceneModel,
//	int iViewportX, int iViewportY,
//	int iViewportWidth, int iViewportHeight)
//{
//	if (pMainModel == nullptr)
//	{
//		return;
//	}
//
//	float fXmin = -1.f;
//	float fXmax = 1.f;
//	float fYmin = -1.f;
//	float fYmax = 1.f;
//	float fZmin = -1.f;
//	float fZmax = 1.f;
//	pMainModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);
//
//	_prepare(
//		iViewportX, iViewportY,
//		iViewportWidth, iViewportHeight,
//		fXmin, fXmax,
//		fYmin, fYmax,
//		fZmin, fZmax,
//		true,
//		true);
//
//	/* Model */
//	DrawModel(pMainModel);
//
//	/* Scene */
//	if (getShowCoordinateSystem() && !TEST_MODE)
//	{
//		DrawModel(pSceneModel);
//	}
//}

void CRDFOpenGLView::DrawNavigatorModel(
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

	// Always use Perspective Projection
	enumProjection enProjection = m_enProjection;
	m_enProjection = enumProjection::Perspective;

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

	// Restore
	m_enProjection = enProjection;
}

void CRDFOpenGLView::DrawModel(_model* pM)
{
	//m_pOGLProgram->_enableTexture(false);

	///* Non-transparent faces */
	//DrawFaces(pM, false);

	///* Transparent faces */
	//DrawFaces(pM, true);

	///* Pointed face */
	//DrawPointedFace(pM);

	///* Faces polygons */
	//DrawFacesPolygons(pM);

	///* Conceptual faces polygons */
	//DrawConceptualFacesPolygons(pM);

	///* Lines */
	//DrawLines(pM);

	///* Points */
	//DrawPoints(pM);

	///* Bounding boxes */
	//DrawBoundingBoxes(pM);

	///* Normal vectors */
	//DrawNormalVectors(pM);

	///* Tangent vectors */
	//DrawTangentVectors(pM);

	///* Bi-Normal vectors */
	//DrawBiNormalVectors(pM);
}

void CRDFOpenGLView::DrawFaces(_model* pM, bool bTransparent)
{
//	auto pModel = dynamic_cast<CRDFModel*>(pM);
//	if (pModel == nullptr)
//	{
//		return;
//	}
//
//	if (!getShowFaces(pModel))
//	{
//		return;
//	}
//
//	auto pController = GetController();
//	if (pController == nullptr)
//	{
//		assert(false);
//
//		return;
//	}
//
//	auto begin = std::chrono::steady_clock::now();
//
//	CString strCullFaces = getCullFacesMode(pModel);
//
//	if (bTransparent)
//	{
//		glEnable(GL_BLEND);
//		glBlendEquation(GL_FUNC_ADD);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	}
//	else
//	{
//		if ((strCullFaces == CULL_FACES_FRONT) || (strCullFaces == CULL_FACES_BACK))
//		{
//			glEnable(GL_CULL_FACE);
//			glCullFace(strCullFaces == CULL_FACES_FRONT ? GL_FRONT : GL_BACK);
//		}
//	}
//	
//#ifdef _BLINN_PHONG_SHADERS
//	m_pOGLProgram->_enableBlinnPhongModel(TEST_MODE ? false : true);
//#else
//	m_pOGLProgram->_enableLighting(TEST_MODE ? false : true);
//#endif
//
//	const auto pPointedInstance = pModel == pController->getModel() ?
//		m_pPointedInstance : m_pNavigatorPointedInstance;
//	const auto pPointedInstanceMaterial = pModel == pController->getModel() ? 
//		m_pPointedInstanceMaterial : m_pNavigatorPointedInstanceMaterial;
//
//	for (auto itCohort : m_oglBuffers.cohorts())
//	{
//		glBindVertexArray(itCohort.first);
//
//		for (auto pInstance : itCohort.second)
//		{
//			if ((pInstance->getModel() != pModel->getInstance()) || !pInstance->getEnable())
//			{
//				continue;
//			}
//			
//			for (auto pConcFacesCohort : pInstance->concFacesCohorts())
//			{
//				const _material* pMaterial =
//					pInstance == m_pSelectedInstance ? m_pSelectedInstanceMaterial :
//					pInstance == pPointedInstance ? pPointedInstanceMaterial :
//					pConcFacesCohort->getMaterial();
//
//				if (bTransparent)
//				{
//					if (pMaterial->getA() == 1.0)
//					{
//						continue;
//					}
//				}
//				else
//				{
//					if (pMaterial->getA() < 1.0)
//					{
//						continue;
//					}
//				}
//				
//				if (pMaterial->hasTexture())
//				{
//					auto pOGLTexture = pModel->GetTexture(pMaterial->texture());
//					
//					m_pOGLProgram->_enableTexture(true);
//
//					glActiveTexture(GL_TEXTURE0);
//					glBindTexture(GL_TEXTURE_2D, pOGLTexture->getOGLName());
//
//					m_pOGLProgram->_setSampler(0);
//				}
//				else
//				{			
//					m_pOGLProgram->_setMaterial(pMaterial);
//				}
//
//				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pConcFacesCohort->IBO());
//				glDrawElementsBaseVertex(GL_TRIANGLES,
//					(GLsizei)pConcFacesCohort->indices().size(),
//					GL_UNSIGNED_INT,
//					(void*)(sizeof(GLuint) * pConcFacesCohort->IBOOffset()),
//					pInstance->VBOOffset());
//
//				if (pMaterial->hasTexture())
//				{
//					m_pOGLProgram->_enableTexture(false);
//				}
//			} // for (auto pConcFacesCohort ...
//		} // for (auto pInstance ...
//
//		glBindVertexArray(0);
//	} // for (auto itCohort ...
//
//	if (bTransparent)
//	{
//		glDisable(GL_BLEND);
//	}
//	else
//	{
//		if ((strCullFaces == CULL_FACES_FRONT) || (strCullFaces == CULL_FACES_BACK))
//		{
//			glDisable(GL_CULL_FACE);
//		}
//	}
//
//	_oglUtils::checkForErrors();
//
//	auto end = std::chrono::steady_clock::now();
//	TRACE(L"\n*** DrawFaces() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());	
}

void CRDFOpenGLView::DrawFacesPolygons(_model* pM)
{
	if (pM == nullptr)
	{
		return;
	}

	if (!getShowFacesPolygons())
	{
		return;
	}

	auto begin = std::chrono::steady_clock::now();

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(false);
#else
	m_pOGLProgram->_enableLighting(false);
#endif
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	//for (auto itCohort : m_oglBuffers.cohorts())
	//{
	//	glBindVertexArray(itCohort.first);

	//	for (auto itInstance : itCohort.second)
	//	{
	//		_geometry* pInstance = itInstance;

	//		if ((pInstance->getModel() != pM->getInstance()) || !pInstance->getEnable())
	//		{
	//			continue;
	//		}

	//		for (size_t iCohort = 0; iCohort < pInstance->facePolygonsCohorts().size(); iCohort++)
	//		{
	//			_cohort* pCohort = pInstance->facePolygonsCohorts()[iCohort];

	//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->IBO());
	//			glDrawElementsBaseVertex(GL_LINES,
	//				(GLsizei)pCohort->indices().size(),
	//				GL_UNSIGNED_INT,
	//				(void*)(sizeof(GLuint) * pCohort->IBOOffset()),
	//				pInstance->VBOOffset());
	//		}
	//	} // for (auto itInstance ...

	//	glBindVertexArray(0);
	//} // for (auto itCohort ...

	//_oglUtils::checkForErrors();

	//auto end = std::chrono::steady_clock::now();
	//TRACE(L"\n*** DrawFacesPolygons() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
}

void CRDFOpenGLView::DrawConceptualFacesPolygons(_model* pM)
{
	if (pM == nullptr)
	{
		return;
	}

	if (!getShowConceptualFacesPolygons())
	{
		return;
	}

	auto begin = std::chrono::steady_clock::now();

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(false);
#else
	m_pOGLProgram->_enableLighting(false);
#endif
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	//for (auto itCohort : m_oglBuffers.cohorts())
	//{
	//	glBindVertexArray(itCohort.first);

	//	for (auto pInstance : itCohort.second)
	//	{
	//		if ((pInstance->getModel() != pM->getInstance()) || !pInstance->getEnable())
	//		{
	//			continue;
	//		}

	//		for (auto pCohort : pInstance->concFacePolygonsCohorts())
	//		{
	//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->IBO());
	//			glDrawElementsBaseVertex(GL_LINES,
	//				(GLsizei)pCohort->indices().size(),
	//				GL_UNSIGNED_INT,
	//				(void*)(sizeof(GLuint) * pCohort->IBOOffset()),
	//				pInstance->VBOOffset());
	//		}
	//	} // for (auto itInstance ...

	//	glBindVertexArray(0);
	//} // for (auto pInstance ...

	//_oglUtils::checkForErrors();

	//auto end = std::chrono::steady_clock::now();
	//TRACE(L"\n*** DrawConceptualFacesPolygons() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());	
}

void CRDFOpenGLView::DrawLines(_model* pM)
{
	/*if (pM == nullptr)
	{
		return;
	}

	if (!getShowLines())
	{
		return;
	}

	auto pController = GetController();
	if (pController == nullptr)
	{
		assert(false);

		return;
	}*/

	auto begin = std::chrono::steady_clock::now();

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(false);
#else
	m_pOGLProgram->_enableLighting(false);
#endif
	m_pOGLProgram->_setTransparency(1.f);

	//for (auto itCohort : m_oglBuffers.cohorts())
	//{
	//	glBindVertexArray(itCohort.first);

	//	for (auto pInstance : itCohort.second)
	//	{
	//		if ((pInstance->getModel() != pM->getInstance()) || !pInstance->getEnable())
	//		{
	//			continue;
	//		}

	//		for (auto pCohort : pInstance->linesCohorts())
	//		{
	//			const _material* pMaterial = pCohort->getMaterial();
	//			m_pOGLProgram->_setAmbientColor(
	//				pMaterial->getDiffuseColor().r(),
	//				pMaterial->getDiffuseColor().g(),
	//				pMaterial->getDiffuseColor().b());

	//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->IBO());
	//			glDrawElementsBaseVertex(GL_LINES,
	//				(GLsizei)pCohort->indices().size(),
	//				GL_UNSIGNED_INT,
	//				(void*)(sizeof(GLuint) * pCohort->IBOOffset()),
	//				pInstance->VBOOffset());
	//		}
	//	} // for (auto pInstance ...

	//	glBindVertexArray(0);
	//} // for (auto itCohort ...

	//_oglUtils::checkForErrors();

	//auto end = std::chrono::steady_clock::now();
	//TRACE(L"\n*** DrawLines() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
}

void CRDFOpenGLView::DrawPoints(_model* pM)
{
	/*if (pM == nullptr)
	{
		return;
	}

	if (!getShowPoints())
	{
		return;
	}

	auto pController = GetController();
	if (pController == nullptr)
	{
		assert(false);

		return;
	}*/

	auto begin = std::chrono::steady_clock::now();

	glEnable(GL_PROGRAM_POINT_SIZE);

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(false);
#else
	m_pOGLProgram->_enableLighting(false);
#endif
	m_pOGLProgram->_setTransparency(1.f);

	//for (auto itCohort : m_oglBuffers.cohorts())
	//{
	//	glBindVertexArray(itCohort.first);

	//	for (auto pInstance : itCohort.second)
	//	{
	//		if ((pInstance->getModel() != pM->getInstance()) || !pInstance->getEnable())
	//		{
	//			continue;
	//		}

	//		for (auto pCohort : pInstance->pointsCohorts())
	//		{
	//			const _material* pMaterial = pCohort->getMaterial();
	//			m_pOGLProgram->_setAmbientColor(
	//				pMaterial->getDiffuseColor().r(),
	//				pMaterial->getDiffuseColor().g(),
	//				pMaterial->getDiffuseColor().b());

	//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pCohort->IBO());
	//			glDrawElementsBaseVertex(GL_POINTS,
	//				(GLsizei)pCohort->indices().size(),
	//				GL_UNSIGNED_INT,
	//				(void*)(sizeof(GLuint) * pCohort->IBOOffset()),
	//				pInstance->VBOOffset());
	//		} // for (auto pCohort ...		
	//	} // for (auto pInstance ...

	//	glBindVertexArray(0);
	//} // for (auto itCohort ...

	//glDisable(GL_PROGRAM_POINT_SIZE);

	//_oglUtils::checkForErrors();

	//auto end = std::chrono::steady_clock::now();
	//TRACE(L"\n*** DrawPoints() : %lld [탎]", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());	
}

void CRDFOpenGLView::TransformBBVertex(_vector3d& vecBBVertex, const _matrix* pBBTransformation, const _vector3d& vecVertexBufferOffset, double dScaleFactor)
{
	// Transformation
	_transform(&vecBBVertex, pBBTransformation, &vecBBVertex);

	// http://rdf.bg/gkdoc/CP64/SetVertexBufferOffset.html
	vecBBVertex.x += vecVertexBufferOffset.x;
	vecBBVertex.y += vecVertexBufferOffset.y;
	vecBBVertex.z += vecVertexBufferOffset.z;

	// Scale: [-1, +1]
	vecBBVertex.x /= dScaleFactor;
	vecBBVertex.y /= dScaleFactor;
	vecBBVertex.z /= dScaleFactor;
}

void CRDFOpenGLView::DrawBoundingBoxes(_model* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (pModel->isDecoration())
	{
		return;
	}

	if (!getShowBoundingBoxes())
	{
		return;
	}

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(false);
#else
	m_pOGLProgram->_enableLighting(false);
#endif
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	_oglUtils::checkForErrors();

	bool bIsNew = false;
	GLuint iVAO = m_oglBuffers.getVAOcreateNewIfNeeded(BOUNDING_BOX_VAO, bIsNew);

	if (iVAO == 0)
	{
		assert(false);

		return;
	}	
	
	GLuint iVBO = 0;

	if (bIsNew)
	{
		glBindVertexArray(iVAO);

		iVBO = m_oglBuffers.getBufferCreateNewIfNeeded(BOUNDING_BOX_VBO, bIsNew);
		if ((iVBO == 0) || !bIsNew)
		{
			assert(false);

			return;
		}

		glBindBuffer(GL_ARRAY_BUFFER, iVBO);
		m_oglBuffers.setVBOAttributes(m_pOGLProgram);

		GLuint iIBO = m_oglBuffers.getBufferCreateNewIfNeeded(BOUNDING_BOX_IBO, bIsNew);
		if ((iIBO == 0) || !bIsNew)
		{
			assert(false);

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
			assert(false);

			return;
		}
	}

	for (auto pGeometry : pModel->getGeometries())
	{
		assert(pGeometry->getInstances().size() == 1);
		if ((pGeometry->getBBTransformation() == nullptr) ||
			(pGeometry->getBBMin() == nullptr) ||
			(pGeometry->getBBMax() == nullptr))
		{
			continue;
		}

		_vector3d vecBoundingBoxMin = { pGeometry->getBBMin()->x, pGeometry->getBBMin()->y, pGeometry->getBBMin()->z };
		_vector3d vecBoundingBoxMax = { pGeometry->getBBMax()->x, pGeometry->getBBMax()->y, pGeometry->getBBMax()->z };

		_vector3d vecVertexBufferOffset;
		GetVertexBufferOffset(pGeometry->getOwlModel(), (double*)&vecVertexBufferOffset);

		double dScaleFactor = (double)pModel->getOriginalBoundingSphereDiameter() / 2.;

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
		TransformBBVertex(vecMin1, pGeometry->getBBTransformation(), vecVertexBufferOffset, dScaleFactor);

		_vector3d vecMin2 = { vecBoundingBoxMax.x, vecBoundingBoxMin.y, vecBoundingBoxMin.z };
		TransformBBVertex(vecMin2, pGeometry->getBBTransformation(), vecVertexBufferOffset, dScaleFactor);

		_vector3d vecMin3 = { vecBoundingBoxMax.x, vecBoundingBoxMin.y, vecBoundingBoxMax.z };
		TransformBBVertex(vecMin3, pGeometry->getBBTransformation(), vecVertexBufferOffset, dScaleFactor);

		_vector3d vecMin4 = { vecBoundingBoxMin.x, vecBoundingBoxMin.y, vecBoundingBoxMax.z };
		TransformBBVertex(vecMin4, pGeometry->getBBTransformation(), vecVertexBufferOffset, dScaleFactor);

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
		TransformBBVertex(vecMax1, pGeometry->getBBTransformation(), vecVertexBufferOffset, dScaleFactor);

		_vector3d vecMax2 = { vecBoundingBoxMin.x, vecBoundingBoxMax.y, vecBoundingBoxMax.z };
		TransformBBVertex(vecMax2, pGeometry->getBBTransformation(), vecVertexBufferOffset, dScaleFactor);

		_vector3d vecMax3 = { vecBoundingBoxMin.x, vecBoundingBoxMax.y, vecBoundingBoxMin.z };
		TransformBBVertex(vecMax3, pGeometry->getBBTransformation(), vecVertexBufferOffset, dScaleFactor);

		_vector3d vecMax4 = { vecBoundingBoxMax.x, vecBoundingBoxMax.y, vecBoundingBoxMin.z };
		TransformBBVertex(vecMax4, pGeometry->getBBTransformation(), vecVertexBufferOffset, dScaleFactor);

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

void CRDFOpenGLView::DrawNormalVectors(_model* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (pModel->isDecoration())
	{
		return;
	}

	if (!getShowNormalVectors())
	{
		return;
	}	

	_instance* pSelectedInstance = nullptr;
	if (getController()->getSelectedInstances().size() == 1)
	{		
		auto pSelectedInstanceModel = getController()->getModelByInstance(getController()->getSelectedInstances()[0]->getOwlModel());
		if (pSelectedInstanceModel == pModel)
		{
			pSelectedInstance = getController()->getSelectedInstances()[0];
		}
	}

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(false);
#else
	m_pOGLProgram->_enableLighting(false);
#endif
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

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

	const auto VERTEX_LENGTH = pModel->getVertexLength();
	const float SCALE_FACTOR = getScaleVectors() ? sqrt(pow(fWorldXmax - fWorldXmin, 2.f) + pow(fWorldYmax - fWorldYmin, 2.f) + pow(fWorldZmax - fWorldZmin, 2.f)) * 0.1f : 1.f;

	bool bIsNew = false;
	GLuint iVAO = m_oglBuffers.getVAOcreateNewIfNeeded(NORMAL_VECS_VAO, bIsNew);

	if (iVAO == 0)
	{
		assert(false);

		return;
	}

	GLuint iVBO = 0;

	if (bIsNew)
	{
		glBindVertexArray(iVAO);

		iVBO = m_oglBuffers.getBufferCreateNewIfNeeded(NORMAL_VECS_VBO, bIsNew);
		if ((iVBO == 0) || !bIsNew)
		{
			assert(false);

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
			assert(false);

			return;
		}
	}

	// X, Y, Z, Nx, Ny, Nz, Tx, Ty
	vector<float> vecVertices;

	if (pSelectedInstance == nullptr)
	{
		for (auto pGeometry : pModel->getGeometries())
		{
			assert(pGeometry->getInstances().size() == 1);
			if (!pGeometry->getInstances()[0]->getEnable())
			{
				continue;
			}

			auto& vecTriangles = pGeometry->getTriangles();
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
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 3] * SCALE_FACTOR);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 4] * SCALE_FACTOR);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 5] * SCALE_FACTOR);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // for (auto pGeometry : ...
	} // if (pSelectedInstance == nullptr)
	else
	{
		auto pGeometry = pSelectedInstance->getGeometry();
		assert(pGeometry->getInstances().size() == 1);

		auto& vecTriangles = pGeometry->getTriangles();
		assert(!vecTriangles.empty());

		if (m_iPointedFace == -1)
		{
			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				auto pTriangle = const_cast<_primitives*>(&vecTriangles[iTriangle]);

				for (int64_t iIndex = pTriangle->startIndex();
					iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
					iIndex++)
				{
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 3] * SCALE_FACTOR);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 4] * SCALE_FACTOR);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 5] * SCALE_FACTOR);

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
			assert((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

			auto pTriangle = const_cast<_primitives*>(&vecTriangles[m_iPointedFace]);

			for (int64_t iIndex = pTriangle->startIndex();
				iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
				iIndex++)
			{
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

				vecVertices.push_back(0.f); // Nx
				vecVertices.push_back(0.f); // Ny
				vecVertices.push_back(0.f); // Nz
				vecVertices.push_back(0.f); // Tx
				vecVertices.push_back(0.f); // Ty

				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
					pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 3] * SCALE_FACTOR);
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
					pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 4] * SCALE_FACTOR);
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
					pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 5] * SCALE_FACTOR);

				vecVertices.push_back(0.f); // Nx
				vecVertices.push_back(0.f); // Ny
				vecVertices.push_back(0.f); // Nz
				vecVertices.push_back(0.f); // Tx
				vecVertices.push_back(0.f); // Ty
			} // for (size_t iIndex = ...
		} // else if (m_iPointedFace == -1)
	} // else if (pSelectedInstance == nullptr)

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

void CRDFOpenGLView::DrawTangentVectors(_model* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (pModel->isDecoration())
	{
		return;
	}

	if (!getShowTangentVectors())
	{
		return;
	}

	_instance* pSelectedInstance = nullptr;
	if (getController()->getSelectedInstances().size() == 1)
	{
		auto pSelectedInstanceModel = getController()->getModelByInstance(getController()->getSelectedInstances()[0]->getOwlModel());
		if (pSelectedInstanceModel == pModel)
		{
			pSelectedInstance = getController()->getSelectedInstances()[0];
		}
	}

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(false);
#else
	m_pOGLProgram->_enableLighting(false);
#endif
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

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

	const auto VERTEX_LENGTH = pModel->getVertexLength();
	const float SCALE_FACTOR = getScaleVectors() ? sqrt(pow(fWorldXmax - fWorldXmin, 2.f) + pow(fWorldYmax - fWorldYmin, 2.f) + pow(fWorldZmax - fWorldZmin, 2.f)) * 0.1f : 1.f;

	bool bIsNew = false;
	GLuint iVAO = m_oglBuffers.getVAOcreateNewIfNeeded(TANGENT_VECS_VAO, bIsNew);

	if (iVAO == 0)
	{
		assert(false);

		return;
	}

	GLuint iVBO = 0;

	if (bIsNew)
	{
		glBindVertexArray(iVAO);

		iVBO = m_oglBuffers.getBufferCreateNewIfNeeded(TANGENT_VECS_VBO, bIsNew);
		if ((iVBO == 0) || !bIsNew)
		{
			assert(false);

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
			assert(false);

			return;
		}
	}

	// X, Y, Z, Nx, Ny, Nz, Tx, Ty
	vector<float> vecVertices;

	if (pSelectedInstance == nullptr)
	{
		for (auto pGeometry : pModel->getGeometries())
		{
			assert(pGeometry->getInstances().size() == 1);
			if (!pGeometry->getInstances()[0]->getEnable())
			{
				continue;
			}

			auto& vecTriangles = pGeometry->getTriangles();
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
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 12] * SCALE_FACTOR);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 13] * SCALE_FACTOR);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 14] * SCALE_FACTOR);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // for (auto pGeometry : ...
	} // if (pSelectedInstance == nullptr)
	else
	{
		auto pGeometry = pSelectedInstance->getGeometry();
		assert(pGeometry->getInstances().size() == 1);

		auto& vecTriangles = pGeometry->getTriangles();
		assert(!vecTriangles.empty());

		if (m_iPointedFace == -1)
		{
			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				auto pTriangle = const_cast<_primitives*>(&vecTriangles[iTriangle]);

				for (int64_t iIndex = pTriangle->startIndex();
					iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
					iIndex++)
				{
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 12] * SCALE_FACTOR);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 13] * SCALE_FACTOR);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 14] * SCALE_FACTOR);

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
			assert((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

			auto pTriangle = const_cast<_primitives*>(&vecTriangles[m_iPointedFace]);

			for (int64_t iIndex = pTriangle->startIndex();
				iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
				iIndex++)
			{
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

				vecVertices.push_back(0.f); // Nx
				vecVertices.push_back(0.f); // Ny
				vecVertices.push_back(0.f); // Nz
				vecVertices.push_back(0.f); // Tx
				vecVertices.push_back(0.f); // Ty

				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
					pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 12] * SCALE_FACTOR);
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
					pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 13] * SCALE_FACTOR);
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
					pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 14] * SCALE_FACTOR);

				vecVertices.push_back(0.f); // Nx
				vecVertices.push_back(0.f); // Ny
				vecVertices.push_back(0.f); // Nz
				vecVertices.push_back(0.f); // Tx
				vecVertices.push_back(0.f); // Ty
			} // for (size_t iIndex = ...
		} // else if (m_iPointedFace == -1)
	} // else if (pSelectedInstance == nullptr)

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

void CRDFOpenGLView::DrawBiNormalVectors(_model* pModel)
{
	if (pModel == nullptr)
	{
		return;
	}

	if (pModel->isDecoration())
	{
		return;
	}

	if (!getShowBiNormalVectors())
	{
		return;
	}

	_instance* pSelectedInstance = nullptr;
	if (getController()->getSelectedInstances().size() == 1)
	{
		auto pSelectedInstanceModel = getController()->getModelByInstance(getController()->getSelectedInstances()[0]->getOwlModel());
		if (pSelectedInstanceModel == pModel)
		{
			pSelectedInstance = getController()->getSelectedInstances()[0];
		}
	}

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(false);
#else
	m_pOGLProgram->_enableLighting(false);
#endif
	m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

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

	const auto VERTEX_LENGTH = pModel->getVertexLength();
	const float SCALE_FACTOR = getScaleVectors() ? sqrt(pow(fWorldXmax - fWorldXmin, 2.f) + pow(fWorldYmax - fWorldYmin, 2.f) + pow(fWorldZmax - fWorldZmin, 2.f)) * 0.1f : 1.f;

	bool bIsNew = false;
	GLuint iVAO = m_oglBuffers.getVAOcreateNewIfNeeded(BINORMAL_VECS_VAO, bIsNew);

	if (iVAO == 0)
	{
		assert(false);

		return;
	}

	GLuint iVBO = 0;

	if (bIsNew)
	{
		glBindVertexArray(iVAO);

		iVBO = m_oglBuffers.getBufferCreateNewIfNeeded(BINORMAL_VECS_VBO, bIsNew);
		if ((iVBO == 0) || !bIsNew)
		{
			assert(false);

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
			assert(false);

			return;
		}
	}

	// X, Y, Z, Nx, Ny, Nz, Tx, Ty
	vector<float> vecVertices;

	if (pSelectedInstance == nullptr)
	{
		for (auto pGeometry : pModel->getGeometries())
		{
			assert(pGeometry->getInstances().size() == 1);
			if (!pGeometry->getInstances()[0]->getEnable())
			{
				continue;
			}

			auto& vecTriangles = pGeometry->getTriangles();
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
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 15] * SCALE_FACTOR);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 16] * SCALE_FACTOR);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 17] * SCALE_FACTOR);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty
				} // for (size_t iIndex = ...
			} // for (size_t iTriangle = ...
		} // for (auto pGeometry : ...
	} // if (pSelectedInstance == nullptr)
	else
	{
		auto pGeometry = pSelectedInstance->getGeometry();
		assert(pGeometry->getInstances().size() == 1);

		auto& vecTriangles = pGeometry->getTriangles();
		assert(!vecTriangles.empty());

		if (m_iPointedFace == -1)
		{
			for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
			{
				auto pTriangle = const_cast<_primitives*>(&vecTriangles[iTriangle]);

				for (int64_t iIndex = pTriangle->startIndex();
					iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
					iIndex++)
				{
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

					vecVertices.push_back(0.f); // Nx
					vecVertices.push_back(0.f); // Ny
					vecVertices.push_back(0.f); // Nz
					vecVertices.push_back(0.f); // Tx
					vecVertices.push_back(0.f); // Ty

					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 15] * SCALE_FACTOR);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 16] * SCALE_FACTOR);
					vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
						pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 17] * SCALE_FACTOR);

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
			assert((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

			auto pTriangle = const_cast<_primitives*>(&vecTriangles[m_iPointedFace]);

			for (int64_t iIndex = pTriangle->startIndex();
				iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
				iIndex++)
			{
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0]);
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1]);
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2]);

				vecVertices.push_back(0.f); // Nx
				vecVertices.push_back(0.f); // Ny
				vecVertices.push_back(0.f); // Nz
				vecVertices.push_back(0.f); // Tx
				vecVertices.push_back(0.f); // Ty

				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 0] +
					pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 15] * SCALE_FACTOR);
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 1] +
					pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 16] * SCALE_FACTOR);
				vecVertices.push_back(pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 2] +
					pGeometry->getVertices()[(pGeometry->getIndices()[iIndex] * VERTEX_LENGTH) + 17] * SCALE_FACTOR);

				vecVertices.push_back(0.f); // Nx
				vecVertices.push_back(0.f); // Ny
				vecVertices.push_back(0.f); // Nz
				vecVertices.push_back(0.f); // Tx
				vecVertices.push_back(0.f); // Ty
			} // for (size_t iIndex = ...
		} // else if (m_iPointedFace == -1)
	} // else if (pSelectedInstance == nullptr)

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

void CRDFOpenGLView::DrawNavigatorModelSelectionBuffers(
	_model* pM,
	int /*iViewportX*/, int iViewportY,
	int iViewportWidth, int /*iViewportHeight*/,
	_oglSelectionFramebuffer* pInstanceSelectionFrameBuffer)
{
	auto pNavigatorModel = dynamic_cast<CRDFModel*>(pM);
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

	//DrawInstancesFrameBuffer(pNavigatorModel, pInstanceSelectionFrameBuffer);
}
void CRDFOpenGLView::DrawFacesFrameBuffer()
{
	if (m_pPointedFaceInstance == nullptr)
	{
		return;
	}

	auto pGeometry = m_pPointedFaceInstance->getGeometry();
	assert(pGeometry->getInstances().size() == 1);

	//
	// Create a frame buffer
	//

	BOOL bResult = m_pOGLContext->makeCurrent();
	VERIFY(bResult);

	m_pPointFaceFrameBuffer->create();

	// Selection colors
	if (m_pPointFaceFrameBuffer->encoding().empty())
	{
		auto& vecTriangles = pGeometry->getTriangles();
		assert(!vecTriangles.empty());

		for (int64_t iTriangle = 0; iTriangle < (int64_t)vecTriangles.size(); iTriangle++)
		{
			float fR, fG, fB;
			_i64RGBCoder::encode(iTriangle, fR, fG, fB);

			m_pPointFaceFrameBuffer->encoding()[iTriangle] = _color(fR, fG, fB);
		}
	}

	//
	// Draw
	//

	m_pPointFaceFrameBuffer->bind();

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

	_oglUtils::checkForErrors();

	GLuint iVAO = m_oglBuffers.findVAO(pGeometry);
	if (iVAO == 0)
	{
		assert(false);

		return;
	}

	bool bIsNew = false;
	GLuint iIBO = m_oglBuffers.getBufferCreateNewIfNeeded(FACE_SELECTION_IBO, bIsNew);

	if (iIBO == 0)
	{
		assert(false);

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

	auto& vecTriangles = pGeometry->getTriangles();
	assert(!vecTriangles.empty());

	for (size_t iTriangle = 0; iTriangle < vecTriangles.size(); iTriangle++)
	{
		auto pTriangle = const_cast<_primitives*>(&vecTriangles[iTriangle]);

		vector<unsigned int> vecIndices;
		for (int64_t iIndex = pTriangle->startIndex();
			iIndex < pTriangle->startIndex() + pTriangle->indicesCount();
			iIndex++)
		{
			vecIndices.push_back(pGeometry->getIndices()[iIndex]);
		}

		if (!vecIndices.empty())
		{
			auto itSelectionColor = m_pPointFaceFrameBuffer->encoding().find(iTriangle);
			assert(itSelectionColor != m_pPointFaceFrameBuffer->encoding().end());

			m_pOGLProgram->_setAmbientColor(
				itSelectionColor->second.r(),
				itSelectionColor->second.g(),
				itSelectionColor->second.b());

			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vecIndices.size(), vecIndices.data(), GL_DYNAMIC_DRAW);
			glDrawElementsBaseVertex(GL_TRIANGLES,
				(GLsizei)vecIndices.size(),
				GL_UNSIGNED_INT,
				(void*)(sizeof(GLuint) * 0),
				pGeometry->VBOOffset());
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_pPointFaceFrameBuffer->unbind();

	_oglUtils::checkForErrors();
}

void CRDFOpenGLView::DrawPointedFace()
{
	if (m_pPointedFaceInstance == nullptr)
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
	auto pGeometry = m_pPointedFaceInstance->getGeometry();
	assert(pGeometry->getInstances().size() == 1);

	auto& vecTriangles = pGeometry->getTriangles();

	assert(!vecTriangles.empty());
	assert((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

#ifdef _BLINN_PHONG_SHADERS
	m_pOGLProgram->_enableBlinnPhongModel(false);
#else
	m_pOGLProgram->_enableLighting(false);
#endif
	m_pOGLProgram->_setAmbientColor(0.f, 1.f, 0.f);
	m_pOGLProgram->_setTransparency(1.f);

	_oglUtils::checkForErrors();	

	GLuint iVAO = m_oglBuffers.findVAO(pGeometry);
	if (iVAO == 0)
	{
		assert(false);

		return;
	}

	bool bIsNew = false;
	GLuint iIBO = m_oglBuffers.getBufferCreateNewIfNeeded(FACE_SELECTION_IBO, bIsNew);

	if (iIBO == 0)
	{
		assert(false);

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
		vecIndices.push_back(pGeometry->getIndices()[iIndex]);
	}

	if (!vecIndices.empty())
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vecIndices.size(), vecIndices.data(), GL_DYNAMIC_DRAW);
		glDrawElementsBaseVertex(GL_TRIANGLES,
			(GLsizei)vecIndices.size(),
			GL_UNSIGNED_INT,
			(void*)(sizeof(GLuint) * 0),
			pGeometry->VBOOffset());

		if (m_iNearestVertex != -1)
		{
			vecIndices = vector<unsigned int>{ (unsigned int)m_iNearestVertex };

			m_pOGLProgram->_setAmbientColor(0.f, 0.f, 0.f);

			glDisable(GL_DEPTH_TEST);
			glEnable(GL_PROGRAM_POINT_SIZE);			

			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vecIndices.size(), vecIndices.data(), GL_DYNAMIC_DRAW);
			glDrawElementsBaseVertex(GL_POINTS,
				(GLsizei)vecIndices.size(),
				GL_UNSIGNED_INT,
				(void*)(sizeof(GLuint) * 0),
				pGeometry->VBOOffset());
			
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_PROGRAM_POINT_SIZE);
		} // if (m_iNearestVertex != -1)
	} // if (!vecIndices.empty())

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	_oglUtils::checkForErrors();
}

pair<int64_t, int64_t> CRDFOpenGLView::GetNearestVertex(_model* pM, float fX, float fY, float fZ, float& fVertexX, float& fVertexY, float& fVertexZ)
{	
	if (pM == nullptr)
	{
		return pair<int64_t, int64_t>(-1, -1);
	}

	if (m_pSelectedInstance == nullptr)
	{
		return pair<int64_t, int64_t>(-1, -1);
	}

	if ((m_pSelectedInstance->getModel() != pM->getInstance()) || !m_pSelectedInstance->getEnable())
	{
		return pair<int64_t, int64_t>(-1, -1);
	}

	if (m_iPointedFace == -1)
	{
		return pair<int64_t, int64_t>(-1, -1);
	}

	const auto VERTEX_LENGTH = pM->getVertexLength();

	float* pVertices = m_pSelectedInstance->getVertices();
	ASSERT(pVertices != nullptr);

	int32_t* pIndices = m_pSelectedInstance->getIndices();
	ASSERT(pIndices != nullptr);

	/*
	* Triangles
	*/
	auto& vecTriangles = m_pSelectedInstance->getTriangles();

	assert(!vecTriangles.empty());
	assert((m_iPointedFace >= 0) && (m_iPointedFace < (int64_t)vecTriangles.size()));

	int64_t iVertexIndex = -1;
	int64_t iConcFaceVertexIndex = -1;
	double dMinDistance = DBL_MAX;

	auto pConceptulFace = const_cast<_primitives*>(&vecTriangles[m_iPointedFace]);

	int64_t iZeroBasedIndex = 0;
	for (int64_t iIndex = pConceptulFace->startIndex();
		iIndex < pConceptulFace->startIndex() + pConceptulFace->indicesCount();
		iIndex++, iZeroBasedIndex++)
	{
		fVertexX = pVertices[(pIndices[iIndex] * VERTEX_LENGTH) + 0];
		fVertexY = pVertices[(pIndices[iIndex] * VERTEX_LENGTH) + 1];
		fVertexZ = pVertices[(pIndices[iIndex] * VERTEX_LENGTH) + 2];

		double dDistance = sqrt(pow(fX - fVertexX, 2.f) + pow(fY - fVertexY, 2.f) + pow(fZ - fVertexZ, 2.f));
		if (dMinDistance > dDistance)
		{
			iConcFaceVertexIndex = iZeroBasedIndex;
			iVertexIndex = pIndices[iIndex];			
			dMinDistance = dDistance;
		}
	}

	return pair<int64_t, int64_t>(iConcFaceVertexIndex, iVertexIndex);
}

void CRDFOpenGLView::PointNavigatorInstance(const CPoint& point)
{
	/*auto pController = GetController();
	if (pController == nullptr)
	{
		assert(false);

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

		BOOL bResult = m_pOGLContext->makeCurrent();
		VERIFY(bResult);

		CRect rcClient;
		m_pWnd->GetClientRect(&rcClient);

		iWidth = rcClient.Width();
		iHeight = rcClient.Height();

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
				assert(pPointedInstance != nullptr);
			}

			if (m_pNavigatorPointedInstance != pPointedInstance)
			{
				m_pNavigatorPointedInstance = pPointedInstance;

				_redraw();
			}
		}
	}*/
}

bool CRDFOpenGLView::SelectNavigatorInstance()
{
	if (m_pNavigatorPointedInstance == nullptr)
	{
		return false;
	}

	wstring strInstanceName = m_pNavigatorPointedInstance->getName();
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
void CRDFOpenGLView::OnMouseMoveEvent(UINT nFlags, const CPoint& point)
{
//	auto pController = GetController();
//	if (pController == nullptr)
//	{
//		assert(false);
//
//		return;
//	}
//
//	auto pModel = pController->GetModel();
//	if (pModel == nullptr)
//	{
//		return;
//	}
//
//	/*
//	* Selection
//	*/
//	if (((nFlags & MK_LBUTTON) != MK_LBUTTON) && ((nFlags & MK_MBUTTON) != MK_MBUTTON) && ((nFlags & MK_RBUTTON) != MK_RBUTTON))
//	{
//		/*
//		* Select an instance
//		*/
//		if (m_pSelectInstanceFrameBuffer->isInitialized())
//		{
//			int iWidth = 0;
//			int iHeight = 0;
//
//			BOOL bResult = m_pOGLContext->makeCurrent();
//			VERIFY(bResult);
//
//			CRect rcClient;
//			m_pWnd->GetClientRect(&rcClient);
//
//			iWidth = rcClient.Width();
//			iHeight = rcClient.Height();
//
//			GLubyte arPixels[4];
//			memset(arPixels, 0, sizeof(GLubyte) * 4);
//
//			double dX = (double)point.x * ((double)BUFFER_SIZE / (double)iWidth);
//			double dY = ((double)iHeight - (double)point.y) * ((double)BUFFER_SIZE / (double)iHeight);
//
//			m_pSelectInstanceFrameBuffer->bind();
//
//			glReadPixels(
//				(GLint)dX,
//				(GLint)dY,
//				1, 1,
//				GL_RGBA,
//				GL_UNSIGNED_BYTE,
//				arPixels);
//
//			m_pSelectInstanceFrameBuffer->unbind();
//
//			CRDFInstance* pPointedInstance = nullptr;
//			if (arPixels[3] != 0)
//			{
//				int64_t iInstanceID = _i64RGBCoder::decode(arPixels[0], arPixels[1], arPixels[2]);
//				pPointedInstance = pModel->GetInstanceByID(iInstanceID);
//				assert(pPointedInstance != nullptr);
//			}
//
//			if (m_pPointedInstance != pPointedInstance)
//			{
//				m_pPointedInstance = pPointedInstance;
//
//				_redraw();
//			}
//			
//			if (m_pPointedInstance == nullptr)
//			{
//				PointNavigatorInstance(point);
//			}
//		} // if (m_pSelectInstanceFrameBuffer->isInitialized())
//
//		/*
//		* Select a face
//		*/
//		if ((m_pPointFaceFrameBuffer->isInitialized() != 0) && 
//			(m_pSelectedInstance != nullptr) && 
//			m_pSelectedInstance->getEnable())
//		{
//			int iWidth = 0;
//			int iHeight = 0;
//
//			BOOL bResult = m_pOGLContext->makeCurrent();
//			VERIFY(bResult);
//
//			CRect rcClient;
//			m_pWnd->GetClientRect(&rcClient);
//
//			iWidth = rcClient.Width();
//			iHeight = rcClient.Height();
//
//			GLubyte arPixels[4];
//			memset(arPixels, 0, sizeof(GLubyte) * 4);
//
//			double dX = (double)point.x * ((double)BUFFER_SIZE / (double)iWidth);
//			double dY = ((double)iHeight - (double)point.y) * ((double)BUFFER_SIZE / (double)iHeight);
//
//			m_pPointFaceFrameBuffer->bind();
//
//			glReadPixels(
//				(GLint)dX,
//				(GLint)dY,
//				1, 1,
//				GL_RGBA,
//				GL_UNSIGNED_BYTE,
//				arPixels);
//
//			m_pPointFaceFrameBuffer->unbind();
//
//			int64_t iPointedFace = -1;
//			if (arPixels[3] != 0)
//			{
//				iPointedFace = _i64RGBCoder::decode(arPixels[0], arPixels[1], arPixels[2]);
//				assert(m_pPointFaceFrameBuffer->encoding().find(iPointedFace) != m_pPointFaceFrameBuffer->encoding().end());
//			}
//
//			if (m_iPointedFace != iPointedFace)
//			{
//				m_iPointedFace = iPointedFace;
//				m_iNearestVertex = -1;
//
//				_redraw();
//			}
//		} // if ((m_pPointFaceFrameBuffer->isInitialized() != 0) && ...
//	} // if (((nFlags & MK_LBUTTON) != MK_LBUTTON) && ...
//
//#ifdef _TOOLTIPS_SUPPORT
//	if (m_pPointedInstance != nullptr)
//	{
//		clock_t timeSpan = clock() - m_tmShowTooltip;
//		if (timeSpan >= 200)
//		{
//			CString strInstanceMetaData = pModel->GetInstanceMetaData(m_pPointedInstance);
//
//			GLdouble dX = 0.;
//			GLdouble dY = 0.;
//			GLdouble dZ = 0.;
//			//if (GetOGLPos(point.x, point.y, -FLT_MAX, dX, dY, dZ))
//			//{
//			//	_vector3d vecVertexBufferOffset;
//			//	GetVertexBufferOffset(pModel->getInstance(), (double*)&vecVertexBufferOffset);
//
//			//	auto dScaleFactor = pModel->GetOriginalBoundingSphereDiameter() / 2.;
//
//			//	GLdouble dWorldX = -vecVertexBufferOffset.x + (dX * dScaleFactor);
//			//	GLdouble dWorldY = -vecVertexBufferOffset.y + (dY * dScaleFactor);
//			//	GLdouble dWorldZ = -vecVertexBufferOffset.z + (dZ * dScaleFactor);
//
//			//	strInstanceMetaData += L"\n\n";
//			//	strInstanceMetaData += L"X/Y/Z: ";
//			//	strInstanceMetaData += to_wstring(dWorldX).c_str();
//			//	strInstanceMetaData += L", ";
//			//	strInstanceMetaData += to_wstring(dWorldY).c_str();
//			//	strInstanceMetaData += L", ";
//			//	strInstanceMetaData += to_wstring(dWorldZ).c_str();
//
//			//	if (m_iPointedFace != -1)
//			//	{
//			//		strInstanceMetaData += L"\n";
//			//		strInstanceMetaData += L"Conceptual Face: ";
//			//		strInstanceMetaData += to_wstring(m_iPointedFace).c_str();
//
//			//		float fVertexX = 0.f;
//			//		float fVertexY = 0.f;
//			//		float fVertexZ = 0.f;
//			//		pair<int64_t, int64_t> prVertexIndex = GetNearestVertex(pModel, dX, dY, dZ, fVertexX, fVertexY, fVertexZ);
//			//		if (prVertexIndex.first != -1)
//			//		{
//			//			strInstanceMetaData += L"\n";
//			//			strInstanceMetaData += L"Nearest Vertex: ";
//			//			strInstanceMetaData += to_wstring(prVertexIndex.first).c_str();
//			//			strInstanceMetaData += L" (";
//			//			strInstanceMetaData += to_wstring(prVertexIndex.second).c_str();
//			//			strInstanceMetaData += L")";
//
//			//			m_iNearestVertex = prVertexIndex.second;
//
//			//			_redraw();
//			//		}
//			//	} // if (m_iPointedFace != -1)
//			//} // if (GetOGLPos(point.x, point.y, -FLT_MAX, dX, dY, dZ))
//
//			m_tmShowTooltip = clock();
//
//			_showTooltip(TOOLTIP_INFORMATION, strInstanceMetaData);
//		} // if (timeSpan >= ...
//	} // if (m_pPointedInstance != nullptr)
//	else
//	{
//		_hideTooltip();
//	}
//#endif
//
//	if (m_ptPrevMousePosition.x == -1)
//	{
//		return;
//	}
//
//	/*
//	* Rotate
//	*/
//	if ((nFlags & MK_LBUTTON) == MK_LBUTTON)
//	{
//		_rotateMouseLButton(
//			(float)point.y - (float)m_ptPrevMousePosition.y,
//			(float)point.x - (float)m_ptPrevMousePosition.x);
//
//		m_ptPrevMousePosition = point;
//
//		return;
//	}
//
//	/*
//	* Zoom
//	*/
//	if ((nFlags & MK_MBUTTON) == MK_MBUTTON)
//	{
//		_zoomMouseMButton(point.y - m_ptPrevMousePosition.y);
//
//		m_ptPrevMousePosition = point;
//
//		return;
//	}
//
//	/*
//	* Pan
//	*/
//	if ((nFlags & MK_RBUTTON) == MK_RBUTTON)
//	{
//		CRect rcClient;
//		m_pWnd->GetClientRect(&rcClient);
//
//		_panMouseRButton(
//			((float)point.x - (float)m_ptPrevMousePosition.x) / rcClient.Width(),
//			((float)point.y - (float)m_ptPrevMousePosition.y) / rcClient.Height());
//
//		m_ptPrevMousePosition = point;
//
//		return;
//	}
}

// ------------------------------------------------------------------------------------------------
// http://nehe.gamedev.net/article/using_gluunproject/16013/
bool CRDFOpenGLView::GetOGLPos(int iX, int iY, float fDepth, GLdouble& dX, GLdouble& dY, GLdouble& dZ)
{
	//auto pController = GetController();
	//if (pController == nullptr)
	//{
	//	return false;
	//}

	//auto pMainModel = pController->GetModel();
	//if (pMainModel == nullptr)
	//{
	//	return false;
	//}
	//
	//float fXmin = -1.f;
	//float fXmax = 1.f;
	//float fYmin = -1.f;
	//float fYmax = 1.f;
	//float fZmin = -1.f;
	//float fZmax = 1.f;
	//pMainModel->GetWorldDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	//CRect rcClient;
	//m_pWnd->GetClientRect(&rcClient);

	//_prepare(
	//	0, 0,
	//	rcClient.Width(), rcClient.Height(),
	//	fXmin, fXmax,
	//	fYmin, fYmax,
	//	fZmin, fZmax,
	//	true,
	//	true);

	///* Store Matrices */
	//GLfloat arModelViewMatrix[16];
	//glGetUniformfv(m_pOGLProgram->_getID(), glGetUniformLocation(m_pOGLProgram->_getID(), "ModelViewMatrix"), arModelViewMatrix);

	//GLfloat arProjectionMatrix[16];
	//glGetUniformfv(m_pOGLProgram->_getID(), glGetUniformLocation(m_pOGLProgram->_getID(), "ProjectionMatrix"), arProjectionMatrix);

	///* Model - Restore Z buffer */
	//DrawModel(pMainModel);

	//GLint arViewport[4] = { 0, 0, rcClient.Width(), rcClient.Height() };

	//GLdouble arModelView[16];
	//GLdouble arProjection[16];
	//for (int i = 0; i < 16; i++)
	//{
	//	arModelView[i] = arModelViewMatrix[i];
	//	arProjection[i] = arProjectionMatrix[i];
	//}

	//GLdouble dWinX = (double)iX;
	//GLdouble dWinY = (double)arViewport[3] - (double)iY;

	//double dWinZ = 0.;
	//if (fDepth == -FLT_MAX)
	//{
	//	float fWinZ = 0.f;
	//	glReadPixels(iX, (int)dWinY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &fWinZ);

	//	dWinZ = fWinZ;

	//	_oglUtils::checkForErrors();
	//}
	//else
	//{
	//	dWinZ = fDepth;
	//}

	//if (dWinZ >= 1.)
	//{
	//	return false;
	//}

	//GLint iResult = gluUnProject(dWinX, dWinY, dWinZ, arModelView, arProjection, arViewport, &dX, &dY, &dZ);

	//_oglUtils::checkForErrors();

	//return iResult == GL_TRUE;
	return false;
}

// ------------------------------------------------------------------------------------------------
void CRDFOpenGLView::OGLProject(GLdouble dInX, GLdouble dInY, GLdouble dInZ, GLdouble & dOutX, GLdouble & dOutY, GLdouble & dOutZ) const
{
	GLint arViewport[4];
	GLdouble arModelView[16];
	GLdouble arProjection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, arModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, arProjection);
	glGetIntegerv(GL_VIEWPORT, arViewport);

	GLint iResult = gluProject(dInX, dInY, dInZ, arModelView, arProjection, arViewport, &dOutX, &dOutY, &dOutZ);
	VERIFY(iResult == GL_TRUE);
}

// ------------------------------------------------------------------------------------------------
// https://community.khronos.org/t/taking-screenshots-how-to/19154/3
void CRDFOpenGLView::TakeScreenshot(unsigned char*& arPixels, unsigned int& iWidth, unsigned int& iHeight)
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
bool CRDFOpenGLView::SaveScreenshot(const wchar_t* szFilePath)
{
	unsigned char* arPixels;
	unsigned int iWidth;
	unsigned int iHeight;
	TakeScreenshot(arPixels, iWidth, iHeight);

	bool bResult = ::SaveScreenshot(arPixels, iWidth, iHeight, szFilePath);

	free(arPixels);

	return bResult;
}