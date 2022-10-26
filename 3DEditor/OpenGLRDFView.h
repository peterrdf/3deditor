#ifndef _OPEN_GL_IFC_VIEW_H_
#define _OPEN_GL_IFC_VIEW_H_

// ------------------------------------------------------------------------------------------------
#include "Generic.h"
#include "RDFView.h"
#ifdef _LINUX
#include <wx/wx.h>
#include <wx/glcanvas.h>
#else
#include "OpenGLContext.h"
#endif // _LINUX
#include "RDFInstance.h"

// ------------------------------------------------------------------------------------------------
#define _USE_MATH_DEFINES
#include <math.h>

// ------------------------------------------------------------------------------------------------
#ifdef _LINUX
#define MK_LBUTTON 1
#define MK_MBUTTON 2
#define MK_RBUTTON 4
#endif

// ------------------------------------------------------------------------------------------------
#include "BinnPhongGLProgram.h"
#include "GLShader.h"

#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4x4.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "glew.h"

// ------------------------------------------------------------------------------------------------
// Mouse support
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

// ------------------------------------------------------------------------------------------------
enum enumDrawMetaDataType
{
	mdtGeometry = 0,
	mdtVectors = 1,
};

// ------------------------------------------------------------------------------------------------
typedef map<GLuint, vector<CRDFInstance*>> VBOGROUPS;

// ------------------------------------------------------------------------------------------------
// VBOs
class CDrawMetaData
{

private: // Members	

	// --------------------------------------------------------------------------------------------
	// Type
	enumDrawMetaDataType m_enDrawMetaDataType;

	// --------------------------------------------------------------------------------------------
	// VBO : CRDFInstance-s
	VBOGROUPS m_mapVBO2RDFInstances;

	// --------------------------------------------------------------------------------------------
	// VBO-s
	vector<GLuint> m_vecVBOs;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CDrawMetaData(enumDrawMetaDataType enDrawMetaDataType)
		: m_enDrawMetaDataType(enDrawMetaDataType)
		, m_mapVBO2RDFInstances()
		, m_vecVBOs()
	{
	}

	// --------------------------------------------------------------------------------------------
	// dtor
	~CDrawMetaData()
	{
		VBOGROUPS::iterator itVBO2RDFInstances = m_mapVBO2RDFInstances.begin();
		for (; itVBO2RDFInstances != m_mapVBO2RDFInstances.end(); itVBO2RDFInstances++)
		{
			glDeleteVertexArrays(1, &(itVBO2RDFInstances->first));
		}

		for (auto itVBO : m_vecVBOs)
		{
			glDeleteBuffers(1, &itVBO);
		}
	}

	// --------------------------------------------------------------------------------------------
	// Getter
	enumDrawMetaDataType GetType() const
	{
		return m_enDrawMetaDataType;
	}

	// --------------------------------------------------------------------------------------------
	// Adds a group of RDFInstance-s that share VBO
	void AddGroup(GLuint iVAO, GLuint iVBO, const vector<CRDFInstance*>& vecRDFInstances)
	{
		VBOGROUPS::iterator itVBO2RDFInstances = m_mapVBO2RDFInstances.find(iVAO);
		ASSERT(itVBO2RDFInstances == m_mapVBO2RDFInstances.end());

		m_mapVBO2RDFInstances[iVAO] = vecRDFInstances;

		m_vecVBOs.push_back(iVBO);
	}

	// --------------------------------------------------------------------------------------------
	// Getter
	const VBOGROUPS& getVBOGroups() const
	{
		return m_mapVBO2RDFInstances;
	}
};

// ------------------------------------------------------------------------------------------------
#define CULL_FACES_NONE L"<none>"
#define CULL_FACES_FRONT L"Front"
#define CULL_FACES_BACK L"Back"

// ------------------------------------------------------------------------------------------------
// Open GL View
class COpenGLRDFView : public CRDFView
{

#pragma region Members

private: // Members

	// --------------------------------------------------------------------------------------------
	// Window
#ifdef _LINUX
    wxGLCanvas * m_pWnd;
#else
	CWnd * m_pWnd;
#endif // _LINUX

	// --------------------------------------------------------------------------------------------
	CBinnPhongGLProgram* m_pProgram;

	// --------------------------------------------------------------------------------------------
	CGLShader* m_pVertexShader;

	// --------------------------------------------------------------------------------------------
	CGLShader* m_pFragmentShader;

	// --------------------------------------------------------------------------------------------
	glm::mat4 m_modelViewMatrix;

	// --------------------------------------------------------------------------------------------
	// Instances
	BOOL m_bShowReferencedInstances;

	// --------------------------------------------------------------------------------------------
	// Coordinate System
	BOOL m_bShowCoordinateSystem;

	// --------------------------------------------------------------------------------------------
	// Faces
	BOOL m_bShowFaces;

	// --------------------------------------------------------------------------------------------
	// Cull Faces
	CString m_strCullFaces;

	// --------------------------------------------------------------------------------------------
	// Faces polygons
	BOOL m_bShowFacesPolygons;

	// --------------------------------------------------------------------------------------------
	// Conceptual faces polygons
	BOOL m_bShowConceptualFacesPolygons;

	// --------------------------------------------------------------------------------------------
	// Lines
	BOOL m_bShowLines;

	// --------------------------------------------------------------------------------------------
	// Line width
	GLfloat m_fLineWidth;

	// --------------------------------------------------------------------------------------------
	// Points
	BOOL m_bShowPoints;

	// --------------------------------------------------------------------------------------------
	// Point size
	GLfloat m_fPointSize;

	// --------------------------------------------------------------------------------------------
	// Bounding boxes
	BOOL m_bShowBoundingBoxes;

	// --------------------------------------------------------------------------------------------
	// Normal vectors
	BOOL m_bShowNormalVectors;

	// --------------------------------------------------------------------------------------------
	// Tangent vectors
	BOOL m_bShowTangenVectors;

	// --------------------------------------------------------------------------------------------
	// Bi-normal vectors
	BOOL m_bShowBiNormalVectors;

	// --------------------------------------------------------------------------------------------
	// Scale all vectors
	BOOL m_bScaleVectors;

	// --------------------------------------------------------------------------------------------
	// OpenGL context
#ifdef _LINUX
    wxGLContext * m_pOGLContext;
#else
	COpenGLContext * m_pOGLContext;
#endif // _LINUX

	// --------------------------------------------------------------------------------------------
	// Rotation - X
	float m_fXAngle;

	// --------------------------------------------------------------------------------------------
	// Rotation - Y
	float m_fYAngle;

	// --------------------------------------------------------------------------------------------
	// Translation - X
	float m_fXTranslation;

	// --------------------------------------------------------------------------------------------
	// Translation - Y
	float m_fYTranslation;

	// --------------------------------------------------------------------------------------------
	// Translation - Z
	float m_fZTranslation;

	// --------------------------------------------------------------------------------------------
	// Mouse position
	CPoint m_ptStartMousePosition;

	// --------------------------------------------------------------------------------------------
	// Mouse position
	CPoint m_ptPrevMousePosition;

	// --------------------------------------------------------------------------------------------
	// Selection support
	GLuint m_iInstanceSelectionFrameBuffer;

	// --------------------------------------------------------------------------------------------
	// Selection support
	GLuint m_iInstanceSelectionTextureBuffer;

	// --------------------------------------------------------------------------------------------
	// Selection support
	GLuint m_iInstanceSelectionDepthRenderBuffer;

	// --------------------------------------------------------------------------------------------
	// Selection support - (Instance : Color)
	map<int64_t, CRDFColor> m_mapInstancesSelectionColors;

	// --------------------------------------------------------------------------------------------
	// Pointed instance (mouse move)
	CRDFInstance * m_pPointedInstance;

	// --------------------------------------------------------------------------------------------
	// Selected instance (mouse click)
	CRDFInstance * m_pSelectedInstance;

	// --------------------------------------------------------------------------------------------
	// Selected point (mouse click) in window coordinates
	CPoint m_ptSelectedPoint;

	// --------------------------------------------------------------------------------------------
	// Selected point (mouse click) in OpenGL coordinates
	GLfloat m_arSelectedPoint[3];

	// --------------------------------------------------------------------------------------------
	// Camera/eye
	GLfloat m_arCamera[3];

	// --------------------------------------------------------------------------------------------
	// Selection support
	GLuint m_iFaceSelectionFrameBuffer;

	// --------------------------------------------------------------------------------------------
	// Selection support
	GLuint m_iFaceSelectionTextureBuffer;

	// --------------------------------------------------------------------------------------------
	// Selection support
	GLuint m_iFaceSelectionDepthRenderBuffer;

	// --------------------------------------------------------------------------------------------
	// Selection support
	GLuint m_iFaceSelectionIBO;

	// --------------------------------------------------------------------------------------------
	// Selection support - (Face index : Color) for m_pSelectedInstance
	map<int64_t, CRDFColor> m_mapFacesSelectionColors;

	// --------------------------------------------------------------------------------------------
	// Pointed face (mouse move) for m_pSelectedInstance
	int64_t m_iPointedFace;

	// --------------------------------------------------------------------------------------------
	// VBOs
	vector<CDrawMetaData*> m_vecDrawMetaData;

	// --------------------------------------------------------------------------------------------
	// Shared IBOs
	vector<GLuint> m_vecIBOs;

	// --------------------------------------------------------------------------------------------
	// Selected instances
	CRDFMaterial* m_pSelectedInstanceMaterial;

	// --------------------------------------------------------------------------------------------
	// Pointed instances
	CRDFMaterial* m_pPointedInstanceMaterial;

	// --------------------------------------------------------------------------------------------
	// Bounding boxes
	GLuint m_iBoundingBoxesVAO;
	GLuint m_iBoundingBoxesVBO;	
	GLuint m_iBoundingBoxesIBO;

	// --------------------------------------------------------------------------------------------
	// Normals
	GLuint m_iNormalVectorsVAO;
	GLuint m_iNormalVectorsVBO;

	// --------------------------------------------------------------------------------------------
	// Normals
	GLuint m_iTangentVectorsVAO;
	GLuint m_iTangentVectorsVBO;

	// --------------------------------------------------------------------------------------------
	// BiNormals
	GLuint m_iBiNormalVectorsVAO;
	GLuint m_iBiNormalVectorsVBO;	

#pragma endregion // Members

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
#ifdef _LINUX
    COpenGLRDFView(wxGLCanvas * pWnd);
#else
	COpenGLRDFView(CWnd * pWnd);
#endif //_LINUX

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~COpenGLRDFView();

	// --------------------------------------------------------------------------------------------
	// Setter
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
	// Helper
	GLuint FindVAO(CRDFInstance* pRDFInstance);

	// --------------------------------------------------------------------------------------------
	// Helper
	float* GetVertices(const vector<CRDFInstance*>& vecRDFInstances, int_t& iVerticesCount);

	// --------------------------------------------------------------------------------------------
	// Helper
	unsigned int* GetMaterialsIndices(const vector<CRDFGeometryWithMaterial*>& vecIFCMaterials, int_t& iIndicesCount);

	// --------------------------------------------------------------------------------------------
	// Helper
	unsigned int* GetLinesCohortsIndices(const vector<CLinesCohort*>& vecLinesCohorts, int_t& iIndicesCount);

	// --------------------------------------------------------------------------------------------
	// Helper
	unsigned int* GetPointsCohortsIndices(const vector<CPointsCohort*>& vecPointsCohorts, int_t& iIndicesCount);

	// --------------------------------------------------------------------------------------------
	// Helper
	unsigned int* GetWireframesCohortsIndices(const vector<CWireframesCohort*>& vecWireframesCohorts, int_t& iIndicesCount);

	// --------------------------------------------------------------------------------------------
	// Helper
	void ReleaseBuffers();

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
