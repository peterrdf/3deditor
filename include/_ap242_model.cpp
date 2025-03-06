#include "_host.h"
#include "_ap242_model.h"
#include "_ap242_product_definition.h"
#include "_ap242_draughting_model.h"
#include "_ap242_instance.h"
#include "_rdf_instance.h"

// ************************************************************************************************
_ap242_model::_ap242_model(bool bLoadInstancesOnDemand/* = false*/)
	: _ap_model(enumAP::STEP)
	, m_bLoadInstancesOnDemand(bLoadInstancesOnDemand)
	, m_mapExpressID2Assembly()
	, m_vecDraughtingModels()
{
}

/*virtual*/ _ap242_model::~_ap242_model()
{
	clean();
}

/*virtual*/ _instance* _ap242_model::loadInstance(int64_t iInstance) /*override*/
{
	assert(iInstance != 0);
	SdaiInstance sdaiInstance = (SdaiInstance)iInstance;

	clean(false);

	m_bUpdteVertexBuffers = true;

	OwlInstance owlInstance = _ap_geometry::buildOwlInstance(sdaiInstance);
	if (owlInstance != 0)
	{
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

	auto itExpressID2Assembly = m_mapExpressID2Assembly.begin();
	for (; itExpressID2Assembly != m_mapExpressID2Assembly.end(); itExpressID2Assembly++)
	{
		delete itExpressID2Assembly->second;
	}
	m_mapExpressID2Assembly.clear();

	for (auto pDraughtingModel : m_vecDraughtingModels)
	{
		delete pDraughtingModel;
	}
	m_vecDraughtingModels.clear();
}

/*virtual*/ void _ap242_model::attachModelCore() /*override*/
{
	if (!m_bLoadInstancesOnDemand)
	{
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
	for (SdaiInteger i = 0; i < iProductDefinitionsCount; i++)
	{
		SdaiInstance sdaiProductDefinitionInstance = 0;
		sdaiGetAggrByIndex(sdaiProductDefinitionAggr, i, sdaiINSTANCE, &sdaiProductDefinitionInstance);
		assert(sdaiProductDefinitionInstance != 0);

		loadProductDefinition(sdaiProductDefinitionInstance);
	}
}

_ap242_product_definition* _ap242_model::loadProductDefinition(SdaiInstance sdaiProductDefinitionInstance)
{
	OwlInstance owlInstance = _ap_geometry::buildOwlInstance(sdaiProductDefinitionInstance);	
	auto pGeometry = new _ap242_product_definition(owlInstance, sdaiProductDefinitionInstance);
	addGeometry(pGeometry);

	return pGeometry;
}

_ap242_product_definition* _ap242_model::getProductDefinition(SdaiInstance sdaiProductDefinitionInstance, bool bRelatingProduct, bool bRelatedProduct)
{
	ExpressID expressID = internalGetP21Line(sdaiProductDefinitionInstance);

	auto pGeometry = getGeometryByExpressID(expressID);
	if (pGeometry != nullptr)
	{
		_ptr<_ap242_product_definition> apProductDefinition(pGeometry);
		if (bRelatingProduct)
		{
			apProductDefinition->m_iRelatingProducts++;
		}

		if (bRelatedProduct)
		{
			apProductDefinition->m_iRelatedProducts++;
		}

		return apProductDefinition;
	} // if (pGeometry != nullptr)

	auto pDefinition = loadProductDefinition(sdaiProductDefinitionInstance);
	if (bRelatingProduct)
	{
		pDefinition->m_iRelatingProducts++;
	}

	if (bRelatedProduct)
	{
		pDefinition->m_iRelatedProducts++;
	}

	return pDefinition;
}

void _ap242_model::loadAssemblies()
{
	SdaiAggr sdaiNextAssemblyUsageOccurrenceAggr = sdaiGetEntityExtentBN(getSdaiModel(), "NEXT_ASSEMBLY_USAGE_OCCURRENCE");

	SdaiInteger iNextAssemblyUsageOccurrencesCount = sdaiGetMemberCount(sdaiNextAssemblyUsageOccurrenceAggr);
	for (SdaiInteger i = 0; i < iNextAssemblyUsageOccurrencesCount; i++)
	{
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
	for (auto pGeometry : getGeometries())
	{
		_ptr<_ap242_product_definition> apProductDefinition(pGeometry);
		if (apProductDefinition->getRelatedProducts() == 0)
		{
			walkAssemblyTreeRecursively(apProductDefinition, nullptr, nullptr);
		}
	}
}

void _ap242_model::walkAssemblyTreeRecursively(_ap242_product_definition* pProductDefinition, _ap242_assembly* /*pParentAssembly*/, _matrix4x3* pParentMatrix)
{
	auto itExpressID2Assembly = m_mapExpressID2Assembly.begin();
	for (; itExpressID2Assembly != m_mapExpressID2Assembly.end(); itExpressID2Assembly++)
	{
		auto pAssembly = itExpressID2Assembly->second;

		if (pAssembly->getRelatingProductDefinition() == pProductDefinition)
		{
			int64_t	owlInstanceMatrix = 0;
			owlBuildInstance(getSdaiModel(), internalGetInstanceFromP21Line(getSdaiModel(), pAssembly->getExpressID()), &owlInstanceMatrix);

			if (owlInstanceMatrix && GetInstanceClass(owlInstanceMatrix) == GetClassByName(::GetModel(owlInstanceMatrix), "Transformation"))
			{
				owlInstanceMatrix = _model::getInstanceObjectProperty(owlInstanceMatrix, "matrix");
			}

			assert(owlInstanceMatrix == 0 || GetInstanceClass(owlInstanceMatrix) == GetClassByName(::GetModel(owlInstanceMatrix), "Matrix") ||
				GetInstanceClass(owlInstanceMatrix) == GetClassByName(::GetModel(owlInstanceMatrix), "MatrixMultiplication"));

			_matrix4x3 matrix;
			_matrix4x3Identity(&matrix);

			if (owlInstanceMatrix)
			{
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

			if (pParentMatrix)
			{
				_matrix4x3Multiply(&matrix, &matrix, pParentMatrix);
			}

			walkAssemblyTreeRecursively(pAssembly->getRelatedProductDefinition(), pAssembly, &matrix);
		} // if (pAssembly->m_pRelatingProductDefinition == ...
	} // for (; itAssembly != ...

	auto pInstance = new _ap242_instance(
		_model::getNextInstanceID(),
		pProductDefinition,
		pParentMatrix);
	addInstance(pInstance);
}

void _ap242_model::loadDraughtingModels()
{
	SdaiAggr sdaiDraughtingModelAggr = xxxxGetEntityAndSubTypesExtentBN(getSdaiModel(), "DRAUGHTING_MODEL");
	assert(sdaiDraughtingModelAggr != nullptr);

	SdaiInteger iDraughtingModelsCount = sdaiGetMemberCount(sdaiDraughtingModelAggr);
	for (SdaiInteger i = 0; i < iDraughtingModelsCount; i++)
	{
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
		for (SdaiInteger j = 0; j < iItemsCount; j++)
		{
			SdaiInstance sdaiItemInstance = 0;
			sdaiGetAggrByIndex(sdaiItemsAggr, j, sdaiINSTANCE, &sdaiItemInstance);

			if (sdaiGetInstanceType(sdaiItemInstance) == sdaiGetEntity(getSdaiModel(), "ANNOTATION_PLANE"))
			{
				auto pGeometry = getGeometryByInstance(sdaiItemInstance);
				if (pGeometry == nullptr)
				{
					pDraughtingModel->m_vecAnnotationPlanes.push_back(loadAnnotationPlane(sdaiItemInstance));
				}				
			}
			else if (sdaiGetInstanceType(sdaiItemInstance) == sdaiGetEntity(getSdaiModel(), "DRAUGHTING_CALLOUT"))
			{
				auto pGeometry = getGeometryByInstance(sdaiItemInstance);
				if (pGeometry == nullptr)
				{
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
	if (owlInstance != 0)
	{
		preLoadInstance(owlInstance);
	}

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
	if (owlInstance != 0)
	{
		preLoadInstance(owlInstance);
	}

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
	assert(0); //#todo
}