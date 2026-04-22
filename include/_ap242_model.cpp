#include "_host.h"
#include "_ap242_model.h"
#include "_ap242_product_definition.h"
#include "_ap242_draughting_model.h"
#include "_ap242_instance.h"
#include "_ap242_model_structure.h"
#include "_rdf_instance.h"

// ************************************************************************************************
_ap242_model::_ap242_model(_log* pLog, bool bLoadProductRepresentationItems, bool bLoadInstancesOnDemand)
	: _ap_model(pLog, enumAP::STEP)
	, m_bLoadProductRepresentationItems(bLoadProductRepresentationItems)
	, m_bLoadInstancesOnDemand(bLoadInstancesOnDemand)
	, m_pModelStructure(nullptr)
	, m_pPropertyProvider(nullptr)
	, m_mapExpressID2Assembly()
	, m_vecDraughtingModels()
{}

/*virtual*/ _ap242_model::~_ap242_model()
{
	clean();
}

_ap242_assembly* _ap242_model::getAssemblyByInstance(SdaiInstance sdaiInstance) const
{
	assert(sdaiInstance != 0);

	ExpressID expressID = internalGetP21Line(sdaiInstance);

	// Search in cache
	auto itExpressID2Assembly = m_mapExpressID2Assembly.find(expressID);
	if (itExpressID2Assembly != m_mapExpressID2Assembly.end()) {
		return itExpressID2Assembly->second;
	}

	return nullptr;
}

/*virtual*/ _instance* _ap242_model::loadInstance(int64_t iInstance) /*override*/
{
	assert(iInstance != 0);
	SdaiInstance sdaiInstance = (SdaiInstance)iInstance;

	clean(false);

	m_bUpdateVertexBuffers = true;

	OwlInstance owlInstance = _ap_geometry::buildOwlInstance(sdaiInstance);
	if (owlInstance != 0) {
		preLoadInstance(owlInstance);
	}

	auto pGeometry = new _ap242_geometry(owlInstance, sdaiInstance);
	addGeometry(pGeometry);

	auto pInstance = new _ap242_instance(
		_model::getNextInstanceID(),
		pGeometry,
		nullptr);
	addInstance(pInstance);

	scale();

	return pInstance;
}

/*virtual*/ void _ap242_model::clean(bool bCloseModel/*= true*/) /*override*/
{
	_ap_model::clean(bCloseModel);

	if (bCloseModel) {
		delete m_pModelStructure;
		m_pModelStructure = nullptr;

		delete m_pPropertyProvider;
		m_pPropertyProvider = nullptr;
	}

	auto itExpressID2Assembly = m_mapExpressID2Assembly.begin();
	for (; itExpressID2Assembly != m_mapExpressID2Assembly.end(); itExpressID2Assembly++) {
		delete itExpressID2Assembly->second;
	}
	m_mapExpressID2Assembly.clear();

	for (auto pDraughtingModel : m_vecDraughtingModels) {
		delete pDraughtingModel;
	}
	m_vecDraughtingModels.clear();
}

/*virtual*/ void _ap242_model::attachModelCore() /*override*/
{
	if (!m_bLoadInstancesOnDemand) {
		loadProductDefinitions();
		loadAssemblies();
		loadGeometry();

		loadDraughtingModels();

		scale();
	}
}

void _ap242_model::loadProductDefinitions()
{
	SdaiAggr sdaiProductDefinitionAggr = sdaiGetEntityExtentBN(getSdaiModel(), "PRODUCT_DEFINITION");

	SdaiInteger iProductDefinitionsCount = sdaiGetMemberCount(sdaiProductDefinitionAggr);
	for (SdaiInteger i = 0; i < iProductDefinitionsCount; i++) {
		SdaiInstance sdaiProductDefinitionInstance = 0;
		sdaiGetAggrByIndex(sdaiProductDefinitionAggr, i, sdaiINSTANCE, &sdaiProductDefinitionInstance);
		assert(sdaiProductDefinitionInstance != 0);

		auto pProductDefinition = loadProductDefinition(sdaiProductDefinitionInstance);
		if (m_bLoadProductRepresentationItems) {
			loadProductDefinitionShapes(pProductDefinition);
		}
	}
}

void _ap242_model::loadProductDefinitionShapes(_ap242_product_definition* pProductDefinition)
{
	assert(pProductDefinition != nullptr);
	assert(sdaiGetInstanceType(pProductDefinition->getSdaiInstance()) == sdaiGetEntity(getSdaiModel(), "PRODUCT_DEFINITION"));

	SdaiInstance sdaiRelevantProductDefinitionShapeInstance = 0;

	SdaiAggr sdaiProductDefinitionShapeAggr = sdaiGetEntityExtentBN(getSdaiModel(), "PRODUCT_DEFINITION_SHAPE");
	SdaiInteger sdaiProductDefinitionShapeInstancesCnt = sdaiGetMemberCount(sdaiProductDefinitionShapeAggr);
	for (SdaiInteger index = 0; index < sdaiProductDefinitionShapeInstancesCnt; index++) {
		SdaiInstance sdaiProductDefinitionShapeInstance = 0;
		sdaiGetAggrByIndex(sdaiProductDefinitionShapeAggr, index, sdaiINSTANCE, &sdaiProductDefinitionShapeInstance);
		assert(sdaiProductDefinitionShapeInstance != 0);

		SdaiInstance sdaiCharacterizedDefinitionInstance = 0;
		sdaiGetAttrBN(sdaiProductDefinitionShapeInstance, "definition", sdaiINSTANCE, &sdaiCharacterizedDefinitionInstance);
		assert(sdaiCharacterizedDefinitionInstance != 0);

		if (sdaiCharacterizedDefinitionInstance == pProductDefinition->getSdaiInstance()) {
			assert(sdaiRelevantProductDefinitionShapeInstance == 0);
			sdaiRelevantProductDefinitionShapeInstance = sdaiProductDefinitionShapeInstance;
		}
	}

	assert(sdaiRelevantProductDefinitionShapeInstance);
	loadProductDefinitionShape(pProductDefinition, sdaiRelevantProductDefinitionShapeInstance);
}

void _ap242_model::loadProductDefinitionShape(_ap242_product_definition* pProductDefinition, SdaiInstance sdaiProductDefinitionShapeInstance)
{
	assert(pProductDefinition != nullptr);
	assert(sdaiProductDefinitionShapeInstance != 0);
	assert(sdaiGetInstanceType(sdaiProductDefinitionShapeInstance) == sdaiGetEntity(getSdaiModel(), "PRODUCT_DEFINITION_SHAPE"));

	auto pProductShape = new _ap242_product_shape(pProductDefinition, sdaiProductDefinitionShapeInstance);
	pProductDefinition->setProductShape(pProductShape);
	addGeometry(pProductShape);

	auto pInstance = new _ap242_instance(
		_model::getNextInstanceID(),
		pProductShape,
		nullptr);
	addInstance(pInstance);

	SdaiAggr sdaiProductShapeDefinitionRepresentationAggr = sdaiGetEntityExtentBN(getSdaiModel(), "SHAPE_DEFINITION_REPRESENTATION");
	SdaiInteger sdaiProductShapeDefinitionRepresentationInstancesCnt = sdaiGetMemberCount(sdaiProductShapeDefinitionRepresentationAggr);
	for (SdaiInteger index = 0; index < sdaiProductShapeDefinitionRepresentationInstancesCnt; index++) {
		SdaiInstance sdaiProductShapeDefinitionRepresentationInstance = 0;
		sdaiGetAggrByIndex(sdaiProductShapeDefinitionRepresentationAggr, index, sdaiINSTANCE, &sdaiProductShapeDefinitionRepresentationInstance);
		assert(sdaiProductShapeDefinitionRepresentationInstance != 0);

		SdaiInstance sdaiRepresentedDefinitionInstance = 0;
		sdaiGetAttrBN(sdaiProductShapeDefinitionRepresentationInstance, "definition", sdaiINSTANCE, &sdaiRepresentedDefinitionInstance);
		assert(sdaiRepresentedDefinitionInstance != 0);

		if (sdaiRepresentedDefinitionInstance == sdaiProductDefinitionShapeInstance) {
			SdaiInstance sdaiRepresentationInstance = 0;
			sdaiGetAttrBN(sdaiProductShapeDefinitionRepresentationInstance, "used_representation", sdaiINSTANCE, &sdaiRepresentationInstance);
			assert(sdaiRepresentationInstance != 0);

			auto pProductShapeRepresentation = new _ap242_product_shape_representation(pProductShape, sdaiRepresentationInstance);
			pProductShape->addProductShapeRepresentation(pProductShapeRepresentation);
			addGeometry(pProductShapeRepresentation);

			pInstance = new _ap242_instance(
				_model::getNextInstanceID(),
				pProductShapeRepresentation,
				nullptr);
			addInstance(pInstance);

			if (sdaiGetInstanceType(sdaiRepresentationInstance) == sdaiGetEntity(getSdaiModel(), "SHAPE_REPRESENTATION")) {
				loadShapeRepresentationItems(pProductShapeRepresentation, sdaiRepresentationInstance);
			}
			else {
				loadRepresentationItems(pProductShapeRepresentation, sdaiRepresentationInstance);
			}
		}
	}
}

void _ap242_model::loadShapeRepresentationItems(_ap242_product_shape_representation* pProductShapeRepresentation, SdaiInstance sdaiRepresentationInstance)
{
	assert(pProductShapeRepresentation != nullptr);
	assert(sdaiRepresentationInstance != 0);

	bool bAdded = false;
	
	SdaiAggr sdaiShapeRepresentationRelationshipAggr = sdaiGetEntityExtentBN(getSdaiModel(), "SHAPE_REPRESENTATION_RELATIONSHIP");
	SdaiInteger shapeRepresentationRelationshipInstancesCnt = sdaiGetMemberCount(sdaiShapeRepresentationRelationshipAggr);
	if (shapeRepresentationRelationshipInstancesCnt) {
		for (SdaiInteger index = 0; index < shapeRepresentationRelationshipInstancesCnt; index++) {
			SdaiInstance sdaiShapeRepresentationRelationshipInstance = 0;
			sdaiGetAggrByIndex(sdaiShapeRepresentationRelationshipAggr, index, sdaiINSTANCE, &sdaiShapeRepresentationRelationshipInstance);

			SdaiInstance sdaiRep_1Instance = 0;
			sdaiGetAttrBN(sdaiShapeRepresentationRelationshipInstance, "rep_1", sdaiINSTANCE, &sdaiRep_1Instance);

			SdaiInstance sdaiRep_2Instance = 0;
			sdaiGetAttrBN(sdaiShapeRepresentationRelationshipInstance, "rep_2", sdaiINSTANCE, &sdaiRep_2Instance);
			if (sdaiRep_2Instance && sdaiRep_1Instance != sdaiRep_2Instance &&
				sdaiRep_1Instance == sdaiRepresentationInstance) {
				loadRepresentationItems(pProductShapeRepresentation, sdaiRep_2Instance);
				bAdded = true;
			}

			if (sdaiRep_1Instance && sdaiRep_1Instance != sdaiRep_2Instance &&
				sdaiRep_2Instance == sdaiRepresentationInstance) {
				loadRepresentationItems(pProductShapeRepresentation, sdaiRep_1Instance);
				bAdded = true;
			}
		}
	}
	
	if (!bAdded) {
		loadRepresentationItems(pProductShapeRepresentation, sdaiRepresentationInstance);
	}
}

void _ap242_model::loadRepresentationItems(_ap242_product_shape_representation* pProductShapeRepresentation, SdaiInstance sdaiRepresentationInstance)
{
	assert(pProductShapeRepresentation != nullptr);
	assert(sdaiRepresentationInstance != 0);

	SdaiAggr sdaiRepresentationItemsAggr = nullptr;
	sdaiGetAttrBN(sdaiRepresentationInstance, "items", sdaiAGGR, &sdaiRepresentationItemsAggr);

	SdaiInteger representationItemInstancesCnt = sdaiGetMemberCount(sdaiRepresentationItemsAggr);
	for (SdaiInteger index = 0; index < representationItemInstancesCnt; index++) {
		SdaiInstance sdaiRepresentationItemInstance = 0;
		sdaiGetAggrByIndex(sdaiRepresentationItemsAggr, index, sdaiINSTANCE, &sdaiRepresentationItemInstance);
		assert(sdaiRepresentationItemInstance);

		OwlInstance owlInstance = 0;
		owlBuildInstanceInContext(sdaiRepresentationItemInstance, sdaiRepresentationInstance, &owlInstance);
		if (owlInstance) {
			if (!getGeometryByInstance(sdaiRepresentationItemInstance)) {
				auto pProductShapeRepresentationItem = new _ap242_product_shape_representation_item(pProductShapeRepresentation, owlInstance, sdaiRepresentationItemInstance);
				addGeometry(pProductShapeRepresentationItem);

				pProductShapeRepresentation->addRepresentationItem(pProductShapeRepresentationItem);
			}
		}
	}
}

_ap242_product_definition* _ap242_model::loadProductDefinition(SdaiInstance sdaiProductDefinitionInstance)
{
	OwlInstance owlInstance = 0;
	if (!m_bLoadProductRepresentationItems) {
		owlInstance = _ap_geometry::buildOwlInstance(sdaiProductDefinitionInstance);
	}
	auto pGeometry = new _ap242_product_definition(owlInstance, sdaiProductDefinitionInstance);
	addGeometry(pGeometry);

	return pGeometry;
}

_ap242_product_definition* _ap242_model::getProductDefinition(SdaiInstance sdaiProductDefinitionInstance, bool bRelatingProduct, bool bRelatedProduct)
{
	ExpressID expressID = internalGetP21Line(sdaiProductDefinitionInstance);

	auto pGeometry = getGeometryByExpressID(expressID);
	if (pGeometry != nullptr) {
		_ptr<_ap242_product_definition> apProductDefinition(pGeometry);
		if (bRelatingProduct) {
			apProductDefinition->m_iRelatingProducts++;
		}

		if (bRelatedProduct) {
			apProductDefinition->m_iRelatedProducts++;
		}

		return apProductDefinition;
	} // if (pGeometry != nullptr)

	auto pDefinition = loadProductDefinition(sdaiProductDefinitionInstance);
	if (bRelatingProduct) {
		pDefinition->m_iRelatingProducts++;
	}

	if (bRelatedProduct) {
		pDefinition->m_iRelatedProducts++;
	}

	return pDefinition;
}

void _ap242_model::loadAssemblies()
{
	SdaiAggr sdaiNextAssemblyUsageOccurrenceAggr = sdaiGetEntityExtentBN(getSdaiModel(), "NEXT_ASSEMBLY_USAGE_OCCURRENCE");

	SdaiInteger iNextAssemblyUsageOccurrencesCount = sdaiGetMemberCount(sdaiNextAssemblyUsageOccurrenceAggr);
	for (SdaiInteger i = 0; i < iNextAssemblyUsageOccurrencesCount; i++) {
		SdaiInstance sdaiNextAssemblyUsageOccurrenceInstance = 0;
		sdaiGetAggrByIndex(sdaiNextAssemblyUsageOccurrenceAggr, i, sdaiINSTANCE, &sdaiNextAssemblyUsageOccurrenceInstance);

		SdaiInstance sdaiRelatingProductDefinition = 0;
		sdaiGetAttrBN(sdaiNextAssemblyUsageOccurrenceInstance, "relating_product_definition", sdaiINSTANCE, &sdaiRelatingProductDefinition);

		auto pRelatingProductDefinition = getProductDefinition(sdaiRelatingProductDefinition, true, false);

		SdaiInstance sdaiRelatedProductDefinition = 0;
		sdaiGetAttrBN(sdaiNextAssemblyUsageOccurrenceInstance, "related_product_definition", sdaiINSTANCE, &sdaiRelatedProductDefinition);

		auto pRelatedProductDefinition = getProductDefinition(sdaiRelatedProductDefinition, false, true);

		auto pAssembly = new _ap242_assembly(sdaiNextAssemblyUsageOccurrenceInstance, pRelatingProductDefinition, pRelatedProductDefinition);
		assert(m_mapExpressID2Assembly.find(pAssembly->getExpressID()) == m_mapExpressID2Assembly.end());

		m_mapExpressID2Assembly[pAssembly->getExpressID()] = pAssembly;
	}
}

void _ap242_model::loadGeometry()
{
	for (auto pGeometry : getGeometries()) {
		_ptr<_ap242_product_definition> ap242ProductDefinition(pGeometry, false);
		if (!ap242ProductDefinition) {
			assert(m_bLoadProductRepresentationItems);
			continue;
		}

		if (ap242ProductDefinition->getRelatedProducts() == 0) {
			walkAssemblyTreeRecursively(ap242ProductDefinition, nullptr, nullptr);
		}
	}
}

void _ap242_model::walkAssemblyTreeRecursively(_ap242_product_definition* pProductDefinition, _ap242_assembly* /*pParentAssembly*/, _matrix4x3* pParentMatrix)
{
	auto itExpressID2Assembly = m_mapExpressID2Assembly.begin();
	for (; itExpressID2Assembly != m_mapExpressID2Assembly.end(); itExpressID2Assembly++) {
		auto pAssembly = itExpressID2Assembly->second;

		if (pAssembly->getRelatingProductDefinition() == pProductDefinition) {
			int64_t	owlInstanceMatrix = 0;
			owlBuildInstance(getSdaiModel(), internalGetInstanceFromP21Line(getSdaiModel(), pAssembly->getExpressID()), &owlInstanceMatrix);

			if (owlInstanceMatrix && GetInstanceClass(owlInstanceMatrix) == GetClassByName(::GetModel(owlInstanceMatrix), "Transformation")) {
				owlInstanceMatrix = _model::getInstanceObjectProperty(owlInstanceMatrix, "matrix");
			}

			assert(owlInstanceMatrix == 0 || GetInstanceClass(owlInstanceMatrix) == GetClassByName(::GetModel(owlInstanceMatrix), "Matrix") ||
				GetInstanceClass(owlInstanceMatrix) == GetClassByName(::GetModel(owlInstanceMatrix), "MatrixMultiplication"));

			_matrix4x3 matrix;
			_matrix4x3Identity(&matrix);

			if (owlInstanceMatrix) {
				InferenceInstance(owlInstanceMatrix);
				matrix._11 = _model::getInstanceDoubleProperty(owlInstanceMatrix, "_11");
				matrix._12 = _model::getInstanceDoubleProperty(owlInstanceMatrix, "_12");
				matrix._13 = _model::getInstanceDoubleProperty(owlInstanceMatrix, "_13");
				matrix._21 = _model::getInstanceDoubleProperty(owlInstanceMatrix, "_21");
				matrix._22 = _model::getInstanceDoubleProperty(owlInstanceMatrix, "_22");
				matrix._23 = _model::getInstanceDoubleProperty(owlInstanceMatrix, "_23");
				matrix._31 = _model::getInstanceDoubleProperty(owlInstanceMatrix, "_31");
				matrix._32 = _model::getInstanceDoubleProperty(owlInstanceMatrix, "_32");
				matrix._33 = _model::getInstanceDoubleProperty(owlInstanceMatrix, "_33");
				matrix._41 = _model::getInstanceDoubleProperty(owlInstanceMatrix, "_41");
				matrix._42 = _model::getInstanceDoubleProperty(owlInstanceMatrix, "_42");
				matrix._43 = _model::getInstanceDoubleProperty(owlInstanceMatrix, "_43");
			}

			if (pParentMatrix) {
				_matrix4x3Multiply(&matrix, &matrix, pParentMatrix);
			}

			walkAssemblyTreeRecursively(pAssembly->getRelatedProductDefinition(), pAssembly, &matrix);
		} // if (pAssembly->m_pRelatingProductDefinition == ...
	} // for (; itAssembly != ...

	// Create instance for current product definition
	addInstance(new _ap242_instance(
		_model::getNextInstanceID(),
		pProductDefinition,
		pParentMatrix));

	// Create instances for product shape representation items
	if (m_bLoadProductRepresentationItems) {
		for (auto pProductShapeRepresentation : pProductDefinition->getProductShape()->getProductShapeRepresentations()) {
			for (auto pRepresentationItem : pProductShapeRepresentation->getRepresentationItems()) {
				addInstance(new _ap242_product_shape_representation_item_instance(
					_model::getNextInstanceID(),
					pRepresentationItem,
					pParentMatrix));
			}
		}
	}
}

void _ap242_model::loadDraughtingModels()
{
	SdaiAggr sdaiDraughtingModelAggr = xxxxGetEntityAndSubTypesExtentBN(getSdaiModel(), "DRAUGHTING_MODEL");
	assert(sdaiDraughtingModelAggr != nullptr);

	SdaiInteger iDraughtingModelsCount = sdaiGetMemberCount(sdaiDraughtingModelAggr);
	for (SdaiInteger i = 0; i < iDraughtingModelsCount; i++) {
		SdaiInstance sdaiDraughtingModelInstance = 0;
		sdaiGetAggrByIndex(sdaiDraughtingModelAggr, i, sdaiINSTANCE, &sdaiDraughtingModelInstance);
		assert(sdaiDraughtingModelInstance != 0);

		auto pDraughtingModel = new _ap242_draughting_model(sdaiDraughtingModelInstance);
		m_vecDraughtingModels.push_back(pDraughtingModel);

		SdaiAttr sdaiItemsAttr = sdaiGetAttrDefinition(sdaiGetEntity(getSdaiModel(), "REPRESENTATION"), "items");
		assert(sdaiItemsAttr != nullptr);

		SdaiAggr sdaiItemsAggr = nullptr;
		sdaiGetAttr(sdaiDraughtingModelInstance, sdaiItemsAttr, sdaiAGGR, &sdaiItemsAggr);

		SdaiInteger iItemsCount = sdaiGetMemberCount(sdaiItemsAggr);
		for (SdaiInteger j = 0; j < iItemsCount; j++) {
			SdaiInstance sdaiItemInstance = 0;
			sdaiGetAggrByIndex(sdaiItemsAggr, j, sdaiINSTANCE, &sdaiItemInstance);

			if (sdaiGetInstanceType(sdaiItemInstance) == sdaiGetEntity(getSdaiModel(), "ANNOTATION_PLANE")) {
				auto pGeometry = getGeometryByInstance(sdaiItemInstance);
				if (pGeometry == nullptr) {
					pDraughtingModel->m_vecAnnotationPlanes.push_back(loadAnnotationPlane(sdaiItemInstance));
				}
			}
			else if (sdaiGetInstanceType(sdaiItemInstance) == sdaiGetEntity(getSdaiModel(), "DRAUGHTING_CALLOUT")) {
				auto pGeometry = getGeometryByInstance(sdaiItemInstance);
				if (pGeometry == nullptr) {
					pDraughtingModel->m_vecDraughtingCallouts.push_back(loadDraughtingCallout(sdaiItemInstance));
				}
			}
		}
	} // for (SdaiInteger i = ...
}

_ap242_annotation_plane* _ap242_model::loadAnnotationPlane(SdaiInstance sdaiInstance)
{
	assert(sdaiInstance != 0);

	OwlInstance owlInstance = _ap_geometry::buildOwlInstance(sdaiInstance);

	auto pGeometry = new _ap242_annotation_plane(owlInstance, sdaiInstance);
	addGeometry(pGeometry);

	auto pInstance = new _ap_instance(
		_model::getNextInstanceID(),
		pGeometry,
		nullptr);
	addInstance(pInstance);

	return pGeometry;
}

_ap242_draughting_callout* _ap242_model::loadDraughtingCallout(SdaiInstance sdaiInstance)
{
	assert(sdaiInstance != 0);

	OwlInstance owlInstance = _ap_geometry::buildOwlInstance(sdaiInstance);

	auto pGeometry = new _ap242_draughting_callout(owlInstance, sdaiInstance);
	addGeometry(pGeometry);

	auto pInstance = new _ap_instance(
		_model::getNextInstanceID(),
		pGeometry,
		nullptr);
	addInstance(pInstance);

	return pGeometry;
}

void _ap242_model::save(const wchar_t* /*szPath*/)
{
	assert(0); // Not implemented
}

_ap242_model_structure* _ap242_model::getModelStructure()
{
	if (m_pModelStructure == nullptr) {
		m_pModelStructure = new _ap242_model_structure(this);
		m_pModelStructure->build();
	}
	return m_pModelStructure;
}

_ap242_property_provider* _ap242_model::getPropertyProvider()
{
	if (m_pPropertyProvider == nullptr) {
		m_pPropertyProvider = new _ap242_property_provider(getSdaiModel());
	}
	return m_pPropertyProvider;
}
