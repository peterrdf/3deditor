#pragma once

#include "_base64.h"
#ifdef _WINDOWS
#include "_importer_t.h"
#include "_json.h"
#else
#include "../parsers/_importer_t.h"
#include "../parsers/_json.h"
#endif



// ************************************************************************************************
namespace _eng
{
	// ********************************************************************************************
	class _gltf_importer_t : public _importer_t<_json::_document, _json::_document_site>
	{

	private: // Members

		string m_strRootFolder;
		string m_strOutputFolder;

		const _json::_object* m_pRootObject;
		const _json::_property* m_pScenesProperty;
		const _json::_property* m_pNodesProperty;
		const _json::_property* m_pMeshesProperty;
		const _json::_property* m_pAccessorsProperty;
		const _json::_property* m_pBufferViewsProperty;
		const _json::_property* m_pBuffersProperty;
		const _json::_property* m_pMaterialsProperty;
		const _json::_property* m_pTexturesProperty;
		const _json::_property* m_pImagesProperty;

		OwlInstance m_iRootInstance;

		// Texture
		bool m_bTextureFlipV;
		RdfProperty m_rdfTextureFlipYProperty;

	protected: // Members

		map<string, OwlClass> m_mapClasses;

	private: // Members

		// Cache
		map<int, OwlInstance> m_mapNodes; // index : instance
		map<int, vector<OwlInstance>> m_mapMeshes; // index : instance
		map<int, OwlInstance> m_mapMaterials; // index : instance
		map<int, OwlInstance> m_mapTextures; // index : instance

	public: // Methods

		_gltf_importer_t(OwlModel owlModel, const string& strRootFolder, const string& strOutputFolder, int iValidationLevel, bool bTextureFlipV = false);
		virtual ~_gltf_importer_t();

		OwlInstance getRootInstance() const { return m_iRootInstance; }

	protected: // Methods

		// _importer_t
		virtual bool preBuildModel() override;
		virtual void buildModel() override;

		// _document_site
		virtual void onValueLoaded(_json::_value* /*pValue*/) override {};
		virtual void onPropertyLoaded(_json::_property* /*pProperty*/) override {};

		virtual OwlInstance createRootInstance();
		virtual void createScenesProperty();
		virtual OwlInstance createNodeInstance(const _json::_simple* pNodeIndex);
		virtual void createNodeGeometry(const _json::_simple* pMeshIndex, vector<OwlInstance>& vecGeometries);
		virtual OwlInstance createPoints(
			int iPositionAccessorIndex,
			int iIndicesAccessorIndex,
			int iMaterialIndex);
		virtual OwlInstance createLines(
			int iPositionAccessorIndex,
			int iIndicesAccessorIndex,
			int iMaterialIndex);
		virtual OwlInstance createMesh(
			int iPositionAccessorIndex,
			int iNormalAccessorIndex,
			int iTexcoord0AccessorIndex,
			int iIndicesAccessorIndex,
			int iMaterialIndex);

		const _json::_object* getNode(int iIndex);
		const _json::_object* getMesh(int iIndex);
		const _json::_object* getAccessor(int iIndex);
		const _json::_object* getBufferView(int iIndex);
		const _json::_object* getBuffer(int iIndex);
		const _json::_object* getMaterial(int iIndex);
		const _json::_object* getTexture(int iIndex);
		const _json::_object* getImage(int iIndex);

		virtual void createStringProperty(OwlInstance iInstance, const string& strProperty, const vector<const _json::_value*>& vecValues);
		virtual void createIntProperty(OwlInstance iInstance, const string& strProperty, const vector<const _json::_value*>& vecValues);
		virtual void createDoubleProperty(OwlInstance iInstance, const string& strProperty, const vector<const _json::_value*>& vecValues);
		virtual void createDoubleArrayProperty(OwlInstance iInstance, const string& strProperty, const _json::_array* pValues);

		OwlInstance createMaterialInstance(int iMaterialIndex);
		void saveTextureToFile(const string& strFile, const vector<uint8_t>& vecTextureData);
		void saveTextureToFile(const string& strFile, int iOffset, int iByteLength);

		virtual string getURI(const _json::_object* pBufferObject) const;
		virtual streampos getBINDataOffset() const;		

	private: // Methods

		void readIndices(int iIndicesAccessorIndex, vector<unsigned int>& vecIndices);
		void readVertices(int iPositionAccessorIndex, const string& strType, vector<float>& vecVertices);
		template<class T>
		void readBuffer(const string& strURI, streamoff iOffset, streamsize iBytesCount, vector<T>& vecValues)
		{
			VERIFY_STLOBJ_IS_NOT_EMPTY(strURI);
			VERIFY_EXPRESSION(iBytesCount > 0);
			VERIFY_STLOBJ_IS_EMPTY(vecValues);

			size_t iIndex = strURI.find("base64,");
			if (iIndex != string::npos) {
				string strBase64Content = strURI.substr(iIndex + string("base64,").size());
				VERIFY_STLOBJ_IS_NOT_EMPTY(strBase64Content);

				vector<BYTE> vecBuffer = base64_decode(strBase64Content);
				VERIFY_STLOBJ_IS_NOT_EMPTY(vecBuffer);

				vecValues.resize(iBytesCount / sizeof(T), T());
				memcpy(vecValues.data(), vecBuffer.data() + iOffset, iBytesCount);
			} // if (iIndex != string::npos)
			else {
				fs::path pthBinFile = m_strRootFolder;
				pthBinFile.append(strURI);

				std::ifstream binStream(pthBinFile.c_str(), std::ios::in | std::ios::binary);
				VERIFY_EXPRESSION(binStream.good());

				binStream.seekg(iOffset, ios::beg);

				vecValues.resize(iBytesCount / sizeof(T), T());
				binStream.read((char*)vecValues.data(), iBytesCount);
				binStream.close();
			} // else if (iIndex != string::npos)
		}

		template<class T1, class T2>
		void convertType(const vector<T1>& vecIn, vector<T2>& vecOut)
		{
			VERIFY_STLOBJ_IS_NOT_EMPTY(vecIn);
			VERIFY_STLOBJ_IS_EMPTY(vecOut);

			vecOut.resize(vecIn.size(), 0);
			for (size_t i = 0; i < vecIn.size(); i++) {
				vecOut[i] = (T2)vecIn[i];
			}
		}

		template<class T>
		void textureFlipY(vector<T>& vecInOut)
		{
			VERIFY_STLOBJ_IS_NOT_EMPTY(vecInOut);
			for (size_t i = 0; i < vecInOut.size(); i += 2) {
				vecInOut[i + 1] = -vecInOut[i + 1];
			}
		}
	};
};