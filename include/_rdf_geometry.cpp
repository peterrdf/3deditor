#include "_host.h"
#include "_rdf_geometry.h"
#include "_oglUtils.h"
#include <bitset>

// ************************************************************************************************
_rdf_geometry::_rdf_geometry(OwlInstance owlInstance)
	: _geometry(owlInstance)
	, m_pOriginalVertexBuffer(nullptr)
{
	calculate();

	loadName();
}

/*virtual*/ _rdf_geometry::~_rdf_geometry()
{
}

/*virtual*/ void _rdf_geometry::preCalculate() /*override*/
{
	setRDFFormatSettings();
}

/*virtual*/ void _rdf_geometry::calculateCore() /*override*/
{
	const auto VERTEX_LENGTH = getVertexLength();

	assert(m_pOriginalVertexBuffer == nullptr);
	m_pOriginalVertexBuffer = new _vertices_f(VERTEX_LENGTH);

	assert(m_pVertexBuffer == nullptr);

	assert(m_pIndexBuffer == nullptr);
	m_pIndexBuffer = new _indices_i32();

	/* BBs/AABBs */

	assert(m_pmtxOriginalBBTransformation == nullptr);
	m_pmtxOriginalBBTransformation = new _matrix();

	assert(m_pvecOriginalBBMin == nullptr);
	m_pvecOriginalBBMin = new _vector3d();

	assert(m_pvecOriginalBBMax == nullptr);
	m_pvecOriginalBBMax = new _vector3d();

	assert(m_pmtxBBTransformation == nullptr);
	m_pmtxBBTransformation = new _matrix();

	assert(m_pvecBBMin == nullptr);
	m_pvecBBMin = new _vector3d();

	assert(m_pvecBBMax == nullptr);
	m_pvecBBMax = new _vector3d();

	assert(m_pvecAABBMin == nullptr);
	m_pvecAABBMin = new _vector3d();

	assert(m_pvecAABBMax == nullptr);
	m_pvecAABBMax = new _vector3d();

	if (!calculateInstance(m_pOriginalVertexBuffer, m_pIndexBuffer)) {
		return;
	}

	m_pVertexBuffer = new _vertices_f(VERTEX_LENGTH);
	m_pVertexBuffer->size() = m_pOriginalVertexBuffer->size();
	m_pVertexBuffer->data() = new float[(uint32_t)m_pOriginalVertexBuffer->size() * (int64_t)VERTEX_LENGTH];
	memcpy(m_pVertexBuffer->data(), m_pOriginalVertexBuffer->data(), (uint32_t)m_pOriginalVertexBuffer->size() * (int64_t)VERTEX_LENGTH * sizeof(float));

	GetBoundingBox(
		getOwlInstance(),
		(double*)m_pmtxOriginalBBTransformation,
		(double*)m_pvecOriginalBBMin,
		(double*)m_pvecOriginalBBMax);

	memcpy(m_pmtxBBTransformation, m_pmtxOriginalBBTransformation, sizeof(_matrix));
	memcpy(m_pvecBBMin, m_pvecOriginalBBMin, sizeof(_vector3d));
	memcpy(m_pvecBBMax, m_pvecOriginalBBMax, sizeof(_vector3d));

	GetBoundingBox(
		getOwlInstance(),
		(double*)m_pvecAABBMin,
		(double*)m_pvecAABBMax);

	double arOffset[3];
	GetVertexBufferOffset(getOwlModel(), arOffset);

	m_pvecAABBMin->x += arOffset[0];
	m_pvecAABBMin->y += arOffset[1];
	m_pvecAABBMin->z += arOffset[2];

	m_pvecAABBMax->x += arOffset[0];
	m_pvecAABBMax->y += arOffset[1];
	m_pvecAABBMax->z += arOffset[2];

	MATERIALS mapMaterial2ConcFaces;
	MATERIALS mapMaterial2ConcFaceLines;
	MATERIALS mapMaterial2ConcFacePoints;

	m_iConceptualFacesCount = GetConceptualFaceCnt(getOwlInstance());
	for (int64_t iConceptualFaceIndex = 0; iConceptualFaceIndex < m_iConceptualFacesCount; iConceptualFaceIndex++) {
		int64_t iStartIndexTriangles = 0;
		int64_t iIndicesCountTriangles = 0;
		int64_t iStartIndexLines = 0;
		int64_t iIndicesCountLines = 0;
		int64_t iStartIndexPoints = 0;
		int64_t iIndicesCountPoints = 0;
		int64_t iStartIndexFacePolygons = 0;
		int64_t iIndicesCountFacePolygons = 0;
		int64_t iStartIndexConceptualFacePolygons = 0;
		int64_t iIndicesCountConceptualFacePolygons = 0;

		ConceptualFace iConceptualFace = GetConceptualFace(
			getOwlInstance(),
			iConceptualFaceIndex,
			&iStartIndexTriangles, &iIndicesCountTriangles,
			&iStartIndexLines, &iIndicesCountLines,
			&iStartIndexPoints, &iIndicesCountPoints,
			&iStartIndexFacePolygons, &iIndicesCountFacePolygons,
			&iStartIndexConceptualFacePolygons, &iIndicesCountConceptualFacePolygons);

		wstring strTexture = getConcFaceTexture(iConceptualFace);

		if (iIndicesCountTriangles > 0) {
			int32_t iIndexValue = *(m_pIndexBuffer->data() + iStartIndexTriangles);
			iIndexValue *= VERTEX_LENGTH;

			float fColor = *(m_pVertexBuffer->data() + iIndexValue + 8);
			unsigned int iAmbientColor = *(reinterpret_cast<unsigned int*>(&fColor));
			float fTransparency = (float)(iAmbientColor & (255)) / (float)255;

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 9);
			unsigned int iDiffuseColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 10);
			unsigned int iEmissiveColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 11);
			unsigned int iSpecularColor = *(reinterpret_cast<unsigned int*>(&fColor));

			_material material(
				iAmbientColor,
				iDiffuseColor,
				iEmissiveColor,
				iSpecularColor,
				fTransparency,
				!strTexture.empty() ? strTexture.c_str() : nullptr);

			addTriangles(iConceptualFaceIndex, iStartIndexTriangles, iIndicesCountTriangles, material, mapMaterial2ConcFaces);
		}

		if (iIndicesCountFacePolygons > 0) {
			addFacePolygons(iStartIndexFacePolygons, iIndicesCountFacePolygons);
		}

		if (iIndicesCountConceptualFacePolygons > 0) {
			addConcFacePolygons(iStartIndexConceptualFacePolygons, iIndicesCountConceptualFacePolygons);
		}

		if (iIndicesCountLines > 0) {
			int32_t iIndexValue = *(m_pIndexBuffer->data() + iStartIndexLines);
			iIndexValue *= VERTEX_LENGTH;

			float fColor = *(m_pVertexBuffer->data() + iIndexValue + 8);
			unsigned int iAmbientColor = *(reinterpret_cast<unsigned int*>(&fColor));
			float fTransparency = (float)(iAmbientColor & (255)) / (float)255;

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 9);
			unsigned int iDiffuseColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 10);
			unsigned int iEmissiveColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 11);
			unsigned int iSpecularColor = *(reinterpret_cast<unsigned int*>(&fColor));

			_material material(
				iAmbientColor,
				iDiffuseColor,
				iEmissiveColor,
				iSpecularColor,
				fTransparency,
				!strTexture.empty() ? strTexture.c_str() : nullptr);

			addLines(iConceptualFaceIndex, iStartIndexLines, iIndicesCountLines, material, mapMaterial2ConcFaceLines);
		}

		if (iIndicesCountPoints > 0) {
			int32_t iIndexValue = *(m_pIndexBuffer->data() + iStartIndexPoints);
			iIndexValue *= VERTEX_LENGTH;

			float fColor = *(m_pVertexBuffer->data() + iIndexValue + 8);
			unsigned int iAmbientColor = *(reinterpret_cast<unsigned int*>(&fColor));
			float fTransparency = (float)(iAmbientColor & (255)) / (float)255;

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 9);
			unsigned int iDiffuseColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 10);
			unsigned int iEmissiveColor = *(reinterpret_cast<unsigned int*>(&fColor));

			fColor = *(m_pVertexBuffer->data() + iIndexValue + 11);
			unsigned int iSpecularColor = *(reinterpret_cast<unsigned int*>(&fColor));

			_material material(
				iAmbientColor,
				iDiffuseColor,
				iEmissiveColor,
				iSpecularColor,
				fTransparency,
				!strTexture.empty() ? strTexture.c_str() : nullptr);

			addPoints(iConceptualFaceIndex, iStartIndexPoints, iIndicesCountPoints, material, mapMaterial2ConcFacePoints);
		}
	} // for (int64_t iConceptualFaceIndex = ...

	// Build the cohorts
	buildConcFacesCohorts(mapMaterial2ConcFaces, _oglUtils::getIndicesCountLimit());
	buildFacePolygonsCohorts(_oglUtils::getIndicesCountLimit());
	buildConcFacePolygonsCohorts(_oglUtils::getIndicesCountLimit());
	buildLinesCohorts(mapMaterial2ConcFaceLines, _oglUtils::getIndicesCountLimit());
	buildPointsCohorts(mapMaterial2ConcFacePoints, _oglUtils::getIndicesCountLimit());
}

/*virtual*/ void _rdf_geometry::clean() /*override*/
{
	_geometry::clean();

	delete m_pOriginalVertexBuffer;
	m_pOriginalVertexBuffer = nullptr;
}

void _rdf_geometry::loadName()
{
	OwlClass owlClass = GetInstanceClass(getOwlInstance());
	assert(owlClass != 0);

	wchar_t* szClassName = nullptr;
	GetNameOfClassW(owlClass, &szClassName);

	wchar_t* szName = nullptr;
	GetNameOfInstanceW(getOwlInstance(), &szName);

	if (szName == nullptr) {
		RdfProperty iTagProperty = GetPropertyByName(getOwlModel(), "tag");
		if (iTagProperty != 0) {
			SetCharacterSerialization(getOwlModel(), 0, 0, false);

			int64_t iCard = 0;
			wchar_t** szValue = nullptr;
			GetDatatypeProperty(getOwlInstance(), iTagProperty, (void**)&szValue, &iCard);

			if (iCard == 1) {
				szName = szValue[0];
			}

			SetCharacterSerialization(getOwlModel(), 0, 0, true);
		}
	} // if (szName == nullptr)

	wchar_t szUniqueName[512];

	if (szName != nullptr) {
		m_strName = szName;
		swprintf(szUniqueName, 512, L"%s (%s)", szName, szClassName);
	} else {
		m_strName = szClassName;
		swprintf(szUniqueName, 512, L"#%lld (%s)", getOwlInstance(), szClassName);
	}

	m_strUniqueName = szUniqueName;
}

void _rdf_geometry::loadOriginalData()
{
	if (getVerticesCount() == 0) {
		return;
	}

	// Vertices
	assert(m_pVertexBuffer != nullptr);
	m_pVertexBuffer->copyFrom(m_pOriginalVertexBuffer);

	// Bounding box
	memcpy(m_pmtxBBTransformation, m_pmtxOriginalBBTransformation, sizeof(_matrix));
	memcpy(m_pvecBBMin, m_pvecOriginalBBMin, sizeof(_vector3d));
	memcpy(m_pvecBBMax, m_pvecOriginalBBMax, sizeof(_vector3d));
}

void _rdf_geometry::recalculate()
{
	clean();
	calculate();
}

void _rdf_geometry::setRDFFormatSettings()
{
	string strSettings = "111111000000001111000001110001";

	bitset<64> bitSettings(strSettings);
	int64_t iSettings = bitSettings.to_ulong();

	string strMask = "11111111111111111111011101110111";
	bitset <64> bitMask(strMask);
	int64_t iMask = bitMask.to_ulong();

	SetFormat(getOwlModel(), (int64_t)iSettings, (int64_t)iMask);
	SetBehavior(getOwlModel(), 2048 + 4096, 2048 + 4096);
}