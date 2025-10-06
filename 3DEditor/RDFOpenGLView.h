#ifndef _OPEN_GL_IFC_VIEW_H_
#define _OPEN_GL_IFC_VIEW_H_

#include "RDFModel.h"
#include <ctime>

#include "_rdf_instance.h"
#include "_rdf_mvc.h"
#include "_oglUtils.h"

// ************************************************************************************************
class CRDFOpenGLView : public _oglView
{

private: // Fields

	// Point Face	
	_oglSelectionFramebuffer* m_pPointFaceFrameBuffer;
	int64_t m_iPointedFace;
	int64_t m_iNearestVertex;

public:
	//#dragface
	BOOL m_bDragFaceMode;
	_instance* m_pDragFaceInstance;
	int64_t m_iDragFace;

public: // Methods
	
	CRDFOpenGLView(CWnd* pWnd);	
	virtual ~CRDFOpenGLView();

	// _view
	virtual void onInstanceSelected(_view* pSender) override;
	virtual void onInstancesEnabledStateChanged(_view* pSender) override;

	// _rdf_view (owner)
	void onInstancePropertySelected(_view* pSender);
	void onInstanceCreated(_view* pSender, _rdf_instance* pInstance);
	void onInstanceDeleted(_view* pSender, _rdf_instance* pInstance);
	void onInstancesDeleted(_view* pSender);
	void onMeasurementsAdded(_view* pSender, _rdf_instance* pInstance);
	void onInstancePropertyEdited(_view* pSender, _rdf_instance* pInstance, _rdf_property* pProperty);

	// _oglView
	virtual void _postDraw() override;
	virtual void _drawBuffers() override;
	virtual void _onMouseMove(const CPoint& point) override;
	virtual void _onShowTooltip(GLdouble dX, GLdouble dY, GLdouble dZ, wstring& strInformation) override;

private: // Methods

	void TransformBBVertex(_vector3d& vecBBVertex, const _matrix* pBBTransformation, const _vector3d& vecVertexBufferOffset, double dScaleFactor);
	void DrawBoundingBoxes(_model* pModel);
	void DrawNormalVectors(_model* pModel);
	void DrawTangentVectors(_model* pModel);
	void DrawBiNormalVectors(_model* pModel);
	
	void DrawFacesFrameBuffer();
	void DrawPointedFace();
	pair<int64_t, int64_t> GetNearestVertex(float fX, float fY, float fZ, float& fVertexX, float& fVertexY, float& fVertexZ);

	void OnDragFace(
		OwlInstance		instance,
		int				iConceptualFace,
		GLdouble		startDragPoint[3],
		const CPoint&	endPoint
	);

public:

	// Test
	void _test_SetRotation(float fX, float fY, BOOL bRedraw);
	void _test_GetRotation(float& fX, float& fY);
	void _test_SetTranslation(float fX, float fY, float fZ, BOOL bRedraw);
	void _test_GetTranslation(float& fX, float& fY, float& fZ);
	// https://community.khronos.org/t/taking-screenshots-how-to/19154/3
	void _test_TakeScreenshot(unsigned char*& arPixels, unsigned int& iWidth, unsigned int& iHeight);
	bool _test_SaveScreenshot(const wchar_t* szFilePath);
};

#endif // _OPEN_GL_IFC_VIEW_H_
