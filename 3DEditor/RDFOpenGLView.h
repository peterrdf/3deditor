#ifndef _OPEN_GL_IFC_VIEW_H_
#define _OPEN_GL_IFC_VIEW_H_

#include "Generic.h"
#include "RDFModel.h"
#include <ctime>

#include "_rdf_instance.h"
#include "_rdf_mvc.h"
#include "_oglUtils.h"

class CRDFOpenGLView : public _oglView
{

#pragma region Members

private: // Fields

	// Point Face	
	_oglSelectionFramebuffer* m_pPointFaceFrameBuffer;
	int64_t m_iPointedFace;
	int64_t m_iNearestVertex;

	// Navigator
	_oglSelectionFramebuffer* m_pNavigatorSelectionFrameBuffer;
	_rdf_instance* m_pNavigatorPointedInstance;
	_material* m_pNavigatorPointedInstanceMaterial;

#pragma endregion // Fields

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
	virtual void _preDraw() override;
	virtual void _postDraw() override;
	virtual void _drawBuffers() override;
	virtual void _onMouseMove(const CPoint& point) override;

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

private: // Methods

	void TransformBBVertex(_vector3d& vecBBVertex, const _matrix* pBBTransformation, const _vector3d& vecVertexBufferOffset, double dScaleFactor);
	void DrawBoundingBoxes(_model* pModel);
	void DrawNormalVectors(_model* pModel);
	void DrawTangentVectors(_model* pModel);
	void DrawBiNormalVectors(_model* pModel);
	
	void DrawFacesFrameBuffer();
	void DrawPointedFace();
	pair<int64_t, int64_t> GetNearestVertex(_model* pM, float fX, float fY, float fZ, float& fVertexX, float& fVertexY, float& fVertexZ);

public:

	// https://community.khronos.org/t/taking-screenshots-how-to/19154/3
	void TakeScreenshot(unsigned char*& arPixels, unsigned int& iWidth, unsigned int& iHeight);
	bool SaveScreenshot(const wchar_t* szFilePath);
};

#endif // _OPEN_GL_IFC_VIEW_H_
