#include "stdafx.h"

#include "_json_importer_t.h"

#include "_gltf_importer_t.h"

// ************************************************************************************************
namespace _eng
{
	// ********************************************************************************************
	template<typename Document, typename Site>
	_json_importer_t<Document, Site>::_json_importer_t(OwlModel iModel, int iValidationLevel)
		: m_iModel(iModel)
		, m_iValidationLevel(iValidationLevel)
		, m_iTagProperty(0)
		, m_iRelationsProperty(0)
		, m_iValueProperty(0)
		, m_iDoubleValueProperty(0)
		, m_iDefaultMaterialInstance(0)
		, m_pDocument(nullptr)
	{
		VERIFY_INSTANCE(m_iModel);

		m_iTagProperty = CreateProperty(
			getModel(),
			DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY,
			"tag");
		VERIFY_INSTANCE(m_iTagProperty);

		m_iRelationsProperty = CreateProperty(
			getModel(),
			OBJECTTYPEPROPERTY_TYPE,
			"$relations");
		VERIFY_INSTANCE(m_iRelationsProperty);

		m_iValueProperty = CreateProperty(
			getModel(),
			DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY,
			"value");
		VERIFY_INSTANCE(m_iValueProperty);

		m_iDoubleValueProperty = CreateProperty(
			getModel(),
			DATATYPEPROPERTY_TYPE_DOUBLE,
			"double-value");
		VERIFY_INSTANCE(m_iDoubleValueProperty);
	}

	template<typename Document, typename Site>
	/*virtual*/ _json_importer_t<Document, Site>::~_json_importer_t()
	{
		clean();
	}

	template<typename Document, typename Site>
	/*virtual*/ void _json_importer_t<Document, Site>::load(const char* szFile, bool bLoadSchemas)
	{
		loadDocument(szFile);

		if (bLoadSchemas)
		{
			auto start = std::chrono::steady_clock::now();

			getDocument()->loadSchemas();

			auto end = std::chrono::steady_clock::now();
			Site::logInfof("Loading schemas: %lld [ms]", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
		}

		if (szFile == nullptr)
		{
			// Empty document
			return;
		}

		postLoad();
	}

	template<typename Document, typename Site>
	/*virtual*/ void _json_importer_t<Document, Site>::loadArray(const unsigned char* szData, size_t iSize, bool bLoadSchemas)
	{
		stringstream ssData;
		ssData.write((const char*)szData, iSize);

		loadStream(&ssData, bLoadSchemas);
	}

	template<typename Document, typename Site>
	/*virtual*/ void _json_importer_t<Document, Site>::loadStream(istream* pStream, bool bLoadSchemas)
	{
		loadDocument(pStream);

		if (bLoadSchemas) {
			auto start = std::chrono::steady_clock::now();

			getDocument()->loadSchemas();

			auto end = std::chrono::steady_clock::now();
			Site::logInfof("Loading schemas: %lld [ms]", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
		}

		postLoad();
	}

	template<typename Document, typename Site>
	/*virtual*/ void _json_importer_t<Document, Site>::loadDocument(const char* szFile)
	{
		auto start = std::chrono::steady_clock::now();

		setDocument(new Document(this));
		getDocument()->load(szFile);

		auto end = std::chrono::steady_clock::now();
		Site::logInfof("Loading document: %lld [ms]", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	}

	template<typename Document, typename Site>
	/*virtual*/ void _json_importer_t<Document, Site>::loadDocument(istream* pStream)
	{
		auto start = std::chrono::steady_clock::now();

		setDocument(new Document(this));
		getDocument()->load(pStream);

		auto end = std::chrono::steady_clock::now();
		Site::logInfof("Loading document: %lld [ms]", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	}

	template<typename Document, typename Site>
	/*virtual*/ void _json_importer_t<Document, Site>::postLoad()
	{
		auto start = std::chrono::steady_clock::now();

		if (preBuildModel())
		{
			buildModel();
			postBuildModel();
		}

		auto end = std::chrono::steady_clock::now();
		Site::logInfof("Building model: %lld [ms]", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	}

	template<typename Document, typename Site>
	/*virtual*/ void _json_importer_t<Document, Site>::clean()
	{
		delete m_pDocument;
		m_pDocument = nullptr;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::getDefaultMaterialInstance()
	{
		if (m_iDefaultMaterialInstance == 0)
		{
			createDefaultMaterial();
		}

		return m_iDefaultMaterialInstance;
	}

	template<typename Document, typename Site>
	/*virtual*/ void _json_importer_t<Document, Site>::buildClassName(const string& strClass, string& strName) const
	{
		strName = ns_class;
		strName += strClass;
	}

	template<typename Document, typename Site>
	/*virtual*/ void _json_importer_t<Document, Site>::addDataTypeAsString(int iType, string& strType) const
	{
		switch (iType)
		{
		case OBJECTTYPEPROPERTY_TYPE:
		{
			strType += "obj";
		}
		break;

		case DATATYPEPROPERTY_TYPE_BOOLEAN:
		{
			strType += "bool";
		}
		break;

		case DATATYPEPROPERTY_TYPE_STRING:
		case DATATYPEPROPERTY_TYPE_CHAR_ARRAY:
		case DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY:
		{
			strType += "str";
		}
		break;

		case DATATYPEPROPERTY_TYPE_INTEGER:
		{
			strType += "int";
		}
		break;

		case DATATYPEPROPERTY_TYPE_DOUBLE:
		{
			strType += "dbl";
		}
		break;

		case DATATYPEPROPERTY_TYPE_BYTE:
		{
			strType += "byte";
		}
		break;

		default:
			THROW_INTERNAL_ERROR();
		} // switch (iType)
	}

	template<typename Document, typename Site>
	/*virtual*/ void _json_importer_t<Document, Site>::buildAttributeName(const string& strAttribute, int iType, string& strName) const
	{
		strName = ns_attr;
		addDataTypeAsString(iType, strName);
		strName += ":";
		strName += strAttribute;
	}

	template<typename Document, typename Site>
	/*virtual*/ void _json_importer_t<Document, Site>::buildPropertyName(const string& strProperty, int iType, string& strName) const
	{
		strName = ns_prop;
		addDataTypeAsString(iType, strName);
		strName += ":";
		strName += strProperty;
	}

	template<typename Document, typename Site>
	void _json_importer_t<Document, Site>::createDefaultMaterial()
	{
		m_iDefaultMaterialInstance = CreateInstance(GetClassByName(getModel(), "Material"));
		VERIFY_INSTANCE(m_iDefaultMaterialInstance);

		setTag(m_iDefaultMaterialInstance, default_material_name);

		OwlInstance iColorInstance = CreateInstance(GetClassByName(getModel(), "Color"));
		VERIFY_INSTANCE(iColorInstance);

		setTag(iColorInstance, default_color_name);

		SetObjectProperty(
			m_iDefaultMaterialInstance,
			GetPropertyByName(getModel(), "color"),
			&iColorInstance,
			1);

		OwlInstance iColorComponentInstance = createColorComponentInstance(default_color_name, 0., 0., 1.);
		VERIFY_INSTANCE(iColorComponentInstance);

		SetObjectProperty(
			iColorInstance,
			GetPropertyByName(getModel(), "ambient"),
			&iColorComponentInstance,
			1);

		double dTransparency = 0.25;
		SetDatatypeProperty(
			iColorInstance,
			GetPropertyByName(getModel(), "transparency"),
			&dTransparency,
			1);
	}

	template<typename Document, typename Site>
	/*virtual*/ OwlInstance _json_importer_t<Document, Site>::createColorComponentInstance(const string& strName, double dR, double dG, double dB)
	{
		OwlInstance iColorComponentInstance = CreateInstance(GetClassByName(getModel(), "ColorComponent"));
		VERIFY_INSTANCE(iColorComponentInstance);

		setTag(iColorComponentInstance, !strName.empty() ? strName.c_str() : "ColorComponent");

		SetDatatypeProperty(
			iColorComponentInstance,
			GetPropertyByName(getModel(), "R"),
			&dR,
			1);

		SetDatatypeProperty(
			iColorComponentInstance,
			GetPropertyByName(getModel(), "G"),
			&dG,
			1);

		SetDatatypeProperty(
			iColorComponentInstance,
			GetPropertyByName(getModel(), "B"),
			&dB,
			1);

		return iColorComponentInstance;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createInstance(const string& strClass, const string& strInstance)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strClass);

		string strName;
		buildClassName(strClass, strName);

		OwlClass iClass = CreateClass(m_iModel, strName.c_str());
		VERIFY_INSTANCE(iClass);

		OwlInstance iInstance = CreateInstance(iClass);
		VERIFY_INSTANCE(iInstance);

		setTag(iInstance, !strInstance.empty() ? strInstance.c_str() : strClass.c_str());

		return iInstance;
	}

	template<typename Document, typename Site>
	OwlClass  _json_importer_t<Document, Site>::createClass(const string& strClass)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strClass);

		string strName;
		buildClassName(strClass, strName);

		OwlClass iClass = CreateClass(m_iModel, strName.c_str());
		VERIFY_INSTANCE(iClass);

		return iClass;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createInstance(OwlClass iClass, const string& strInstance)
	{
		VERIFY_INSTANCE(iClass);

		OwlInstance iInstance = CreateInstance(iClass);
		VERIFY_INSTANCE(iInstance);

		string strName = strInstance;
		if (strName.empty())
		{
			char* szClassName = nullptr;
			GetNameOfClass(iClass, &szClassName);

			strName = szClassName;
		}

		setTag(iInstance, strName.c_str());

		return iInstance;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createExtensionInstance(const string& strClass, const string& strBaseClass, const string& strInstance)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strClass);
		VERIFY_STLOBJ_IS_NOT_EMPTY(strBaseClass);

		string strName;
		buildClassName(strClass, strName);

		OwlClass iClass = CreateClass(m_iModel, strName.c_str());
		VERIFY_INSTANCE(iClass);

		OwlClass iBaseClass = GetClassByName(m_iModel, strBaseClass.c_str());
		VERIFY_INSTANCE(iBaseClass);

		SetClassParent(iClass, iBaseClass);

		OwlInstance iInstance = CreateInstance(iClass);
		VERIFY_INSTANCE(iInstance);

		setTag(iInstance, !strInstance.empty() ? strInstance.c_str() : strClass.c_str());

		return iInstance;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createCollectionInstance(const string& strClass, const string& strInstance)
	{
		return createExtensionInstance(strClass, "Collection", strInstance);
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createNillInstance(const string& strClass, const string& strInstance)
	{
		return createExtensionInstance(strClass, "Nill", strInstance);
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createInvertedSurface(const string& strName, OwlInstance iSurface)
	{
		VERIFY_INSTANCE(iSurface);

		OwlClass iClass = GetClassByName(getModel(), "InvertedSurface");
		VERIFY_INSTANCE(iClass);

		OwlInstance iInstance = CreateInstance(iClass);
		VERIFY_INSTANCE(iInstance);

		setTag(iInstance, !strName.empty() ? strName.c_str() : "InvertedSurface");

		SetObjectProperty(
			iInstance,
			GetPropertyByName(getModel(), "surface"),
			&iSurface,
			1);

		return iInstance;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createObjectPropertyInstance(const string& strProperty)
	{
		string strName;
		buildPropertyName(strProperty, OBJECTTYPEPROPERTY_TYPE, strName);

		RdfProperty iProperty = CreateProperty(
			m_iModel,
			OBJECTTYPEPROPERTY_TYPE,
			strName.c_str());
		VERIFY_INSTANCE(iProperty);

		return iProperty;
	}

	template<typename Document, typename Site>
	RdfProperty _json_importer_t<Document, Site>::createDataAttributeInstance(const string& strAttribute, int iType)
	{
		string strName;
		buildAttributeName(strAttribute, iType, strName);

		RdfProperty iAttribute = CreateProperty(
			m_iModel,
			iType,
			strName.c_str());
		VERIFY_INSTANCE(iAttribute);

		return iAttribute;
	}

	template<typename Document, typename Site>
	RdfProperty _json_importer_t<Document, Site>::createDataPropertyInstance(const string& strProperty, int iType)
	{
		string strName;
		buildPropertyName(strProperty, iType, strName);

		RdfProperty iProperty = CreateProperty(
			m_iModel,
			iType,
			strName.c_str());
		VERIFY_INSTANCE(iProperty);

		return iProperty;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createBoundaryRepresentationInstance(
		const string& strInstance,
		const vector<int64_t>& vecIndices,
		const vector<double>& vecVertices,
		OwlInstance iMaterialInstance/* = 0*/)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecIndices);
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecVertices);

		OwlClass iClass = GetClassByName(getModel(), "BoundaryRepresentation");
		VERIFY_INSTANCE(iClass);

		OwlInstance iInstance = CreateInstance(iClass);
		VERIFY_INSTANCE(iInstance);

		setTag(iInstance, !strInstance.empty() ? strInstance.c_str() : "BoundaryRepresentation");

		SetDatatypeProperty(
			iInstance,
			GetPropertyByName(getModel(), "indices"),
			vecIndices.data(),
			vecIndices.size());

		SetDatatypeProperty(
			iInstance,
			GetPropertyByName(getModel(), "vertices"),
			vecVertices.data(),
			vecVertices.size());

		if (iMaterialInstance != 0)
		{
			SetObjectProperty(
				iInstance,
				GetPropertyByName(getModel(), "material"),
				&iMaterialInstance,
				1);
		}

		return iInstance;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createPoint3DInstance(
		const string& strInstance,
		const vector<double>& vecPoints,
		OwlInstance iMaterialInstance/* = 0*/)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecPoints);

		OwlClass iClass = GetClassByName(getModel(), "Point3D");
		VERIFY_INSTANCE(iClass);

		OwlInstance iInstance = CreateInstance(iClass);
		VERIFY_INSTANCE(iInstance);

		setTag(iInstance, !strInstance.empty() ? strInstance.c_str() : "Point3D");

		SetDatatypeProperty(
			iInstance,
			GetPropertyByName(getModel(), "points"),
			vecPoints.data(),
			vecPoints.size());

		if (iMaterialInstance != 0)
		{
			SetObjectProperty(
				iInstance,
				GetPropertyByName(getModel(), "material"),
				&iMaterialInstance,
				1);
		}

		return iInstance;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createPoint3DSetDInstance(
		const string& strInstance,
		const vector<double>& vecPoints,
		OwlInstance iMaterialInstance/* = 0*/)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecPoints);

		OwlClass iClass = GetClassByName(getModel(), "Point3DSet");
		VERIFY_INSTANCE(iClass);

		OwlInstance iInstance = CreateInstance(iClass);
		VERIFY_INSTANCE(iInstance);

		setTag(iInstance, !strInstance.empty() ? strInstance.c_str() : "Point3DSet");

		SetDatatypeProperty(
			iInstance,
			GetPropertyByName(getModel(), "points"),
			vecPoints.data(),
			vecPoints.size());

		if (iMaterialInstance != 0)
		{
			SetObjectProperty(
				iInstance,
				GetPropertyByName(getModel(), "material"),
				&iMaterialInstance,
				1);
		}

		return iInstance;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createTriangleSetInstance(
		const string& strInstance,
		const vector<int64_t>& vecIndices,
		const vector<double>& vecVertices,
		OwlInstance iMaterialInstance/* = 0*/)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecIndices);
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecVertices);

		OwlClass iClass = GetClassByName(getModel(), "TriangleSet");
		VERIFY_INSTANCE(iClass);

		OwlInstance iInstance = CreateInstance(iClass);
		VERIFY_INSTANCE(iInstance);

		setTag(iInstance, !strInstance.empty() ? strInstance.c_str() : "TriangleSet");

		SetDatatypeProperty(
			iInstance,
			GetPropertyByName(getModel(), "indices"),
			vecIndices.data(),
			vecIndices.size());

		SetDatatypeProperty(
			iInstance,
			GetPropertyByName(getModel(), "vertices"),
			vecVertices.data(),
			vecVertices.size());

		if (iMaterialInstance != 0)
		{
			SetObjectProperty(
				iInstance,
				GetPropertyByName(getModel(), "material"),
				&iMaterialInstance,
				1);
		}

		return iInstance;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createPolyLine3DInstance(
		const string& strInstance,
		const vector<double>& vecPoints,
		OwlInstance iMaterialInstance/* = 0*/)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecPoints);

		OwlClass iClass = GetClassByName(getModel(), "PolyLine3D");
		VERIFY_INSTANCE(iClass);

		OwlInstance iInstance = CreateInstance(iClass);
		VERIFY_INSTANCE(iInstance);

		setTag(iInstance, !strInstance.empty() ? strInstance.c_str() : "PolyLine3D");

		SetDatatypeProperty(
			iInstance,
			GetPropertyByName(getModel(), "points"),
			vecPoints.data(),
			vecPoints.size());

		if (iMaterialInstance != 0)
		{
			SetObjectProperty(
				iInstance,
				GetPropertyByName(getModel(), "material"),
				&iMaterialInstance,
				1);
		}

		return iInstance;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createLine3DSetInstance(
		const string& strInstance,
		const vector<double>& vecPoints,
		OwlInstance iMaterialInstance/* = 0*/)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecPoints);

		OwlClass iClass = GetClassByName(getModel(), "Line3DSet");
		VERIFY_INSTANCE(iClass);

		OwlInstance iInstance = CreateInstance(iClass);
		VERIFY_INSTANCE(iInstance);

		setTag(iInstance, !strInstance.empty() ? strInstance.c_str() : "Line3DSet");

		SetDatatypeProperty(
			iInstance,
			GetPropertyByName(getModel(), "points"),
			vecPoints.data(),
			vecPoints.size());

		if (iMaterialInstance != 0)
		{
			SetObjectProperty(
				iInstance,
				GetPropertyByName(getModel(), "material"),
				&iMaterialInstance,
				1);
		}

		return iInstance;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createLine3DInstance(
		const string& strInstance,
		const vector<double>& vecPoints,
		OwlInstance iMaterialInstance/* = 0*/)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecPoints);

		OwlClass iClass = GetClassByName(getModel(), "Line3D");
		VERIFY_INSTANCE(iClass);

		OwlInstance iInstance = CreateInstance(iClass);
		VERIFY_INSTANCE(iInstance);

		setTag(iInstance, !strInstance.empty() ? strInstance.c_str() : "Line3D");

		SetDatatypeProperty(
			iInstance,
			GetPropertyByName(getModel(), "points"),
			vecPoints.data(),
			vecPoints.size());

		if (iMaterialInstance != 0)
		{
			SetObjectProperty(
				iInstance,
				GetPropertyByName(getModel(), "material"),
				&iMaterialInstance,
				1);
		}

		return iInstance;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createArc3DInstance(
		const string& strInstance,
		double dRadius,
		double dStart,
		double dSize,
		int64_t iSegmentationParts/* = 36*/)
	{
		OwlClass iClass = GetClassByName(getModel(), "Arc3D");
		VERIFY_INSTANCE(iClass);

		OwlInstance iInstance = CreateInstance(iClass);
		VERIFY_INSTANCE(iInstance);

		setTag(iInstance, !strInstance.empty() ? strInstance.c_str() : "Arc3D");

		SetDatatypeProperty(
			iInstance,
			GetPropertyByName(getModel(), "radius"),
			&dRadius,
			1);

		SetDatatypeProperty(
			iInstance,
			GetPropertyByName(getModel(), "start"),
			&dStart,
			1);

		SetDatatypeProperty(
			iInstance,
			GetPropertyByName(getModel(), "size"),
			&dSize,
			1);

		SetDatatypeProperty(
			iInstance,
			GetPropertyByName(getModel(), "segmentationParts"),
			&iSegmentationParts,
			1);

		return iInstance;
	}

	template<typename Document, typename Site>
	OwlInstance _json_importer_t<Document, Site>::createTransformation(
		const string& strInstance,
		const vector<double>& vecTransformationMatrix)
	{
		if (vecTransformationMatrix.size() != 12)
		{
			THROW_INTERNAL_ERROR();
		}

		/* Matrix */
		OwlInstance iMatrixInstance = CreateInstance(GetClassByName(getModel(), "Matrix"));
		VERIFY_INSTANCE(iMatrixInstance);

		setTag(iMatrixInstance, !strInstance.empty() ? strInstance.c_str() : "Matrix");

		SetDatatypeProperty(
			iMatrixInstance,
			GetPropertyByName(getModel(), "coordinates"),
			vecTransformationMatrix.data(),
			vecTransformationMatrix.size());

		/* Transformation */
		OwlInstance iTransformationInstance = CreateInstance(GetClassByName(getModel(), "Transformation"));
		VERIFY_INSTANCE(iTransformationInstance);

		setTag(iTransformationInstance, !strInstance.empty() ? strInstance.c_str() : "Transformation");

		SetObjectProperty(
			iTransformationInstance,
			GetPropertyByName(getModel(), "matrix"),
			&iMatrixInstance, 1);

		return iTransformationInstance;
	}

	// ********************************************************************************************
	template class _json_importer_t<_json::_document, _json::_document_site>;
};