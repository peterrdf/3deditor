#ifndef _OPEN_GL_IFC_VIEW_H_
#define _OPEN_GL_IFC_VIEW_H_

#include "Generic.h"
#include "RDFView.h"
#include "_oglUtils.h"
#include "RDFInstance.h"
#include "RDFModel.h"
#include <ctime>

// ------------------------------------------------------------------------------------------------
class CRDFOpenGLView : public _oglView
{

#pragma region Members

private: // Members

	// Mouse
	CPoint m_ptStartMousePosition;
	CPoint m_ptPrevMousePosition;

	// Selection	
	_oglSelectionFramebuffer* m_pFaceSelectionFrameBuffer;
	int64_t m_iPointedFace;
	int64_t m_iNearestVertex;

	_oglSelectionFramebuffer* m_pNavigatorSelectionFrameBuffer;
	CRDFInstance* m_pNavigatorPointedInstance;
	
	// Materials
	_material* m_pSelectedInstanceMaterial;
	_material* m_pPointedInstanceMaterial;
	_material* m_pNavigatorPointedInstanceMaterial;

	// Tooltip
	clock_t m_tmShowTooltip;

#pragma endregion // Members

public: // Methods
	
	CRDFOpenGLView(CWnd* pWnd);	
	virtual ~CRDFOpenGLView();

	// _oglView
	virtual void _postDraw() override;

	// Test
	void SetRotation(float fX, float fY, BOOL bRedraw);
	void GetRotation(float& fX, float& fY);
	void SetTranslation(float fX, float fY, float fZ, BOOL bRedraw);
	void GetTranslation(float& fX, float& fY, float& fZ);

	// Mouse
	void OnMouseEvent(enumMouseEvent enEvent, UINT nFlags, CPoint point);
	void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	// Keyboard
	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	// CRDFView	
	/*virtual void OnModelChanged() override;
	virtual void OnWorldDimensionsChanged() override;
	virtual void OnInstancePropertyEdited(CRDFInstance * pInstance, CRDFProperty * pProperty) override;
	virtual void OnNewInstanceCreated(CRDFView * pSender, CRDFInstance * pInstance) override;
	virtual void OnInstanceDeleted(CRDFView * pSender, int64_t iInstance) override;
	virtual void OnInstancesDeleted(CRDFView * pSender) override;
	virtual void OnMeasurementsAdded(CRDFView * pSender, CRDFInstance * pInstance) override;
	virtual void OnInstanceSelected(CRDFView * pSender) override;
	virtual void OnInstancePropertySelected() override;
	virtual void OnInstancesEnabledStateChanged() override;
	virtual void OnApplicationPropertyChanged(CRDFView* pSender, enumApplicationProperty enApplicationProperty) override;*/

private: // Methods

	void DrawNavigatorModel(
		CRDFModel* pNavigatorModel,
		int iViewportX, int iViewportY,
		int iViewportWidth, int iViewportHeight);
	void DrawModel(_model* pM);
	void DrawFaces(_model* pM, bool bTransparent);
	void DrawFacesPolygons(_model* pM);
	void DrawConceptualFacesPolygons(_model* pM);
	void DrawLines(_model* pM);
	void DrawPoints(_model* pM);

	// --------------------------------------------------------------------------------------------
	// Bounding box for each 3D object
	void TransformBBVertex(_vector3d& vecBBVertex, const _matrix* pBBTransformation, const _vector3d& vecVertexBufferOffset, double dScaleFactor);

	void DrawBoundingBoxes(_model* pModel);
	void DrawNormalVectors(_model* pModel);
	void DrawTangentVectors(_model* pModel);
	void DrawBiNormalVectors(_model* pModel);
	void DrawMainModelSelectionBuffers(
		_model* pM,
		int iViewportX, int iViewportY,
		int iViewportWidth, int iViewportHeight,
		_oglSelectionFramebuffer* pInstanceSelectionFrameBuffer);
	void DrawNavigatorModelSelectionBuffers(
		_model* pM,
		int iViewportX, int iViewportY,
		int iViewportWidth, int iViewportHeight,
		_oglSelectionFramebuffer* pInstanceSelectionFrameBuffer);
	void DrawInstancesFrameBuffer(_model* pM, _oglSelectionFramebuffer* pInstanceSelectionFrameBuffer);
	void DrawFacesFrameBuffer(_model* pM);
	void DrawPointedFace(_model* pM);
	pair<int64_t, int64_t> GetNearestVertex(_model* pM, float fX, float fY, float fZ, float& fVertexX, float& fVertexY, float& fVertexZ);
	void PointNavigatorInstance(const CPoint& point);
	bool SelectNavigatorInstance();

	// --------------------------------------------------------------------------------------------
	// Handler
	void OnMouseMoveEvent(UINT nFlags, const CPoint& point);

	// --------------------------------------------------------------------------------------------
	// Screen -> Open GL coordinates
	bool GetOGLPos(int iX, int iY, float fDepth, GLdouble& dX, GLdouble& dY, GLdouble& dZ);

	// --------------------------------------------------------------------------------------------
	// Wrapper for gluProject
	void OGLProject(GLdouble dInX, GLdouble dInY, GLdouble dInZ, GLdouble & dOutX, GLdouble & dOutY, GLdouble & dOutZ) const;

public:

	// --------------------------------------------------------------------------------------------
	// https://community.khronos.org/t/taking-screenshots-how-to/19154/3
	void TakeScreenshot(unsigned char*& arPixels, unsigned int& iWidth, unsigned int& iHeight);
	bool SaveScreenshot(const wchar_t* szFilePath);
};

#endif // _OPEN_GL_IFC_VIEW_H_
