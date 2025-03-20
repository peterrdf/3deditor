#include "stdafx.h"

#include "RDFModel.h"
#include "Generic.h"
#include "ProgressIndicator.h"
#include "ProgressDialog.h"

#ifdef _GIS_SUPPORT
#include "gisengine.h"
#endif
#ifdef _DXF_SUPPORT
#include "_dxf_parser.h"
#endif

#include <bitset>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

// ************************************************************************************************
extern BOOL TEST_MODE;

// ************************************************************************************************
CProgress* m_pProgress = nullptr;

// ************************************************************************************************
#ifdef _GIS_SUPPORT
void STDCALL LogCallbackImpl(int iEvent, const char* szEvent)
{
	assert(m_pProgress != nullptr);

	m_pProgress->Log((int)iEvent, szEvent);
}
#endif

// ************************************************************************************************
class CLoadTask : public CTask
{

private: // Fields

	CRDFModel* m_pModel;
	const wchar_t* m_szPath;
	bool m_bAdd;

public: // Methods

	CLoadTask(CRDFModel* pModel, const wchar_t* szPath, bool bAdd)
		: CTask()
		, m_pModel(pModel)
		, m_szPath(szPath)
		, m_bAdd(bAdd)
	{
		assert(m_pModel != nullptr);
		assert(szPath != nullptr);
	}

	virtual ~CLoadTask()
	{
	}

	virtual void Run() override
	{
		if (m_pProgress != nullptr) {
			CString strLog;
			if (m_bAdd) {
				strLog.Format(_T("*** Importing '%s' ***"), m_szPath);
			} else {
				strLog.Format(_T("*** Loading '%s' ***"), m_szPath);
			}

			if (!TEST_MODE) {
				if (m_pProgress != nullptr) {
					m_pProgress->Log(0/*info*/, CW2A(strLog));
				}
			}
		}

		CString strExtension = PathFindExtension(m_szPath);
		strExtension.MakeUpper();

#ifdef _DXF_SUPPORT
		if (strExtension == L".DXF") {
			m_pModel->LoadDXF(m_szPath);
		}
#endif
#ifdef _GIS_SUPPORT
		else if ((strExtension == L".GML") ||
			(strExtension == L".CITYGML") ||
			(strExtension == L".GMLZIP") ||
			(strExtension == L".GMLZ") ||
			(strExtension == L".XML") ||
			(strExtension == L".JSON")) {
			m_pModel->LoadGISModel(m_szPath);
		} else
#endif		
		{
			if (m_bAdd) {
				m_pModel->importModel(m_szPath);
			} else {
				OwlModel owlModel = OpenModelW(m_szPath);
				if (owlModel) {
					m_pModel->attachModel(m_szPath, owlModel);
				}
			}
		}

		if (m_pModel->getOwlModel() == 0) {
			CString strError;
			strError.Format(L"Failed to open '%s'.", m_szPath);

			if (!TEST_MODE) {
				if (m_pProgress != nullptr) {
					m_pProgress->Log(2/*error*/, CW2A(strError));
				}
				::MessageBox(
					::AfxGetMainWnd()->GetSafeHwnd(),
					strError, L"Error", MB_SYSTEMMODAL | MB_ICONERROR | MB_OK);
			} else {
				TRACE(L"\nError: %s", (LPCTSTR)strError);
			}
		} else {
			if (!TEST_MODE) {
				if (m_pProgress != nullptr) {
					m_pProgress->Log(0/*info*/, "*** Done. ***");
				}
			}
		}
	}
};

// ************************************************************************************************
CRDFModel::CRDFModel()
	: _rdf_model()
	, m_pDefaultTexture(nullptr)
{
}

CRDFModel::~CRDFModel()
{
	clean();
}

/*virtual*/ _texture* CRDFModel::getDefaultTexture() /*override*/
{
	if (m_pDefaultTexture == nullptr) {
		wchar_t szAppPath[_MAX_PATH];
		::GetModuleFileName(::GetModuleHandle(nullptr), szAppPath, sizeof(szAppPath));

		CString strDefaultTexture = szAppPath;
		strDefaultTexture.MakeLower();

		int iLastSlash = strDefaultTexture.ReverseFind(L'\\');
		assert(iLastSlash != -1);

		strDefaultTexture = strDefaultTexture.Left(iLastSlash + 1);
		strDefaultTexture += L"data\\texture.jpg";

		LPCTSTR szDefaultTexture = (LPCTSTR)strDefaultTexture;

		m_pDefaultTexture = new _texture();
		if (!m_pDefaultTexture->load(szDefaultTexture)) {
			MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"The default texture is not found.", L"Error", MB_ICONERROR | MB_OK);
		}
	}

	return m_pDefaultTexture;
}

/*virtual*/ void CRDFModel::clean(bool bCloseModel/* = true*/) /*override*/
{
	_rdf_model::clean(bCloseModel);

	delete m_pDefaultTexture;
	m_pDefaultTexture = nullptr;
}

void CRDFModel::Load(const wchar_t* szPath, bool bAdd)
{
	CLoadTask loadTask(this, szPath, bAdd);
#ifdef _PROGRESS_UI_SUPPORT
	if (!TEST_MODE) {
		CProgressDialog dlgProgress(::AfxGetMainWnd(), &loadTask);
		m_pProgress = &dlgProgress;
		dlgProgress.DoModal();
		m_pProgress = nullptr;
	} else
#endif
	{
		loadTask.Run();
	}
}

#ifdef _DXF_SUPPORT
void CRDFModel::LoadDXF(const wchar_t* szPath)
{
	try {
		OwlModel owlModel = CreateModel();
		ASSERT(owlModel != 0);

		_dxf::_parser parser(owlModel);
		parser.load(CW2A(szPath));

		attachModel(szPath, owlModel);
	} catch (const std::runtime_error& ex) {
		::MessageBox(
			::AfxGetMainWnd()->GetSafeHwnd(),
			CA2W(ex.what()), L"Error", MB_SYSTEMMODAL | MB_ICONERROR | MB_OK);
	}
}
#endif

#ifdef _GIS_SUPPORT
void CRDFModel::LoadGISModel(const wchar_t* szPath)
{
	try {
		wchar_t szAppPath[_MAX_PATH];
		::GetModuleFileName(::GetModuleHandle(nullptr), szAppPath, sizeof(szAppPath));

		fs::path pthExe = szAppPath;
		auto pthRootFolder = pthExe.parent_path();
		wstring strRootFolder = pthRootFolder.wstring();
		strRootFolder += L"\\";

		SetGISOptionsW(strRootFolder.c_str(), true, LogCallbackImpl);

		OwlModel owlModel = CreateModel();
		ASSERT(owlModel != 0);

		ImportGISModel(owlModel, CW2A(szPath));

		attachModel(szPath, owlModel);
	} catch (const std::runtime_error& err) {
		::MessageBox(
			::AfxGetMainWnd()->GetSafeHwnd(),
			CA2W(err.what()), L"Error", MB_SYSTEMMODAL | MB_ICONERROR | MB_OK);
	} catch (...) {
		::MessageBox(
			::AfxGetMainWnd()->GetSafeHwnd(),
			L"Unknown error.", L"Error", MB_SYSTEMMODAL | MB_ICONERROR | MB_OK);
	}
}
#endif // _GIS_SUPPORT

// ************************************************************************************************
CDefaultModel::CDefaultModel()
	: CRDFModel()
{
	Create();
}

/*virtual*/ CDefaultModel::~CDefaultModel()
{
}

void CDefaultModel::Create()
{
	OwlModel owlModel = CreateModel();
	assert(owlModel != 0);

	// Cube 1
	{
		auto pAmbient = GEOM::ColorComponent::Create(owlModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(1.);
		pAmbient.set_B(0.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(owlModel);
		pColor.set_ambient(pAmbient);

		auto pTexture = GEOM::Texture::Create(owlModel);
		pTexture.set_scalingX(1.);
		pTexture.set_scalingY(1.);
		pTexture.set_name("data\\texture.jpg");
		vector<GEOM::Texture> vecTexures = { pTexture };

		auto pMaterial = GEOM::Material::Create(owlModel);
		pMaterial.set_color(pColor);
		pMaterial.set_textures(&vecTexures[0], 1);

		auto pCube = GEOM::Cube::Create(owlModel, "Cube 1");
		pCube.set_material(pMaterial);
		pCube.set_length(7.);
	}

	// Cone 1
	{
		auto pAmbient = GEOM::ColorComponent::Create(owlModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(0.);
		pAmbient.set_B(1.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(owlModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(owlModel);
		pMaterial.set_color(pColor);

		auto pCone = GEOM::Cone::Create(owlModel, "Cone 1");
		pCone.set_material(pMaterial);
		pCone.set_radius(4.);
		pCone.set_height(12.);
		pCone.set_segmentationParts(36);
	}

	// Cylinder 1
	{
		auto pAmbient = GEOM::ColorComponent::Create(owlModel);
		pAmbient.set_R(1.);
		pAmbient.set_G(0.);
		pAmbient.set_B(0.);
		pAmbient.set_W(.5);

		auto pColor = GEOM::Color::Create(owlModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(owlModel);
		pMaterial.set_color(pColor);

		auto pCylinder = GEOM::Cylinder::Create(owlModel, "Cylinder 1");
		pCylinder.set_material(pMaterial);
		pCylinder.set_radius(6.);
		pCylinder.set_length(6.);
		pCylinder.set_segmentationParts(36);
	}

	attachModel(L"_DEFAULT_", owlModel);
}

// ************************************************************************************************
CCoordinateSystemModel::CCoordinateSystemModel(_model* pWorld)
	: CRDFModel()
	, m_pWorld(pWorld)
	, m_pTextBuilder(new CTextBuilder())
{
	Create();
}

/*virtual*/ CCoordinateSystemModel::~CCoordinateSystemModel()
{
	delete m_pTextBuilder;
}

/*virtual*/ void CCoordinateSystemModel::preLoad() /*override*/
{
	getInstancesDefaultEnableState();

	if (m_pWorld != nullptr) {
		double arOffset[3];
		GetVertexBufferOffset(m_pWorld->getOwlModel(), arOffset);

		double dScaleFactor = m_pWorld->getOriginalBoundingSphereDiameter() / 2.;

		TRACE(L"\n*** SetVertexBufferOffset *** => x/y/z: %.16f, %.16f, %.16f",
			arOffset[0] / dScaleFactor,
			arOffset[1] / dScaleFactor,
			arOffset[2] / dScaleFactor);

		// http://rdf.bg/gkdoc/CP64/SetVertexBufferOffset.html
		SetVertexBufferOffset(
			getOwlModel(),
			arOffset[0] / dScaleFactor,
			arOffset[1] / dScaleFactor,
			arOffset[2] / dScaleFactor);

		// http://rdf.bg/gkdoc/CP64/ClearedExternalBuffers.html
		ClearedExternalBuffers(getOwlModel());
	}
}

void CCoordinateSystemModel::Create()
{
	const double AXIS_LENGTH = 3.;
	const double ARROW_OFFSET = AXIS_LENGTH;

	OwlModel owlModel = CreateModel();
	assert(owlModel != 0);

	m_pTextBuilder->Initialize(owlModel);

	// Coordinate System
	vector<OwlInstance> vecInstances;

	// Coordinate System/X (1 Line3D)
	OwlInstance owlXAxisMaterialInstance = 0;
	{
		auto pAmbient = GEOM::ColorComponent::Create(owlModel);
		pAmbient.set_R(1.);
		pAmbient.set_G(0.);
		pAmbient.set_B(0.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(owlModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(owlModel);
		pMaterial.set_color(pColor);

		owlXAxisMaterialInstance = (int64_t)pMaterial;

		vector<double> vecPoints =
		{
			0., 0., 0.,
			AXIS_LENGTH / 2., 0., 0.,
		};

		auto pXAxis = GEOM::Line3D::Create(owlModel);
		pXAxis.set_material(pMaterial);
		pXAxis.set_points(vecPoints.data(), vecPoints.size());

		vecInstances.push_back((int64_t)pXAxis);
	}

	// Coordinate System/Y (Line3D)
	OwlInstance owlYAxisMaterialInstance = 0;
	{
		auto pAmbient = GEOM::ColorComponent::Create(owlModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(1.);
		pAmbient.set_B(0.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(owlModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(owlModel);
		pMaterial.set_color(pColor);

		owlYAxisMaterialInstance = (int64_t)pMaterial;

		vector<double> vecPoints =
		{
			0., 0., 0.,
			0., AXIS_LENGTH / 2., 0.,
		};

		auto pYAxis = GEOM::Line3D::Create(owlModel);
		pYAxis.set_material(pMaterial);
		pYAxis.set_points(vecPoints.data(), vecPoints.size());

		vecInstances.push_back((int64_t)pYAxis);
	}

	// Coordinate System/Z (Line3D)
	OwlInstance owlZAxisMaterialInstance = 0;
	{
		auto pAmbient = GEOM::ColorComponent::Create(owlModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(0.);
		pAmbient.set_B(1.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(owlModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(owlModel);
		pMaterial.set_color(pColor);

		owlZAxisMaterialInstance = (int64_t)pMaterial;

		vector<double> vecPoints =
		{
			0., 0., 0.,
			0., 0., AXIS_LENGTH / 2.,
		};

		auto pZAxis = GEOM::Line3D::Create(owlModel);
		pZAxis.set_material(pMaterial);
		pZAxis.set_points(vecPoints.data(), vecPoints.size());

		vecInstances.push_back((int64_t)pZAxis);
	}

	// Arrows (1 Cone => 3 Transformations)
	{
		auto pArrow = GEOM::Cone::Create(owlModel);
		pArrow.set_height(AXIS_LENGTH / 15.);
		pArrow.set_radius(.075);

		// +X
		{
			OwlInstance owlPlusXInstance = translateTransformation(
				owlModel,
				rotateTransformation(owlModel, (int64_t)pArrow, 0., 2 * PI * 90. / 360., 0.),
				ARROW_OFFSET / 2., 0., 0.,
				1., 1., 1.);
			SetNameOfInstance(owlPlusXInstance, "#(+X)");

			SetObjectProperty(
				owlPlusXInstance,
				GetPropertyByName(owlModel, "material"),
				&owlXAxisMaterialInstance,
				1);

			vecInstances.push_back(owlPlusXInstance);
		}

		// +Y
		{
			OwlInstance owlPlusYInstance = translateTransformation(
				owlModel,
				rotateTransformation(owlModel, (int64_t)pArrow, 2 * PI * 270. / 360., 0., 0.),
				0., ARROW_OFFSET / 2., 0.,
				1., 1., 1.);
			SetNameOfInstance(owlPlusYInstance, "#(+Y)");

			SetObjectProperty(
				owlPlusYInstance,
				GetPropertyByName(owlModel, "material"),
				&owlYAxisMaterialInstance,
				1);

			vecInstances.push_back(owlPlusYInstance);
		}

		// +Z
		{
			OwlInstance owlPlusZInstance = translateTransformation(
				owlModel,
				(int64_t)pArrow,
				0., 0., ARROW_OFFSET / 2.,
				1., 1., 1.);
			SetNameOfInstance(owlPlusZInstance, "#(+Z)");

			SetObjectProperty(
				owlPlusZInstance,
				GetPropertyByName(owlModel, "material"),
				&owlZAxisMaterialInstance,
				1);

			vecInstances.push_back(owlPlusZInstance);
		}
	}

	/* Labels */
	double dXmin = DBL_MAX;
	double dXmax = -DBL_MAX;
	double dYmin = DBL_MAX;
	double dYmax = -DBL_MAX;
	double dZmin = DBL_MAX;
	double dZmax = -DBL_MAX;

	// X-axis
	OwlInstance owlPlusXLabelInstance = m_pTextBuilder->BuildText("X-axis", true);
	assert(owlPlusXLabelInstance != 0);

	_geometry::calculateBB(
		owlPlusXLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	// Y-axis
	OwlInstance owlPlusYLabelInstance = m_pTextBuilder->BuildText("Y-axis", true);
	assert(owlPlusYLabelInstance != 0);

	_geometry::calculateBB(
		owlPlusYLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	// Z-axis
	OwlInstance owlPlusZLabelInstance = m_pTextBuilder->BuildText("Z-axis", true);
	assert(owlPlusZLabelInstance != 0);

	_geometry::calculateBB(
		owlPlusZLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Scale Factor */
	double dMaxLength = dXmax - dXmin;
	dMaxLength = fmax(dMaxLength, dYmax - dYmin);
	dMaxLength = fmax(dMaxLength, dZmax - dZmin);

	double dScaleFactor = ((AXIS_LENGTH / 2.) * .75) / dMaxLength;

	/* Transform Labels */

	// X-axis
	OwlInstance owlInstance = translateTransformation(
		owlModel,
		rotateTransformation(owlModel, scaleTransformation(owlModel, owlPlusXLabelInstance, dScaleFactor / 2.), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 180.),
		AXIS_LENGTH / 1.4, 0., 0.,
		-1., 1., 1.);

	SetNameOfInstance(owlInstance, "#X-axis");
	SetObjectProperty(
		owlInstance,
		GetPropertyByName(owlModel, "material"),
		&owlXAxisMaterialInstance,
		1);

	// Y-axis
	owlInstance = translateTransformation(
		owlModel, 
		rotateTransformation(owlModel, scaleTransformation(owlModel, owlPlusYLabelInstance, dScaleFactor / 2.), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
		0., AXIS_LENGTH / 1.4, 0.,
		-1., 1., 1.);

	SetNameOfInstance(owlInstance, "#Y-axis");
	SetObjectProperty(
		owlInstance,
		GetPropertyByName(owlModel, "material"),
		&owlYAxisMaterialInstance,
		1);

	// Z-axis
	owlInstance = translateTransformation(
		owlModel, 
		rotateTransformation(owlModel, scaleTransformation(owlModel, owlPlusZLabelInstance, dScaleFactor / 2.), 2 * PI * 270. / 360., 2 * PI * 90. / 360., 0.),
		0., 0., AXIS_LENGTH / 1.4,
		1., 1., -1.);

	SetNameOfInstance(owlInstance, "#Z-axis");
	SetObjectProperty(
		owlInstance,
		GetPropertyByName(owlModel, "material"),
		&owlZAxisMaterialInstance,
		1);

	/* Collection */
	OwlInstance owlCollectionInstance = CreateInstance(GetClassByName(owlModel, "Collection"), "#Coordinate System#");
	assert(owlCollectionInstance != 0);

	SetObjectProperty(
		owlCollectionInstance,
		GetPropertyByName(owlModel, "objects"),
		vecInstances.data(),
		vecInstances.size());

	attachModel(L"_COORDINATE_SYSTEM_", owlModel);
}

// ************************************************************************************************
CNavigatorModel::CNavigatorModel()
	: CRDFModel()
	, m_pTextBuilder(new CTextBuilder())
{
	Create();
}

/*virtual*/ CNavigatorModel::~CNavigatorModel()
{
	delete m_pTextBuilder;
}

/*virtual*/ bool CNavigatorModel::prepareScene(_oglScene* pScene) /*override*/
{
	assert(pScene != nullptr);

	const int NAVIGATION_VIEW_LENGTH = 200;

	float fXmin = FLT_MAX;
	float fXmax = -FLT_MAX;
	float fYmin = FLT_MAX;
	float fYmax = -FLT_MAX;
	float fZmin = FLT_MAX;
	float fZmax = -FLT_MAX;
	getDimensions(fXmin, fXmax, fYmin, fYmax, fZmin, fZmax);

	pScene->_prepare(
		true,
		0, NAVIGATION_VIEW_LENGTH,
		NAVIGATION_VIEW_LENGTH, NAVIGATION_VIEW_LENGTH,
		fXmin, fXmax,
		fYmin, fYmax,
		fZmin, fZmax,
		false,
		false);

	return true;
}

/*virtual*/ void CNavigatorModel::preLoad() /*override*/
{
	getInstancesDefaultEnableState();
}

void CNavigatorModel::Create()
{
	OwlModel owlModel = CreateModel();
	assert(owlModel != 0);

	m_pTextBuilder->Initialize(owlModel);

	// Cube (BoundaryRepresentations)
	{
		auto pAmbient = GEOM::ColorComponent::Create(owlModel);
		pAmbient.set_R(.1);
		pAmbient.set_G(.1);
		pAmbient.set_B(.1);
		pAmbient.set_W(.05);

		auto pColor = GEOM::Color::Create(owlModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(owlModel);
		pMaterial.set_color(pColor);

		vector<double> vecVertices =
		{
			-.75, -.75, 0, // 1 (Bottom/Left)
			.75, -.75, 0,  // 2 (Bottom/Right)
			.75, .75, 0,   // 3 (Top/Right)
			-.75, .75, 0,  // 4 (Top/Left)
		};
		vector<int64_t> vecIndices =
		{
			0, 1, 2, 3, -1,
		};

		auto pBoundaryRepresentation = GEOM::BoundaryRepresentation::Create(owlModel);
		pBoundaryRepresentation.set_material(pMaterial);
		pBoundaryRepresentation.set_vertices(vecVertices.data(), vecVertices.size());
		pBoundaryRepresentation.set_indices(vecIndices.data(), vecIndices.size());

		// Front
		OwlInstance iInstance = translateTransformation(
			owlModel,
			rotateTransformation(owlModel, (int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 0.),
			0., -.75, 0.,
			1., -1., 1.);
		SetNameOfInstance(iInstance, "#front");

		// Back
		iInstance = translateTransformation(
			owlModel,
			rotateTransformation(owlModel, (int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 0.),
			0., .75, 0.,
			-1., 1., 1.);
		SetNameOfInstance(iInstance, "#back");

		// Top
		iInstance = translateTransformation(
			owlModel,
			(int64_t)pBoundaryRepresentation,
			0., 0., .75,
			1., 1., -1.);
		SetNameOfInstance(iInstance, "#top");

		// Bottom
		iInstance = translateTransformation(
			owlModel,
			(int64_t)pBoundaryRepresentation,
			0., 0., -.75,
			1, 1., 1.);
		SetNameOfInstance(iInstance, "#bottom");

		// Left
		iInstance = translateTransformation(
			owlModel,
			rotateTransformation(owlModel, (int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
			-.75, 0., 0.,
			1., -1., 1.);
		SetNameOfInstance(iInstance, "#left");

		// Right
		iInstance = translateTransformation(
			owlModel,
			rotateTransformation(owlModel, (int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
			.75, 0., 0.,
			-1., 1., 1.);
		SetNameOfInstance(iInstance, "#right");
	}

	// Sphere (Sphere)
	{
		auto pAmbient = GEOM::ColorComponent::Create(owlModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(0.);
		pAmbient.set_B(1.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(owlModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(owlModel);
		pMaterial.set_color(pColor);

		auto pSphere = GEOM::Sphere::Create(owlModel);
		pSphere.set_material(pMaterial);
		pSphere.set_radius(.1);
		pSphere.set_segmentationParts(36);

		// Front/Top/Left
		OwlInstance iInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			-.75, -.75, .75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#front-top-left");

		// Front/Top/Right
		iInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			.75, -.75, .75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#front-top-right");

		// Front/Bottom/Left
		iInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			-.75, -.75, -.75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#front-bottom-left");

		// Front/Bottom/Right
		iInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			.75, -.75, -.75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#front-bottom-right");

		// Back/Top/Left
		iInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			.75, .75, .75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#back-top-left");

		// Back/Top/Right
		iInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			-.75, .75, .75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#back-top-right");

		// Back/Bottom/Left
		iInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			.75, .75, -.75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#back-bottom-left");

		// Back/Bottom/Right
		iInstance = translateTransformation(
			owlModel,
			(int64_t)pSphere,
			-.75, .75, -.75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#back-bottom-right");
	}

	CreateLabels(owlModel);

	attachModel(L"_NAVIGATOR_", owlModel);
}

void CNavigatorModel::CreateLabels(OwlModel owlModel)
{
	double dXmin = DBL_MAX;
	double dXmax = -DBL_MAX;
	double dYmin = DBL_MAX;
	double dYmax = -DBL_MAX;
	double dZmin = DBL_MAX;
	double dZmax = -DBL_MAX;

	/* Top */
	OwlInstance iTopLabelInstance = m_pTextBuilder->BuildText("top", true);
	assert(iTopLabelInstance != 0);

	_geometry::calculateBB(
		iTopLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Bottom */
	OwlInstance iBottomLabelInstance = m_pTextBuilder->BuildText("bottom", true);
	assert(iBottomLabelInstance != 0);

	_geometry::calculateBB(
		iBottomLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Front */
	OwlInstance iFrontLabelInstance = m_pTextBuilder->BuildText("front", true);
	assert(iFrontLabelInstance != 0);

	_geometry::calculateBB(
		iFrontLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Back */
	OwlInstance iBackLabelInstance = m_pTextBuilder->BuildText("back", true);
	assert(iBackLabelInstance != 0);

	_geometry::calculateBB(
		iBackLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Left */
	OwlInstance iLeftLabelInstance = m_pTextBuilder->BuildText("left", true);
	assert(iLeftLabelInstance != 0);

	_geometry::calculateBB(
		iLeftLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Right */
	OwlInstance iRightLabelInstance = m_pTextBuilder->BuildText("right", true);
	assert(iRightLabelInstance != 0);

	_geometry::calculateBB(
		iRightLabelInstance,
		dXmin, dXmax,
		dYmin, dYmax,
		dZmin, dZmax);

	/* Scale Factor */
	double dMaxLength = dXmax - dXmin;
	dMaxLength = fmax(dMaxLength, dYmax - dYmin);
	dMaxLength = fmax(dMaxLength, dZmax - dZmin);

	double dScaleFactor = (1.5 * .9) / dMaxLength;

	// Front
	OwlInstance iInstance = translateTransformation(
		owlModel,
		rotateTransformation(owlModel, scaleTransformation(owlModel, iFrontLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 0.),
		0., -.751, 0.,
		1., 1., 1.);
	SetNameOfInstance(iInstance, "#front-label");

	// Back
	iInstance = translateTransformation(
		owlModel,
		rotateTransformation(owlModel, scaleTransformation(owlModel, iBackLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 0.),
		0., .751, 0.,
		-1., 1., 1.);
	SetNameOfInstance(iInstance, "#back-label");

	// Top
	iInstance = translateTransformation(
		owlModel,
		scaleTransformation(owlModel, iTopLabelInstance, dScaleFactor),
		0., 0., .751,
		1., 1., 1.);
	SetNameOfInstance(iInstance, "#top-label");

	// Bottom
	iInstance = translateTransformation(
		owlModel,
		scaleTransformation(owlModel, iBottomLabelInstance, dScaleFactor),
		0., 0., -.751,
		-1, 1., 1.);
	SetNameOfInstance(iInstance, "#bottom-label");

	// Left
	iInstance = translateTransformation(
		owlModel,
		rotateTransformation(owlModel, scaleTransformation(owlModel, iLeftLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
		-.751, 0., 0.,
		1., -1., 1.);
	SetNameOfInstance(iInstance, "#left-label");

	// Right
	iInstance = translateTransformation(
		owlModel,
		rotateTransformation(owlModel, scaleTransformation(owlModel, iRightLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
		.751, 0., 0.,
		1., 1., 1.);
	SetNameOfInstance(iInstance, "#right-label");
}