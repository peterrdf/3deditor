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
	bool m_bLoading;

public: // Methods

	CLoadTask(CRDFModel* pModel, const wchar_t* szPath, bool bLoading)
		: CTask()
		, m_pModel(pModel)
		, m_szPath(szPath)
		, m_bLoading(bLoading)
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
			if (m_bLoading) {
				strLog.Format(_T("*** Loading '%s' ***"), m_szPath);
			} else {
				strLog.Format(_T("*** Importing '%s' ***"), m_szPath);
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
			if (m_bLoading) {
				m_pModel->m_iModel = OpenModelW(m_szPath);
				if (m_pModel->m_iModel != 0) {
					m_pModel->SetFormatSettings(m_pModel->m_iModel);

					m_pModel->load();
				}
			} else {
				assert(m_pModel->m_iModel != 0);

				ImportModelW(m_pModel->m_iModel, m_szPath);

				m_pModel->load();
			}
		}

		if (m_pModel->m_iModel == 0) {
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
	, m_iModel(0)
	, m_bExternalModel(false)
	, m_mapInstances()
	, m_dVertexBuffersOffsetX(0.)
	, m_dVertexBuffersOffsetY(0.)
	, m_dVertexBuffersOffsetZ(0.)
	, m_dOriginalBoundingSphereDiameter(2.)
	, m_fXmin(-1.f)
	, m_fXmax(1.f)
	, m_fYmin(-1.f)
	, m_fYmax(1.f)
	, m_fZmin(-1.f)
	, m_fZmax(1.f)
	, m_fBoundingSphereDiameter(1.f)
	, m_pDefaultTexture(nullptr)
	, m_pTextBuilder(new CTextBuilder())
{
}

CRDFModel::~CRDFModel()
{
	Clean();

	delete m_pTextBuilder;
}

/*virtual*/ void CRDFModel::CreateDefaultModel()
{
	m_strPath = L"DEFAULT";

	Clean();

	m_iModel = CreateModel();
	assert(m_iModel != 0);

	//SetFormatSettings(m_iModel);

	// Cube 1
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(1.);
		pAmbient.set_B(0.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pTexture = GEOM::Texture::Create(m_iModel);
		pTexture.set_scalingX(1.);
		pTexture.set_scalingY(1.);
		pTexture.set_name("data\\texture.jpg");
		vector<GEOM::Texture> vecTexures = { pTexture };

		auto pMaterial = GEOM::Material::Create(m_iModel);
		pMaterial.set_color(pColor);
		pMaterial.set_textures(&vecTexures[0], 1);

		auto pCube = GEOM::Cube::Create(m_iModel, "Cube 1");
		pCube.set_material(pMaterial);
		pCube.set_length(7.);
	}

	// Cone 1
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(0.);
		pAmbient.set_B(1.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(m_iModel);
		pMaterial.set_color(pColor);

		auto pCone = GEOM::Cone::Create(m_iModel, "Cone 1");
		pCone.set_material(pMaterial);
		pCone.set_radius(4.);
		pCone.set_height(12.);
		pCone.set_segmentationParts(36);
	}

	// Cylinder 1
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(1.);
		pAmbient.set_G(0.);
		pAmbient.set_B(0.);
		pAmbient.set_W(.5);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(m_iModel);
		pMaterial.set_color(pColor);

		auto pCylinder = GEOM::Cylinder::Create(m_iModel, "Cylinder 1");
		pCylinder.set_material(pMaterial);
		pCylinder.set_radius(6.);
		pCylinder.set_length(6.);
		pCylinder.set_segmentationParts(36);
	}

	load();
}

void CRDFModel::Load(const wchar_t* szPath, bool bLoading)
{
	if (bLoading) {
		Clean();

		m_strPath = szPath;
	}

	CLoadTask loadTask(this, szPath, bLoading);
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

void CRDFModel::Load(OwlInstance iInstance)
{
	ASSERT(FALSE);//#todo
	ASSERT(iInstance);

	//Clean();

	//m_strPath = L"";

	//m_iModel = GetModel(iInstance);
	//ASSERT(m_iModel != 0);

	//m_bExternalModel = true;

	//PreLoadDRFModel();

	//PostLoadDRFModel();

	//SetFormatSettings(m_iModel);

	////m_mapInstances[iInstance] = new CRDFInstance(m_iID++, iInstance);

	//ScaleAndCenter(true);

	//scale();
}

#ifdef _DXF_SUPPORT
void CRDFModel::LoadDXF(const wchar_t* szPath)
{
	if (m_iModel == 0) {
		m_iModel = CreateModel();
		assert(m_iModel != 0);

		SetFormatSettings(m_iModel);
	}

	try {
		_dxf::_parser parser(m_iModel);
		parser.load(CW2A(szPath));
	} catch (const std::runtime_error& ex) {
		::MessageBox(
			::AfxGetMainWnd()->GetSafeHwnd(),
			CA2W(ex.what()), L"Error", MB_SYSTEMMODAL | MB_ICONERROR | MB_OK);

		return;
	}

	load();
}
#endif

#ifdef _GIS_SUPPORT
void CRDFModel::LoadGISModel(const wchar_t* szPath)
{
	if (m_iModel == 0) {
		m_iModel = CreateModel();
		assert(m_iModel != 0);

		SetFormatSettings(m_iModel);
	}

	try {
		wchar_t szAppPath[_MAX_PATH];
		::GetModuleFileName(::GetModuleHandle(nullptr), szAppPath, sizeof(szAppPath));

		fs::path pthExe = szAppPath;
		auto pthRootFolder = pthExe.parent_path();
		wstring strRootFolder = pthRootFolder.wstring();
		strRootFolder += L"\\";

		SetGISOptionsW(strRootFolder.c_str(), true, LogCallbackImpl);

		ImportGISModel(m_iModel, CW2A(szPath));
	} catch (const std::runtime_error& err) {
		::MessageBox(
			::AfxGetMainWnd()->GetSafeHwnd(),
			CA2W(err.what()), L"Error", MB_SYSTEMMODAL | MB_ICONERROR | MB_OK);

		return;
	} catch (...) {
		::MessageBox(
			::AfxGetMainWnd()->GetSafeHwnd(),
			L"Unknown error.", L"Error", MB_SYSTEMMODAL | MB_ICONERROR | MB_OK);
	}

	load();
}
#endif // _GIS_SUPPORT

void CRDFModel::ImportModel(const wchar_t* szPath)
{
	Load(szPath, false);
}

void CRDFModel::Save(const wchar_t* szPath)
{
	SaveModelW(m_iModel, szPath);
}

_rdf_instance* CRDFModel::GetInstanceByID(int64_t iID)
{
	assert(iID != 0);

	auto itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++) {
		if (itInstance->second->getID() == iID) {
			return itInstance->second;
		}
	}

	assert(false);

	return nullptr;
}

_rdf_instance* CRDFModel::GetInstanceByIInstance(int64_t iInstance)
{
	assert(iInstance != 0);

	map<int64_t, _rdf_instance*>::iterator itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++) {
		if (itInstance->first == iInstance) {
			return itInstance->second;
		}
	}

	return nullptr;
}

_rdf_instance* CRDFModel::AddNewInstance(int64_t /*pThing*/)
{
	ASSERT(FALSE);//#todo
	return nullptr;
	/*auto pInstance = new CRDFInstance(m_iID++, pThing);
	pInstance->calculateMinMax(m_fXmin, m_fXmax, m_fYmin, m_fYmax, m_fZmin, m_fZmax);

	m_mapInstances[pThing] = pInstance;

	return pInstance;*/
}

//void CRDFModel::GetVertexBuffersOffset(double& dVertexBuffersOffsetX, double& dVertexBuffersOffsetY, double& dVertexBuffersOffsetZ) const
//{
//	dVertexBuffersOffsetX = m_dVertexBuffersOffsetX;
//	dVertexBuffersOffsetY = m_dVertexBuffersOffsetY;
//	dVertexBuffersOffsetZ = m_dVertexBuffersOffsetZ;
//}

//double CRDFModel::GetOriginalBoundingSphereDiameter() const
//{
//	return m_dOriginalBoundingSphereDiameter;
//}

//void CRDFModel::GetWorldDimensions(float& fXmin, float& fXmax, float& fYmin, float& fYmax, float& fZmin, float& fZmax) const
//{
//	fXmin = m_fXmin;
//	fXmax = m_fXmax;
//	fYmin = m_fYmin;
//	fYmax = m_fYmax;
//	fZmin = m_fZmin;
//	fZmax = m_fZmax;
//}

//float CRDFModel::GetBoundingSphereDiameter() const
//{
//	return m_fBoundingSphereDiameter;
//}

/*virtual*/ void CRDFModel::ScaleAndCenter(bool bLoadingModel/* = false*/)
{
	ASSERT(FALSE);//#todo
	//ProgressStatus stat(L"Calculate scene sizes...");
	//if (m_pProgress != nullptr)
	//{
	//	m_pProgress->Log(0/*info*/, "Calculate scene sizes...");
	//}

	///* World */
	//m_fBoundingSphereDiameter = 0.f;

	///* Min/Max */
	//m_fXmin = FLT_MAX;
	//m_fXmax = -FLT_MAX;
	//m_fYmin = FLT_MAX;
	//m_fYmax = -FLT_MAX;
	//m_fZmin = FLT_MAX;
	//m_fZmax = -FLT_MAX;

	//auto itInstance = m_mapInstances.begin();
	//for (; itInstance != m_mapInstances.end(); itInstance++)
	//{
	//	if (!itInstance->second->getEnable())
	//	{
	//		continue;
	//	}

	//	if (!bLoadingModel)
	//	{
	//		itInstance->second->LoadOriginalData();
	//	}
	//	
	//	itInstance->second->calculateBB(
	//		itInstance->second,
	//		m_fXmin, m_fXmax, 
	//		m_fYmin, m_fYmax, 
	//		m_fZmin, m_fZmax);
	//}

	//if ((m_fXmin == FLT_MAX) ||
	//	(m_fXmax == -FLT_MAX) ||
	//	(m_fYmin == FLT_MAX) ||
	//	(m_fYmax == -FLT_MAX) ||
	//	(m_fZmin == FLT_MAX) ||
	//	(m_fZmax == -FLT_MAX))
	//{
	//	// TODO: new status bar for geometry
	//	/*::MessageBox(
	//		m_pProgress != nullptr ? m_pProgress->GetSafeHwnd() : ::AfxGetMainWnd()->GetSafeHwnd(), 
	//		_T("Internal error."), _T("Error"), MB_ICONERROR | MB_OK);*/

	//	return;
	//}
	//
	///* World */
	//m_fBoundingSphereDiameter = m_fXmax - m_fXmin;
	//m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fYmax - m_fYmin);
	//m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fZmax - m_fZmin);

	//TRACE(L"\n*** Scale and Center I *** => Xmin/max, Ymin/max, Zmin/max: %.16f, %.16f, %.16f, %.16f, %.16f, %.16f",
	//	m_fXmin,
	//	m_fXmax,
	//	m_fYmin,
	//	m_fYmax,
	//	m_fZmin,
	//	m_fZmax);
	//TRACE(L"\n*** Scale and Center, Bounding sphere I *** =>  %.16f", m_fBoundingSphereDiameter);

	///* Scale and Center */
	//itInstance = m_mapInstances.begin();
	//for (; itInstance != m_mapInstances.end(); itInstance++)
	//{
	//	if (!itInstance->second->getEnable())
	//	{
	//		continue;
	//	}

	//	itInstance->second->_geometry::scale(m_fBoundingSphereDiameter / 2.f);
	//}

	///* Min/Max */
	//m_fXmin = FLT_MAX;
	//m_fXmax = -FLT_MAX;
	//m_fYmin = FLT_MAX;
	//m_fYmax = -FLT_MAX;
	//m_fZmin = FLT_MAX;
	//m_fZmax = -FLT_MAX;

	//itInstance = m_mapInstances.begin();
	//for (; itInstance != m_mapInstances.end(); itInstance++)
	//{
	//	if (!itInstance->second->getEnable())
	//	{
	//		continue;
	//	}

	//	itInstance->second->calculateBB(
	//		itInstance->second,
	//		m_fXmin, m_fXmax,
	//		m_fYmin, m_fYmax,
	//		m_fZmin, m_fZmax);
	//}

	//if ((m_fXmin == FLT_MAX) ||
	//	(m_fXmax == -FLT_MAX) ||
	//	(m_fYmin == FLT_MAX) ||
	//	(m_fYmax == -FLT_MAX) ||
	//	(m_fZmin == FLT_MAX) ||
	//	(m_fZmax == -FLT_MAX))
	//{
	//	// TODO: new status bar for geometry
	//	/*
	//	::MessageBox(
	//		::AfxGetMainWnd()->GetSafeHwnd(), 
	//		_T("Internal error."), _T("Error"), MB_SYSTEMMODAL | MB_ICONERROR | MB_OK);
	//		*/
	//	return;
	//}

	///* World */
	//m_fBoundingSphereDiameter = m_fXmax - m_fXmin;
	//m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fYmax - m_fYmin);
	//m_fBoundingSphereDiameter = max(m_fBoundingSphereDiameter, m_fZmax - m_fZmin);

	//TRACE(L"\n*** Scale and Center II *** => Xmin/max, Ymin/max, Zmin/max: %.16f, %.16f, %.16f, %.16f, %.16f, %.16f",
	//	m_fXmin,
	//	m_fXmax,
	//	m_fYmin,
	//	m_fYmax,
	//	m_fZmin,
	//	m_fZmax);
	//TRACE(L"\n*** Scale and Center, Bounding sphere II *** =>  %.16f", m_fBoundingSphereDiameter);
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

void CRDFModel::Clean()
{
	/*
	* Model
	*/
	if (!m_bExternalModel) {
		if (m_iModel != 0) {
			CloseModel(m_iModel);
			m_iModel = 0;
		}
	} else {
		m_iModel = 0;

		m_bExternalModel = false;
	}

	auto itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++) {
		delete itInstance->second;
	}
	m_mapInstances.clear();

	/*
	* Texture
	*/
	delete m_pDefaultTexture;
	m_pDefaultTexture = nullptr;
}

OwlInstance CRDFModel::Translate(
	OwlInstance iInstance,
	double dX, double dY, double dZ,
	double d11, double d22, double d33)
{
	assert(iInstance != 0);

	int64_t iMatrixInstance = CreateInstance(GetClassByName(m_iModel, "Matrix"));
	assert(iMatrixInstance != 0);

	vector<double> vecTransformationMatrix =
	{
		d11, 0., 0.,
		0., d22, 0.,
		0., 0., d33,
		dX, dY, dZ,
	};

	SetDatatypeProperty(
		iMatrixInstance,
		GetPropertyByName(m_iModel, "coordinates"),
		vecTransformationMatrix.data(),
		vecTransformationMatrix.size());

	int64_t iTransformationInstance = CreateInstance(GetClassByName(m_iModel, "Transformation"));
	assert(iTransformationInstance != 0);

	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "matrix"), &iMatrixInstance, 1);
	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "object"), &iInstance, 1);

	return iTransformationInstance;
}

OwlInstance CRDFModel::Scale(OwlInstance iInstance, double dFactor)
{
	assert(iInstance != 0);

	int64_t iMatrixInstance = CreateInstance(GetClassByName(m_iModel, "Matrix"));
	assert(iMatrixInstance != 0);

	vector<double> vecTransformationMatrix =
	{
		dFactor, 0., 0.,
		0., dFactor, 0.,
		0., 0., dFactor,
		0., 0., 0.,
	};

	SetDatatypeProperty(
		iMatrixInstance,
		GetPropertyByName(m_iModel, "coordinates"),
		vecTransformationMatrix.data(),
		vecTransformationMatrix.size());

	int64_t iTransformationInstance = CreateInstance(GetClassByName(m_iModel, "Transformation"));
	assert(iTransformationInstance != 0);

	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "matrix"), &iMatrixInstance, 1);
	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "object"), &iInstance, 1);

	return iTransformationInstance;
}

OwlInstance CRDFModel::Rotate(
	OwlInstance iInstance,
	double alpha, double beta, double gamma)
{
	assert(iInstance != 0);

	int64_t iMatrixInstance = CreateInstance(GetClassByName(m_iModel, "Matrix"));
	assert(iMatrixInstance != 0);

	_matrix matrix;
	memset(&matrix, 0, sizeof(_matrix));
	_matrixRotateByEulerAngles(&matrix, alpha, beta, gamma);

	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_11"), &matrix._11, 1);
	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_12"), &matrix._12, 1);
	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_13"), &matrix._13, 1);

	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_21"), &matrix._21, 1);
	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_22"), &matrix._22, 1);
	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_23"), &matrix._23, 1);

	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_31"), &matrix._31, 1);
	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_32"), &matrix._32, 1);
	SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_33"), &matrix._33, 1);

	int64_t iTransformationInstance = CreateInstance(GetClassByName(m_iModel, "Transformation"));
	assert(iTransformationInstance != 0);

	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "matrix"), &iMatrixInstance, 1);
	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "object"), &iInstance, 1);

	return iTransformationInstance;
}

// ************************************************************************************************
CSceneRDFModel::CSceneRDFModel()
	: CRDFModel()
{
}

/*virtual*/ CSceneRDFModel::~CSceneRDFModel()
{
}

void CSceneRDFModel::TranslateModel(float fX, float fY, float fZ)
{
	auto itInstance = m_mapInstances.begin();
	for (; itInstance != m_mapInstances.end(); itInstance++) {
		if (!itInstance->second->getEnable()) {
			continue;
		}

		ASSERT(FALSE);//#todo
		/*itInstance->second->LoadOriginalData();
		itInstance->second->_geometry::translate(fX, fY, fZ);*/
	}
}

/*virtual*/ void CSceneRDFModel::CreateDefaultModel() /*override*/
{
	Clean();

	m_iModel = CreateModel();
	assert(m_iModel != 0);

	SetFormatSettings(m_iModel);

	// ASCII Chars
	m_pTextBuilder->Initialize(m_iModel);

	//CreateCoordinateSystem();

	load();
}

/*virtual*/ void CSceneRDFModel::PostLoadDRFModel() /*override*/
{
	//GetInstancesDefaultState();//#todo
}

void CSceneRDFModel::CreateCoordinateSystem()
{
	//#todo

	//const double AXIS_LENGTH = 2.5;

	//// Coordinate System
	//vector<OwlInstance> vecInstances;

	//// Coordinate System/X (1 Line3D)
	//OwlInstance iXAxisMaterial = 0;
	//{
	//	auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
	//	pAmbient.set_R(1.);
	//	pAmbient.set_G(0.);
	//	pAmbient.set_B(0.);
	//	pAmbient.set_W(1.);

	//	auto pColor = GEOM::Color::Create(m_iModel);
	//	pColor.set_ambient(pAmbient);

	//	auto pMaterial = GEOM::Material::Create(m_iModel);
	//	pMaterial.set_color(pColor);

	//	iXAxisMaterial = (int64_t)pMaterial;

	//	vector<double> vecPoints =
	//	{
	//		0., 0., 0.,
	//		AXIS_LENGTH / 2., 0., 0.,
	//	};

	//	auto pXAxis = GEOM::Line3D::Create(m_iModel);
	//	pXAxis.set_material(pMaterial);
	//	pXAxis.set_points(vecPoints.data(), vecPoints.size());

	//	vecInstances.push_back((int64_t)pXAxis);
	//}

	//// Coordinate System/Y (Line3D)
	//OwlInstance iYAxisMaterial = 0;
	//{
	//	auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
	//	pAmbient.set_R(0.);
	//	pAmbient.set_G(1.);
	//	pAmbient.set_B(0.);
	//	pAmbient.set_W(1.);

	//	auto pColor = GEOM::Color::Create(m_iModel);
	//	pColor.set_ambient(pAmbient);

	//	auto pMaterial = GEOM::Material::Create(m_iModel);
	//	pMaterial.set_color(pColor);

	//	iYAxisMaterial = (int64_t)pMaterial;

	//	vector<double> vecPoints =
	//	{
	//		0., 0., 0.,
	//		0., AXIS_LENGTH / 2., 0.,
	//	};

	//	auto pYAxis = GEOM::Line3D::Create(m_iModel);
	//	pYAxis.set_material(pMaterial);
	//	pYAxis.set_points(vecPoints.data(), vecPoints.size());

	//	vecInstances.push_back((int64_t)pYAxis);
	//}

	//// Coordinate System/Z (Line3D)
	//OwlInstance iZAxisMaterial = 0;
	//{
	//	auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
	//	pAmbient.set_R(0.);
	//	pAmbient.set_G(0.);
	//	pAmbient.set_B(1.);
	//	pAmbient.set_W(1.);

	//	auto pColor = GEOM::Color::Create(m_iModel);
	//	pColor.set_ambient(pAmbient);

	//	auto pMaterial = GEOM::Material::Create(m_iModel);
	//	pMaterial.set_color(pColor);

	//	iZAxisMaterial = (int64_t)pMaterial;

	//	vector<double> vecPoints =
	//	{
	//		0., 0., 0.,
	//		0., 0., AXIS_LENGTH / 2.,
	//	};

	//	auto pZAxis = GEOM::Line3D::Create(m_iModel);
	//	pZAxis.set_material(pMaterial);
	//	pZAxis.set_points(vecPoints.data(), vecPoints.size());

	//	vecInstances.push_back((int64_t)pZAxis);
	//}

	//// Arrows (1 Cone => 3 Transformations)
	//{
	//	const double ARROW_OFFSET = 2.5;

	//	auto pArrow = GEOM::Cone::Create(m_iModel);
	//	pArrow.set_height(AXIS_LENGTH / 15.);
	//	pArrow.set_radius(.075);

	//	// +X
	//	{
	//		OwlInstance iPlusXInstance = Translate(
	//			Rotate((int64_t)pArrow, 0., 2 * PI * 90. / 360., 0.),
	//			ARROW_OFFSET / 2., 0., 0.,
	//			1., 1., 1.);
	//		SetNameOfInstance(iPlusXInstance, "#(+X)");

	//		SetObjectProperty(
	//			iPlusXInstance,
	//			GetPropertyByName(m_iModel, "material"),
	//			&iXAxisMaterial,
	//			1);

	//		vecInstances.push_back(iPlusXInstance);
	//	}

	//	// +Y
	//	{
	//		OwlInstance iPlusYInstance = Translate(
	//			Rotate((int64_t)pArrow, 2 * PI * 270. / 360., 0., 0.),
	//			0., ARROW_OFFSET / 2., 0.,
	//			1., 1., 1.);
	//		SetNameOfInstance(iPlusYInstance, "#(+Y)");

	//		SetObjectProperty(
	//			iPlusYInstance,
	//			GetPropertyByName(m_iModel, "material"),
	//			&iYAxisMaterial,
	//			1);

	//		vecInstances.push_back(iPlusYInstance);
	//	}

	//	// +Z
	//	{
	//		OwlInstance iPlusZInstance = Translate(
	//			(int64_t)pArrow,
	//			0., 0., ARROW_OFFSET / 2.,
	//			1., 1., 1.);
	//		SetNameOfInstance(iPlusZInstance, "#(+Z)");

	//		SetObjectProperty(
	//			iPlusZInstance,
	//			GetPropertyByName(m_iModel, "material"),
	//			&iZAxisMaterial,
	//			1);

	//		vecInstances.push_back(iPlusZInstance);
	//	}
	//}

	///* Labels */
	//double dXmin = DBL_MAX;
	//double dXmax = -DBL_MAX;
	//double dYmin = DBL_MAX;
	//double dYmax = -DBL_MAX;
	//double dZmin = DBL_MAX;
	//double dZmax = -DBL_MAX;

	//// X-axis
	//OwlInstance iPlusXLabelInstance = m_pTextBuilder->BuildText("X-axis", true);
	//assert(iPlusXLabelInstance != 0);

	//CRDFInstance::calculateBBMinMax(
	//	iPlusXLabelInstance,
	//	dXmin, dXmax,
	//	dYmin, dYmax,
	//	dZmin, dZmax);

	//// Y-axis
	//OwlInstance iPlusYLabelInstance = m_pTextBuilder->BuildText("Y-axis", true);
	//assert(iPlusYLabelInstance != 0);

	//CRDFInstance::calculateBBMinMax(
	//	iPlusYLabelInstance,
	//	dXmin, dXmax,
	//	dYmin, dYmax,
	//	dZmin, dZmax);

	//// Z-axis
	//OwlInstance iPlusZLabelInstance = m_pTextBuilder->BuildText("Z-axis", true);
	//assert(iPlusZLabelInstance != 0);

	//CRDFInstance::calculateBBMinMax(
	//	iPlusZLabelInstance,
	//	dXmin, dXmax,
	//	dYmin, dYmax,
	//	dZmin, dZmax);

	///* Scale Factor */
	//double dMaxLength = dXmax - dXmin;
	//dMaxLength = fmax(dMaxLength, dYmax - dYmin);
	//dMaxLength = fmax(dMaxLength, dZmax - dZmin);

	//double dScaleFactor = ((AXIS_LENGTH / 2.) * .75) / dMaxLength;

	///* Transform Labels */

	//// X-axis
	//OwlInstance iInstance = Translate(
	//	Rotate(Scale(iPlusXLabelInstance, dScaleFactor / 2.), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 180.),
	//	AXIS_LENGTH / 1.4, 0., 0.,
	//	-1., 1., 1.);
	//
	//SetNameOfInstance(iInstance, "#X-axis");
	//SetObjectProperty(
	//	iInstance,
	//	GetPropertyByName(m_iModel, "material"),
	//	&iXAxisMaterial,
	//	1);

	//// Y-axis
	//iInstance = Translate(
	//	Rotate(Scale(iPlusYLabelInstance, dScaleFactor / 2.), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
	//	0., AXIS_LENGTH / 1.4, 0.,
	//	-1., 1., 1.);

	//SetNameOfInstance(iInstance, "#Y-axis");
	//SetObjectProperty(
	//	iInstance,
	//	GetPropertyByName(m_iModel, "material"),
	//	&iYAxisMaterial,
	//	1);

	//// Z-axis
	//iInstance = Translate(
	//	Rotate(Scale(iPlusZLabelInstance, dScaleFactor / 2.), 2 * PI * 270. / 360., 2 * PI * 90. / 360., 0.),
	//	0., 0., AXIS_LENGTH / 1.4,
	//	1., 1., -1.);

	//SetNameOfInstance(iInstance, "#Z-axis");
	//SetObjectProperty(
	//	iInstance,
	//	GetPropertyByName(m_iModel, "material"),
	//	&iZAxisMaterial,
	//	1);

	///* Collection */
	//OwlInstance iCollectionInstance = CreateInstance(GetClassByName(m_iModel, "Collection"), "#Coordinate System#");
	//assert(iCollectionInstance != 0);

	//SetObjectProperty(
	//	iCollectionInstance,
	//	GetPropertyByName(m_iModel, "objects"),
	//	vecInstances.data(),
	//	vecInstances.size());
}

// ************************************************************************************************
CNavigatorRDFModel::CNavigatorRDFModel()
	: CSceneRDFModel()
{
}

/*virtual*/ CNavigatorRDFModel::~CNavigatorRDFModel()
{
}

/*virtual*/ void CNavigatorRDFModel::CreateDefaultModel() /*override*/
{
	Clean();

	m_iModel = CreateModel();
	assert(m_iModel != 0);

	SetFormatSettings(m_iModel);

	// ASCII Chars
	m_pTextBuilder->Initialize(m_iModel);

	CreateCoordinateSystem();

	CreateNaigator();

	CreateNaigatorLabels();

	load();
}

void CNavigatorRDFModel::CreateNaigator()
{
	// Cube (BoundaryRepresentations)
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(.1);
		pAmbient.set_G(.1);
		pAmbient.set_B(.1);
		pAmbient.set_W(.05);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(m_iModel);
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

		auto pBoundaryRepresentation = GEOM::BoundaryRepresentation::Create(m_iModel);
		pBoundaryRepresentation.set_material(pMaterial);
		pBoundaryRepresentation.set_vertices(vecVertices.data(), vecVertices.size());
		pBoundaryRepresentation.set_indices(vecIndices.data(), vecIndices.size());

		// Front
		OwlInstance iInstance = Translate(
			Rotate((int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 0.),
			0., -.75, 0.,
			1., -1., 1.);
		SetNameOfInstance(iInstance, "#front");

		// Back
		iInstance = Translate(
			Rotate((int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 0.),
			0., .75, 0.,
			-1., 1., 1.);
		SetNameOfInstance(iInstance, "#back");

		// Top
		iInstance = Translate(
			(int64_t)pBoundaryRepresentation,
			0., 0., .75,
			1., 1., -1.);
		SetNameOfInstance(iInstance, "#top");

		// Bottom
		iInstance = Translate(
			(int64_t)pBoundaryRepresentation,
			0., 0., -.75,
			1, 1., 1.);
		SetNameOfInstance(iInstance, "#bottom");

		// Left
		iInstance = Translate(
			Rotate((int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
			-.75, 0., 0.,
			1., -1., 1.);
		SetNameOfInstance(iInstance, "#left");

		// Right
		iInstance = Translate(
			Rotate((int64_t)pBoundaryRepresentation, 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
			.75, 0., 0.,
			-1., 1., 1.);
		SetNameOfInstance(iInstance, "#right");
	}

	// Sphere (Sphere)
	{
		auto pAmbient = GEOM::ColorComponent::Create(m_iModel);
		pAmbient.set_R(0.);
		pAmbient.set_G(0.);
		pAmbient.set_B(1.);
		pAmbient.set_W(1.);

		auto pColor = GEOM::Color::Create(m_iModel);
		pColor.set_ambient(pAmbient);

		auto pMaterial = GEOM::Material::Create(m_iModel);
		pMaterial.set_color(pColor);

		auto pSphere = GEOM::Sphere::Create(m_iModel);
		pSphere.set_material(pMaterial);
		pSphere.set_radius(.1);
		pSphere.set_segmentationParts(36);

		// Front/Top/Left
		OwlInstance iInstance = Translate(
			(int64_t)pSphere,
			-.75, -.75, .75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#front-top-left");

		// Front/Top/Right
		iInstance = Translate(
			(int64_t)pSphere,
			.75, -.75, .75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#front-top-right");

		// Front/Bottom/Left
		iInstance = Translate(
			(int64_t)pSphere,
			-.75, -.75, -.75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#front-bottom-left");

		// Front/Bottom/Right
		iInstance = Translate(
			(int64_t)pSphere,
			.75, -.75, -.75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#front-bottom-right");

		// Back/Top/Left
		iInstance = Translate(
			(int64_t)pSphere,
			.75, .75, .75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#back-top-left");

		// Back/Top/Right
		iInstance = Translate(
			(int64_t)pSphere,
			-.75, .75, .75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#back-top-right");

		// Back/Bottom/Left
		iInstance = Translate(
			(int64_t)pSphere,
			.75, .75, -.75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#back-bottom-left");

		// Back/Bottom/Right
		iInstance = Translate(
			(int64_t)pSphere,
			-.75, .75, -.75,
			1., 1., 1.);
		SetNameOfInstance(iInstance, "#back-bottom-right");
	}
}

void CNavigatorRDFModel::CreateNaigatorLabels()
{
	//#todo
	//double dXmin = DBL_MAX;
	//double dXmax = -DBL_MAX;
	//double dYmin = DBL_MAX;
	//double dYmax = -DBL_MAX;
	//double dZmin = DBL_MAX;
	//double dZmax = -DBL_MAX;

	///* Top */
	//OwlInstance iTopLabelInstance = m_pTextBuilder->BuildText("top", true);
	//assert(iTopLabelInstance != 0);

	//CRDFInstance::calculateBBMinMax(
	//	iTopLabelInstance,
	//	dXmin, dXmax,
	//	dYmin, dYmax,
	//	dZmin, dZmax);

	///* Bottom */
	//OwlInstance iBottomLabelInstance = m_pTextBuilder->BuildText("bottom", true);
	//assert(iBottomLabelInstance != 0);

	//CRDFInstance::calculateBBMinMax(
	//	iBottomLabelInstance,
	//	dXmin, dXmax,
	//	dYmin, dYmax,
	//	dZmin, dZmax);

	///* Front */
	//OwlInstance iFrontLabelInstance = m_pTextBuilder->BuildText("front", true);
	//assert(iFrontLabelInstance != 0);

	//CRDFInstance::calculateBBMinMax(
	//	iFrontLabelInstance,
	//	dXmin, dXmax,
	//	dYmin, dYmax,
	//	dZmin, dZmax);

	///* Back */
	//OwlInstance iBackLabelInstance = m_pTextBuilder->BuildText("back", true);
	//assert(iBackLabelInstance != 0);

	//CRDFInstance::calculateBBMinMax(
	//	iBackLabelInstance,
	//	dXmin, dXmax,
	//	dYmin, dYmax,
	//	dZmin, dZmax);

	///* Left */
	//OwlInstance iLeftLabelInstance = m_pTextBuilder->BuildText("left", true);
	//assert(iLeftLabelInstance != 0);

	//CRDFInstance::calculateBBMinMax(
	//	iLeftLabelInstance,
	//	dXmin, dXmax,
	//	dYmin, dYmax,
	//	dZmin, dZmax);

	///* Right */
	//OwlInstance iRightLabelInstance = m_pTextBuilder->BuildText("right", true);
	//assert(iRightLabelInstance != 0);

	//CRDFInstance::calculateBBMinMax(
	//	iRightLabelInstance,
	//	dXmin, dXmax,
	//	dYmin, dYmax,
	//	dZmin, dZmax);

	///* Scale Factor */
	//double dMaxLength = dXmax - dXmin;
	//dMaxLength = fmax(dMaxLength, dYmax - dYmin);
	//dMaxLength = fmax(dMaxLength, dZmax - dZmin);

	//double dScaleFactor = (1.5 * .9) / dMaxLength;

	//// Front
	//OwlInstance iInstance = Translate(
	//	Rotate(Scale(iFrontLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 0.),
	//	0., -.751, 0.,
	//	1., 1., 1.);
	//SetNameOfInstance(iInstance, "#front-label");

	//// Back
	//iInstance = Translate(
	//	Rotate(Scale(iBackLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 0.),
	//	0., .751, 0.,
	//	-1., 1., 1.);
	//SetNameOfInstance(iInstance, "#back-label");

	//// Top
	//iInstance = Translate(
	//	Scale(iTopLabelInstance, dScaleFactor),
	//	0., 0., .751,
	//	1., 1., 1.);
	//SetNameOfInstance(iInstance, "#top-label");

	//// Bottom
	//iInstance = Translate(
	//	Scale(iBottomLabelInstance, dScaleFactor),
	//	0., 0., -.751,
	//	-1, 1., 1.);
	//SetNameOfInstance(iInstance, "#bottom-label");

	//// Left
	//iInstance = Translate(
	//	Rotate(Scale(iLeftLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
	//	-.751, 0., 0.,
	//	1., -1., 1.);
	//SetNameOfInstance(iInstance, "#left-label");

	//// Right
	//iInstance = Translate(
	//	Rotate(Scale(iRightLabelInstance, dScaleFactor), 2 * PI * 90. / 360., 0., 2 * PI * 90. / 360.),
	//	.751, 0., 0.,
	//	1., 1., 1.);
	//SetNameOfInstance(iInstance, "#right-label");
}