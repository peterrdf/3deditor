#ifndef _OPEN_GL_IFC_VIEW_H_
#define _OPEN_GL_IFC_VIEW_H_

#include "Generic.h"
#include "RDFView.h"
#include "_oglUtils.h"
#include "RDFInstance.h"
#include "RDFModel.h"

// ------------------------------------------------------------------------------------------------
class COpenGLRDFView 
	: public _oglRenderer
	, public CRDFView
{

#pragma region Members

private: // Members	

	// Mouse
	CPoint m_ptStartMousePosition;
	CPoint m_ptPrevMousePosition;

	// Selection
	_oglSelectionFramebuffer* m_pInstanceSelectionFrameBuffer;	
	CRDFInstance* m_pPointedInstance;
	CRDFInstance* m_pSelectedInstance;

	// Selection	
	_oglSelectionFramebuffer* m_pFaceSelectionFrameBuffer;
	int64_t m_iPointedFace;	

	_oglSelectionFramebuffer* m_pNavigatorSelectionFrameBuffer;
	CRDFInstance* m_pNavigatorPointedInstance;
	
	// Materials
	_material* m_pSelectedInstanceMaterial;
	_material* m_pPointedInstanceMaterial;
	_material* m_pNavigatorPointedInstanceMaterial;

#pragma endregion // Members

public: // Methods
	
	COpenGLRDFView(CWnd* pWnd);	
	virtual ~COpenGLRDFView();

	// _oglRendererSettings
	virtual _controller* getController() const override;
	virtual _model* getModel() const override;
	virtual void saveSetting(const string& strName, const string& strValue) override;
	virtual string loadSetting(const string& strName) override;

	// Test
	void SetRotation(float fX, float fY, BOOL bRedraw);
	void GetRotation(float& fX, float& fY);
	void SetTranslation(float fX, float fY, float fZ, BOOL bRedraw);
	void GetTranslation(float& fX, float& fY, float& fZ);
	
	// Draw
	void Draw(CDC* pDC);

	// Mouse
	void OnMouseEvent(enumMouseEvent enEvent, UINT nFlags, CPoint point);
	void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	// Keyboard
	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	// CRDFView	
	virtual void OnModelChanged() override;
	virtual void OnWorldDimensionsChanged() override;
	virtual void OnInstancePropertyEdited(CRDFInstance * pInstance, CRDFProperty * pProperty) override;
	virtual void OnNewInstanceCreated(CRDFView * pSender, CRDFInstance * pInstance) override;
	virtual void OnInstanceDeleted(CRDFView * pSender, int64_t iInstance) override;
	virtual void OnInstancesDeleted(CRDFView * pSender) override;
	virtual void OnMeasurementsAdded(CRDFView * pSender, CRDFInstance * pInstance) override;
	virtual void OnInstanceSelected(CRDFView * pSender) override;
	virtual void OnInstancePropertySelected() override;
	virtual void OnInstancesEnabledStateChanged() override;
	virtual void OnApplicationPropertyChanged(CRDFView* pSender, enumApplicationProperty enApplicationProperty) override;

protected: // Methods

	// Overridden
	virtual void OnControllerChanged() override;

private: // Methods

	void LoadModel(CRDFModel* pModel);

	void DrawMainModel(
		CRDFModel* pMainModel,
		CRDFModel* pSceneModel,
		int iViewportX, int iViewportY,
		int iViewportWidth, int iViewportHeight);
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

	void DrawBoundingBoxes(_model* pM);
	void DrawNormalVectors(_model* pM);
	void DrawTangentVectors(_model* pM);
	void DrawBiNormalVectors(_model* pM);
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
	void PointNavigatorInstance(const CPoint& point);
	bool SelectNavigatorInstance();

	// --------------------------------------------------------------------------------------------
	// Handler
	void OnMouseMoveEvent(UINT nFlags, const CPoint& point);

	// --------------------------------------------------------------------------------------------
	// Screen -> Open GL coordinates
	bool GetOGLPos(int iX, int iY, float fDepth, GLdouble& dX, GLdouble& dY, GLdouble& dZ) const;

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
