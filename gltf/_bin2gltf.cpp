#include "_host.h"
#include "_bin2gltf.h"

#include "_base64.h"

// ************************************************************************************************
namespace _bin2gltf
{
	_exporter::_exporter(_model* pModel, const char* szOutputFile, bool bEmbeddedBuffers)
		: _log_client()
		, m_pModel(pModel)
		, m_bEmbeddedBuffers(bEmbeddedBuffers)
		, m_pPolygonsMaterial(nullptr)
		, m_vecMaterials()
		, m_mapMaterials()
		, m_mapImages()
		, m_vecNodes()
		, m_vecSceneRootNodes()
		, m_strOutputFolder("")
		, m_pOutputStream(nullptr)
		, m_iIndent(-1)
		, m_iBuffersCount(0)
		, m_iBufferViewsCount(0)
		, m_iMeshesCount(0)
	{
		VERIFY_POINTER(m_pModel);
		VERIFY_POINTER(szOutputFile);

		m_pPolygonsMaterial = new _material();
		addMaterial(m_pPolygonsMaterial);

		fs::path pthOutputFile = szOutputFile;
		m_strOutputFolder = pthOutputFile.parent_path().string();
		m_pOutputStream = new wofstream(szOutputFile, std::ios::out | std::ios::trunc);

		// UTF-8 locale
		std::locale loc(std::locale("C"), new std::codecvt_utf8<char>);
		m_pOutputStream->imbue(loc);
	}

	/*virtual*/ _exporter::~_exporter()
	{
		delete m_pPolygonsMaterial;

		for (auto pNode : m_vecNodes) {
			delete pNode;
		}

		delete m_pOutputStream;
	}

	void _exporter::execute()
	{
		if (!preExecute()) {
			getLog()->logWrite(enumLogEvent::error, "Invalid model.");

			return;
		}

		// {root
		indent()++;
		{
			writeStartObjectTag(false);

			indent()++;
			{
				// asset
				{
					writeAssetProperty();
					*m_pOutputStream << COMMA;
				}

				// buffers
				{
					writeBuffersProperty();
					*m_pOutputStream << COMMA;
				}

				// bufferViews
				{
					writeBufferViewsProperty();
					*m_pOutputStream << COMMA;
				}

				// accessors
				{
					writeAccessorsProperty();
					*m_pOutputStream << COMMA;
				}

				// meshes
				{
					writeMeshesProperty();
					*m_pOutputStream << COMMA;
				}

				// nodes
				{
					writeNodesProperty();
					*m_pOutputStream << COMMA;
				}

				// scene
				{
					writeSceneProperty();
					*m_pOutputStream << COMMA;
				}

				// scenes
				{
					writeScenesProperty();
					*m_pOutputStream << COMMA;
				}

				// materials
				{
					writeMaterials();
				}

				// images/textures
				{
					if (!m_mapImages.empty()) {
						*m_pOutputStream << COMMA;
						writeImages();
						*m_pOutputStream << COMMA;
						writeSamplers();
						*m_pOutputStream << COMMA;
						writeTextures();
					}
				}
			}
			indent()--;

			writeEndObjectTag();
			indent()--;
		}
		// root}

		getLog()->logWrite(enumLogEvent::info, _string::format("Buffers count: %d", m_iBuffersCount));
		getLog()->logWrite(enumLogEvent::info, _string::format("Buffer Views count: %d", m_iBufferViewsCount));
		getLog()->logWrite(enumLogEvent::info, _string::format("Meshes count: %d", m_iMeshesCount));

		postExecute();
	}

	/*virtual*/ bool _exporter::preExecute()
	{
		for (auto pGeometry : m_pModel->getGeometries()) {
			if (!pGeometry->isPlaceholder() && pGeometry->hasGeometry()) {
				auto pNode = new _node(pGeometry);
				m_vecNodes.push_back(pNode);
			}
		}

		return !m_vecNodes.empty();
	}

	/*virtual*/ void _exporter::postExecute()
	{
	}

	void _exporter::writeIndent()
	{
		for (int iTab = 0; iTab < m_iIndent; iTab++) {
			*m_pOutputStream << TAB;
		}
	}

	void _exporter::writeStartObjectTag(bool bNewLine/* = true*/)
	{
		if (bNewLine) {
			*getOutputStream() << "\n";
			writeIndent();
		}

		*getOutputStream() << "{";
	}

	void _exporter::writeEndObjectTag()
	{
		*getOutputStream() << "\n";
		writeIndent();
		*getOutputStream() << "}";
	}

	void _exporter::writeStartArrayTag(bool bNewLine/* = true*/)
	{
		if (bNewLine) {
			*getOutputStream() << "\n";
			writeIndent();
		}

		*getOutputStream() << "[";
	}

	void _exporter::writeEndArrayTag()
	{
		*getOutputStream() << "\n";
		writeIndent();
		*getOutputStream() << "]";
	}

	string _exporter::buildStringProperty(const string& strName, const string& strValue)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strName);
		VERIFY_STLOBJ_IS_NOT_EMPTY(strValue);

		string strProperty;

		strProperty += DOULE_QUOT_MARK;
		strProperty += strName.c_str();
		strProperty += DOULE_QUOT_MARK;
		strProperty += COLON;
		strProperty += SPACE;
		strProperty += DOULE_QUOT_MARK;
		strProperty += strValue.c_str();
		strProperty += DOULE_QUOT_MARK;

		return strProperty;
	}

	string _exporter::buildNumberProperty(const string& strName, const string& strValue)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strName);
		VERIFY_STLOBJ_IS_NOT_EMPTY(strValue);

		string strProperty;

		strProperty += DOULE_QUOT_MARK;
		strProperty += strName.c_str();
		strProperty += DOULE_QUOT_MARK;
		strProperty += COLON;
		strProperty += SPACE;
		strProperty += strValue.c_str();

		return strProperty;
	}

	string _exporter::buildArrayProperty(const string& strName, const vector<string>& vecValues)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strName);
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecValues);

		string strProperty;

		strProperty += DOULE_QUOT_MARK;
		strProperty += strName.c_str();
		strProperty += DOULE_QUOT_MARK;
		strProperty += COLON;
		strProperty += SPACE;

		strProperty += "[";
		for (size_t iIndex = 0; iIndex < vecValues.size(); iIndex++) {
			if (iIndex > 0) {
				strProperty += COMMA;
				strProperty += SPACE;
			}

			strProperty += vecValues[iIndex];
		}
		strProperty += "]";

		return strProperty;
	}

	void _exporter::writeStringProperty(const string& strName, const string& strValue)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strName);

		*getOutputStream() << "\n";
		writeIndent();

		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << strName.c_str();
		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << COLON;
		*m_pOutputStream << SPACE;
		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << strValue.c_str();
		*m_pOutputStream << DOULE_QUOT_MARK;
	}

	void _exporter::writeIntProperty(const string& strName, int iValue)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strName);

		*getOutputStream() << "\n";
		writeIndent();

		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << strName.c_str();
		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << COLON;
		*m_pOutputStream << SPACE;
		*m_pOutputStream << iValue;
	}

	void _exporter::writeUIntProperty(const string& strName, uint32_t iValue)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strName);

		*getOutputStream() << "\n";
		writeIndent();

		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << strName.c_str();
		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << COLON;
		*m_pOutputStream << SPACE;
		*m_pOutputStream << iValue;
	}

	void _exporter::writeFloatProperty(const string& strName, float fValue)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strName);

		*getOutputStream() << "\n";
		writeIndent();

		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << strName.c_str();
		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << COLON;
		*m_pOutputStream << SPACE;
		*m_pOutputStream << fValue;
	}

	void _exporter::writeDoubleProperty(const string& strName, double dValue)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strName);

		*getOutputStream() << "\n";
		writeIndent();

		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << strName.c_str();
		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << COLON;
		*m_pOutputStream << SPACE;
		*m_pOutputStream << dValue;
	}

	void _exporter::writeBoolProperty(const string& strName, bool bValue)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strName);

		*getOutputStream() << "\n";
		writeIndent();

		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << strName.c_str();
		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << COLON;
		*m_pOutputStream << SPACE;
		*m_pOutputStream << (bValue ? "true" : "false");
	}

	void _exporter::writeObjectProperty(const string& strName, const vector<string>& vecProperties)
	{
		VERIFY_STLOBJ_IS_NOT_EMPTY(strName);
		VERIFY_STLOBJ_IS_NOT_EMPTY(vecProperties);

		*getOutputStream() << "\n";
		writeIndent();

		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << strName.c_str();
		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << COLON;
		*m_pOutputStream << SPACE;

		writeStartObjectTag(false);

		indent()++;
		for (size_t iIndex = 0; iIndex < vecProperties.size(); iIndex++) {
			if (iIndex > 0) {
				*m_pOutputStream << COMMA;
			}

			*getOutputStream() << "\n";
			writeIndent();
			*m_pOutputStream << vecProperties[iIndex].c_str();
		}
		indent()--;

		writeEndObjectTag();
	}

	void _exporter::writeAssetProperty()
	{
		vector<string> vecProperties;
		vecProperties.push_back(buildStringProperty(GENERATOR_PROP, GENERATOR));
		vecProperties.push_back(buildStringProperty(VERSION_PROP, VERSION));

		writeObjectProperty(ASSET_PROP, vecProperties);
	}

	void _exporter::writeBuffersProperty()
	{
		*getOutputStream() << "\n";
		writeIndent();

		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << BUFFERS_PROP;
		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << COLON;
		*m_pOutputStream << SPACE;

		writeStartArrayTag(false);

		for (size_t iIndex = 0; iIndex < m_vecNodes.size(); iIndex++) {
			auto pNode = m_vecNodes[iIndex];

			const auto VERTEX_LENGTH = pNode->getGeometry()->getVertexLength();

			// buffer: byteLength
			uint32_t iBufferByteLength = 0;

			// buffer: uri
			string strBinFileName = _string::sformat("%lld.bin", getGeometryID(pNode->getGeometry()));
			fs::path pthNodeBinData = m_strOutputFolder;
			pthNodeBinData.append(strBinFileName);

			pNode->bufferBinFile() = strBinFileName;

			std::ostream* pNodeBinDataStream = nullptr;
			if (m_bEmbeddedBuffers) {
				pNodeBinDataStream = new std::stringstream();
			} else {
				pNodeBinDataStream = new std::ofstream();
				((std::ofstream*)pNodeBinDataStream)->open(pthNodeBinData.string(), std::ios::out | std::ios::binary | std::ios::trunc);
			}

			// vertices/POSITION
			for (int64_t iVertex = 0; iVertex < pNode->getGeometry()->getVerticesCount(); iVertex++) {
				float fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 0];
				pNodeBinDataStream->write(reinterpret_cast<const char*>(&fValue), sizeof(float));

				fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 1];
				pNodeBinDataStream->write(reinterpret_cast<const char*>(&fValue), sizeof(float));

				fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 2];
				pNodeBinDataStream->write(reinterpret_cast<const char*>(&fValue), sizeof(float));
			}

			pNode->verticesBufferViewByteLength() = (uint32_t)pNode->getGeometry()->getVerticesCount() * 3 * (uint32_t)sizeof(float);
			iBufferByteLength += pNode->verticesBufferViewByteLength();

			// vertices/NORMAL
			for (int64_t iVertex = 0; iVertex < pNode->getGeometry()->getVerticesCount(); iVertex++) {
				float fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 3];
				pNodeBinDataStream->write(reinterpret_cast<const char*>(&fValue), sizeof(float));

				fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 4];
				pNodeBinDataStream->write(reinterpret_cast<const char*>(&fValue), sizeof(float));

				fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 5];
				pNodeBinDataStream->write(reinterpret_cast<const char*>(&fValue), sizeof(float));
			}

			pNode->normalsBufferViewByteLength() = (uint32_t)pNode->getGeometry()->getVerticesCount() * 3 * (uint32_t)sizeof(float);
			iBufferByteLength += pNode->normalsBufferViewByteLength();

			// vertices/TEXCOORD_0
			for (int64_t iVertex = 0; iVertex < pNode->getGeometry()->getVerticesCount(); iVertex++) {
				float fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 6];
				pNodeBinDataStream->write(reinterpret_cast<const char*>(&fValue), sizeof(float));

				fValue = pNode->getGeometry()->getVertices()[(iVertex * VERTEX_LENGTH) + 7];
				pNodeBinDataStream->write(reinterpret_cast<const char*>(&fValue), sizeof(float));
			}

			pNode->texturesBufferViewByteLength() = (uint32_t)pNode->getGeometry()->getVerticesCount() * 2 * (uint32_t)sizeof(float);
			iBufferByteLength += pNode->texturesBufferViewByteLength();

			// Conceptual faces/indices
			for (auto pCohort : pNode->getGeometry()->concFacesCohorts()) {
				uint32_t iIndicesByteLength = (uint32_t)pCohort->indices().size() * (uint32_t)sizeof(GLuint);
				pNode->indicesBufferViewsByteLength().push_back(iIndicesByteLength);

				iBufferByteLength += iIndicesByteLength;

				for (size_t i = 0; i < pCohort->indices().size(); i++) {
					GLuint iIndexValue = pCohort->indices()[i];
					pNodeBinDataStream->write(reinterpret_cast<const char*>(&iIndexValue), sizeof(GLuint));
				}
			}

			// Conceptual faces polygons/indices
			for (auto pCohort : pNode->getGeometry()->concFacePolygonsCohorts()) {
				uint32_t iIndicesByteLength = (uint32_t)pCohort->indices().size() * (uint32_t)sizeof(GLuint);
				pNode->indicesBufferViewsByteLength().push_back(iIndicesByteLength);

				iBufferByteLength += iIndicesByteLength;

				for (size_t i = 0; i < pCohort->indices().size(); i++) {
					GLuint iIndexValue = pCohort->indices()[i];
					pNodeBinDataStream->write(reinterpret_cast<const char*>(&iIndexValue), sizeof(GLuint));
				}
			}

			// Lines
			for (auto pCohort : pNode->getGeometry()->linesCohorts()) {
				uint32_t iIndicesByteLength = (uint32_t)pCohort->indices().size() * (uint32_t)sizeof(GLuint);
				pNode->indicesBufferViewsByteLength().push_back(iIndicesByteLength);

				iBufferByteLength += iIndicesByteLength;

				for (size_t i = 0; i < pCohort->indices().size(); i++) {
					GLuint iIndexValue = pCohort->indices()[i];
					pNodeBinDataStream->write(reinterpret_cast<const char*>(&iIndexValue), sizeof(GLuint));
				}
			}

			// Points
			for (auto pCohort : pNode->getGeometry()->pointsCohorts()) {
				uint32_t iIndicesByteLength = (uint32_t)pCohort->indices().size() * (uint32_t)sizeof(GLuint);
				pNode->indicesBufferViewsByteLength().push_back(iIndicesByteLength);

				iBufferByteLength += iIndicesByteLength;

				for (size_t i = 0; i < pCohort->indices().size(); i++) {
					GLuint iIndexValue = pCohort->indices()[i];
					pNodeBinDataStream->write(reinterpret_cast<const char*>(&iIndexValue), sizeof(GLuint));
				}
			}

			pNode->bufferByteLength() = iBufferByteLength;

			if (iIndex > 0) {
				*m_pOutputStream << COMMA;
			}

			indent()++;
			writeStartObjectTag();

			indent()++;
			writeUIntProperty("byteLength", iBufferByteLength);
			*m_pOutputStream << COMMA;
			if (m_bEmbeddedBuffers) {
				std::string strBufferData = ((std::stringstream*)pNodeBinDataStream)->str();
				std::string strBase64BufferData = "data:application/octet-stream;base64,";
				strBase64BufferData += base64_encode(reinterpret_cast<const unsigned char*>(strBufferData.data()), (unsigned int)strBufferData.size());
				writeStringProperty("uri", strBase64BufferData);
			} else {
				writeStringProperty("uri", pthNodeBinData.stem().string() + ".bin");
			}
			indent()--;

			writeEndObjectTag();
			indent()--;

			delete pNodeBinDataStream;

			m_iBuffersCount++;
		} // for (for (size_t iIndex = ...

		writeEndArrayTag();
	}

	void _exporter::writeBufferViewsProperty()
	{
		*getOutputStream() << "\n";
		writeIndent();

		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << BUFFER_VIEWS_PROP;
		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << COLON;
		*m_pOutputStream << SPACE;

		writeStartArrayTag(false);

		// ARRAY_BUFFER/ELEMENT_ARRAY_BUFFER
		for (size_t iNodeIndex = 0; iNodeIndex < m_vecNodes.size(); iNodeIndex++) {
			auto pNode = m_vecNodes[iNodeIndex];

			assert(pNode->indicesBufferViewsByteLength().size() ==
				pNode->getGeometry()->concFacesCohorts().size() +
				pNode->getGeometry()->concFacePolygonsCohorts().size() +
				pNode->getGeometry()->linesCohorts().size() +
				pNode->getGeometry()->pointsCohorts().size());

			if (iNodeIndex > 0) {
				*m_pOutputStream << COMMA;
			}

			uint32_t iByteOffset = 0;

			// vertices/ARRAY_BUFFER/POSITION
			{
				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", (uint32_t)iNodeIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteLength", pNode->verticesBufferViewByteLength());
				*m_pOutputStream << COMMA;
				writeIntProperty("byteOffset", iByteOffset);
				*m_pOutputStream << COMMA;
				writeIntProperty("target", 34962/*ARRAY_BUFFER*/);
				indent()--;

				writeEndObjectTag();
				indent()--;

				iByteOffset += pNode->verticesBufferViewByteLength();
			}

			*m_pOutputStream << COMMA;

			// normals/ARRAY_BUFFER/NORMAL
			{
				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", (uint32_t)iNodeIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteLength", pNode->normalsBufferViewByteLength());
				*m_pOutputStream << COMMA;
				writeIntProperty("byteOffset", iByteOffset);
				*m_pOutputStream << COMMA;
				writeIntProperty("target", 34962/*ARRAY_BUFFER*/);
				indent()--;

				writeEndObjectTag();
				indent()--;

				iByteOffset += pNode->normalsBufferViewByteLength();
			}

			*m_pOutputStream << COMMA;

			// textures/ARRAY_BUFFER/TEXCOORD_0
			{
				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", (uint32_t)iNodeIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteLength", pNode->texturesBufferViewByteLength());
				*m_pOutputStream << COMMA;
				writeIntProperty("byteOffset", iByteOffset);
				*m_pOutputStream << COMMA;
				writeIntProperty("target", 34962/*ARRAY_BUFFER*/);
				indent()--;

				writeEndObjectTag();
				indent()--;

				iByteOffset += pNode->texturesBufferViewByteLength();
			}

			// indices/ELEMENT_ARRAY_BUFFER
			for (size_t iIndicesBufferViewIndex = 0; iIndicesBufferViewIndex < pNode->indicesBufferViewsByteLength().size(); iIndicesBufferViewIndex++) {
				uint32_t iByteLength = pNode->indicesBufferViewsByteLength()[iIndicesBufferViewIndex];

				*m_pOutputStream << COMMA;

				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("buffer", (uint32_t)iNodeIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteLength", iByteLength);
				*m_pOutputStream << COMMA;
				writeIntProperty("byteOffset", iByteOffset);
				*m_pOutputStream << COMMA;
				writeIntProperty("target", 34963/*ELEMENT_ARRAY_BUFFER*/);
				indent()--;

				writeEndObjectTag();
				indent()--;

				iByteOffset += iByteLength;
			} // for (size_t iIndicesBufferViewIndex = ...

			m_iBufferViewsCount++;
		} // for (size_t iNodeIndex = ...

		writeEndArrayTag();
	}

	void _exporter::writeAccessorsProperty()
	{
		_vector3d vecVertexBufferOffset;
		GetVertexBufferOffset(m_pModel->getOwlModel(), (double*)&vecVertexBufferOffset);

		double dScaleFactor = m_pModel->getOriginalBoundingSphereDiameter() / 2.;

		*getOutputStream() << "\n";
		writeIndent();

		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << ACCESSORS_PROP;
		*m_pOutputStream << DOULE_QUOT_MARK;
		*m_pOutputStream << COLON;
		*m_pOutputStream << SPACE;

		writeStartArrayTag(false);

		int iBufferViewIndex = 0;
		for (size_t iNodeIndex = 0; iNodeIndex < m_vecNodes.size(); iNodeIndex++) {
			auto pNode = m_vecNodes[iNodeIndex];

			assert(pNode->indicesBufferViewsByteLength().size() ==
				pNode->getGeometry()->concFacesCohorts().size() +
				pNode->getGeometry()->concFacePolygonsCohorts().size() +
				pNode->getGeometry()->linesCohorts().size() +
				pNode->getGeometry()->pointsCohorts().size());

			if (iNodeIndex > 0) {
				*m_pOutputStream << COMMA;
			}

			// vertices/ARRAY_BUFFER/POSITION
			{
				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("bufferView", (uint32_t)iBufferViewIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteOffset", 0);
				*m_pOutputStream << COMMA;
				writeUIntProperty("componentType", 5126/*FLOAT*/);
				*m_pOutputStream << COMMA;
				writeUIntProperty("count", (uint32_t)pNode->getGeometry()->getVerticesCount());
				*m_pOutputStream << COMMA;
				*getOutputStream() << "\n";
				writeIndent();
				*getOutputStream() << buildArrayProperty("min", vector<string>
				{
					to_string((pNode->getGeometry()->getBBMin()->x + vecVertexBufferOffset.x) / dScaleFactor),
						to_string((pNode->getGeometry()->getBBMin()->y + vecVertexBufferOffset.y) / dScaleFactor),
						to_string((pNode->getGeometry()->getBBMin()->z + vecVertexBufferOffset.z) / dScaleFactor)
				}).c_str();
				*m_pOutputStream << COMMA;
				*getOutputStream() << "\n";
				writeIndent();
				*getOutputStream() << buildArrayProperty("max", vector<string>
				{
					to_string((pNode->getGeometry()->getBBMax()->x + vecVertexBufferOffset.x) / dScaleFactor),
						to_string((pNode->getGeometry()->getBBMax()->y + vecVertexBufferOffset.y) / dScaleFactor),
						to_string((pNode->getGeometry()->getBBMax()->z + vecVertexBufferOffset.z) / dScaleFactor)
				}).c_str();
				*m_pOutputStream << COMMA;
				writeStringProperty("type", "VEC3");
				indent()--;

				writeEndObjectTag();
				indent()--;

				pNode->accessors().push_back(iBufferViewIndex);

				iBufferViewIndex++;
			}
			// vertices/ARRAY_BUFFER/POSITION

			*m_pOutputStream << COMMA;

			// normals/ARRAY_BUFFER/NORMAL
			{
				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("bufferView", (uint32_t)iBufferViewIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteOffset", 0);
				*m_pOutputStream << COMMA;
				writeUIntProperty("componentType", 5126/*FLOAT*/);
				*m_pOutputStream << COMMA;
				writeUIntProperty("count", (uint32_t)pNode->getGeometry()->getVerticesCount());
				*m_pOutputStream << COMMA;
				writeStringProperty("type", "VEC3");
				indent()--;

				writeEndObjectTag();
				indent()--;

				pNode->accessors().push_back(iBufferViewIndex);

				iBufferViewIndex++;
			}
			// normals/ARRAY_BUFFER/NORMAL

			*m_pOutputStream << COMMA;

			// normals/ARRAY_BUFFER/TEXCOORD_0
			{
				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("bufferView", (uint32_t)iBufferViewIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteOffset", 0);
				*m_pOutputStream << COMMA;
				writeUIntProperty("componentType", 5126/*FLOAT*/);
				*m_pOutputStream << COMMA;
				writeUIntProperty("count", (uint32_t)pNode->getGeometry()->getVerticesCount());
				*m_pOutputStream << COMMA;
				*getOutputStream() << "\n";
				writeIndent();
				*getOutputStream() << buildArrayProperty("min", vector<string> { "-1.0", "-1.0" }).c_str();
				*m_pOutputStream << COMMA;
				*getOutputStream() << "\n";
				writeIndent();
				*getOutputStream() << buildArrayProperty("max", vector<string> { "1.0", "1.0" }).c_str();
				*m_pOutputStream << COMMA;
				writeStringProperty("type", "VEC2");
				indent()--;

				writeEndObjectTag();
				indent()--;

				pNode->accessors().push_back(iBufferViewIndex);

				iBufferViewIndex++;
			}
			// normals/ARRAY_BUFFER/NORMAL

			// indices/ELEMENT_ARRAY_BUFFER

			// Conceptual faces
			for (auto pCohort : pNode->getGeometry()->concFacesCohorts()) {
				*m_pOutputStream << COMMA;

				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("bufferView", (uint32_t)iBufferViewIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteOffset", 0);
				*m_pOutputStream << COMMA;
				writeUIntProperty("componentType", 5125/*UNSIGNED_INT*/);
				*m_pOutputStream << COMMA;
				writeUIntProperty("count", (uint32_t)pCohort->indices().size());
				*m_pOutputStream << COMMA;
				writeStringProperty("type", "SCALAR");
				indent()--;

				writeEndObjectTag();
				indent()--;

				pNode->accessors().push_back(iBufferViewIndex);

				iBufferViewIndex++;
			}

			// Conceptual faces polygons
			for (auto pCohort : pNode->getGeometry()->concFacePolygonsCohorts()) {
				*m_pOutputStream << COMMA;

				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("bufferView", (uint32_t)iBufferViewIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteOffset", 0);
				*m_pOutputStream << COMMA;
				writeUIntProperty("componentType", 5125/*UNSIGNED_INT*/);
				*m_pOutputStream << COMMA;
				writeUIntProperty("count", (uint32_t)pCohort->indices().size());
				*m_pOutputStream << COMMA;
				writeStringProperty("type", "SCALAR");
				indent()--;

				writeEndObjectTag();
				indent()--;

				pNode->accessors().push_back(iBufferViewIndex);

				iBufferViewIndex++;
			}

			// Lines
			for (auto pCohort : pNode->getGeometry()->linesCohorts()) {
				*m_pOutputStream << COMMA;

				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("bufferView", (uint32_t)iBufferViewIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteOffset", 0);
				*m_pOutputStream << COMMA;
				writeUIntProperty("componentType", 5125/*UNSIGNED_INT*/);
				*m_pOutputStream << COMMA;
				writeUIntProperty("count", (uint32_t)pCohort->indices().size());
				*m_pOutputStream << COMMA;
				writeStringProperty("type", "SCALAR");
				indent()--;

				writeEndObjectTag();
				indent()--;

				pNode->accessors().push_back(iBufferViewIndex);

				iBufferViewIndex++;
			}

			// Points
			for (auto pCohort : pNode->getGeometry()->pointsCohorts()) {
				*m_pOutputStream << COMMA;

				indent()++;
				writeStartObjectTag();

				indent()++;
				writeUIntProperty("bufferView", (uint32_t)iBufferViewIndex);
				*m_pOutputStream << COMMA;
				writeUIntProperty("byteOffset", 0);
				*m_pOutputStream << COMMA;
				writeUIntProperty("componentType", 5125/*UNSIGNED_INT*/);
				*m_pOutputStream << COMMA;
				writeUIntProperty("count", (uint32_t)pCohort->indices().size());
				*m_pOutputStream << COMMA;
				writeStringProperty("type", "SCALAR");
				indent()--;

				writeEndObjectTag();
				indent()--;

				pNode->accessors().push_back(iBufferViewIndex);

				iBufferViewIndex++;
			}
		} // for (size_t iNodeIndex = ...

		writeEndArrayTag();
	}

	void _exporter::writeMeshesProperty()
	{
		*getOutputStream() << "\n";
		writeIndent();

		// meshes
		{
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << MESHES_PROP;
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << COLON;
			*m_pOutputStream << SPACE;

			writeStartArrayTag(false);

			uint32_t iMeshIndex = 0;
			for (size_t iNodeIndex = 0; iNodeIndex < m_vecNodes.size(); iNodeIndex++) {
				auto pNode = m_vecNodes[iNodeIndex];

				assert(pNode->accessors().size() ==
					3/*vertices, normals & textures bufferView-s*/ +
					pNode->getGeometry()->concFacesCohorts().size() +
					pNode->getGeometry()->concFacePolygonsCohorts().size() +
					pNode->getGeometry()->linesCohorts().size() +
					pNode->getGeometry()->pointsCohorts().size());

				// Conceptual faces
				for (size_t iConcFacesCohortIndex = 0; iConcFacesCohortIndex < pNode->getGeometry()->concFacesCohorts().size(); iConcFacesCohortIndex++) {
					if (iMeshIndex > 0) {
						*m_pOutputStream << COMMA;
					}

					// mesh
					{
						indent()++;
						writeStartObjectTag();

						indent()++;
						writeStringProperty("name", _string::sformat("%lld-conceptual-face-%lld", getGeometryID(pNode->getGeometry()), iConcFacesCohortIndex));
						*m_pOutputStream << COMMA;

						*getOutputStream() << "\n";
						writeIndent();

						*m_pOutputStream << DOULE_QUOT_MARK;
						*m_pOutputStream << "primitives";
						*m_pOutputStream << DOULE_QUOT_MARK;
						*m_pOutputStream << COLON;
						*m_pOutputStream << SPACE;

						writeStartArrayTag(false);

						// attributes
						{
							indent()++;
							writeStartObjectTag();

							// POSITION, NORMAL, etc.
							{
								indent()++;

								auto pMaterial = pNode->getGeometry()->concFacesCohorts()[iConcFacesCohortIndex]->getMaterial();
								size_t iMaterialIndex = addMaterial(pMaterial);

								vector<string> vecAttributes;
								vecAttributes.push_back(buildNumberProperty("POSITION", to_string(pNode->accessors()[0])));
								vecAttributes.push_back(buildNumberProperty("NORMAL", to_string(pNode->accessors()[1])));
								if (pMaterial->hasTexture()) {
									vecAttributes.push_back(buildNumberProperty("TEXCOORD_0", to_string(pNode->accessors()[2])));
								}
								writeObjectProperty("attributes", vecAttributes);

								*m_pOutputStream << COMMA;
								writeUIntProperty("indices", pNode->accessors()[
									iConcFacesCohortIndex +
										3/*skip vertices, normals & textures accessor-s*/]);
								*m_pOutputStream << COMMA;
								writeUIntProperty("material", (uint32_t)iMaterialIndex);
								*m_pOutputStream << COMMA;
								writeUIntProperty("mode", 4/*TRIANGLES*/);

								indent()--;
							}

							writeEndObjectTag();
							indent()--;
						}
						// attributes

						writeEndArrayTag();
						indent()--;

						writeEndObjectTag();
						indent()--;
					}
					// mesh					

					pNode->meshes().push_back(iMeshIndex);

					iMeshIndex++;

					m_iMeshesCount++;
				} // for (size_t iConcFacesCohortIndex = ...

				// Conceptual faces polygons
				for (size_t iConcFacePolygonsCohortIndex = 0; iConcFacePolygonsCohortIndex < pNode->getGeometry()->concFacePolygonsCohorts().size(); iConcFacePolygonsCohortIndex++) {
					if (iMeshIndex > 0) {
						*m_pOutputStream << COMMA;
					}

					// mesh
					{
						indent()++;
						writeStartObjectTag();

						indent()++;
						writeStringProperty("name", _string::sformat("%lld-conceptual-face-polygons-%lld", getGeometryID(pNode->getGeometry()), iConcFacePolygonsCohortIndex));
						*m_pOutputStream << COMMA;

						*getOutputStream() << "\n";
						writeIndent();

						*m_pOutputStream << DOULE_QUOT_MARK;
						*m_pOutputStream << "primitives";
						*m_pOutputStream << DOULE_QUOT_MARK;
						*m_pOutputStream << COLON;
						*m_pOutputStream << SPACE;

						writeStartArrayTag(false);

						// attributes
						{
							indent()++;
							writeStartObjectTag();

							// POSITION
							{
								indent()++;

								vector<string> vecAttributes;
								vecAttributes.push_back(buildNumberProperty("POSITION", to_string(pNode->accessors()[0])));
								writeObjectProperty("attributes", vecAttributes);

								*m_pOutputStream << COMMA;
								writeUIntProperty("indices", pNode->accessors()[
									iConcFacePolygonsCohortIndex +
										3/*skip vertices, normals & textures accessor-s*/ +
										pNode->getGeometry()->concFacesCohorts().size()]);
								*m_pOutputStream << COMMA;
								writeUIntProperty("material", 0);
								*m_pOutputStream << COMMA;
								writeUIntProperty("mode", 1/*LINES*/);

								indent()--;
							}

							writeEndObjectTag();
							indent()--;
						}
						// attributes

						writeEndArrayTag();
						indent()--;

						writeEndObjectTag();
						indent()--;
					}
					// mesh					

					pNode->meshes().push_back(iMeshIndex);

					iMeshIndex++;

					m_iMeshesCount++;
				} // for (size_t iConcFacePolygonsCohortIndex = ...

				// Lines
				for (size_t iLinesCohortIndex = 0; iLinesCohortIndex < pNode->getGeometry()->linesCohorts().size(); iLinesCohortIndex++) {
					if (iMeshIndex > 0) {
						*m_pOutputStream << COMMA;
					}

					// mesh
					{
						indent()++;
						writeStartObjectTag();

						indent()++;
						writeStringProperty("name", _string::sformat("%lld-lines-%lld", getGeometryID(pNode->getGeometry()), iLinesCohortIndex));
						*m_pOutputStream << COMMA;

						*getOutputStream() << "\n";
						writeIndent();

						*m_pOutputStream << DOULE_QUOT_MARK;
						*m_pOutputStream << "primitives";
						*m_pOutputStream << DOULE_QUOT_MARK;
						*m_pOutputStream << COLON;
						*m_pOutputStream << SPACE;

						writeStartArrayTag(false);

						// attributes
						{
							indent()++;
							writeStartObjectTag();

							// POSITION
							{
								indent()++;

								auto pMaterial = pNode->getGeometry()->linesCohorts()[iLinesCohortIndex]->getMaterial();
								size_t iMaterialIndex = addMaterial(pMaterial);

								vector<string> vecAttributes;
								vecAttributes.push_back(buildNumberProperty("POSITION", to_string(pNode->accessors()[0])));
								writeObjectProperty("attributes", vecAttributes);

								*m_pOutputStream << COMMA;
								writeUIntProperty("indices", pNode->accessors()[
									iLinesCohortIndex +
										3/*skip vertices, normals & textures accessor-s*/ +
										pNode->getGeometry()->concFacesCohorts().size() +
										pNode->getGeometry()->concFacePolygonsCohorts().size()]);
								*m_pOutputStream << COMMA;
								writeUIntProperty("material", (uint32_t)iMaterialIndex);
								*m_pOutputStream << COMMA;
								writeUIntProperty("mode", 1/*LINES*/);

								indent()--;
							}

							writeEndObjectTag();
							indent()--;
						}
						// attributes

						writeEndArrayTag();
						indent()--;

						writeEndObjectTag();
						indent()--;
					}
					// mesh					

					pNode->meshes().push_back(iMeshIndex);

					iMeshIndex++;

					m_iMeshesCount++;
				} // for (size_t iLinesCohortIndex = ...

				// Points
				for (size_t iPointsCohortIndex = 0; iPointsCohortIndex < pNode->getGeometry()->pointsCohorts().size(); iPointsCohortIndex++) {
					if (iMeshIndex > 0) {
						*m_pOutputStream << COMMA;
					}

					// mesh
					{
						indent()++;
						writeStartObjectTag();

						indent()++;
						writeStringProperty("name", _string::sformat("%lld-points-%lld", getGeometryID(pNode->getGeometry()), iPointsCohortIndex));
						*m_pOutputStream << COMMA;

						*getOutputStream() << "\n";
						writeIndent();

						*m_pOutputStream << DOULE_QUOT_MARK;
						*m_pOutputStream << "primitives";
						*m_pOutputStream << DOULE_QUOT_MARK;
						*m_pOutputStream << COLON;
						*m_pOutputStream << SPACE;

						writeStartArrayTag(false);

						// attributes
						{
							indent()++;
							writeStartObjectTag();

							// POSITION
							{
								indent()++;

								auto pMaterial = pNode->getGeometry()->pointsCohorts()[iPointsCohortIndex]->getMaterial();
								size_t iMaterialIndex = addMaterial(pMaterial);

								vector<string> vecAttributes;
								vecAttributes.push_back(buildNumberProperty("POSITION", to_string(pNode->accessors()[0])));
								writeObjectProperty("attributes", vecAttributes);

								*m_pOutputStream << COMMA;
								writeUIntProperty("indices", pNode->accessors()[
									iPointsCohortIndex +
										3/*skip vertices, normals & textures accessor-s*/ +
										pNode->getGeometry()->concFacesCohorts().size() +
										pNode->getGeometry()->concFacePolygonsCohorts().size() +
										pNode->getGeometry()->linesCohorts().size()]);
								*m_pOutputStream << COMMA;
								writeUIntProperty("material", (uint32_t)iMaterialIndex);
								*m_pOutputStream << COMMA;
								writeUIntProperty("mode", 0/*POINTS*/);

								indent()--;
							}

							writeEndObjectTag();
							indent()--;
						}
						// attributes

						writeEndArrayTag();
						indent()--;

						writeEndObjectTag();
						indent()--;
					}
					// mesh					

					pNode->meshes().push_back(iMeshIndex);

					iMeshIndex++;

					m_iMeshesCount++;
				} // for (size_t iPointsCohortIndex = ...
			} // for (size_t iNodeIndex = ...

			writeEndArrayTag();
		}
		// meshes
	}

	void _exporter::writeNodesProperty()
	{
		*getOutputStream() << "\n";
		writeIndent();

		// nodes
		{
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << NODES_PROP;
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << COLON;
			*m_pOutputStream << SPACE;

			writeStartArrayTag(false);

			uint32_t iSceneNodeIndex = 0;
			for (size_t iNodeIndex = 0; iNodeIndex < m_vecNodes.size(); iNodeIndex++) {
				auto pNode = m_vecNodes[iNodeIndex];

				assert(pNode->meshes().size() ==
					pNode->getGeometry()->concFacesCohorts().size() +
					pNode->getGeometry()->concFacePolygonsCohorts().size() +
					pNode->getGeometry()->linesCohorts().size() +
					pNode->getGeometry()->pointsCohorts().size());
				assert(!pNode->getGeometry()->getInstances().empty());

				//
				// Transformations
				// 
				for (auto pInstance : pNode->getGeometry()->getInstances()) {
					if (!pInstance->getEnable()) {
						continue;
					}

					auto pTransformation = pInstance->getTransformationMatrix();

					if (iSceneNodeIndex > 0) {
						*m_pOutputStream << COMMA;
					}

					// root
					{
						m_vecSceneRootNodes.push_back(iSceneNodeIndex);

						vector<string> vecNodeChildren;
						for (size_t iMeshIndex = 0; iMeshIndex < pNode->meshes().size(); iMeshIndex++) {
							vecNodeChildren.push_back(to_string(++iSceneNodeIndex));
						}

						indent()++;
						writeStartObjectTag();

						indent()++;
						writeStringProperty("name", _string::sformat("%lld-instance", getGeometryID(pNode->getGeometry())));
						*m_pOutputStream << COMMA;
						*getOutputStream() << "\n";
						writeIndent();
						*getOutputStream() << buildArrayProperty("children", vecNodeChildren).c_str();
						*m_pOutputStream << COMMA;
						*getOutputStream() << "\n";
						writeIndent();
						*getOutputStream() << buildArrayProperty("matrix", vector<string>
						{
							to_string(pTransformation->_11),
								to_string(pTransformation->_12),
								to_string(pTransformation->_13),
								to_string(pTransformation->_14),
								to_string(pTransformation->_21),
								to_string(pTransformation->_22),
								to_string(pTransformation->_23),
								to_string(pTransformation->_24),
								to_string(pTransformation->_31),
								to_string(pTransformation->_32),
								to_string(pTransformation->_33),
								to_string(pTransformation->_34),
								to_string(pTransformation->_41),
								to_string(pTransformation->_42),
								to_string(pTransformation->_43),
								to_string(pTransformation->_44)
						}).c_str();
						indent()--;

						writeEndObjectTag();
						indent()--;
					}
					// root

					// children
					{
						for (size_t iMeshIndex = 0; iMeshIndex < pNode->meshes().size(); iMeshIndex++) {
							*m_pOutputStream << COMMA;

							indent()++;
							writeStartObjectTag();

							indent()++;
							writeStringProperty("name", _string::sformat("%lld-conceptual-face-%lld", getGeometryID(pNode->getGeometry()), iMeshIndex));
							*m_pOutputStream << COMMA;
							writeUIntProperty("mesh", pNode->meshes()[iMeshIndex]);
							indent()--;

							writeEndObjectTag();
							indent()--;
						}
					}
					// children

					// next root
					iSceneNodeIndex++;
				} //for (size_t iTransformation = ...
			} // for (size_t iNodeIndex = ...

			writeEndArrayTag();
		}
		// nodes
	}

	void _exporter::writeSceneProperty()
	{
		writeIntProperty(SCENE_PROP, 0);
	}

	void _exporter::writeScenesProperty()
	{
		*getOutputStream() << "\n";
		writeIndent();

		// scenes
		{
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << SCENES_PROP;
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << COLON;
			*m_pOutputStream << SPACE;

			// [
			{
				writeStartArrayTag(false);

				indent()++;
				writeStartObjectTag();

				vector<string> vecRootNodes;
				for (size_t iRootNodeIndex = 0; iRootNodeIndex < m_vecSceneRootNodes.size(); iRootNodeIndex++) {
					vecRootNodes.push_back(to_string(m_vecSceneRootNodes[iRootNodeIndex]));
				}

				*getOutputStream() << "\n";
				indent()++;
				writeIndent();
				*getOutputStream() << buildArrayProperty("nodes", vecRootNodes).c_str();
				indent()--;

				writeEndObjectTag();
				indent()--;

				writeEndArrayTag();
			}
			// ]			
		}
		// scenes
	}

	void _exporter::writeMaterials()
	{
		*getOutputStream() << "\n";
		writeIndent();

		// materials
		{
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << MATERIALS_PROP;
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << COLON;
			*m_pOutputStream << SPACE;

			// [
			{
				writeStartArrayTag(false);

				for (size_t iMaterialIndex = 0; iMaterialIndex < m_vecMaterials.size(); iMaterialIndex++) {
					auto pMaterial = m_vecMaterials[iMaterialIndex];

					if (iMaterialIndex > 0) {
						*m_pOutputStream << COMMA;
					}

					if (pMaterial->hasTexture()) {
						uint32_t iImageIndex = (uint32_t)-1;

						wstring strTexture = pMaterial->texture();

						// \ => /
						size_t iIndex = 0;
						while (true) {
							iIndex = strTexture.find(L"\\", iIndex);
							if (iIndex == string::npos) {
								break;
							}

							strTexture.replace(iIndex, wcslen(L"\\"), L"/");

							iIndex += wcslen(L"/");
						}

						auto itImage = m_mapImages.find(strTexture);
						if (itImage == m_mapImages.end()) {
							iImageIndex = (uint32_t)m_mapImages.size();
							m_mapImages[strTexture] = iImageIndex;
						} else {
							iImageIndex = itImage->second;
						}

						// texture
						{
							indent()++;
							writeStartObjectTag();

							indent()++;
							*getOutputStream() << "\n";
							writeIndent();
							*m_pOutputStream << DOULE_QUOT_MARK;
							*m_pOutputStream << "pbrMetallicRoughness";
							*m_pOutputStream << DOULE_QUOT_MARK;
							*m_pOutputStream << COLON;
							*m_pOutputStream << SPACE;

							// pbrMetallicRoughness
							writeStartObjectTag();

							// baseColorTexture
							{
								indent()++;
								writeObjectProperty("baseColorTexture", vector<string>{ buildNumberProperty("index", to_string(iImageIndex)) });
								indent()--;
							}
							// baseColorTexture

							writeEndObjectTag();
							indent()--;

							*m_pOutputStream << COMMA;

							// doubleSided
							{
								indent()++;
								writeBoolProperty("doubleSided", true);
								indent()--;
							}
							// doubleSided

							writeEndObjectTag();
							indent()--;
							// pbrMetallicRoughness							
						}
						// texture
					} else {
						// material					
						indent()++;
						writeStartObjectTag();

						// pbrMetallicRoughness
						indent()++;
						*getOutputStream() << "\n";
						writeIndent();
						*m_pOutputStream << DOULE_QUOT_MARK;
						*m_pOutputStream << "pbrMetallicRoughness";
						*m_pOutputStream << DOULE_QUOT_MARK;
						*m_pOutputStream << COLON;
						*m_pOutputStream << SPACE;

						// baseColorFactor						
						{
							writeStartObjectTag();

							*getOutputStream() << "\n";
							indent()++;
							writeIndent();
							*getOutputStream() << buildArrayProperty("baseColorFactor", vector<string>
							{
								to_string(pMaterial->getAmbientColor().r()),
									to_string(pMaterial->getAmbientColor().g()),
									to_string(pMaterial->getAmbientColor().b()),
									to_string(pMaterial->getA()),
							}).c_str();
							indent()--;
							writeEndObjectTag();
						}
						// baseColorFactor

						*m_pOutputStream << COMMA;

						// emissiveFactor
						{
							*getOutputStream() << "\n";
							writeIndent();
							*getOutputStream() << buildArrayProperty("emissiveFactor", vector<string>
							{
								to_string(pMaterial->getEmissiveColor().r()),
									to_string(pMaterial->getEmissiveColor().g()),
									to_string(pMaterial->getEmissiveColor().b())
							}).c_str();
						}
						// emissiveFactor					

						*m_pOutputStream << COMMA;

						// doubleSided
						{
							writeBoolProperty("doubleSided", true);
						}
						// doubleSided

						// alphaMode: OPAQUE/BLEND
						{
							if (pMaterial->getA() < 1.f) {
								*m_pOutputStream << COMMA;
								writeStringProperty("alphaMode", "BLEND");
							}
						}
						// alphaMode: OPAQUE/BLEND

						indent()--;
						writeEndObjectTag();

						indent()--;
						// material
					}
				} // for (size_t iMaterialIndex = ...

				writeEndArrayTag();
			}
			// ]			
		}
		// materials
	}

	void _exporter::writeImages()
	{
		*getOutputStream() << "\n";
		writeIndent();

		// images
		{
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << IMAGES_PROP;
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << COLON;
			*m_pOutputStream << SPACE;

			// [
			{
				writeStartArrayTag(false);

				map<uint32_t, wstring> mapImageURIs;
				for (auto itImage : m_mapImages) {
					mapImageURIs[itImage.second] = itImage.first;
				}

				uint32_t iIndex = 0;
				for (auto itImageURI : mapImageURIs) {
					if (iIndex++ > 0) {
						*m_pOutputStream << COMMA;
					}

					// image
					{
						indent()++;
						writeStartObjectTag();

						// uri
						{
							indent()++;
							writeStringProperty("uri", CW2A(itImageURI.second.c_str()).m_psz);
							indent()--;
						}

						writeEndObjectTag();
						indent()--;
					}
					// image
				}

				writeEndArrayTag();
			}
			// ]
		}
		// images
	}

	void _exporter::writeSamplers()
	{
		*getOutputStream() << "\n";
		writeIndent();

		// samplers
		{
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << SAMPLERS_PROP;
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << COLON;
			*m_pOutputStream << SPACE;

			// [
			{
				writeStartArrayTag(false);

				*getOutputStream() << "\n";
				writeIndent();
				*getOutputStream() << "{}";

				writeEndArrayTag();
			}
			// ]
		}
		// samplers
	}

	void _exporter::writeTextures()
	{
		*getOutputStream() << "\n";
		writeIndent();

		// images
		{
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << TEXTURES_PROP;
			*m_pOutputStream << DOULE_QUOT_MARK;
			*m_pOutputStream << COLON;
			*m_pOutputStream << SPACE;

			// [
			{
				writeStartArrayTag(false);

				map<uint32_t, wstring> mapTextureSource;
				for (auto itImage : m_mapImages) {
					mapTextureSource[itImage.second] = itImage.first;
				}

				uint32_t iIndex = 0;
				for (auto itTextureSource : mapTextureSource) {
					if (iIndex++ > 0) {
						*m_pOutputStream << COMMA;
					}

					// image
					{
						indent()++;
						writeStartObjectTag();

						// sampler, source
						{
							indent()++;
							writeUIntProperty("sampler", 0);
							*m_pOutputStream << COMMA;
							writeUIntProperty("source", itTextureSource.first);
							indent()--;
						}

						writeEndObjectTag();
						indent()--;
					}
					// image
				}

				writeEndArrayTag();
			}
			// ]
		}
		// images
	}

	size_t _exporter::addMaterial(const _material* pMaterial)
	{
		VERIFY_POINTER(pMaterial);

		auto itMaterial = m_mapMaterials.find(*pMaterial);
		if (itMaterial != m_mapMaterials.end()) {
			return itMaterial->second;
		}

		size_t iMaterialIndex = m_vecMaterials.size();

		m_mapMaterials.insert(pair<_material, size_t>(*pMaterial, iMaterialIndex));
		m_vecMaterials.push_back(pMaterial);

		return iMaterialIndex;
	}
};
