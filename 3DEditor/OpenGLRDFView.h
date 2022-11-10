#ifndef _OPEN_GL_IFC_VIEW_H_
#define _OPEN_GL_IFC_VIEW_H_

#include "Generic.h"
#include "RDFView.h"
#ifdef _LINUX
#include <wx/wx.h>
#include <wx/glcanvas.h>
#else
#include "OpenGLContext.h"
#endif // _LINUX
#include "RDFInstance.h"
#include "_oglFramebuffer.h"

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _LINUX
#define MK_LBUTTON 1
#define MK_MBUTTON 2
#define MK_RBUTTON 4
#endif

#include "BinnPhongGLProgram.h"
#include "GLShader.h"

#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4x4.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "glew.h"

enum enumMouseEvent
{
	meMove = 0,
	meLBtnDown = 1,
	meLBtnUp = 2,
	meMBtnDown = 3,
	meMBtnUp = 4,
	meRBtnDown = 5,
	meRBtnUp = 6,
};

#define CULL_FACES_NONE L"<none>"
#define CULL_FACES_FRONT L"Front"
#define CULL_FACES_BACK L"Back"

class COpenGLRDFView : public CRDFView
{

#pragma region Members

private: // Members

#ifdef _LINUX
    wxGLCanvas * m_pWnd;
#else
	CWnd * m_pWnd;
#endif // _LINUX

	// OpenGL
	CBinnPhongGLProgram* m_pProgram;
	CGLShader* m_pVertexShader;
	CGLShader* m_pFragmentShader;
	glm::mat4 m_matModelView;

	// UI
	BOOL m_bShowReferencedInstances;
	BOOL m_bShowCoordinateSystem;
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
	
#ifdef _LINUX
    wxGLContext * m_pOGLContext;
#else
	COpenGLContext * m_pOGLContext;
#endif // _LINUX

	// Transformations
	float m_fXAngle;
	float m_fYAngle;
	float m_fXTranslation;
	float m_fYTranslation;
	float m_fZTranslation;

	// Draw
	_openGLBuffers<CRDFInstance> m_openGLBuffers;

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

	// Normals
	GLuint m_iNormalVectorsVAO;
	GLuint m_iNormalVectorsVBO;

	// Normals
	GLuint m_iTangentVectorsVAO;
	GLuint m_iTangentVectorsVBO;

	// BiNormals
	GLuint m_iBiNormalVectorsVAO;
	GLuint m_iBiNormalVectorsVBO;	

#pragma endregion // Members

public: // Methods
	
#ifdef _LINUX
    COpenGLRDFView(wxGLCanvas * pWnd);
#else
	COpenGLRDFView(CWnd * pWnd);
#endif //_LINUX
	
	virtual ~COpenGLRDFView();
	
	void SetRotation(float fX, float fY, BOOL bRedraw);

	// --------------------------------------------------------------------------------------------
	// Getter
	void GetRotation(float& fX, float& fY);

	// --------------------------------------------------------------------------------------------
	// Setter
	void SetTranslation(float fX, float fY, float fZ, BOOL bRedraw);

	// --------------------------------------------------------------------------------------------
	// Getter
	void GetTranslation(float& fX, float& fY, float& fZ);

	// --------------------------------------------------------------------------------------------
	// Setter
	void ShowReferencedInstances(BOOL bShow);

	// --------------------------------------------------------------------------------------------
	// Getter
	BOOL AreReferencedInstancesShown() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void ShowCoordinateSystem(BOOL bShow);

	// --------------------------------------------------------------------------------------------
	// Getter
	BOOL IsCoordinateSystemShown() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void ShowFaces(BOOL bShow);

	// --------------------------------------------------------------------------------------------
	// Getter
	BOOL AreFacesShown() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void SetCullFacesMode(LPCTSTR szMode);

	// --------------------------------------------------------------------------------------------
	// Getter
	LPCTSTR GetCullFacesMode() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void ShowFacesPolygons(BOOL bShow);

	// --------------------------------------------------------------------------------------------
	// Getter
	BOOL AreFacesPolygonsShown() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void ShowConceptualFacesPolygons(BOOL bShow);

	// --------------------------------------------------------------------------------------------
	// Getter
	BOOL AreConceptualFacesPolygonsShown() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void ShowLines(BOOL bShow);

	// --------------------------------------------------------------------------------------------
	// Getter
	BOOL AreLinesShown() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void SetLineWidth(GLfloat fWidth);

	// --------------------------------------------------------------------------------------------
	// Getter
	GLfloat GetLineWidth() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void ShowPoints(BOOL bShow);

	// --------------------------------------------------------------------------------------------
	// Getter
	BOOL ArePointsShown() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void SetPointSize(GLfloat fSize);

	// --------------------------------------------------------------------------------------------
	// Getter
	GLfloat GetPointSize() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void ShowBoundingBoxes(BOOL bShow);

	// --------------------------------------------------------------------------------------------
	// Getter
	BOOL AreBoundingBoxesShown() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void ShowNormalVectors(BOOL bShow);

	// --------------------------------------------------------------------------------------------
	// Getter
	BOOL AreNormalVectorsShown() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void ShowTangentVectors(BOOL bShow);

	// --------------------------------------------------------------------------------------------
	// Getter
	BOOL AreTangentVectorsShown() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void ShowBiNormalVectors(BOOL bShow);

	// --------------------------------------------------------------------------------------------
	// Getter
	BOOL AreBiNormalVectorsShown() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void ScaleVectors(BOOL bShow);

	// --------------------------------------------------------------------------------------------
	// Getter
	BOOL AreVectorsScaled() const;
	
	// --------------------------------------------------------------------------------------------
	// Draws the scene
#ifdef _LINUX
    void Draw(wxPaintDC * pDC);
#else
	void Draw(CDC * pDC);
#endif // _LINUX

	// --------------------------------------------------------------------------------------------
	// Handles the events
	void OnMouseEvent(enumMouseEvent enEvent, UINT nFlags, CPoint point);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnModelChanged();

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnWorldDimensionsChanged();

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnInstancePropertyEdited(CRDFInstance * pInstance, CRDFProperty * pProperty);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnNewInstanceCreated(CRDFView * pSender, CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnInstanceDeleted(CRDFView * pSender, int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnInstancesDeleted(CRDFView * pSender);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnMeasurementsAdded(CRDFView * pSender, CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void Reset();

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnInstanceSelected(CRDFView * pSender);

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnInstancePropertySelected();

	// --------------------------------------------------------------------------------------------
	// CRDFView
	virtual void OnInstancesEnabledStateChanged();

protected: // Methods

	// --------------------------------------------------------------------------------------------
	// Overridden
	virtual void OnControllerChanged();

private: // Methods

	// --------------------------------------------------------------------------------------------
	// Clip space
	void DrawClipSpace();

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
	void Rotate(float fXSpin, float fYSpin);

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
