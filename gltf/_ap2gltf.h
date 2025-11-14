#pragma once

#ifdef _WINDOWS
#include "engine.h"
#else
#include "../../include/engine.h"
#endif

#include "_ptr.h"
#include "_geometry.h"
#include "_instance.h"

#include "_mvc.h"

#include "_log.h"
#include "_io.h"
#include "_errors.h"

#include "_node.h"

#include "_ifc_model_structure.h"
#include "_ap242_model_structure.h"

#include <set>
#include <map>
#include <vector>
using namespace std;

// ************************************************************************************************
namespace _ap2gltf
{
	// ********************************************************************************************
	const char GENERATOR[] = "RDF LTD glTF 2.0 exporter";
	const char VERSION[] = "2.0";

	// ********************************************************************************************
	const char TAB = '\t';
	const char SPACE = ' ';
	const char COMMA = ',';
	const char DOULE_QUOT_MARK = '"';
	const char COLON = ':';

	// ********************************************************************************************
	const char ASSET_PROP[] = "asset";
	const char GENERATOR_PROP[] = "generator";
	const char VERSION_PROP[] = "version";
	const char SCENE_PROP[] = "scene";
	const char SCENES_PROP[] = "scenes";
	const char NODES_PROP[] = "nodes";
	const char CHILDREN_PROP[] = "children";
	const char BUFFERS_PROP[] = "buffers";
	const char BUFFER_VIEWS_PROP[] = "bufferViews";
	const char ACCESSORS_PROP[] = "accessors";
	const char MESHES_PROP[] = "meshes";
	const char MESH_PROP[] = "mesh";
	const char MATERIALS_PROP[] = "materials";
	const char IMAGES_PROP[] = "images";
	const char SAMPLERS_PROP[] = "samplers";
	const char TEXTURES_PROP[] = "textures";

	// ********************************************************************************************
	typedef map<_material, size_t, _materialsComparator> GLTFMATERIALS;

	// ********************************************************************************************
	class _exporter : public _log_client
	{

	protected: // Members

		_model* m_pModel;
		set<SdaiInstance> m_setTargetInstances;

		bool m_bEmbeddedBuffers;

		_material* m_pPolygonsMaterial;
		vector<const _material*> m_vecMaterials;
		GLTFMATERIALS m_mapMaterials;
		map<wstring, uint32_t> m_mapImages;
		uint32_t m_iSceneNodeIndex;
		vector<_node*> m_vecNodes;
		map<_geometry*,  _node*> m_mapNodes;
		map<string, uint32_t> m_mapName2Index;
		vector<uint32_t> m_vecSceneRootNodes;
		uint32_t m_iRootNodeIndex;

		string m_strOutputFile;
		string m_strOutputFolder;
		ostream* m_pOutputStream;
		int m_iIndent; // TAB-s count

		uint32_t m_iBuffersCount;
		uint32_t m_iBufferViewsCount;
		uint32_t m_iMeshesCount;

		bool m_bExportGeometriesOnly;
		bool m_bWriteModelMetadataJSON;

		// IFC
		_ifc_model_structure* m_pIFCModelStructure;

		// AP242
		_ap242_model_structure* m_pAP242ModelStructure;

	public: // Methods

		_exporter(_model* pModel, const char* szOutputFile, bool bEmbeddedBuffers);
		virtual ~_exporter();

		void execute();
		void execute(const set<SdaiInstance>& setTargetInstances);

	protected: // Methods

		virtual bool createOuputStream();

		virtual bool ignoreGeometry(_geometry* pGeometry);

		virtual bool preExecute();
		virtual void postExecute();

		string buildStringProperty(const string& strName, const string& strValue);
		string buildNumberProperty(const string& strName, const string& strValue);
		string buildArrayProperty(const string& strName, const vector<string>& vecValues);
		string buildArrayProperty(const string& strName, const vector<uint32_t>& vecValues);

		virtual void writeIndent();
		void writeStartObjectTag(bool bNewLine = true);
		void writeEndObjectTag();
		void writeStartArrayTag(bool bNewLine = true);
		void writeEndArrayTag();
		void writeStringProperty(const string& strName, const string& strValue);
		void writeIntProperty(const string& strName, int iValue);
		void writeUIntProperty(const string& strName, uint32_t iValue);
		void writeFloatProperty(const string& strName, float fValue);
		void writeDoubleProperty(const string& strName, double dValue);
		void writeBoolProperty(const string& strName, bool bValue);
		void writeObjectProperty(const string& strName, const vector<string>& vecProperties);
		void writeAssetProperty();
		virtual void writeBuffersProperty();
		virtual void writeBufferViewsProperty();
		void writeAccessorsProperty();
		void writeMeshesProperty();
		void writeNodesProperty();
		void writeNodesPropertyModelStructure();
		void writeNodesPropertyModelStructureIFC(_ifc_model* pIfcModel);
		void writeNodesPropertyModelStructureIFC(_ifc_model* pIfcModel, _ifc_node* pParent, vector<uint32_t>& vecParentChildren);
		void writeNodesPropertyModelStructureAP242(_ap242_model* pAP242Model);
		void writeNodesPropertyModelStructureAP242(_ap242_model* pAP242Model, _ap242_node* pParent, vector<uint32_t>& vecParentChildren);
		void writeSceneProperty();
		void writeScenesProperty();
		void writeMaterials();
		void writeImages();
		void writeSamplers();
		void writeTextures();
		string escapeJsonString(const string& input) const;

		size_t addMaterial(const _material* pMaterial);

		// XKT metadata
		void writeMetadata();
		void writeMetadataProperties();
		void writeMetadataPropertiesIFC(_ifc_model* pIfcModel);
		void writeMetadataPropertiesSTEP(_ap242_model* pAP242Model);
		void writeMetadataUnits();
		void writeMetadataGroups();
		void writeMetadataObjects();
		void writeMetadataObjectsIFC(_ifc_model* pIfcModel, const map<SdaiInstance, vector<SdaiInstance>>& mapGroupedInstances);
		void writeMetadataObjectChildrenIFC(_ifc_node* pNode, const map<SdaiInstance, vector<SdaiInstance>>& mapGroupedInstances, _ifc_property_provider* pPropertyProvider);
		void writeMetadataObjectsSTEP(_ap242_model* pAP242Model);		
		void writeMetadataObjectChildrenSTEP(_ap242_node* pNode, _ap242_model* pAP242Model, _ap242_property_provider* pPropertyProvider);

	public: // Properties

		OwlModel getModel() const { return m_pModel->getOwlModel(); }
		ostream* getOutputStream() const { return m_pOutputStream; }
		int& indent() { return m_iIndent; }
		bool& exportGeometriesOnly() { return m_bExportGeometriesOnly; }
		bool& writeModelMetadataJSON() { return m_bWriteModelMetadataJSON; }
		virtual const char* getNewLine() const { return "\n"; }
		virtual int64_t getGeometryID(_geometry* pGeometry) { return (int64_t)pGeometry; }
	};
};


