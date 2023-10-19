#ifndef _OPEN_GL_IFC_VIEW_H_
#define _OPEN_GL_IFC_VIEW_H_

#include "Generic.h"
#include "RDFView.h"
#ifdef _LINUX
#include <wx/wx.h>
#include <wx/glcanvas.h>
#else
#include "_oglUtils.h"
#endif // _LINUX
#include "RDFInstance.h"
#include "RDFModel.h"

#ifdef _LINUX
#define MK_LBUTTON 1
#define MK_MBUTTON 2
#define MK_RBUTTON 4
#endif

// ------------------------------------------------------------------------------------------------
enum class enumMouseEvent : int
{
	Move = 0,
	LBtnDown,
	LBtnUp,
	MBtnDown,
	MBtnUp,
	RBtnDown,
	RBtnUp,
};

// ------------------------------------------------------------------------------------------------
#define CULL_FACES_NONE L"<none>"
#define CULL_FACES_FRONT L"Front"
#define CULL_FACES_BACK L"Back"

// ------------------------------------------------------------------------------------------------
class COpenGLRDFView 
	: public _oglRenderer<CRDFInstance>
	, public CRDFView
{

#pragma region Members

private: // Members

	// UI
	BOOL m_bShowFaces;
	CString m_strCullFaces;
	BOOL m_bShowFacesPolygons;
	BOOL m_bShowConceptualFacesPolygons;
	BOOL m_bShowLines;
	GLfloat m_fLineWidth;
	BOOL m_bShowPoints;
	GLfloat m_fPointSize;
	BOOL m_bShowBoundingBoxes;
	BOOL m_bShowNormalVectors;
	BOOL m_bShowTangenVectors;
	BOOL m_bShowBiNormalVectors;
	BOOL m_bScaleVectors;

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

	_oglSelectionFramebuffer* m_pSceneSelectionFrameBuffer;
	CRDFInstance* m_pScenePointedInstance;
	
	// Materials
	_material* m_pSelectedInstanceMaterial;
	_material* m_pPointedInstanceMaterial;

#pragma endregion // Members

public: // Methods
	
#ifdef _LINUX
    COpenGLRDFView(wxGLCanvas * pWnd);
#else
	COpenGLRDFView(CWnd * pWnd);
#endif //_LINUX
	
	virtual ~COpenGLRDFView();
	
	// UI
	void ShowFaces(BOOL bShow);
	BOOL AreFacesShown() const;
	void SetCullFacesMode(LPCTSTR szMode);
	LPCTSTR GetCullFacesMode() const;
	void ShowFacesPolygons(BOOL bShow);
	BOOL AreFacesPolygonsShown() const;
	void ShowConceptualFacesPolygons(BOOL bShow);
	BOOL AreConceptualFacesPolygonsShown() const;
	void ShowLines(BOOL bShow);
	BOOL AreLinesShown() const;
	void SetLineWidth(GLfloat fWidth);
	GLfloat GetLineWidth() const;
	void ShowPoints(BOOL bShow);
	BOOL ArePointsShown() const;
	void SetPointSize(GLfloat fSize);
	GLfloat GetPointSize() const;
	void ShowBoundingBoxes(BOOL bShow);
	BOOL AreBoundingBoxesShown() const;
	void ShowNormalVectors(BOOL bShow);
	BOOL AreNormalVectorsShown() const;
	void ShowTangentVectors(BOOL bShow);
	BOOL AreTangentVectorsShown() const;
	void ShowBiNormalVectors(BOOL bShow);
	BOOL AreBiNormalVectorsShown() const;
	void ScaleVectors(BOOL bShow);
	BOOL AreVectorsScaled() const;

	void Reset();

	// Test
	void SetRotation(float fX, float fY, BOOL bRedraw);
	void GetRotation(float& fX, float& fY);
	void SetTranslation(float fX, float fY, float fZ, BOOL bRedraw);
	void GetTranslation(float& fX, float& fY, float& fZ);
	
	// Draw
#ifdef _LINUX
    void Draw(wxPaintDC * pDC);
#else
	void Draw(CDC* pDC);
#endif // _LINUX	

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

	void DrawModel(CRDFModel* pModel);
	void DrawSceneComponents();

	// --------------------------------------------------------------------------------------------
	// Faces
	void DrawFaces(CRDFModel* pModel, bool bTransparent);

	// --------------------------------------------------------------------------------------------
	// Faces polygons
	void DrawFacesPolygons(CRDFModel* pModel);

	// --------------------------------------------------------------------------------------------
	// Conceptual faces polygons
	void DrawConceptualFacesPolygons(CRDFModel* pModel);

	// --------------------------------------------------------------------------------------------
	// Lines
	void DrawLines(CRDFModel* pModel);

	// --------------------------------------------------------------------------------------------
	// Points
	void DrawPoints(CRDFModel* pModel);

	// --------------------------------------------------------------------------------------------
	// Bounding box for each 3D object
	void DrawBoundingBoxes(CRDFModel* pModel);

	// --------------------------------------------------------------------------------------------
	// Normal vectors
	void DrawNormalVectors(CRDFModel* pModel);

	// --------------------------------------------------------------------------------------------
	// Tangent vectors
	void DrawTangentVectors(CRDFModel* pModel);

	// --------------------------------------------------------------------------------------------
	// Bi-normal vectors
	void DrawBiNormalVectors(CRDFModel* pModel);

	// --------------------------------------------------------------------------------------------
	// Selection support
	void DrawInstancesFrameBuffer(CRDFModel* pModel);

	// --------------------------------------------------------------------------------------------
	// Selection support
	void DrawFacesFrameBuffer(CRDFModel* pModel);

	// --------------------------------------------------------------------------------------------
	// Selection support
	void DrawPointedFace(CRDFModel* pModel);

	// --------------------------------------------------------------------------------------------
	// Handler
	void OnMouseMoveEvent(UINT nFlags, const CPoint& point);

	// --------------------------------------------------------------------------------------------
	// Screen -> Open GL coordinates
	void GetOGLPos(int iX, int iY, float fDepth, GLfloat & fX, GLfloat & fY, GLfloat & fZ) const;

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
