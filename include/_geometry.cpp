#include "_host.h"
#include "_geometry.h"
#include "_instance.h"
#include "_oglUtils.h"

#ifdef __EMSCRIPTEN__
	#include "../../gisengine/Parsers/_string.h"
#else 
	#ifdef __GNUG__ 
	#include "_string.h"
	#endif
#endif

// ************************************************************************************************
/*static*/ unsigned int* _cohort::merge(const vector<_cohort*>& vecCohorts, uint32_t& iIndicesCount)
{
    iIndicesCount = 0;
    for (size_t iCohort = 0; iCohort < vecCohorts.size(); iCohort++) {
        iIndicesCount += (uint32_t)vecCohorts[iCohort]->indices().size();
    }

    unsigned int* pIndices = new unsigned int[iIndicesCount];

    uint32_t iOffset = 0;
    for (size_t iCohort = 0; iCohort < vecCohorts.size(); iCohort++) {
        if (vecCohorts[iCohort]->indices().size() == 0) {
            continue;
        }

        memcpy((unsigned int*)pIndices + iOffset, vecCohorts[iCohort]->indices().data(),
               vecCohorts[iCohort]->indices().size() * sizeof(unsigned int));

        iOffset += (uint32_t)vecCohorts[iCohort]->indices().size();
    }

    return pIndices;
}

// ************************************************************************************************
_geometry::_geometry(OwlInstance owlInstance)
    : m_owlInstance(owlInstance)
    , m_strName(L"NA")
    , m_strUniqueName(L"")
    , m_pVertexBuffer(nullptr)
    , m_pIndexBuffer(nullptr)
    , m_iConceptualFacesCount(0)
    , m_bHasGeometry(false)
    , m_bShow(true)
    , m_pmtxBBTransformation(nullptr)
    , m_pvecBBMin(nullptr)
    , m_pvecBBMax(nullptr)
    , m_pvecAABBMin(nullptr)
    , m_pvecAABBMax(nullptr)
    , m_vecTriangles()
    , m_vecFacePolygons()
    , m_vecConcFacePolygons()
    , m_vecLines()
    , m_vecPoints()
    , m_vecConcFacesCohorts()
    , m_vecFacePolygonsCohorts()
    , m_vecConcFacePolygonsCohorts()
    , m_vecLinesCohorts()
    , m_vecPointsCohorts()
    , m_vecNormalVecsCohorts()
    , m_vecBiNormalVecsCohorts()
    , m_vecTangentVecsCohorts()
    , m_vecInstances()
    , m_iVBO(0)
    , m_iVBOOffset(0)
{}

/*virtual*/ _geometry::~_geometry()
{
    clean();
}

void _geometry::calculateBB(
    _instance* pInstance,
    float& fXmin, float& fXmax,
    float& fYmin, float& fYmax,
    float& fZmin, float& fZmax) const
{
    assert(pInstance != nullptr);

    if (ignoreBB() || !hasGeometry()) {
        return;
    }

    if ((m_pvecAABBMin == nullptr) || (m_pvecAABBMax == nullptr)) {
        return;
    }

    calculateBB(
        m_pvecAABBMin->x, m_pvecAABBMin->y, m_pvecAABBMin->z,
        pInstance->getTransformationMatrix(),
        fXmin, fXmax,
        fYmin, fYmax,
        fZmin, fZmax);

    calculateBB(
        m_pvecAABBMin->x, m_pvecAABBMin->y, m_pvecAABBMax->z,
        pInstance->getTransformationMatrix(),
        fXmin, fXmax,
        fYmin, fYmax,
        fZmin, fZmax);

    calculateBB(
        m_pvecAABBMin->x, m_pvecAABBMax->y, m_pvecAABBMin->z,
        pInstance->getTransformationMatrix(),
        fXmin, fXmax,
        fYmin, fYmax,
        fZmin, fZmax);

    calculateBB(
        m_pvecAABBMin->x, m_pvecAABBMax->y, m_pvecAABBMax->z,
        pInstance->getTransformationMatrix(),
        fXmin, fXmax,
        fYmin, fYmax,
        fZmin, fZmax);

    calculateBB(
        m_pvecAABBMax->x, m_pvecAABBMin->y, m_pvecAABBMin->z,
        pInstance->getTransformationMatrix(),
        fXmin, fXmax,
        fYmin, fYmax,
        fZmin, fZmax);

    calculateBB(
        m_pvecAABBMax->x, m_pvecAABBMin->y, m_pvecAABBMax->z,
        pInstance->getTransformationMatrix(),
        fXmin, fXmax,
        fYmin, fYmax,
        fZmin, fZmax);

    calculateBB(
        m_pvecAABBMax->x, m_pvecAABBMax->y, m_pvecAABBMin->z,
        pInstance->getTransformationMatrix(),
        fXmin, fXmax,
        fYmin, fYmax,
        fZmin, fZmax);

    calculateBB(
        m_pvecAABBMax->x, m_pvecAABBMax->y, m_pvecAABBMax->z,
        pInstance->getTransformationMatrix(),
        fXmin, fXmax,
        fYmin, fYmax,
        fZmin, fZmax);
}

void _geometry::calculateBB(
    double dX, double dY, double dZ,
    const _matrix4x4* pTransformationMatrix,
    float& fXmin, float& fXmax,
    float& fYmin, float& fYmax,
    float& fZmin, float& fZmax) const
{
    _vector3 vecPoint = { dX, dY, dZ };
    if (pTransformationMatrix != nullptr) {
        _transform(&vecPoint, pTransformationMatrix, &vecPoint);
    }

    fXmin = (float)fmin(fXmin, vecPoint.x);
    fXmax = (float)fmax(fXmax, vecPoint.x);
    fYmin = (float)fmin(fYmin, vecPoint.y);
    fYmax = (float)fmax(fYmax, vecPoint.y);
    fZmin = (float)fmin(fZmin, vecPoint.z);
    fZmax = (float)fmax(fZmax, vecPoint.z);
}

/*static*/ void _geometry::calculateBB(
    OwlInstance owlInstance,
    double& dXmin, double& dXmax,
    double& dYmin, double& dYmax,
    double& dZmin, double& dZmax)
{
    assert(owlInstance != 0);

    _vector3d vecOriginalBBMin;
    _vector3d vecOriginalBBMax;
    if (GetInstanceGeometryClass(owlInstance) &&
        GetBoundingBox(
            owlInstance,
            (double*)&vecOriginalBBMin,
            (double*)&vecOriginalBBMax)) {
        dXmin = fmin(dXmin, vecOriginalBBMin.x);
        dYmin = fmin(dYmin, vecOriginalBBMin.y);
        dZmin = fmin(dZmin, vecOriginalBBMin.z);

        dXmax = fmax(dXmax, vecOriginalBBMax.x);
        dYmax = fmax(dYmax, vecOriginalBBMax.y);
        dZmax = fmax(dZmax, vecOriginalBBMax.z);
    }
}

void _geometry::calculateVerticesMinMax(
    float& fXmin, float& fXmax,
    float& fYmin, float& fYmax,
    float& fZmin, float& fZmax) const
{
    if (getVerticesCount() == 0) {
        return;
    }

    const auto VERTEX_LENGTH = getVertexLength();

    for (int64_t iVertex = 0; iVertex < m_pVertexBuffer->size(); iVertex++) {
        fXmin = (float)fmin(fXmin, m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 0]);
        fXmax = (float)fmax(fXmax, m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 0]);
        fYmin = (float)fmin(fYmin, m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 1]);
        fYmax = (float)fmax(fYmax, m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 1]);
        fZmin = (float)fmin(fZmin, m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 2]);
        fZmax = (float)fmax(fZmax, m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 2]);        
	}
}

void _geometry::scale(float fScaleFactor)
{
    if (getVerticesCount() == 0) {
        return;
    }

    const auto VERTEX_LENGTH = getVertexLength();

    // Geometry
    for (int64_t iVertex = 0; iVertex < m_pVertexBuffer->size(); iVertex++) {
        m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 0] /= fScaleFactor;
        m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 1] /= fScaleFactor;
        m_pVertexBuffer->data()[(iVertex * VERTEX_LENGTH) + 2] /= fScaleFactor;
    }

    /* BB - Min */
    m_pvecAABBMin->x /= fScaleFactor;
    m_pvecAABBMin->y /= fScaleFactor;
    m_pvecAABBMin->z /= fScaleFactor;

    /* BB - Max */
    m_pvecAABBMax->x /= fScaleFactor;
    m_pvecAABBMax->y /= fScaleFactor;
    m_pvecAABBMax->z /= fScaleFactor;


    // Instances
    for (size_t iInstance = 0; iInstance < m_vecInstances.size(); iInstance++) {
        m_vecInstances[iInstance]->scale(fScaleFactor);
    }
}

void _geometry::translate(float fX, float fY, float fZ)
{
    if (getVerticesCount() == 0) {
        return;
    }

    const auto iVertexLength = getVertexLength();

    // Geometry
    for (int64_t iVertex = 0; iVertex < m_pVertexBuffer->size(); iVertex++) {
        m_pVertexBuffer->data()[(iVertex * iVertexLength) + 0] += fX;
        m_pVertexBuffer->data()[(iVertex * iVertexLength) + 1] += fY;
        m_pVertexBuffer->data()[(iVertex * iVertexLength) + 2] += fZ;
    }

    /* BB - Min */
    m_pvecAABBMin->x += fX;
    m_pvecAABBMin->y += fY;
    m_pvecAABBMin->z += fZ;

    /* BB - Max */
    m_pvecAABBMax->x += fX;
    m_pvecAABBMax->y += fY;
    m_pvecAABBMax->z += fZ;

    // Instances
    for (size_t iInstance = 0; iInstance < m_vecInstances.size(); iInstance++) {
        m_vecInstances[iInstance]->translate(fX, fY, fZ);
    }
}

void _geometry::addInstance(_instance* pInstance)
{
    assert(pInstance != 0);

    m_vecInstances.push_back(pInstance);
}

/*virtual*/ void _geometry::enableInstances(bool bEnable)
{
    for (auto pInstance : m_vecInstances) {
        pInstance->setEnable(bEnable);
    }
}

long _geometry::getEnabledInstancesCount() const
{
    long lCount = 0;
    for (auto pInstance : m_vecInstances) {
        lCount += pInstance->getEnable() ? 1 : 0;
    }

    return lCount;
}

bool  _geometry::calculateInstance(_vertices_f* pVertexBuffer, _indices_i32* pIndexBuffer)
{
    assert(pVertexBuffer != nullptr);
    assert(pIndexBuffer != nullptr);

    if (getOwlInstance() == 0) {
        return false;
    }

    CalculateInstance(getOwlInstance(), &pVertexBuffer->size(), &pIndexBuffer->size(), nullptr);
    if ((pVertexBuffer->size() == 0) || (pIndexBuffer->size() == 0)) {
        return false;
    }

    pVertexBuffer->data() = new float[(uint32_t)pVertexBuffer->size() * (int64_t)pVertexBuffer->getVertexLength()];
    memset(pVertexBuffer->data(), 0, (uint32_t)pVertexBuffer->size() * (int64_t)pVertexBuffer->getVertexLength() * sizeof(float));

    UpdateInstanceVertexBuffer(getOwlInstance(), pVertexBuffer->data());

#ifdef _PRINT_VERTEX_BUFFER
    for (int64_t i = 0; i < pVertexBuffer->size(); i++) {
		TRACE(
			L"V %lld: %f, %f, %f\n",
			i,
			pVertexBuffer->data()[(i * pVertexBuffer->getVertexLength()) + 0],
			pVertexBuffer->data()[(i * pVertexBuffer->getVertexLength()) + 1],
			pVertexBuffer->data()[(i * pVertexBuffer->getVertexLength()) + 2]);
	}
#endif

    pIndexBuffer->data() = new int32_t[(uint32_t)pIndexBuffer->size()];
    memset(pIndexBuffer->data(), 0, (uint32_t)pIndexBuffer->size() * sizeof(int32_t));

    UpdateInstanceIndexBuffer(getOwlInstance(), pIndexBuffer->data());

    m_bHasGeometry = true;

    return true;
}

/*virtual*/ void _geometry::calculateCore()
{
    assert(m_pVertexBuffer == nullptr);
    m_pVertexBuffer = new _vertices_f(getVertexLength());

    assert(m_pIndexBuffer == nullptr);
    m_pIndexBuffer = new _indices_i32();

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

    if (!calculateInstance(m_pVertexBuffer, m_pIndexBuffer)) {
        return;
    }

    GetBoundingBox(
        getOwlInstance(),
        (double*)m_pmtxBBTransformation,
        (double*)m_pvecBBMin,
        (double*)m_pvecBBMax);

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
        int64_t iStartIndexConceptualFacePolygons = 0;
        int64_t iIndicesCountConceptualFacePolygons = 0;
        ConceptualFace iConceptualFace = GetConceptualFace(
            getOwlInstance(),
            iConceptualFaceIndex,
            &iStartIndexTriangles, &iIndicesCountTriangles,
            &iStartIndexLines, &iIndicesCountLines,
            &iStartIndexPoints, &iIndicesCountPoints,
            0, 0,
            &iStartIndexConceptualFacePolygons, &iIndicesCountConceptualFacePolygons);

        //
        // Material
        // 

        uint32_t iAmbientColor = getDefaultColor();
        uint32_t iDiffuseColor = getDefaultColor();
        uint32_t iEmissiveColor = getDefaultColor();
        uint32_t iSpecularColor = getDefaultColor();
        float fTransparency = 1.f;

        OwlInstance iMaterialInstance = GetConceptualFaceMaterial(iConceptualFace);
        if (iMaterialInstance != 0) {
            iAmbientColor = GetMaterialColorAmbient(iMaterialInstance);
            iDiffuseColor = GetMaterialColorDiffuse(iMaterialInstance);
            iEmissiveColor = GetMaterialColorEmissive(iMaterialInstance);
            iSpecularColor = GetMaterialColorSpecular(iMaterialInstance);
            fTransparency = getTransparency(iMaterialInstance);
        }

        _material material(
            iAmbientColor,
            iDiffuseColor,
            iEmissiveColor,
            iSpecularColor,
            fTransparency,
            nullptr,
            false);

        if (iIndicesCountTriangles > 0) {
            addTriangles(iConceptualFaceIndex, iStartIndexTriangles, iIndicesCountTriangles, material, mapMaterial2ConcFaces);
        }

        if (iIndicesCountConceptualFacePolygons > 0) {
            addConcFacePolygons(iStartIndexConceptualFacePolygons, iIndicesCountConceptualFacePolygons);
        }

        if (iIndicesCountLines > 0) {
            addLines(iConceptualFaceIndex, iStartIndexLines, iIndicesCountLines, material, mapMaterial2ConcFaceLines);
        }

        if (iIndicesCountPoints > 0) {
            addPoints(iConceptualFaceIndex, iStartIndexPoints, iIndicesCountPoints, material, mapMaterial2ConcFacePoints);
        }
    } // for (int64_t iConceptualFaceIndex = ...

    // Build the cohorts
    buildConcFacesCohorts(mapMaterial2ConcFaces, _oglUtils::getIndicesCountLimit());
    buildConcFacePolygonsCohorts(_oglUtils::getIndicesCountLimit());
    buildLinesCohorts(mapMaterial2ConcFaceLines, _oglUtils::getIndicesCountLimit());
    buildPointsCohorts(mapMaterial2ConcFacePoints, _oglUtils::getIndicesCountLimit());
}

void _geometry::addMaterial(int64_t iConceptualFaceIndex, int64_t iStartIndex, int64_t iIndicesCount, _material& material, MATERIALS& mapMaterials)
{
    auto itMaterial = mapMaterials.find(material);
    if (itMaterial == mapMaterials.end()) {
        mapMaterials[material] = vector<_face>{ _face(iConceptualFaceIndex, iStartIndex, iIndicesCount) };
    }
    else {
        itMaterial->second.push_back(_face(iConceptualFaceIndex, iStartIndex, iIndicesCount));
    }
}

wstring _geometry::getConcFaceTexture(ConceptualFace conceptualFace, bool& bFlipY)
{
    assert(conceptualFace != 0);
    bFlipY = true; // backward compatibility

    wstring strTexture;    

    OwlInstance iMaterialInstance = GetConceptualFaceMaterial(conceptualFace);
    if (iMaterialInstance != 0) {
        int64_t* piInstances = nullptr;
        int64_t iCard = 0;
        GetObjectProperty(
            iMaterialInstance,
            GetPropertyByName(getOwlModel(), "textures"),
            &piInstances,
            &iCard);

        if (iCard == 1) {
            iCard = 0;
            char** szValue = nullptr;
            GetDatatypeProperty(
                piInstances[0],
                GetPropertyByName(getOwlModel(), "name"),
                (void**)&szValue,
                &iCard);

            if (iCard == 1) {
                strTexture = CA2W(szValue[0]);

                RdfProperty rdfFlipYProperty = GetPropertyByName(getOwlModel(), "flipY");
                if (rdfFlipYProperty != 0) {
                    iCard = 0;
                    bool* pbFlipYValue = nullptr;
                    GetDatatypeProperty(
                        piInstances[0],
                        rdfFlipYProperty,
                        (void**)&pbFlipYValue,
                        &iCard);
                    if (iCard == 1) {
                        bFlipY = *pbFlipYValue;
                    }
				} // if (rdfFlipYProperty != 0)
            }

            if (strTexture.empty()) {
                strTexture = L"default";
            }
        } // if (iCard == 1)
    } // if (iMaterialInstance != 0)

    return strTexture;
}

float _geometry::getTransparency(OwlInstance owlMaterialInstance)
{
    assert(owlMaterialInstance != 0);

    //
    // color
    //

    OwlInstance* powlInstances = nullptr;
    int64_t iCard = 0;
    GetObjectProperty(
        owlMaterialInstance,
        GetPropertyByName(getOwlModel(), "color"),
        &powlInstances,
        &iCard);

    // color
    if (iCard == 1) {
        OwlInstance owlColorInstance = powlInstances[0];
        assert(owlColorInstance != 0);

        //
        // color/transparency
        //

        iCard = 0;
        double* pdValue = nullptr;
        GetDatatypeProperty(
            owlColorInstance,
            GetPropertyByName(getOwlModel(), "transparency"),
            (void**)&pdValue,
            &iCard);

        if (iCard == 1) {
            return (float)pdValue[0];
        }

        //
        // color/ambient/W
        //

        powlInstances = nullptr;
        iCard = 0;
        GetObjectProperty(
            owlColorInstance,
            GetPropertyByName(getOwlModel(), "ambient"),
            &powlInstances,
            &iCard);

        // ambient
        if (iCard == 1) {
            OwlInstance owlColorComponentInstance = powlInstances[0];
            assert(owlColorComponentInstance != 0);

            iCard = 0;
            pdValue = nullptr;
            GetDatatypeProperty(
                owlColorComponentInstance,
                GetPropertyByName(getOwlModel(), "W"),
                (void**)&pdValue,
                &iCard);

            if (iCard == 1) {
                return (float)pdValue[0];
            }
        } // ambient
    } // color

    return 1.f;
}

void _geometry::buildConcFacesCohorts(MATERIALS& mapMaterials, const GLsizei INDICES_COUNT_LIMIT)
{
    auto itMaterial = mapMaterials.begin();
    for (; itMaterial != mapMaterials.end(); itMaterial++) {
        _cohortWithMaterial* pCohort = nullptr;

        for (size_t iConcFace = 0; iConcFace < itMaterial->second.size(); iConcFace++) {
            _face& concFace = itMaterial->second[iConcFace];

            int64_t iStartIndex = concFace.startIndex();
            int64_t iIndicesCount = concFace.indicesCount();

            // Split the conceptual face - isolated case
            if (iIndicesCount > INDICES_COUNT_LIMIT) {
                while (iIndicesCount > INDICES_COUNT_LIMIT) {
                    auto pNewCohort = new _cohortWithMaterial(itMaterial->first);
                    for (int64_t iIndex = iStartIndex;
                         iIndex < iStartIndex + INDICES_COUNT_LIMIT;
                         iIndex++) {
                        pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
                    }

                    concFacesCohorts().push_back(pNewCohort);

                    // Update Conceptual face start index
                    concFace.startIndex() = 0;

                    // Conceptual faces
                    pNewCohort->faces().push_back(concFace);

                    iIndicesCount -= INDICES_COUNT_LIMIT;
                    iStartIndex += INDICES_COUNT_LIMIT;
                }

                if (iIndicesCount > 0) {
                    auto pNewCohort = new _cohortWithMaterial(itMaterial->first);
                    for (int64_t iIndex = iStartIndex;
                         iIndex < iStartIndex + iIndicesCount;
                         iIndex++) {
                        pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
                    }

                    concFacesCohorts().push_back(pNewCohort);

                    // Update Conceptual face start index
                    concFace.startIndex() = 0;

                    // Conceptual faces
                    pNewCohort->faces().push_back(concFace);
                }

                continue;
            } // if (iIndicesCount > INDICES_COUNT_LIMIT)	

            // Create cohort
            if (pCohort == nullptr) {
                pCohort = new _cohortWithMaterial(itMaterial->first);

                concFacesCohorts().push_back(pCohort);
            }

            // Check the limit
            if (pCohort->indices().size() + iIndicesCount > INDICES_COUNT_LIMIT) {
                pCohort = new _cohortWithMaterial(itMaterial->first);

                concFacesCohorts().push_back(pCohort);
            }

            // Update Conceptual face start index
            concFace.startIndex() = pCohort->indices().size();

            // Add the indices
            for (int64_t iIndex = iStartIndex;
                 iIndex < iStartIndex + iIndicesCount;
                 iIndex++) {
                pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
            }

            // Conceptual faces
            pCohort->faces().push_back(concFace);
        } // for (size_t iConcFace = ...
    } // for (; itMaterial != ...
}

void _geometry::buildFacePolygonsCohorts(const GLsizei INDICES_COUNT_LIMIT)
{
    if (m_vecFacePolygons.empty()) {
        return;
    }

    // Use the last cohort (if any)
    _cohort* pCohort = facePolygonsCohorts().empty() ? nullptr : facePolygonsCohorts()[facePolygonsCohorts().size() - 1];

    // Create the cohort
    if (pCohort == nullptr) {
        pCohort = new _cohort();
        facePolygonsCohorts().push_back(pCohort);
    }

    for (size_t iFace = 0; iFace < m_vecFacePolygons.size(); iFace++) {
        int64_t iStartIndex = m_vecFacePolygons[iFace].startIndex();
        int64_t iIndicesCount = m_vecFacePolygons[iFace].indicesCount();

        // Split the conceptual face - isolated case
        if (iIndicesCount > INDICES_COUNT_LIMIT / 2) {
            while (iIndicesCount > INDICES_COUNT_LIMIT / 2) {
                pCohort = new _cohort();
                facePolygonsCohorts().push_back(pCohort);

                for (int64_t iIndex = iStartIndex;
                     iIndex < iStartIndex + INDICES_COUNT_LIMIT / 2;
                     iIndex += 2) {
                    pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
                    pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
                }

                iIndicesCount -= INDICES_COUNT_LIMIT / 2;
                iStartIndex += INDICES_COUNT_LIMIT / 2;
            } // while (iIndicesCount > INDICES_COUNT_LIMIT / 2)

            if (iIndicesCount > 0) {
                pCohort = new _cohort();
                facePolygonsCohorts().push_back(pCohort);

                for (int64_t iIndex = iStartIndex;
                     iIndex < iStartIndex + iIndicesCount;
                     iIndex += 2) {
                    pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
                    pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
                }
            }

            continue;
        } // if (iIndicesCount > INDICES_COUNT_LIMIT / 2)

        // Check the limit
        if ((pCohort->indices().size() + (iIndicesCount * 2)) > INDICES_COUNT_LIMIT) {
            pCohort = new _cohort();
            facePolygonsCohorts().push_back(pCohort);
        }

        for (int64_t iIndex = iStartIndex;
             iIndex < iStartIndex + iIndicesCount;
             iIndex += 2) {
            pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
            pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
        }
    } // for (size_t iFace = ...
}

void _geometry::buildConcFacePolygonsCohorts(const GLsizei INDICES_COUNT_LIMIT)
{
    if (m_vecConcFacePolygons.empty()) {
        return;
    }

    // Use the last cohort (if any)
    _cohort* pCohort = concFacePolygonsCohorts().empty() ? nullptr : concFacePolygonsCohorts()[concFacePolygonsCohorts().size() - 1];

    // Create the cohort
    if (pCohort == nullptr) {
        pCohort = new _cohort();
        concFacePolygonsCohorts().push_back(pCohort);
    }

    for (size_t iFace = 0; iFace < m_vecConcFacePolygons.size(); iFace++) {
        int64_t iStartIndex = m_vecConcFacePolygons[iFace].startIndex();
        int64_t iIndicesCount = m_vecConcFacePolygons[iFace].indicesCount();

        // Split the conceptual face - isolated case
        if (iIndicesCount > INDICES_COUNT_LIMIT / 2) {
            while (iIndicesCount > INDICES_COUNT_LIMIT / 2) {
                pCohort = new _cohort();
                concFacePolygonsCohorts().push_back(pCohort);

                for (int64_t iIndex = iStartIndex;
                     iIndex < iStartIndex + INDICES_COUNT_LIMIT / 2;
                     iIndex += 2) {
                    pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
                    pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
                }

                iIndicesCount -= INDICES_COUNT_LIMIT / 2;
                iStartIndex += INDICES_COUNT_LIMIT / 2;
            } // while (iIndicesCount > INDICES_COUNT_LIMIT / 2)

            if (iIndicesCount > 0) {
                pCohort = new _cohort();
                concFacePolygonsCohorts().push_back(pCohort);

                for (int64_t iIndex = iStartIndex;
                     iIndex < iStartIndex + iIndicesCount;
                     iIndex += 2) {
                    pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
                    pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
                }
            }

            continue;
        } // if (iIndicesCount > INDICES_COUNT_LIMIT / 2)

        // Check the limit
        if ((pCohort->indices().size() + (iIndicesCount * 2)) > INDICES_COUNT_LIMIT) {
            pCohort = new _cohort();
            concFacePolygonsCohorts().push_back(pCohort);
        }

        for (int64_t iIndex = iStartIndex;
             iIndex < iStartIndex + iIndicesCount;
             iIndex += 2) {
            pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 0]);
            pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex + 1]);
        }
    } // for (size_t iFace = ...
}

void _geometry::buildLinesCohorts(MATERIALS& mapMaterials, const GLsizei INDICES_COUNT_LIMIT)
{
    auto itMaterial = mapMaterials.begin();
    for (; itMaterial != mapMaterials.end(); itMaterial++) {
        _cohortWithMaterial* pCohort = nullptr;

        for (size_t iConcFace = 0; iConcFace < itMaterial->second.size(); iConcFace++) {
            _face& concFace = itMaterial->second[iConcFace];

            int64_t iStartIndex = concFace.startIndex();
            int64_t iIndicesCount = concFace.indicesCount();

            // Split the conceptual face - isolated case
            if (iIndicesCount > INDICES_COUNT_LIMIT) {
                while (iIndicesCount > INDICES_COUNT_LIMIT) {
                    auto pNewCohort = new _cohortWithMaterial(itMaterial->first);
                    for (int64_t iIndex = iStartIndex;
                         iIndex < iStartIndex + INDICES_COUNT_LIMIT;
                         iIndex++) {
                        pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
                    }

                    linesCohorts().push_back(pNewCohort);

                    // Update Conceptual face start index
                    concFace.startIndex() = 0;

                    // Conceptual faces
                    pNewCohort->faces().push_back(concFace);

                    iIndicesCount -= INDICES_COUNT_LIMIT;
                    iStartIndex += INDICES_COUNT_LIMIT;
                }

                if (iIndicesCount > 0) {
                    auto pNewCohort = new _cohortWithMaterial(itMaterial->first);
                    for (int64_t iIndex = iStartIndex;
                         iIndex < iStartIndex + iIndicesCount;
                         iIndex++) {
                        pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
                    }

                    linesCohorts().push_back(pNewCohort);

                    // Update Conceptual face start index
                    concFace.startIndex() = 0;

                    // Conceptual faces
                    pNewCohort->faces().push_back(concFace);
                }

                continue;
            } // if (iIndicesCountTriangles > INDICES_COUNT_LIMIT)	

            // Create cohort
            if (pCohort == nullptr) {
                pCohort = new _cohortWithMaterial(itMaterial->first);

                linesCohorts().push_back(pCohort);
            }

            // Check the limit
            if (pCohort->indices().size() + iIndicesCount > INDICES_COUNT_LIMIT) {
                pCohort = new _cohortWithMaterial(itMaterial->first);

                linesCohorts().push_back(pCohort);
            }

            // Update Conceptual face start index
            concFace.startIndex() = pCohort->indices().size();

            // Add the indices
            for (int64_t iIndex = iStartIndex;
                 iIndex < iStartIndex + iIndicesCount;
                 iIndex++) {
                pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
            }

            // Conceptual faces
            pCohort->faces().push_back(concFace);
        } // for (size_t iConcFace = ...
    } // for (; itMaterial != ...
}

void _geometry::buildPointsCohorts(MATERIALS& mapMaterials, const GLsizei INDICES_COUNT_LIMIT)
{
    auto itMaterial = mapMaterials.begin();
    for (; itMaterial != mapMaterials.end(); itMaterial++) {
        _cohortWithMaterial* pCohort = nullptr;

        for (size_t iConcFace = 0; iConcFace < itMaterial->second.size(); iConcFace++) {
            _face& concFace = itMaterial->second[iConcFace];

            int64_t iStartIndex = concFace.startIndex();
            int64_t iIndicesCount = concFace.indicesCount();

            // Split the conceptual face - isolated case
            if (iIndicesCount > INDICES_COUNT_LIMIT) {
                while (iIndicesCount > INDICES_COUNT_LIMIT) {
                    auto pNewCohort = new _cohortWithMaterial(itMaterial->first);
                    for (int64_t iIndex = iStartIndex;
                         iIndex < iStartIndex + INDICES_COUNT_LIMIT;
                         iIndex++) {
                        pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
                    }

                    pointsCohorts().push_back(pNewCohort);

                    // Update Conceptual face start index
                    concFace.startIndex() = 0;

                    // Conceptual faces
                    pNewCohort->faces().push_back(concFace);

                    iIndicesCount -= INDICES_COUNT_LIMIT;
                    iStartIndex += INDICES_COUNT_LIMIT;
                }

                if (iIndicesCount > 0) {
                    auto pNewCohort = new _cohortWithMaterial(itMaterial->first);
                    for (int64_t iIndex = iStartIndex;
                         iIndex < iStartIndex + iIndicesCount;
                         iIndex++) {
                        pNewCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
                    }

                    pointsCohorts().push_back(pNewCohort);

                    // Update Conceptual face start index
                    concFace.startIndex() = 0;

                    // Conceptual faces
                    pNewCohort->faces().push_back(concFace);
                }

                continue;
            } // if (iIndicesCountTriangles > INDICES_COUNT_LIMIT)	

            // Create cohort
            if (pCohort == nullptr) {
                pCohort = new _cohortWithMaterial(itMaterial->first);

                pointsCohorts().push_back(pCohort);
            }

            // Check the limit
            if (pCohort->indices().size() + iIndicesCount > INDICES_COUNT_LIMIT) {
                pCohort = new _cohortWithMaterial(itMaterial->first);

                pointsCohorts().push_back(pCohort);
            }

            // Update Conceptual face start index
            concFace.startIndex() = pCohort->indices().size();

            // Add the indices
            for (int64_t iIndex = iStartIndex;
                 iIndex < iStartIndex + iIndicesCount;
                 iIndex++) {
                pCohort->indices().push_back(m_pIndexBuffer->data()[iIndex]);
            }

            // Add Conceptual faces
            pCohort->faces().push_back(concFace);
        } // for (size_t iConcFace = ...
    } // for (; itMaterial != ...
}

/*virtual*/ void _geometry::clean()
{
    delete m_pVertexBuffer;
    m_pVertexBuffer = nullptr;

    delete m_pIndexBuffer;
    m_pIndexBuffer = nullptr;

    m_iConceptualFacesCount = 0;

    m_bHasGeometry = false;

    delete m_pmtxBBTransformation;
    m_pmtxBBTransformation = nullptr;

    delete m_pvecBBMin;
    m_pvecBBMin = nullptr;

    delete m_pvecBBMax;
    m_pvecBBMax = nullptr;

    delete m_pvecAABBMin;
    m_pvecAABBMin = nullptr;

    delete m_pvecAABBMax;
    m_pvecAABBMax = nullptr;    

    m_vecTriangles.clear();
    m_vecFacePolygons.clear();
    m_vecConcFacePolygons.clear();
    m_vecLines.clear();
    m_vecPoints.clear();

    _cohort::clear(m_vecConcFacesCohorts);
    _cohort::clear(m_vecFacePolygonsCohorts);
    _cohort::clear(m_vecConcFacePolygonsCohorts);
    _cohort::clear(m_vecLinesCohorts);
    _cohort::clear(m_vecPointsCohorts);
    _cohort::clear(m_vecNormalVecsCohorts);
    _cohort::clear(m_vecBiNormalVecsCohorts);
    _cohort::clear(m_vecTangentVecsCohorts);
}

/*virtual*/ void _geometry::cleanCachedGeometry()
{
    m_owlInstance = 0;
}