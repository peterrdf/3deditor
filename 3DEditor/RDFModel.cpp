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
				OwlModel owlModel = OpenModelW(m_szPath);
				if (owlModel) {
					m_pModel->attachModel(m_szPath, owlModel);
				}
			} else {
				assert(m_pModel->getOwlModel() != 0);
				ASSERT(FALSE);//#todo
				ImportModelW(m_pModel->getOwlModel(), m_szPath);

				m_pModel->load();
			}
		}

		//#todo
		//if (m_pModel->m_iModel == 0) {
		//	CString strError;
		//	strError.Format(L"Failed to open '%s'.", m_szPath);

		//	if (!TEST_MODE) {
		//		if (m_pProgress != nullptr) {
		//			m_pProgress->Log(2/*error*/, CW2A(strError));
		//		}

		//		::MessageBox(
		//			::AfxGetMainWnd()->GetSafeHwnd(),
		//			strError, L"Error", MB_SYSTEMMODAL | MB_ICONERROR | MB_OK);
		//	} else {
		//		TRACE(L"\nError: %s", (LPCTSTR)strError);
		//	}
		//} else {
		//	if (!TEST_MODE) {
		//		if (m_pProgress != nullptr) {
		//			m_pProgress->Log(0/*info*/, "*** Done. ***");
		//		}
		//	}
		//}
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

/*virtual*/ void CRDFModel::CreateDefaultModel()
{
	m_strPath = L"DEFAULT";

	clean();

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

void CRDFModel::Load(const wchar_t* szPath, bool bLoading)
{
	if (bLoading) {
		clean();

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

void CRDFModel::ImportModel(const wchar_t* szPath)
{
	Load(szPath, false);
}

#ifdef _DXF_SUPPORT
void CRDFModel::LoadDXF(const wchar_t* /*szPath*/)
{
	ASSERT(FALSE);//#todo
	/*if (m_iModel == 0) {
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

	load();*/
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

		OwlModel olwModel = CreateModel();
		ASSERT(olwModel != 0);

		ImportGISModel(olwModel, CW2A(szPath));

		attachModel(szPath, olwModel);
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
}
#endif // _GIS_SUPPORT

