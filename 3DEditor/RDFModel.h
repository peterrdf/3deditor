#pragma once

#include "_rdf_mvc.h"
#include "_rdf_instance.h"
#include "_rdf_class.h"
#include "_rdf_property.h"

#include <map>
using namespace std;

// ************************************************************************************************
static TCHAR SUPPORTED_FILES[] = _T("BIN Files (*.bin)|*.bin|\
BIN Zip Files (*.binz)|*.binz|\
CityGML Files (*.gml;*citygml)|\
*.gml;*citygml|LandXML Files (*.xml)|*.xml;|\
CityJSON Files (*.city.json;*.json)|*.city.json;*.json|\
Wavefront OBJ Files (*.obj)|*.obj|\
glTF Files (*.gltf)|*.gltf|\
glTF Binary Files (*.glb)|*.glb|\
Autocad DXF Files (*.dxf)|*.dxf|")
#ifdef IMPORT_PLY
_T("Polygon File Format (*.ply)|*.ply|")
#endif
_T("All Files (*.*)|*.*||");

// ************************************************************************************************
class CRDFModel : public _rdf_model
{
	friend class CLoadTask;

private: // Fields

	_texture* m_pDefaultTexture;

public: // Methods

	CRDFModel();
	virtual ~CRDFModel();

public: // Methods

	// _model
	virtual _texture* getDefaultTexture() override;

protected: // Methods

	// _model
	virtual void clean(bool bCloseModel = true) override;

public: // Methods

	void Load(const wchar_t* szPath, bool bAdd);

#ifdef _DXF_SUPPORT
	void LoadDXFModel(const wchar_t* szPath);
#endif

#ifdef _GIS_SUPPORT
	void LoadGISModel(OwlModel owlModel, const wchar_t* szPath);
#endif

	void LoadOBJModel(OwlModel owlModel, const wchar_t* szPath);
	void LoadGLTFModel(OwlModel owlModel, const wchar_t* szPath);
};

// ************************************************************************************************
class CDefaultModel : public CRDFModel
{

public: // Methods

	CDefaultModel();
	virtual ~CDefaultModel();

private: // Methods

	void Create();
};