#pragma once

#ifdef _WINDOWS
#include "../include/engine.h"
#else
#include "../../include/engine.h"
#endif

#include "_log.h"
#include "_io.h"
#include "_errors.h"
#ifdef _LOAD_SCHEMAS
#include "_schema_file_storage.h"
#include "_schema_embedded_storage.h"
#include "_schema_embedded_storage_builder.h"
#endif // _LOAD_SCHEMAS

#define PI 3.14159265

// ************************************************************************************************
namespace _eng
{
	// ********************************************************************************************
	static const char ns_class[] = "class:";
	static const char ns_prop[] = "prop:";
	static const char ns_attr[] = "attr:";

	static const char default_material_name[] = "Default Material";
	static const char default_color_name[] = "Default Color";

	// ********************************************************************************************
	template<typename Document, typename Site>
	class _importer_t : public Site
	{

	private: // Classes

		// ****************************************************************************************
		struct _vector3d
		{
			double x;
			double y;
			double z;
		};

	private: // Members

		OwlModel m_iModel;

		int m_iValidationLevel;

		RdfProperty m_iTagProperty;
		RdfProperty m_iRelationsProperty;
		RdfProperty m_iValueProperty;
		RdfProperty m_iDoubleValueProperty;
		OwlInstance m_iDefaultMaterialInstance;

		Document* m_pDocument;

	protected: // Members

#ifdef _LOAD_SCHEMAS
		_schema_storage* m_pSchemaStorage;
#endif

	public: // Methods

		_importer_t(OwlModel iModel, int iValidationLevel);
		virtual ~_importer_t();

		virtual void load(const char* szFile);
		virtual void loadArray(const unsigned char* szData, size_t iSize);
		virtual void loadStream(istream* pStream);

	protected: // Methods

		virtual void loadDocument(const char* szFile);
		virtual void loadDocument(istream* pStream);

		virtual void postLoad();
		virtual bool preBuildModel() { return true; };
		virtual void buildModel() {};
		virtual void postBuildModel() {};

		virtual void clean();

		OwlModel getModel() const { return m_iModel; }
		int getValidationLevel() const { return m_iValidationLevel; }
		RdfProperty getRelationsProperty() const { return m_iRelationsProperty; }
		RdfProperty getValueProperty() const { return m_iValueProperty; }
		RdfProperty getDoubleValueProperty() const { return m_iDoubleValueProperty; }
		RdfProperty getTagProperty() const { return m_iTagProperty; }
		OwlInstance getDefaultMaterialInstance();
		void setDocument(Document* pDocument) { clean(); m_pDocument = pDocument; }
		Document* getDocument() const { return m_pDocument; }
#ifdef _LOAD_SCHEMAS
		_schema_storage* getSchemaStorage() const { return m_pSchemaStorage; }
#endif // _LOAD_SCHEMAS

		virtual void buildClassName(const string& strClass, string& strName) const;
		virtual void addDataTypeAsString(int iType, string& strName) const;
		virtual void buildAttributeName(const string& strAttribute, int iType, string& strName) const;
		virtual void buildPropertyName(const string& strProperty, int iType, string& strName) const;

		void setTag(OwlInstance iInstance, const string& strValue)
		{
			VERIFY_INSTANCE(iInstance);
			VERIFY_STLOBJ_IS_NOT_EMPTY(strValue);

#ifdef _WINDOWS
			// UTF-8 to UTF-16/char16_t
			u16string u16Str = wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(strValue);

			// UTF-16/char16_t to wstring
			wstring wStr;
			wStr.resize(u16Str.size());
			memcpy((void*)wStr.data(), u16Str.data(), u16Str.size() * sizeof(wchar_t));
#else
			wstring wStr = (LPCWSTR)CA2W(strValue.c_str());
#endif // _WINDOWS

			wchar_t* szValue = new wchar_t[wStr.size() + 1];
			wcscpy(szValue, wStr.c_str());

			SetDatatypeProperty(
				iInstance,
				getTagProperty(),
				&szValue,
				1);

			delete[] szValue;
		}

		void createDefaultMaterial();
		OwlInstance createColorComponentInstance(const string& strName, double dR, double dG, double dB);
		
		OwlInstance createInstance(const string& strClass, const string& strInstance);
		OwlClass createClass(const string& strClass);
		OwlInstance createInstance(OwlClass iClass, const string& strInstance);
		static void calculateBBMinMax(
			OwlInstance iInstance,
			double& dXmin, double& dXmax,
			double& dYmin, double& dYmax,
			double& dZmin, double& dZmax)
		{
			VERIFY_INSTANCE(iInstance);

			_vector3d vecOriginalBBMin;
			_vector3d vecOriginalBBMax;
			if (GetInstanceGeometryClass(iInstance) &&
				GetBoundingBox(
					iInstance,
					(double*)&vecOriginalBBMin,
					(double*)&vecOriginalBBMax))
			{
				dXmin = fmin(dXmin, vecOriginalBBMin.x);
				dYmin = fmin(dYmin, vecOriginalBBMin.y);
				dZmin = fmin(dZmin, vecOriginalBBMin.z);

				dXmax = fmax(dXmax, vecOriginalBBMax.x);
				dYmax = fmax(dYmax, vecOriginalBBMax.y);
				dZmax = fmax(dZmax, vecOriginalBBMax.z);
			}
		}

		// http://rdf.bg/gkdoc/CP64/SetClassParent.html
		OwlInstance createExtensionInstance(const string& strClass, const string& strBaseClass, const string& strInstance);

		// http://rdf.bg/gkdoc/Collection/CP64.html
		OwlInstance createCollectionInstance(const string& strClass, const string& strInstance);

		// http://rdf.bg/gkdoc/Nill/CP64.html
		OwlInstance createNillInstance(const string& strClass, const string& strInstance);

		// http://rdf.bg/gkdoc/InvertedCurve/CP64.html
		OwlInstance createInvertedSurface(const string& strName, OwlInstance iSurface);

		OwlInstance createObjectPropertyInstance(const string& strProperty);
		RdfProperty createDataAttributeInstance(const string& strAttribute, int iType);
		RdfProperty createDataPropertyInstance(const string& strProperty, int iType);
		void setDataProperty(
			OwlInstance iInstance,
			const string& strProperty,
			int iType,
			const void* pValues,
			int64_t iValuesCount)
		{
			VERIFY_INSTANCE(iInstance);
			VERIFY_STLOBJ_IS_NOT_EMPTY(strProperty);
			VERIFY_POINTER(pValues);

			SetDatatypeProperty(
				iInstance,
				createDataPropertyInstance(strProperty, iType),
				pValues,
				iValuesCount);
		}

		void createStringAttribute(OwlInstance iInstance, const string& strAttribute, const string& strValue)
		{
			VERIFY_INSTANCE(iInstance);
			VERIFY_STLOBJ_IS_NOT_EMPTY(strAttribute);

#ifdef _WINDOWS
			// UTF-8 to UTF-16/char16_t
			u16string u16Str = wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(strValue);

			// UTF-16/char16_t to wstring
			wstring wStr;
			wStr.resize(u16Str.size());
			memcpy((void*)wStr.data(), u16Str.data(), u16Str.size() * sizeof(wchar_t));
#else
			wstring wStr = (LPCWSTR)CA2W(strValue.c_str());
#endif // _WINDOWS

			wchar_t* szValue = new wchar_t[wStr.size() + 1];
			wcscpy(szValue, wStr.c_str());

			SetDatatypeProperty(
				iInstance,
				createDataAttributeInstance(strAttribute, DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY),
				&szValue,
				1);

			delete[] szValue;
		}

		void createStringProperty(OwlInstance iInstance, const string& strProperty, const string& strValue)
		{
			VERIFY_INSTANCE(iInstance);
			VERIFY_STLOBJ_IS_NOT_EMPTY(strProperty);

#ifdef _WINDOWS
			// UTF-8 to UTF-16/char16_t
			u16string u16Str = wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(strValue);

			// UTF-16/char16_t to wstring
			wstring wStr;
			wStr.resize(u16Str.size());
			memcpy((void*)wStr.data(), u16Str.data(), u16Str.size() * sizeof(wchar_t));
#else
			wstring wStr = (LPCWSTR)CA2W(strValue.c_str());
#endif // _WINDOWS

			wchar_t* szValue = new wchar_t[wStr.size() + 1];
			wcscpy(szValue, wStr.c_str());

			SetDatatypeProperty(
				iInstance,
				createDataPropertyInstance(strProperty, DATATYPEPROPERTY_TYPE_WCHAR_T_ARRAY),
				&szValue,
				1);

			delete[] szValue;
		}
		 
		void createStringValueProperty(OwlInstance iInstance, const string& strValue)
		{
			VERIFY_INSTANCE(iInstance);

#ifdef _WINDOWS
			// UTF-8 to UTF-16/char16_t
			u16string u16Str = wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(strValue);

			// UTF-16/char16_t to wstring
			wstring wStr;
			wStr.resize(u16Str.size());
			memcpy((void*)wStr.data(), u16Str.data(), u16Str.size() * sizeof(wchar_t));
#else
			wstring wStr = (LPCWSTR)CA2W(strValue.c_str());
#endif // _WINDOWS

			wchar_t* szValue = new wchar_t[wStr.size() + 1];
			wcscpy(szValue, wStr.c_str());

			SetDatatypeProperty(
				iInstance,
				getValueProperty(),
				&szValue,
				1);

			delete[] szValue;
		}

		void createDoubleValueProperty(OwlInstance iInstance, const string& strValues)
		{
			VERIFY_INSTANCE(iInstance);

			vector<string> vecStrValues;
			_string::split(strValues, " ", vecStrValues);

			VERIFY_STLOBJ_IS_NOT_EMPTY(vecStrValues);

			vector<double> vecValues;
			for (auto strValue : vecStrValues)
			{
				vecValues.push_back(atof(strValue.c_str()));
			}

			SetDatatypeProperty(
				iInstance,
				getDoubleValueProperty(),
				vecValues.data(),
				vecValues.size());
		}

		// http://rdf.bg/gkdoc/BoundaryRepresentation/CP64.html
		OwlInstance createBoundaryRepresentationInstance(
			const string& strInstance, 
			const vector<int64_t>& vecIndices,
			const vector<double>& vecVertices,
			OwlInstance iMaterialInstance);

		// http://rdf.bg/gkdoc/Point3D/CP64.html
		OwlInstance createPoint3DInstance(
			const string& strInstance,
			const vector<double>& vecPoints);

		// http://rdf.bg/gkdoc/Point3DSet/CP64.html
		OwlInstance createPoint3DSetDInstance(
			const string& strInstance, 
			const vector<double>& vecPoints);

		// http://rdf.bg/gkdoc/Point3DSet/CP64.html
		OwlInstance createPoint3DSetDInstance(
			const string& strInstance,
			const vector<double>& vecPoints,
			OwlInstance iMaterialInstance);

		// http://rdf.bg/gkdoc/TriangleSet/CP64.html
		OwlInstance createTriangleSetInstance(
			const string& strInstance,
			const vector<int64_t>& vecIndices,
			const vector<double>& vecVertices,
			OwlInstance iMaterialInstance);

		// http://rdf.bg/gkdoc/PolyLine3D/CP64.html
		OwlInstance createPolyLine3DInstance(
			const string& strInstance, 
			const vector<double>& vecPoints);

		// http://rdf.bg/gkdoc/Line3DSet/CP64.html
		OwlInstance createLine3DSetInstance(
			const string& strInstance,
			const vector<double>& vecPoints);

		// http://rdf.bg/gkdoc/Line3DSet/CP64.html
		OwlInstance createLine3DSetInstance(
			const string& strInstance,
			const vector<double>& vecPoints,
			OwlInstance iMaterialInstance);

		// http://rdf.bg/gkdoc/Line3D/CP64.html
		OwlInstance createLine3DInstance(
			const string& strInstance,
			const vector<double>& vecPoints);

		// http://rdf.bg/gkdoc/Arc3D/CP64.html
		OwlInstance createArc3DInstance(
			const string& strInstance,
			double dRadius,
			double dStart,
			double dSize,
			int64_t iSegmentationParts = 36);

		// http://rdf.bg/gkdoc/Transformation/CP64.html
		OwlInstance createTransformation(
			const string& strInstance,
			const vector<double>& vecTransformationMatrix);		

	public: // Methods

#ifdef _LOAD_SCHEMAS
		string getNamespaceRedirection(const string& strNamespace) const { return getSchemaStorage()->getNamespaceRedirection(strNamespace); };
		string getLocationRedirection(const string& strLocation) const { return getSchemaStorage()->getLocationRedirection(strLocation); }

		template<typename Schema>
		Schema* loadSchema(const string& strNamespace, const string& strLocation)
		{
			if (strNamespace.empty() || strLocation.empty()) {
				THROW_ARGUMENT_ERROR();
			}

			string strTargetNamespace = getNamespaceRedirection(strNamespace);
			string strTargetLocation = getLocationRedirection(strLocation);

			Schema* pSchema = nullptr;

			string strStatus = "OK";
			string strSource = "NA";

			try {
				bool bLoaded = false;

				/* Metadata */
				string strAbsoluteLocation = strTargetLocation;

				auto pSchemaMetadata = getSchemaStorage()->getSchema(strTargetNamespace, strTargetLocation);
				if (pSchemaMetadata != nullptr) {
					// relative to absolute URL
					strAbsoluteLocation = pSchemaMetadata->getLocation();
				}

				/* Cache */
				bool bNew = false;
				pSchema = getDocument()->getSchema(strTargetNamespace, strAbsoluteLocation, bNew);
				if (!bNew) {
					return pSchema;
				}

				/* Embedded */
				auto pSchemaEmbeddedStorage = dynamic_cast<_schema_embedded_storage*>(getSchemaStorage());
				if (pSchemaEmbeddedStorage != nullptr) {
					auto pArchive = pSchemaEmbeddedStorage->getArchive(strTargetNamespace, strAbsoluteLocation, false);
					if (pArchive != nullptr) {
						strSource = "Embedded";

						pSchema->deserialize(pArchive);

						bLoaded = true;
					}
				}

				// Storage
				if (!bLoaded) {
#ifdef _WINDOWS
					if ((pSchemaMetadata != nullptr) && !pSchemaMetadata->getPath().empty()) {
						// Known schema - use path attribute

						/* Storage */
						string strFilePath = getSchemaStorage()->getFilePath(pSchemaMetadata->getPath());
						if (!strFilePath.empty()) {
							strSource = "Storage";

							pSchema->load(strFilePath.c_str());

							bLoaded = true;
						}
					} else {
						// Unknown schema - use ./schema/temp

						string strFilePath = getSchemaStorage()->getTempFilePath(schema_temp_folder, strAbsoluteLocation);
						if (!strFilePath.empty()) {
							strSource = "Storage";

							pSchema->load(strFilePath.c_str());

							bLoaded = true;
						}
					}
#endif // _WINDOWS
				} // Storage		

				// HTTP
				if (!bLoaded) {
#ifdef _WINDOWS
					strSource = "HTTP";

					_net::_http_client httpClient;
					httpClient.setLog(Site::getLog());

					downloadSchema(getSchemaStorage(),
						&httpClient,
						strAbsoluteLocation,
						pSchemaMetadata != nullptr ? pSchemaMetadata->getPath() : "");

					pSchema->load((istream*)httpClient.getPayload());

					bLoaded = true;
#endif // _WINDOWS
				} // HTTP

				if (!bLoaded) {
					strStatus = "Error: not supported.";

					pSchema->clean();
				}
			} catch (const std::runtime_error& err) {
				strStatus = "Error: ";
				strStatus += err.what();

				pSchema->clean();
			} catch (...) {
				strStatus = "Error: ";
				strStatus += _err::_internal;

				pSchema->clean();
			}

			pSchema->metaData()["source"] = strSource;
			pSchema->metaData()["status"] = strStatus;

			return pSchema;
		}

		template<typename Schema>
		void loadStorageSchemas()
		{
			for (auto itNamespaceSchemas : getSchemaStorage()->getNamespaceSchemas()) {
				vector<_schema_metadata*> vecSchemas;
				string strTargetNamespace = getSchemaStorage()->getSchemas(itNamespaceSchemas.first, vecSchemas);

				for (auto pMetadata : vecSchemas) {
					loadSchema<Schema>(strTargetNamespace, pMetadata->getLocation());
				}
			} // for (auto itNamespace ...
		}

		void downloadSchema(
			_schema_storage* pSchemaStorage,
			_net::_http_client* pHttpClient,
			const string& strAbsoluteLocation,
			const string& strPath) const;

		void buildEmbeddedStorage(const string& strSourcesRootFolder, const string& strStorage, const vector<string>& vecReferences)
		{
			VERIFY_STLOBJ_IS_NOT_EMPTY(strSourcesRootFolder);
			VERIFY_STLOBJ_IS_NOT_EMPTY(strStorage);

			auto pSchemaFileStorage = dynamic_cast<_schema_file_storage*>(getSchemaStorage());
			if (pSchemaFileStorage == nullptr)
			{
				THROW_INTERNAL_ERROR();
			}

			/* Storage */
			getSchemaStorage()->loadMetadata(strStorage);

			set<string> setNamespaces;
			for (auto itNamespaceSchemas : pSchemaFileStorage->getNamespaceSchemas())
			{
				setNamespaces.insert(itNamespaceSchemas.first);
			}

			set<string> setNamespaceRedirections;
			for (auto itNamespaceRedirection : pSchemaFileStorage->getNamespaceRedirections())
			{
				setNamespaceRedirections.insert(itNamespaceRedirection.first);
			}

			set<string> setLocationRedirections;
			for (auto itLocationRedirection : pSchemaFileStorage->getLocationRedirections())
			{
				setLocationRedirections.insert(itLocationRedirection.first);
			}

			/* References */
			for (auto strReference : vecReferences)
			{
				getSchemaStorage()->loadMetadata(strReference);
			}
			
			load(nullptr);

			/* Build */
			_schema_embedded_storage_builder storageBuilder(
				strStorage,
				pSchemaFileStorage,
				setNamespaces,
				setNamespaceRedirections,
				setLocationRedirections);

			for (auto itNamespace2Schemas : getDocument()->getSchemas())
			{
				if (setNamespaces.find(itNamespace2Schemas.first) == setNamespaces.end())
				{
					continue;
				}

				auto pNamespace = storageBuilder.getNamespace(itNamespace2Schemas.first);
				VERIFY_POINTER(pNamespace);

				for (auto itLocation2Schema : itNamespace2Schemas.second)
				{
					auto pArchive = pNamespace->getArchive(itLocation2Schema.first, true);
					VERIFY_POINTER(pArchive);

					itLocation2Schema.second->serialize(pArchive);
				}
			}

			string strEmbeddedStorageRootFolder = strSourcesRootFolder;
			strEmbeddedStorageRootFolder += strStorage;
			strEmbeddedStorageRootFolder += "\\";

			storageBuilder.save(strEmbeddedStorageRootFolder);
		}
#endif // _LOAD_SCHEMAS
	};
};

// ************************************************************************************************
static void	_transform(const vector<double>& vecIn, const vector<double>& vecMatrix, vector<double>& vecOut)
{
	if ((vecIn.size() != 3) || (vecMatrix.size() != 12))
	{
		THROW_ARGUMENT_ERROR();
	}

	vector<double> vecTmp(3, 0.);
	vecTmp[0] = vecIn[0] * vecMatrix[0] + vecIn[1] * vecMatrix[3] + vecIn[2] * vecMatrix[6] + vecMatrix[9];
	vecTmp[1] = vecIn[0] * vecMatrix[1] + vecIn[1] * vecMatrix[4] + vecIn[2] * vecMatrix[7] + vecMatrix[10];
	vecTmp[2] = vecIn[0] * vecMatrix[2] + vecIn[1] * vecMatrix[5] + vecIn[2] * vecMatrix[8] + vecMatrix[11];

	vecOut[0] = vecTmp[0];
	vecOut[1] = vecTmp[1];
	vecOut[2] = vecTmp[2];
}