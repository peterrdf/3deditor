#pragma once

#include "_bin2gltf.h"

// ************************************************************************************************
namespace _bin2glb
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
	class _exporter : public _bin2gltf::_exporter
	{

	public: // Methods

		_exporter(_model* pModel, const char* szOutputFile);
		virtual ~_exporter();

		void execute();

	protected: // Methods

		virtual void writeBuffersProperty() override;
		virtual void writeBufferViewsProperty() override;
	};
};


