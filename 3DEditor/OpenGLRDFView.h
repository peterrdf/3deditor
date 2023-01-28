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

#ifdef _LINUX
#define MK_LBUTTON 1
#define MK_MBUTTON 2
#define MK_RBUTTON 4
#endif

enum class enumMouseEvent
{
	Move = 0,
	LBtnDown,
	LBtnUp,
	MBtnDown,
	MBtnUp,
	RBtnDown,
	RBtnUp,
};

enum class enumProjection
{
	Perspective = 0,
	Isometric,
};

enum class enumView
{
	Top = 0,
	Left,
	Right,
	Bottom,
	Front,
	Back,
};

#define CULL_FACES_NONE L"<none>"
#define CULL_FACES_FRONT L"Front"
#define CULL_FACES_BACK L"Back"

class COpenGLRDFView 
	: public _oglRenderer<CRDFInstance>
	, public CRDFView
{

#pragma region Members

private: // Members

#ifdef _LINUX
    wxGLCanvas * m_pWnd;
#else
	CWnd * m_pWnd;
#endif // _LINUX	

	// Projection
	enumProjection  m_enProjection;

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
	void SetRotation(float fX, float fY, BOOL bRedraw);
	void GetRotation(float& fX, float& fY);
	void SetTranslation(float fX, float fY, float fZ, BOOL bRedraw);
	void GetTranslation(float& fX, float& fY, float& fZ);
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
	
	// Draw
#ifdef _LINUX
    void Draw(wxPaintDC * pDC);
#else
	void Draw(CDC * pDC);
#endif // _LINUX

	// Projection
	void SetProjection(enumProjection enProjection);
	enumProjection GetProjection() const;

	// View
	void SetView(enumView enView);

	// Mouse
	void OnMouseEvent(enumMouseEvent enEvent, UINT nFlags, CPoint point);
	void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	// Keyboard
	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	// CRDFView
	virtual void OnModelChanged();
	virtual void OnWorldDimensionsChanged();
	virtual void OnInstancePropertyEdited(CRDFInstance * pInstance, CRDFProperty * pProperty);
	virtual void OnNewInstanceCreated(CRDFView * pSender, CRDFInstance * pInstance);
	virtual void OnInstanceDeleted(CRDFView * pSender, int64_t iInstance);
	virtual void OnInstancesDeleted(CRDFView * pSender);
	virtual void OnMeasurementsAdded(CRDFView * pSender, CRDFInstance * pInstance);
	virtual void Reset();
	virtual void OnInstanceSelected(CRDFView * pSender);
	virtual void OnInstancePropertySelected();
	virtual void OnInstancesEnabledStateChanged();

protected: // Methods

	// Overridden
	virtual void OnControllerChanged();

private: // Methods

	// --------------------------------------------------------------------------------------------
	// Faces
	void DrawFaces(bool bTransparent);

	// --------------------------------------------------------------------------------------------
	// Faces polygons
	void DrawFacesPolygons();

	// --------------------------------------------------------------------------------------------
	// Conceptual faces polygons
	void DrawConceptualFacesPolygons();

	// --------------------------------------------------------------------------------------------
	// Lines
	void DrawLines();

	// --------------------------------------------------------------------------------------------
	// Points
	void DrawPoints();

	// --------------------------------------------------------------------------------------------
	// Bounding box for each 3D object
	void DrawBoundingBoxes();

	// --------------------------------------------------------------------------------------------
	// Normal vectors
	void DrawNormalVectors();

	// --------------------------------------------------------------------------------------------
	// Tangent vectors
	void DrawTangentVectors();

	// --------------------------------------------------------------------------------------------
	// Bi-normal vectors
	void DrawBiNormalVectors();

	// --------------------------------------------------------------------------------------------
	// Selection support
	void DrawInstancesFrameBuffer();

	// --------------------------------------------------------------------------------------------
	// Selection support
	void DrawFacesFrameBuffer();

	// --------------------------------------------------------------------------------------------
	// Selection support
	void DrawPointedFace();

	// --------------------------------------------------------------------------------------------
	// Handler
	void OnMouseMoveEvent(UINT nFlags, CPoint point);

	// --------------------------------------------------------------------------------------------
	// Rotate
	void Rotate(float fXAngle, float fYAngle);

	// --------------------------------------------------------------------------------------------
	// Zoom
	void Zoom(float fZTranslation);

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
