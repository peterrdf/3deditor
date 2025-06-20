#include "_host.h"

#include "_gltf_importer_t.h"
#include "_guid.h"

// ************************************************************************************************
namespace _eng
{
	// ********************************************************************************************
	const char UNKNOWN_CLASS[] = "class:Thing";

	const char SCHEMA_PROP[] = "$schema";
	const char ID_PROP[] = "$id";
	const char TITLE_PROP[] = "title";

	const char DIFFUSE_COLOR_PROP[] = "diffuseColor";
	const char EMISSIVE_COLOR_PROP[] = "emissiveColor";
	const char SPECULAR_COLOR_PROP[] = "specularColor";

	// ********************************************************************************************
	_gltf_importer_t::_gltf_importer_t(OwlModel iModel, const string& strRootFolder, const string& strOutputFolder, int iValidationLevel)
		: _importer_t<_json::_document, _json::_document_site>(iModel, iValidationLevel)
		, m_strRootFolder(strRootFolder)
		, m_strOutputFolder(strOutputFolder)
		, m_pRootObject(nullptr)
		, m_pScenesProperty(nullptr)
		, m_pNodesProperty(nullptr)
		, m_pMeshesProperty(nullptr)
		, m_pAccessorsProperty(nullptr)
		, m_pBufferViewsProperty(nullptr)
		, m_pBuffersProperty(nullptr)
		, m_pMaterialsProperty(nullptr)
		, m_pTexturesProperty(nullptr)
		, m_pImagesProperty(nullptr)
		, m_mapNodes()
		, m_mapMeshes()
		, m_mapMaterials()
		, m_mapTextures()
		, m_iRootInstance(0)
		, m_mapClasses()
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(m_strRootFolder);
		VERIFY_STLOBJ_IS_NOT_EMPTY(m_strOutputFolder);
	}

	/*virtual*/ _gltf_importer_t::~_gltf_importer_t()
	{
	}

	/*virtual*/ bool _gltf_importer_t::preBuildModel() /*override*/
	{
		/* Validations */

		// root
		m_pRootObject = getDocument()->getRoot();
		if (m_pRootObject == nullptr) {
			getLog()->logWrite(enumLogEvent::error, "Invalid format.");

			return false;
		}

		// asset
		{
			auto pAssetProperty = m_pRootObject->getPropertyValueAs<_json::_object>("asset");
			if (pAssetProperty == nullptr) {
				getLog()->logWrite(enumLogEvent::error, "Invalid format: 'asset' property is not found.");

				return false;
			}

			// version
			auto pVersionProperty = pAssetProperty->getProperty("version");
			if (pVersionProperty == nullptr) {
				getLog()->logWrite(enumLogEvent::error, "Invalid format: 'asset/version' property is not found.");

				return false;
			}

			// version >= 2.0
			auto pVersionValue = pVersionProperty->getValue()->as<_json::_simple>();
			VERIFY_POINTER(pVersionValue);

			double dVersion = atof(pVersionValue->getValue().c_str());
			if (dVersion < 2.) {
				getLog()->logWrite(enumLogEvent::error, _string::format("Version '%f' is not supported.", dVersion));

				return false;
			}
		}
		// asset

		// scenes
		{
			m_pScenesProperty = m_pRootObject->getProperty("scenes");
			if (m_pScenesProperty == nullptr) {
				getLog()->logWrite(enumLogEvent::error, "Invalid format: 'scenes' property is not found.");

				return false;
			}
		}
		// scenes

		// nodes
		{
			m_pNodesProperty = m_pRootObject->getProperty("nodes");
			if (m_pNodesProperty == nullptr) {
				getLog()->logWrite(enumLogEvent::error, "Invalid format: 'nodes' property is not found.");

				return false;
			}
		}
		// nodes

		// meshes
		{
			m_pMeshesProperty = m_pRootObject->getProperty("meshes");
			if (m_pMeshesProperty == nullptr) {
				getLog()->logWrite(enumLogEvent::error, "Invalid format: 'meshes' property is not found.");

				return false;
			}
		}
		// meshes

		// accessors
		{
			m_pAccessorsProperty = m_pRootObject->getProperty("accessors");
			if (m_pAccessorsProperty == nullptr) {
				getLog()->logWrite(enumLogEvent::error, "Invalid format: 'accessors' property is not found.");

				return false;
			}
		}
		// accessors

		// bufferViews
		{
			m_pBufferViewsProperty = m_pRootObject->getProperty("bufferViews");
			if (m_pBufferViewsProperty == nullptr) {
				getLog()->logWrite(enumLogEvent::error, "Invalid format: 'bufferViews' property is not found.");

				return false;
			}
		}
		// bufferViews

		// buffers
		{
			m_pBuffersProperty = m_pRootObject->getProperty("buffers");
			if (m_pBuffersProperty == nullptr) {
				getLog()->logWrite(enumLogEvent::error, "Invalid format: 'buffers' property is not found.");

				return false;
			}
		}
		// buffers

		// materials
		{
			m_pMaterialsProperty = m_pRootObject->getProperty("materials");
			if (m_pMaterialsProperty == nullptr) {
				getLog()->logWrite(enumLogEvent::warning, "'materials' property is not found.");
			}
		}
		// materials
		
		// textures
		{
			m_pTexturesProperty = m_pRootObject->getProperty("textures");
			if (m_pTexturesProperty == nullptr) {
				getLog()->logWrite(enumLogEvent::warning, "'textures' property is not found.");
			}
		}
		// textures

		// images
		{
			m_pImagesProperty = m_pRootObject->getProperty("images");
		}
		// images

		return  true;
	}

	/*virtual*/ void _gltf_importer_t::buildModel() /*override*/
	{
		createRootInstance();
	}

	/*virtual*/ OwlInstance _gltf_importer_t::createRootInstance()
	{
		VERIFY_POINTER(m_pRootObject);

		m_iRootInstance = createInstance(
			_importer_t::createClass("glTF"),
			"glTF");
		VERIFY_INSTANCE(m_iRootInstance);

		// asset
		auto pAssetProperty = m_pRootObject->getPropertyValueAs<_json::_object>("asset");
		assert(pAssetProperty != nullptr);

		// generator
		auto pGeneratorValue = pAssetProperty->getPropertyValue("generator");
		if (pGeneratorValue != nullptr) {
			createStringProperty(m_iRootInstance,
				"generator",
				vector<const _json::_value*>{ pGeneratorValue->as<_json::_simple>() });
		}

		// version
		auto pVersionValue = pAssetProperty->getPropertyValue("version")->as<_json::_simple>();
		assert(pVersionValue != nullptr);

		createStringProperty(m_iRootInstance,
			"version",
			vector<const _json::_value*>{ pVersionValue });

		createScenesProperty();

		return m_iRootInstance;
	}

	/*virtual*/ void _gltf_importer_t::createScenesProperty()
	{
		VERIFY_INSTANCE(m_iRootInstance);
		VERIFY_POINTER(m_pScenesProperty);

		auto pScenes = m_pScenesProperty->getValue()->as<_json::_array>();
		VERIFY_POINTER(pScenes);

		vector<OwlInstance> vecSceneInstances;
		for (size_t iScene = 0; iScene < pScenes->getValues().size(); iScene++) {
			auto pScene = pScenes->getValues()[iScene]->as<_json::_object>();
			VERIFY_POINTER(pScene);

			auto pNodesValue = pScene->getPropertyValueAs<_json::_array>("nodes");
			VERIFY_POINTER(pNodesValue);

			vector<OwlInstance> vecNodeInstances;
			for (size_t iNode = 0; iNode < pNodesValue->getValues().size(); iNode++) {
				auto pNode = pNodesValue->getValues()[iNode];
				VERIFY_POINTER(pNode);

				OwlInstance iNodeInstance = createNodeInstance(pNode->as<_json::_simple>());
				VERIFY_INSTANCE(iNodeInstance);

				vecNodeInstances.push_back(iNodeInstance);
			} // for (size_t iNode = ...

			OwlInstance iSceneInstance = createInstance(
				_importer_t::createClass("scene"),
				_string::format("scene (%d)", (int)iScene));
			VERIFY_INSTANCE(iSceneInstance);

			SetObjectProperty(
				iSceneInstance,
				createObjectPropertyInstance("nodes"),
				vecNodeInstances.data(),
				vecNodeInstances.size());

			vecSceneInstances.push_back(iSceneInstance);
		} // for (size_t iScene = ...

		SetObjectProperty(
			m_iRootInstance,
			createObjectPropertyInstance("scenes"),
			vecSceneInstances.data(),
			vecSceneInstances.size());
	}

	/*virtual*/ OwlInstance _gltf_importer_t::createNodeInstance(const _json::_simple* pNodeIndex)
	{
		VERIFY_POINTER(pNodeIndex);

		int iNodeIndex = atoi(pNodeIndex->getValue().c_str());

		/* Cache */
		if (m_mapNodes.find(iNodeIndex) != m_mapNodes.end()) {
			return m_mapNodes.at(iNodeIndex);
		}

		auto pNodeObject = getNode(iNodeIndex);
		VERIFY_POINTER(pNodeObject);

		OwlInstance iNodeInstance = createCollectionInstance(
			"node",
			_string::format("node (%d)", (int)iNodeIndex));
		VERIFY_INSTANCE(iNodeInstance);

		/* Cache */
		m_mapNodes[iNodeIndex] = iNodeInstance;

		auto pChildrenProperty = pNodeObject->getProperty("children");
		if (pChildrenProperty != nullptr) {
			auto pChildrenValue = pChildrenProperty->getValue()->as<_json::_array>();
			VERIFY_POINTER(pChildrenValue);

			vector<OwlInstance> vecNodeInstances;
			for (size_t iChild = 0; iChild < pChildrenValue->getValues().size(); iChild++) {
				OwlInstance iChildNodeInstance = createNodeInstance(pChildrenValue->getValues()[iChild]->as<_json::_simple>());
				VERIFY_INSTANCE(iChildNodeInstance);

				vecNodeInstances.push_back(iChildNodeInstance);
			}

			SetObjectProperty(
				iNodeInstance,
				GetPropertyByName(getModel(), "objects"),
				vecNodeInstances.data(),
				vecNodeInstances.size());
		} // if (pChildrenProperty != nullptr)
		else {
			auto pMeshProperty = pNodeObject->getProperty("mesh");
			if (pMeshProperty != nullptr) {
				auto pMeshIndex = pMeshProperty->getValue();
				VERIFY_POINTER(pMeshIndex);

				vector<OwlInstance> vecGeometries;
				createNodeGeometry(pMeshIndex->as<_json::_simple>(), vecGeometries);
				if (!vecGeometries.empty()) {
					SetObjectProperty(
						iNodeInstance,
						GetPropertyByName(getModel(), "objects"),
						vecGeometries.data(),
						vecGeometries.size());
				}
			} // if (pMeshProperty != nullptr)
			else {
				auto pCameraProperty = pNodeObject->getProperty("camera");
				if (pCameraProperty != nullptr) {
					getLog()->logWrite(enumLogEvent::warning, "Property 'camera' is not supported.");
				}
			}
		} // else if (pChildrenProperty != nullptr)

		auto pMatrixProperty = pNodeObject->getProperty("matrix");
		if (pMatrixProperty != nullptr) {
			auto pMatrixValue = pMatrixProperty->getValue()->as<_json::_array>();
			VERIFY_POINTER(pMatrixValue);
			VERIFY_EXPRESSION(pMatrixValue->getValues().size() == 16);

			vector<double> vecMatrix;
			// Column I
			vecMatrix.push_back(atof(pMatrixValue->getValues()[0]->as<_json::_simple>()->getValue().c_str()));
			vecMatrix.push_back(atof(pMatrixValue->getValues()[1]->as<_json::_simple>()->getValue().c_str()));
			vecMatrix.push_back(atof(pMatrixValue->getValues()[2]->as<_json::_simple>()->getValue().c_str()));

			// Column II
			vecMatrix.push_back(atof(pMatrixValue->getValues()[4]->as<_json::_simple>()->getValue().c_str()));
			vecMatrix.push_back(atof(pMatrixValue->getValues()[5]->as<_json::_simple>()->getValue().c_str()));
			vecMatrix.push_back(atof(pMatrixValue->getValues()[6]->as<_json::_simple>()->getValue().c_str()));

			// Column III
			vecMatrix.push_back(atof(pMatrixValue->getValues()[8]->as<_json::_simple>()->getValue().c_str()));
			vecMatrix.push_back(atof(pMatrixValue->getValues()[9]->as<_json::_simple>()->getValue().c_str()));
			vecMatrix.push_back(atof(pMatrixValue->getValues()[10]->as<_json::_simple>()->getValue().c_str()));

			// Column IV
			vecMatrix.push_back(atof(pMatrixValue->getValues()[12]->as<_json::_simple>()->getValue().c_str()));
			vecMatrix.push_back(atof(pMatrixValue->getValues()[13]->as<_json::_simple>()->getValue().c_str()));
			vecMatrix.push_back(atof(pMatrixValue->getValues()[14]->as<_json::_simple>()->getValue().c_str()));

			OwlInstance iTransformationInstance = createTransformation(
				"Transformation",
				vecMatrix);
			VERIFY_INSTANCE(iTransformationInstance);

			SetObjectProperty(
				iTransformationInstance,
				GetPropertyByName(getModel(), "object"),
				&iNodeInstance,
				1);

			/* Cache */
			m_mapNodes[iNodeIndex] = iTransformationInstance;

			return iTransformationInstance;
		} // if (pMatrixProperty != nullptr)

		return iNodeInstance;
	}

	/*virtual*/ void _gltf_importer_t::createNodeGeometry(const _json::_simple* pMeshIndex, vector<OwlInstance>& vecGeometries)
	{
		VERIFY_POINTER(pMeshIndex);
		VERIFY_STLOBJ_IS_EMPTY(vecGeometries);

		int iMeshIndex = atoi(pMeshIndex->getValue().c_str());

		/* Cache */
		if (m_mapMeshes.find(iMeshIndex) != m_mapMeshes.end()) {
			vecGeometries = m_mapMeshes.at(iMeshIndex);

			return;
		}

		auto pMeshObject = getMesh(iMeshIndex);
		VERIFY_POINTER(pMeshObject);

		auto pPrimitivesProperty = pMeshObject->getProperty("primitives");
		VERIFY_POINTER(pPrimitivesProperty);

		auto pPrimitivesValue = pPrimitivesProperty->getValue()->as<_json::_array>();
		VERIFY_POINTER(pPrimitivesValue);

		for (size_t iIndex = 0; iIndex < pPrimitivesValue->getValues().size(); iIndex++) {
			auto pMeshPropertiesObject = pPrimitivesValue->getValues()[iIndex]->as<_json::_object>();
			VERIFY_POINTER(pMeshPropertiesObject);

			auto pAttributesObject = pMeshPropertiesObject->getPropertyValueAs<_json::_object>("attributes");
			VERIFY_POINTER(pAttributesObject);

			auto pPositionAccessorIndexValue = pAttributesObject->getPropertyValueAs<_json::_simple>("POSITION");
			VERIFY_POINTER(pPositionAccessorIndexValue);

			int iPositionAccessorIndex = atoi(pPositionAccessorIndexValue->getValue().c_str());

			int iNormalAccessorIndex = -1;
			auto pNormalProperty = pAttributesObject->getProperty("NORMAL");
			if (pNormalProperty != nullptr) {
				auto pNormalAccessorIndexValue = pNormalProperty->getValue()->as<_json::_simple>();
				VERIFY_POINTER(pNormalAccessorIndexValue);

				iNormalAccessorIndex = atoi(pNormalAccessorIndexValue->getValue().c_str());
			}

			int iTexcoord0AccessorIndex = -1;
			auto pTexcoord0Property = pAttributesObject->getProperty("TEXCOORD_0");
			if (pTexcoord0Property != nullptr) {
				auto pTexcoord0AccessorIndexValue = pTexcoord0Property->getValue()->as<_json::_simple>();
				VERIFY_POINTER(pTexcoord0AccessorIndexValue);

				iTexcoord0AccessorIndex = atoi(pTexcoord0AccessorIndexValue->getValue().c_str());
			}

			auto pIndicesAccessorIndexValue = pMeshPropertiesObject->getPropertyValueAs<_json::_simple>("indices");
			if (pIndicesAccessorIndexValue == nullptr) {
				getLog()->logWrite(enumLogEvent::error, "Invalid format: 'indices' property is not found in mesh primitive attributes.");
				continue;
			}

			int iIndicesAccessorIndex = atoi(pIndicesAccessorIndexValue->getValue().c_str());

			int iMaterialIndex = -1;
			auto pMaterialIndexValue = pMeshPropertiesObject->getPropertyValueAs<_json::_simple>("material");
			if (pMaterialIndexValue != nullptr) {
				iMaterialIndex = atoi(pMaterialIndexValue->getValue().c_str());
			}

			int iMode = 4;
			auto pModeValue = pMeshPropertiesObject->getPropertyValueAs<_json::_simple>("mode");
			if (pModeValue != nullptr) {
				iMode = atoi(pModeValue->getValue().c_str());
			}

			if (iMode == 0/*POINTS*/) {
				OwlInstance iInstance = createPoints(
					iPositionAccessorIndex,
					iIndicesAccessorIndex,
					iMaterialIndex);
				VERIFY_INSTANCE(iInstance);

				vecGeometries.push_back(iInstance);
			} else if (iMode == 1/*LINES*/) {
				OwlInstance iInstance = createLines(
					iPositionAccessorIndex,
					iIndicesAccessorIndex,
					iMaterialIndex);

				VERIFY_INSTANCE(iInstance);

				vecGeometries.push_back(iInstance);
			} else if (iMode == 4/*TRIANGLES*/) {
				OwlInstance iInstance = createMesh(
					iPositionAccessorIndex,
					iNormalAccessorIndex,
					iTexcoord0AccessorIndex,
					iIndicesAccessorIndex,
					iMaterialIndex);

				VERIFY_INSTANCE(iInstance);

				vecGeometries.push_back(iInstance);
			} else {
				getLog()->logWrite(enumLogEvent::error, "TODO.");
			}
		} // for (size_t iIndex = ...

		/* Cache */
		m_mapMeshes[iMeshIndex] = vecGeometries;
	}

	/*virtual*/ OwlInstance _gltf_importer_t::createPoints(
		int iPositionAccessorIndex,
		int iIndicesAccessorIndex,
		int iMaterialIndex)
	{
		vector<double> vecVertices;
		vector<float> vecVerticesF;

		// attributes/POSITION accessor	
		{
			readVertices(iPositionAccessorIndex, "VEC3", vecVerticesF);
			VERIFY_STLOBJ_IS_NOT_EMPTY(vecVerticesF);
		}
		// attributes/POSITION accessor

		// indices accessor			
		{
			vector<unsigned int> vecIndicesUI;
			readIndices(iIndicesAccessorIndex, vecIndicesUI);
			VERIFY_STLOBJ_IS_NOT_EMPTY(vecIndicesUI);

			for (size_t i = 0; i < vecIndicesUI.size(); i++) {
				vecVertices.push_back(vecVerticesF[(vecIndicesUI[i] * 3) + 0]);
				vecVertices.push_back(vecVerticesF[(vecIndicesUI[i] * 3) + 1]);
				vecVertices.push_back(vecVerticesF[(vecIndicesUI[i] * 3) + 2]);
			}
		}
		// indices accessor

		OwlInstance iInstance = createPoint3DSetDInstance(
			"Points",
			vecVertices,
			createMaterialInstance(iMaterialIndex));

		return iInstance;
	}

	/*virtual*/ OwlInstance _gltf_importer_t::createLines(
		int iPositionAccessorIndex,
		int iIndicesAccessorIndex,
		int iMaterialIndex)
	{
		vector<double> vecVertices;
		vector<float> vecVerticesF;

		// attributes/POSITION accessor			
		{
			readVertices(iPositionAccessorIndex, "VEC3", vecVerticesF);
			VERIFY_STLOBJ_IS_NOT_EMPTY(vecVerticesF);
		}
		// attributes/POSITION accessor

		// indices accessor			
		{
			vector<unsigned int> vecIndicesUI;
			readIndices(iIndicesAccessorIndex, vecIndicesUI);
			VERIFY_STLOBJ_IS_NOT_EMPTY(vecIndicesUI);

			for (size_t i = 0; i < vecIndicesUI.size(); i++) {
				vecVertices.push_back(vecVerticesF[(vecIndicesUI[i] * 3) + 0]);
				vecVertices.push_back(vecVerticesF[(vecIndicesUI[i] * 3) + 1]);
				vecVertices.push_back(vecVerticesF[(vecIndicesUI[i] * 3) + 2]);
			}
		}
		// indices accessor

		OwlInstance iInstance = createLine3DSetInstance(
			"Lines",
			vecVertices,
			createMaterialInstance(iMaterialIndex));

		return iInstance;
	}

	/*virtual*/ OwlInstance _gltf_importer_t::createMesh(
		int iPositionAccessorIndex,
		int iNormalAccessorIndex,
		int iTexcoord0AccessorIndex,
		int iIndicesAccessorIndex,
		int iMaterialIndex)
	{
		vector<double> vecVertices;
		vector<double> vecNormals;
		vector<double> vecUVs;
		vector<int64_t> vecIndices;

		// attributes/POSITION accessor			
		{
			vector<float> vecVerticesF;
			readVertices(iPositionAccessorIndex, "VEC3", vecVerticesF);
			VERIFY_STLOBJ_IS_NOT_EMPTY(vecVerticesF);

			convertType<float, double>(vecVerticesF, vecVertices);
		}
		// attributes/POSITION accessor

		// attributes/NORMAL accessor
		if (iNormalAccessorIndex != -1) {
			vector<float> vecNormalsF;
			readVertices(iNormalAccessorIndex, "VEC3", vecNormalsF);
			VERIFY_STLOBJ_IS_NOT_EMPTY(vecNormalsF);

			convertType<float, double>(vecNormalsF, vecNormals);
		}
		// attributes/NORMAL accessor

		// attributes/TEXCOORD_0 accessor
		if (iTexcoord0AccessorIndex != -1) {
			vector<float> vecUVsF;
			readVertices(iTexcoord0AccessorIndex, "VEC2", vecUVsF);
			VERIFY_STLOBJ_IS_NOT_EMPTY(vecUVsF);

			convertType<float, double>(vecUVsF, vecUVs);
			textureFlipY<double>(vecUVs);
		}
		// attributes/TEXCOORD_0 accessor

		// indices accessor			
		{
			vector<unsigned int> vecIndicesUI;
			readIndices(iIndicesAccessorIndex, vecIndicesUI);
			VERIFY_STLOBJ_IS_NOT_EMPTY(vecIndicesUI);

			for (size_t i = 0; i < vecIndicesUI.size(); i += 3) {
				vecIndices.push_back(vecIndicesUI[i + 0]);
				vecIndices.push_back(vecIndicesUI[i + 1]);
				vecIndices.push_back(vecIndicesUI[i + 2]);

				vecIndices.push_back(-1/*outer polygon*/);
			}
		}
		// indices accessor

		OwlInstance iInstance = createBoundaryRepresentationInstance(
			"Mesh",
			vecIndices,
			vecVertices,
			createMaterialInstance(iMaterialIndex));

		if (!vecNormals.empty()) {
			SetDatatypeProperty(
				iInstance,
				GetPropertyByName(getModel(), "normalCoordinates"),
				vecNormals.data(),
				vecNormals.size());
		}

		if (!vecUVs.empty()) {
			SetDatatypeProperty(
				iInstance,
				GetPropertyByName(getModel(), "textureCoordinates"),
				vecUVs.data(),
				vecUVs.size());
		}

		return iInstance;
	}

	const _json::_object* _gltf_importer_t::getNode(int iIndex)
	{
		assert(m_pNodesProperty != nullptr);

		auto pNodes = m_pNodesProperty->getValue()->as<_json::_array>();
		VERIFY_POINTER(pNodes);

		assert((iIndex >= 0) && (iIndex < (int)pNodes->getValues().size()));

		auto pNode = pNodes->getValues()[iIndex];
		VERIFY_POINTER(pNode);

		return pNode->as<_json::_object>();
	}

	const _json::_object* _gltf_importer_t::getMesh(int iIndex)
	{
		assert(m_pMeshesProperty != nullptr);

		auto pMeshes = m_pMeshesProperty->getValue()->as<_json::_array>();
		VERIFY_POINTER(pMeshes);

		assert((iIndex >= 0) && (iIndex < (int)pMeshes->getValues().size()));

		auto pMesh = pMeshes->getValues()[iIndex];
		VERIFY_POINTER(pMesh);

		return pMesh->as<_json::_object>();
	}

	const _json::_object* _gltf_importer_t::getAccessor(int iIndex)
	{
		assert(m_pAccessorsProperty != nullptr);

		auto pAccessors = m_pAccessorsProperty->getValue()->as<_json::_array>();
		VERIFY_POINTER(pAccessors);

		assert((iIndex >= 0) && (iIndex < (int)pAccessors->getValues().size()));

		auto pAccessor = pAccessors->getValues()[iIndex];
		VERIFY_POINTER(pAccessor);

		return pAccessor->as<_json::_object>();
	}

	const _json::_object* _gltf_importer_t::getBufferView(int iIndex)
	{
		assert(m_pBufferViewsProperty != nullptr);

		auto pBufferViews = m_pBufferViewsProperty->getValue()->as<_json::_array>();
		VERIFY_POINTER(pBufferViews);

		assert((iIndex >= 0) && (iIndex < (int)pBufferViews->getValues().size()));

		auto pBufferView = pBufferViews->getValues()[iIndex];
		VERIFY_POINTER(pBufferView);

		return pBufferView->as<_json::_object>();
	}

	const _json::_object* _gltf_importer_t::getBuffer(int iIndex)
	{
		assert(m_pBuffersProperty != nullptr);

		auto pBuffers = m_pBuffersProperty->getValue()->as<_json::_array>();
		VERIFY_POINTER(pBuffers);

		assert((iIndex >= 0) && (iIndex < (int)pBuffers->getValues().size()));

		auto pBuffer = pBuffers->getValues()[iIndex];
		VERIFY_POINTER(pBuffer);

		return pBuffer->as<_json::_object>();
	}

	const _json::_object* _gltf_importer_t::getMaterial(int iIndex)
	{
		assert(m_pMaterialsProperty != nullptr);

		auto pMaterials = m_pMaterialsProperty->getValue()->as<_json::_array>();
		VERIFY_POINTER(pMaterials);

		assert((iIndex >= 0) && (iIndex < (int)pMaterials->getValues().size()));

		auto pMaterial = pMaterials->getValues()[iIndex];
		VERIFY_POINTER(pMaterial);

		return pMaterial->as<_json::_object>();
	}

	const _json::_object* _gltf_importer_t::getTexture(int iIndex)
	{
		assert(m_pTexturesProperty != nullptr);

		auto pTextures = m_pTexturesProperty->getValue()->as<_json::_array>();
		VERIFY_POINTER(pTextures);

		assert((iIndex >= 0) && (iIndex < (int)pTextures->getValues().size()));

		auto pTexture = pTextures->getValues()[iIndex];
		VERIFY_POINTER(pTexture);

		return pTexture->as<_json::_object>();
	}

	const _json::_object* _gltf_importer_t::getImage(int iIndex)
	{
		assert(m_pImagesProperty != nullptr);

		auto pImages = m_pImagesProperty->getValue()->as<_json::_array>();
		VERIFY_POINTER(pImages);

		assert((iIndex >= 0) && (iIndex < (int)pImages->getValues().size()));

		auto pImage = pImages->getValues()[iIndex];
		VERIFY_POINTER(pImage);

		return pImage->as<_json::_object>();
	}

	/*virtual*/ void _gltf_importer_t::createStringProperty(OwlInstance iInstance, const string& strProperty, const vector<const _json::_value*>& vecValues)
	{
		VERIFY_INSTANCE(iInstance);
		VERIFY_STLOBJ_IS_NOT_EMPTY(strProperty);
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecValues);

		vector<char*> vecStrValues;
		for (auto pValue : vecValues) {
			auto pSimpleValue = pValue->as<_json::_simple>();
			VERIFY_POINTER(pSimpleValue);

			char* szValue = new char[pSimpleValue->getValue().size() + 1];
			strcpy(szValue, pSimpleValue->getValue().c_str());
			vecStrValues.push_back(szValue);
		} // for (auto pChild ...

		setDataProperty(
			iInstance,
			strProperty,
			DATATYPEPROPERTY_TYPE_STRING,
			vecStrValues.data(),
			vecStrValues.size());

		for (size_t iValue = 0; iValue < vecStrValues.size(); iValue++) {
			delete[] vecStrValues[iValue];
		}
	}

	/*virtual*/ void _gltf_importer_t::createIntProperty(OwlInstance iInstance, const string& strProperty, const vector<const _json::_value*>& vecValues)
	{
		VERIFY_INSTANCE(iInstance);
		VERIFY_STLOBJ_IS_NOT_EMPTY(strProperty);
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecValues);

		vector<int64_t> vecIntValues;
		for (auto pValue : vecValues) {
			auto pSimpleValue = pValue->as<_json::_simple>();
			VERIFY_POINTER(pSimpleValue);

			vecIntValues.push_back(_atoi64(pSimpleValue->getValue().c_str()));
		}

		setDataProperty(
			iInstance,
			strProperty,
			DATATYPEPROPERTY_TYPE_INTEGER,
			vecIntValues.data(),
			vecIntValues.size());
	}

	/*virtual*/ void _gltf_importer_t::createDoubleProperty(OwlInstance iInstance, const string& strProperty, const vector<const _json::_value*>& vecValues)
	{
		VERIFY_INSTANCE(iInstance);
		VERIFY_STLOBJ_IS_NOT_EMPTY(strProperty);
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecValues);

		vector<double> vecDoubleValues;
		for (auto pValue : vecValues) {
			auto pSimpleValue = pValue->as<_json::_simple>();
			VERIFY_POINTER(pSimpleValue);

			vecDoubleValues.push_back(atof(pSimpleValue->getValue().c_str()));
		}

		setDataProperty(
			iInstance,
			strProperty,
			DATATYPEPROPERTY_TYPE_DOUBLE,
			vecDoubleValues.data(),
			vecDoubleValues.size());
	}

	/*virtual*/ void _gltf_importer_t::createDoubleArrayProperty(OwlInstance iInstance, const string& strProperty, const _json::_array* pValues)
	{
		VERIFY_INSTANCE(iInstance);
		VERIFY_STLOBJ_IS_NOT_EMPTY(strProperty);
		VERIFY_POINTER(pValues);

		vector<double> vecDoubleValues;
		for (auto pValue : pValues->getValues()) {
			auto pSimpleValue = pValue->as<_json::_simple>();
			VERIFY_POINTER(pSimpleValue);

			vecDoubleValues.push_back(atof(pSimpleValue->getValue().c_str()));
		}

		setDataProperty(
			iInstance,
			strProperty,
			DATATYPEPROPERTY_TYPE_DOUBLE,
			vecDoubleValues.data(),
			vecDoubleValues.size());
	}

	/*virtual*/ OwlInstance _gltf_importer_t::createMaterialInstance(int iMaterialIndex)
	{
		if (iMaterialIndex < 0) {
			return getDefaultMaterialInstance();
		}

		/* Cache */
		if (m_mapMaterials.find(iMaterialIndex) != m_mapMaterials.end()) {
			return m_mapMaterials.at(iMaterialIndex);
		}

		auto pMaterialObject = getMaterial(iMaterialIndex);
		VERIFY_POINTER(pMaterialObject);

		string strMaterialName = "Material";
		auto pNameProperty = pMaterialObject->getPropertyValueAs<_json::_simple>("name");
		if (pNameProperty) {
			strMaterialName = pNameProperty->getValue();
		}

		int iTextureIndex = -1;
		const _json::_array* pBaseColorFactorProperty = nullptr;
		const _json::_array* pEmissiveFactorProperty = nullptr;

		auto pMetallicRoughnessProperty = pMaterialObject->getPropertyValueAs<_json::_object>("pbrMetallicRoughness");
		if (pMetallicRoughnessProperty != nullptr) {
			auto pBaseColorTextureProperty = pMetallicRoughnessProperty->getPropertyValueAs<_json::_object>("baseColorTexture");
			if (pBaseColorTextureProperty != nullptr) {
				auto pIndexValue = pBaseColorTextureProperty->getPropertyValueAs<_json::_simple>("index");
				VERIFY_POINTER(pIndexValue);

				iTextureIndex = atoi(pIndexValue->getValue().c_str());
			}

			if (iTextureIndex == -1) {
				pBaseColorFactorProperty = pMetallicRoughnessProperty->getPropertyValueAs<_json::_array>("baseColorFactor");
				if (pBaseColorFactorProperty != nullptr) {
					assert(pBaseColorFactorProperty->getValues().size() == 4);
				}

				pEmissiveFactorProperty = pMaterialObject->getPropertyValueAs<_json::_array>("emissiveFactor");
			} // pbrMetallicRoughness
		} else {
			getLog()->logWrite(enumLogEvent::warning, "'baseColorTexture' property is not found.");

			pEmissiveFactorProperty = pMaterialObject->getPropertyValueAs<_json::_array>("emissiveFactor");
		}  // else pbrMetallicRoughness	

		OwlInstance iMaterialInstance = CreateInstance(GetClassByName(getModel(), "Material"));
		VERIFY_INSTANCE(iMaterialInstance);

		setTag(iMaterialInstance, strMaterialName);

		OwlInstance iColorInstance = CreateInstance(GetClassByName(getModel(), "Color"));
		VERIFY_INSTANCE(iColorInstance);

		setTag(iColorInstance, "Color");

		SetObjectProperty(
			iMaterialInstance,
			GetPropertyByName(getModel(), "color"),
			&iColorInstance,
			1);

		if (pBaseColorFactorProperty != nullptr) {
			OwlInstance iColorComponentInstance = createColorComponentInstance(
				"ColorComponent",
				atof(pBaseColorFactorProperty->getValues()[0]->as<_json::_simple>()->getValue().c_str()),
				atof(pBaseColorFactorProperty->getValues()[1]->as<_json::_simple>()->getValue().c_str()),
				atof(pBaseColorFactorProperty->getValues()[2]->as<_json::_simple>()->getValue().c_str()));
			VERIFY_INSTANCE(iColorComponentInstance);

			SetObjectProperty(
				iColorInstance,
				GetPropertyByName(getModel(), "ambient"),
				&iColorComponentInstance,
				1);

			SetObjectProperty(
				iColorInstance,
				GetPropertyByName(getModel(), "diffuse"),
				&iColorComponentInstance,
				1);

			double dTransparency = atof(pBaseColorFactorProperty->getValues()[3]->as<_json::_simple>()->getValue().c_str());

			SetDatatypeProperty(
				iColorInstance,
				GetPropertyByName(getModel(), "transparency"),
				&dTransparency,
				1);
		} // if (pBaseColorFactorProperty != nullptr)

		if (iTextureIndex != -1) {
			OwlInstance iTextureInstance = 0;
			if (m_mapTextures.find(iTextureIndex) != m_mapTextures.end()) {
				iTextureInstance = m_mapTextures.at(iTextureIndex);
			}

			if (iTextureInstance == 0) {
				iTextureInstance = CreateInstance(GetClassByName(getModel(), "Texture"));
				VERIFY_INSTANCE(iTextureInstance);

				m_mapTextures[iTextureIndex] = iTextureInstance;

				auto pTextureObject = getTexture(iTextureIndex);
				VERIFY_POINTER(pTextureObject);

				auto pSourceValue = pTextureObject->getPropertyValueAs<_json::_simple>("source");
				VERIFY_POINTER(pSourceValue);

				int iImageIndex = atoi(pSourceValue->getValue().c_str());
				auto pImageObject = getImage(iImageIndex);
				VERIFY_POINTER(pImageObject);

				// uri
				string strURI;
				auto pURIValue = pImageObject->getPropertyValueAs<_json::_simple>("uri");
				if (pURIValue == nullptr) {
					strURI = (LPCSTR)CW2A(generateGuid().c_str());

					auto pMimeTypeValue = pImageObject->getPropertyValueAs<_json::_simple>("mimeType");
					VERIFY_POINTER(pMimeTypeValue);

					// Determine file extension based on MIME type
					if (pMimeTypeValue->getValue() == "image/jpeg") {
						strURI += ".jpg";
					} else if (pMimeTypeValue->getValue() == "image/png") {
						strURI += ".png";
					} else if (pMimeTypeValue->getValue() == "image/webp") {
						strURI += ".webp";
					} else {
						strURI += ".bin";
					}

					int iTextureOffset = (int)getBINDataOffset();
					int iTextureByteLength = 0;

					auto pBufferViewValue = pImageObject->getPropertyValueAs<_json::_simple>("bufferView");
					VERIFY_POINTER(pBufferViewValue);

					int iBufferViewIndex = atoi(pBufferViewValue->getValue().c_str());
					auto pBufferViewObject = getBufferView(iBufferViewIndex);
					VERIFY_POINTER(pBufferViewObject);

					// byteOffset
					auto pAccessorByteOffsetValue = pImageObject->getPropertyValueAs<_json::_simple>("byteOffset");
					if (pAccessorByteOffsetValue != nullptr) {
						iTextureOffset += atoi(pAccessorByteOffsetValue->getValue().c_str());
					}

					// byteLength
					auto pByteLengthValue = pBufferViewObject->getPropertyValueAs<_json::_simple>("byteLength");
					VERIFY_POINTER(pByteLengthValue);

					iTextureByteLength = atoi(pByteLengthValue->getValue().c_str());

					// byteOffset
					auto pBufferViewByteOffsetValue = pBufferViewObject->getPropertyValueAs<_json::_simple>("byteOffset");
					if (pBufferViewByteOffsetValue != nullptr) {
						iTextureOffset += atoi(pBufferViewByteOffsetValue->getValue().c_str());
					}								

					// Save the texture to a file
					fs::path pathTextureFile = fs::path(m_strOutputFolder) / fs::path(strURI);
					saveTextureToFile(pathTextureFile.string(), iTextureOffset, iTextureByteLength);
				} else {
					strURI = pURIValue->getValue();
				}

				// tag
				setTag(iTextureInstance, strURI);

				// name
				char* szValue = new char[strURI.size() + 1];
				strcpy(szValue, strURI.c_str());

				SetDatatypeProperty(
					iTextureInstance,
					GetPropertyByName(getModel(), "name"),
					&szValue,
					1);

				delete[] szValue;
			} // if (iTextureInstance == 0)

			SetObjectProperty(
				iMaterialInstance,
				GetPropertyByName(getModel(), "textures"),
				&iTextureInstance,
				1);
		} // if (iTextureIndex != -1)

		if (pEmissiveFactorProperty != nullptr) {
			assert(pEmissiveFactorProperty->getValues().size() == 3);

			OwlInstance iColorComponentInstance = createColorComponentInstance(
				"ColorComponent",
				atof(pEmissiveFactorProperty->getValues()[0]->as<_json::_simple>()->getValue().c_str()),
				atof(pEmissiveFactorProperty->getValues()[1]->as<_json::_simple>()->getValue().c_str()),
				atof(pEmissiveFactorProperty->getValues()[2]->as<_json::_simple>()->getValue().c_str()));
			VERIFY_INSTANCE(iColorComponentInstance);

			SetObjectProperty(
				iColorInstance,
				GetPropertyByName(getModel(), "emissive"),
				&iColorComponentInstance,
				1);

			SetObjectProperty(
				iColorInstance,
				GetPropertyByName(getModel(), "specular"),
				&iColorComponentInstance,
				1);
		} // if (pEmissiveFactorProperty != nullptr)

		/* Cache */
		m_mapMaterials[iMaterialIndex] = iMaterialInstance;

		return iMaterialInstance;
	}

	void _gltf_importer_t::saveTextureToFile(const string& strFile, int iOffset, int iByteLength)
	{
		// Read texture data from the GLB file
		vector<uint8_t> vecTextureData;
		readBuffer<uint8_t>(getURI(nullptr), iOffset, iByteLength, vecTextureData);		

		// Write the texture data to the file
		std::ofstream outFile(strFile, std::ios::binary);
		if (!outFile.is_open()) {
			getLog()->logWrite(enumLogEvent::error, "Failed to create texture file: " + strFile);
			return;
		}

		outFile.write(reinterpret_cast<const char*>(vecTextureData.data()), vecTextureData.size());
		outFile.close();

		getLog()->logWrite(enumLogEvent::info, "Saved texture to file: " + strFile);
	}

	/*virtual*/ string _gltf_importer_t::getURI(const _json::_object* pBufferObject) const
	{
		VERIFY_POINTER(pBufferObject);

		auto pURIValue = pBufferObject->getPropertyValueAs<_json::_simple>("uri");
		if (pURIValue != nullptr) {
			return pURIValue->getValue();
		}

		return "";
	}

	/*virtual*/ streampos _gltf_importer_t::getBINDataOffset() const
	{
		return 0;
	}

	void _gltf_importer_t::readIndices(int iIndicesAccessorIndex, vector<unsigned int>& vecIndices)
	{
		VERIFY_EXPRESSION(iIndicesAccessorIndex >= 0);
		VERIFY_STLOBJ_IS_EMPTY(vecIndices);

		auto pIndicesAccessorObject = getAccessor(iIndicesAccessorIndex);
		VERIFY_POINTER(pIndicesAccessorObject);

		// bufferView
		int iIndicesOffset = (int)getBINDataOffset();
		int iIndicesByteLength = 0;
		string strURI;
		{
			auto pBufferViewValue = pIndicesAccessorObject->getPropertyValueAs<_json::_simple>("bufferView");
			VERIFY_POINTER(pBufferViewValue);

			int iBufferViewIndex = atoi(pBufferViewValue->getValue().c_str());
			auto pBufferViewObject = getBufferView(iBufferViewIndex);
			VERIFY_POINTER(pBufferViewObject);

			// byteOffset
			auto pAccessorByteOffsetValue = pIndicesAccessorObject->getPropertyValueAs<_json::_simple>("byteOffset");
			if (pAccessorByteOffsetValue != nullptr) {
				iIndicesOffset += atoi(pAccessorByteOffsetValue->getValue().c_str());
			}

			// buffer
			{
				auto pBufferValue = pBufferViewObject->getPropertyValueAs<_json::_simple>("buffer");
				VERIFY_POINTER(pBufferValue);

				int iBufferIndex = atoi(pBufferValue->getValue().c_str());
				auto pBufferObject = getBuffer(iBufferIndex);
				VERIFY_POINTER(pBufferObject);

				// byteLength
				/*auto pByteLengthValue = pBufferObject->getPropertyValueAs<_json::_simple>("byteLength");
				VERIFY_POINTER(pByteLengthValue);

				int iByteLength = atoi(pByteLengthValue->getValue().c_str());*/

				// uri
				strURI = getURI(pBufferObject);
			}
			// buffer

			// byteLength
			auto pByteLengthValue = pBufferViewObject->getPropertyValueAs<_json::_simple>("byteLength");
			VERIFY_POINTER(pByteLengthValue);

			iIndicesByteLength = atoi(pByteLengthValue->getValue().c_str());

			// byteOffset
			auto pBufferViewByteOffsetValue = pBufferViewObject->getPropertyValueAs<_json::_simple>("byteOffset");
			if (pBufferViewByteOffsetValue != nullptr) {
				iIndicesOffset += atoi(pBufferViewByteOffsetValue->getValue().c_str());
			}			

			// target
			auto pTargetValue = pBufferViewObject->getPropertyValueAs<_json::_simple>("target");
			if (pTargetValue != nullptr) {
				int iTarget = atoi(pTargetValue->getValue().c_str());
				VERIFY_EXPRESSION(iTarget == 34963/*ELEMENT_ARRAY_BUFFER*/);
			}
		}
		// bufferView		

		// count
		auto pCountValue = pIndicesAccessorObject->getPropertyValueAs<_json::_simple>("count");
		VERIFY_POINTER(pCountValue);

		int iCount = atoi(pCountValue->getValue().c_str());
		VERIFY_EXPRESSION(iCount > 0);

		// componentType
		auto pComponentTypeValue = pIndicesAccessorObject->getPropertyValueAs<_json::_simple>("componentType");
		VERIFY_POINTER(pComponentTypeValue);

		int iComponentType = atoi(pComponentTypeValue->getValue().c_str());
		assert((iComponentType == 5121/*UNSIGNED_BYTE*/) ||
			(iComponentType == 5122/*SHORT*/) ||
			(iComponentType == 5123/*UNSIGNED_SHORT*/) ||
			(iComponentType == 5125/*UNSIGNED_INT*/));

		// type
		auto pTypeValue = pIndicesAccessorObject->getPropertyValueAs<_json::_simple>("type");
		VERIFY_POINTER(pTypeValue);
		assert(pTypeValue->getValue() == "SCALAR");

		if ((iComponentType == 5121/*UNSIGNED_BYTE*/)) {
			vector<BYTE> vecIndicesUS;
			readBuffer<BYTE>(
				strURI.c_str(),
				iIndicesOffset,
				(uint64_t)iCount * 1 * sizeof(BYTE),
				vecIndicesUS);

			convertType<BYTE, unsigned int>(vecIndicesUS, vecIndices);
		} else if ((iComponentType == 5122/*SHORT*/)) {
			vector<short> vecIndicesUS;
			readBuffer<short>(
				strURI.c_str(),
				iIndicesOffset,
				(uint64_t)iCount * 1 * sizeof(short),
				vecIndicesUS);

			convertType<short, unsigned int>(vecIndicesUS, vecIndices);
		} else if ((iComponentType == 5123/*UNSIGNED_SHORT*/)) {
			vector<unsigned short> vecIndicesUS;
			readBuffer<unsigned short>(
				strURI.c_str(),
				iIndicesOffset,
				(uint64_t)iCount * 1 * sizeof(unsigned short),
				vecIndicesUS);

			convertType<unsigned short, unsigned int>(vecIndicesUS, vecIndices);
		} else if (iComponentType == 5125/*UNSIGNED_INT*/) {
			readBuffer<unsigned int>(
				strURI.c_str(),
				iIndicesOffset,
				(uint64_t)iCount * 1 * sizeof(unsigned int),
				vecIndices);
		}
	}

	void _gltf_importer_t::readVertices(int iPositionAccessorIndex, const string& strType, vector<float>& vecVertices)
	{
		VERIFY_EXPRESSION(iPositionAccessorIndex >= 0);
		VERIFY_STLOBJ_IS_NOT_EMPTY(strType);
		VERIFY_STLOBJ_IS_EMPTY(vecVertices);

		auto pPositionAccessorObject = getAccessor(iPositionAccessorIndex);
		VERIFY_POINTER(pPositionAccessorObject);

		// bufferView
		int iVerticesOffset = (int)getBINDataOffset();
		int iVerticesByteLength = 0;
		string strURI;
		{
			auto pBufferViewValue = pPositionAccessorObject->getPropertyValueAs<_json::_simple>("bufferView");
			VERIFY_POINTER(pBufferViewValue);

			int iBufferViewIndex = atoi(pBufferViewValue->getValue().c_str());
			auto pBufferViewObject = getBufferView(iBufferViewIndex);
			VERIFY_POINTER(pBufferViewObject);

			// byteOffset
			auto pAccessorByteOffsetValue = pPositionAccessorObject->getPropertyValueAs<_json::_simple>("byteOffset");
			if (pAccessorByteOffsetValue != nullptr) {
				iVerticesOffset += atoi(pAccessorByteOffsetValue->getValue().c_str());
			}

			// buffer
			{
				auto pBufferValue = pBufferViewObject->getPropertyValueAs<_json::_simple>("buffer");
				VERIFY_POINTER(pBufferValue);

				int iBufferIndex = atoi(pBufferValue->getValue().c_str());
				auto pBufferObject = getBuffer(iBufferIndex);
				VERIFY_POINTER(pBufferObject);

				// byteLength
				/*auto pByteLengthValue = pBufferObject->getPropertyValueAs<_json::_simple>("byteLength");
				VERIFY_POINTER(pByteLengthValue);

				int iByteLength = atoi(pByteLengthValue->getValue().c_str());*/

				// uri
				strURI = getURI(pBufferObject);
			}
			// buffer

			// byteLength
			auto pByteLengthValue = pBufferViewObject->getPropertyValueAs<_json::_simple>("byteLength");
			VERIFY_POINTER(pByteLengthValue);

			iVerticesByteLength = atoi(pByteLengthValue->getValue().c_str());

			// byteOffset
			auto pBufferViewByteOffsetValue = pBufferViewObject->getPropertyValueAs<_json::_simple>("byteOffset");
			if (pBufferViewByteOffsetValue != nullptr) {
				iVerticesOffset += atoi(pBufferViewByteOffsetValue->getValue().c_str());
			}

			// target
			auto pTargetValue = pBufferViewObject->getPropertyValueAs<_json::_simple>("target");
			if (pTargetValue != nullptr) {
				int iTarget = atoi(pTargetValue->getValue().c_str());
				VERIFY_EXPRESSION(iTarget == 34962/*ARRAY_BUFFER*/);
			}
		}
		// bufferView		

		// count
		auto pCountValue = pPositionAccessorObject->getPropertyValueAs<_json::_simple>("count");
		VERIFY_POINTER(pCountValue);

		int iCount = atoi(pCountValue->getValue().c_str());
		VERIFY_EXPRESSION(iCount > 0);

		// componentType
		auto pComponentTypeValue = pPositionAccessorObject->getPropertyValueAs<_json::_simple>("componentType");
		if (pComponentTypeValue != nullptr) {
			int iComponentType = atoi(pComponentTypeValue->getValue().c_str());
			VERIFY_EXPRESSION(iComponentType == 5126/*FLOAT*/);
		}

		// type
		auto pTypeValue = pPositionAccessorObject->getPropertyValueAs<_json::_simple>("type");
		VERIFY_POINTER(pTypeValue);
		assert(pTypeValue->getValue() == strType);

		assert((strType == "VEC2") || (strType == "VEC3"));

		readBuffer<float>(
			strURI.c_str(),
			iVerticesOffset,
			(uint64_t)iCount * (strType == "VEC2" ? 2 : 3) * sizeof(float),
			vecVertices);
	}
};