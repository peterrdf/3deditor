#pragma once

#ifdef _WINDOWS
#include "engine.h"
#else
#include "../include/engine.h"
#endif

#include "_ptr.h"
#include "_geometry.h"
#include "_instance.h"

#include "_mvc.h"

#include "_log.h"
#include "_errors.h"

#include "_node.h"

#include <set>
#include <map>
using namespace std;

// ************************************************************************************************
namespace _bin2gltf
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

		bool m_bEmbeddedBuffers;

		_material* m_pPolygonsMaterial;
		vector<const _material*> m_vecMaterials;
		GLTFMATERIALS m_mapMaterials;
		map<wstring, uint32_t> m_mapImages;
		vector<_node*> m_vecNodes;
		vector<uint32_t> m_vecSceneRootNodes;

		string m_strOutputFile;
		string m_strOutputFolder;
		ostream* m_pOutputStream;
		int m_iIndent; // TAB-s count

		uint32_t m_iBuffersCount;
		uint32_t m_iBufferViewsCount;
		uint32_t m_iMeshesCount;

	public: // Methods

		_exporter(_model* pModel, const char* szOutputFile, bool bEmbeddedBuffers);
		virtual ~_exporter();

		void execute();

	protected: // Methods	

		virtual bool createOuputStream();

		virtual bool preExecute();
		virtual void postExecute();

		string buildStringProperty(const string& strName, const string& strValue);
		string buildNumberProperty(const string& strName, const string& strValue);
		string buildArrayProperty(const string& strName, const vector<string>& vecValues);

		void writeIndent();
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
		void writeSceneProperty();
		void writeScenesProperty();
		void writeMaterials();
		void writeImages();
		void writeSamplers();
		void writeTextures();

		size_t addMaterial(const _material* pMaterial);

	public: // Properties

		OwlModel getModel() const { return m_pModel->getOwlModel(); }
		ostream* getOutputStream() const { return m_pOutputStream; }
		int& indent() { return m_iIndent; }

		virtual int64_t getGeometryID(_geometry* pGeometry) { return (int64_t)pGeometry; }
	};
};


