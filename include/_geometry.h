#pragma once

#include "engine.h"

#include "glew.h"
#include "wglew.h"

#include "_3DUtils.h"
#include "_material.h"

#include <atlbase.h>

#include <string>
#include <vector>
#include <map>
using namespace std;

// ************************************************************************************************
class _primitives
{

private: // Fields

    int64_t m_iStartIndex;
    int64_t m_iIndicesCount;

public: // Methods

    _primitives(int64_t iStartIndex, int64_t iIndicesCount)
        : m_iStartIndex(iStartIndex)
        , m_iIndicesCount(iIndicesCount)
    {}

    virtual ~_primitives()
    {}

public: // Properties

    int64_t& startIndex() { return m_iStartIndex; }
    int64_t& indicesCount() { return m_iIndicesCount; }
};

// ************************************************************************************************
class _cohort
{

private: // Fields

    vector<GLuint> m_vecIndices;

    GLuint m_iIBO;
    GLsizei m_iIBOOffset;

public: // Methods

    _cohort()
        : m_vecIndices()
        , m_iIBO(0)
        , m_iIBOOffset(0)
    {}

    virtual ~_cohort()
    {}

    static unsigned int* merge(const vector<_cohort*>& vecCohorts, uint32_t& iIndicesCount);

    template<class T>
    static void clear(vector<T*>& vecCohorts)
    {
        for (size_t iCohort = 0; iCohort < vecCohorts.size(); iCohort++) {
            delete vecCohorts[iCohort];
        }

        vecCohorts.clear();
    }

public: // Properties

    vector<GLuint>& indices() { return m_vecIndices; }
    GLuint& IBO() { return m_iIBO; }
    GLsizei& IBOOffset() { return m_iIBOOffset; }
};

// ************************************************************************************************
class _face : public _primitives
{

private: // Fields

    int64_t m_iIndex;

public: // Methods

    _face(int64_t iIndex, int64_t iStartIndex, int64_t iIndicesCount)
        : _primitives(iStartIndex, iIndicesCount)
        , m_iIndex(iIndex)
    {}

    virtual ~_face()
    {}

public: // Properties

    int64_t getIndex() const { return m_iIndex; }
};

// ************************************************************************************************
class _cohortWithMaterial : public _cohort
{

private: // Fields

    vector<_face> m_vecFaces;
    _material m_material;

public: // Methods

    _cohortWithMaterial(const _material& material)
        : _cohort()
        , m_vecFaces()
        , m_material(material)
    {}

    virtual ~_cohortWithMaterial()
    {}

public: // Properties

    vector<_face>& faces() { return m_vecFaces; }
    const _material* getMaterial() const { return &m_material; }
};

// ************************************************************************************************
typedef map<_material, vector<_face>, _materialsComparator> MATERIALS;

// ************************************************************************************************
class _model;
class _instance;

// ************************************************************************************************
class _geometry
{

private: // Fields

    OwlInstance m_owlInstance;

protected: // Fields

    wstring m_strName;
    wstring m_strUniqueName;

    // Geometry
    _vertices_f* m_pVertexBuffer; // Vertices
    _indices_i32* m_pIndexBuffer; // Indices
    int64_t m_iConceptualFacesCount; // Conceptual faces
    bool m_bHasGeometry;
    bool m_bShow;

    // BB/AABB
    _matrix* m_pmtxBBTransformation;
    _vector3d* m_pvecBBMin;
    _vector3d* m_pvecBBMax;
    _vector3d* m_pvecAABBMin;
    _vector3d* m_pvecAABBMax;

    // Primitives (Cache)
    vector<_primitives> m_vecTriangles;
    vector<_primitives> m_vecFacePolygons;
    vector<_primitives> m_vecConcFacePolygons;
    vector<_primitives> m_vecLines;
    vector<_primitives> m_vecPoints;

    // Cohorts (Cache)
    vector<_cohortWithMaterial*> m_vecConcFacesCohorts;
    vector<_cohort*> m_vecFacePolygonsCohorts;
    vector<_cohort*> m_vecConcFacePolygonsCohorts;
    vector<_cohortWithMaterial*> m_vecLinesCohorts;
    vector<_cohortWithMaterial*> m_vecPointsCohorts;

    // Vectors (Cache)
    vector<_cohort*> m_vecNormalVecsCohorts;
    vector<_cohort*> m_vecBiNormalVecsCohorts;
    vector<_cohort*> m_vecTangentVecsCohorts;

    // Instances
    vector<_instance*> m_vecInstances;

    // VBO (OpenGL)
    GLuint m_iVBO;
    GLsizei m_iVBOOffset;

public: // Fields

    const uint32_t DEFAULT_COLOR_R = 175;
    const uint32_t DEFAULT_COLOR_G = 175;
    const uint32_t DEFAULT_COLOR_B = 175;
    const uint32_t DEFAULT_COLOR_A = 255;

public: // Methods

    _geometry(OwlInstance iInstance);
    virtual ~_geometry();

    void calculateBB(
        _instance* pInstance,
        float& fXmin, float& fXmax,
        float& fYmin, float& fYmax,
        float& fZmin, float& fZmax) const;

    void calculateBB(
        double dX, double dY, double dZ,
        const _matrix4x4* pTransformationMatrix,
        float& fXmin, float& fXmax,
        float& fYmin, float& fYmax,
        float& fZmin, float& fZmax) const;

    static void calculateBB(
        OwlInstance owlInstance,
        double& dXmin, double& dXmax,
        double& dYmin, double& dYmax,
        double& dZmin, double& dZmax);

    void scale(float fScaleFactor);
    void translate(float fX, float fY, float fZ);

    void addInstance(_instance* pInstance);
    virtual void enableInstances(bool bEnable);
    long getEnabledInstancesCount() const;

protected: // Methods

    bool calculateInstance(_vertices_f* pVertexBuffer, _indices_i32* pIndexBuffer);

    void calculate()
    {
        preCalculate();
        calculateCore();
        postCalculate();
    }

    virtual void preCalculate() {}
    virtual void calculateCore();
    virtual void postCalculate() {}

    void addTriangles(int64_t iConceptualFaceIndex, int64_t iStartIndex, int64_t iIndicesCount, _material& material, MATERIALS& mapMaterials)
    {
        m_vecTriangles.push_back(_primitives(iStartIndex, iIndicesCount));

        addMaterial(iConceptualFaceIndex, iStartIndex, iIndicesCount, material, mapMaterials);
    }

    void addFacePolygons(int64_t iStartIndex, int64_t iIndicesCount)
    {
        m_vecFacePolygons.push_back(_primitives(iStartIndex, iIndicesCount));
    }

    void addConcFacePolygons(int64_t iStartIndex, int64_t iIndicesCount)
    {
        m_vecConcFacePolygons.push_back(_primitives(iStartIndex, iIndicesCount));
    }

    void addLines(int64_t iConceptualFaceIndex, int64_t iStartIndex, int64_t iIndicesCount, _material& material, MATERIALS& mapMaterials)
    {
        m_vecLines.push_back(_primitives(iStartIndex, iIndicesCount));

        addMaterial(iConceptualFaceIndex, iStartIndex, iIndicesCount, material, mapMaterials);
    }

    void addPoints(int64_t iConceptualFaceIndex, int64_t iStartIndex, int64_t iIndicesCount, _material& material, MATERIALS& mapMaterials)
    {
        m_vecPoints.push_back(_primitives(iStartIndex, iIndicesCount));

        addMaterial(iConceptualFaceIndex, iStartIndex, iIndicesCount, material, mapMaterials);
    }

    void addMaterial(int64_t iConceptualFaceIndex, int64_t iStartIndex, int64_t iIndicesCount, _material& material, MATERIALS& mapMaterials);

    virtual uint32_t getDefaultColor() const
    {
        return 256 * 256 * 256 * DEFAULT_COLOR_R +
            256 * 256 * DEFAULT_COLOR_G +
            256 * DEFAULT_COLOR_B +
            DEFAULT_COLOR_A;
    }

    wstring getConcFaceTexture(ConceptualFace iConceptualFace);
    float getTransparency(OwlInstance owlMaterialInstance);

    void buildConcFacesCohorts(MATERIALS& mapMaterials, const GLsizei INDICES_COUNT_LIMIT);
    void buildFacePolygonsCohorts(const GLsizei INDICES_COUNT_LIMIT);
    void buildConcFacePolygonsCohorts(const GLsizei INDICES_COUNT_LIMIT);
    void buildLinesCohorts(MATERIALS& mapMaterials, const GLsizei INDICES_COUNT_LIMIT);
    void buildPointsCohorts(MATERIALS& mapMaterials, const GLsizei INDICES_COUNT_LIMIT);

    virtual void clean();
    virtual void cleanCachedGeometry();

public: // Properties

    // Metadata
    virtual OwlInstance getOwlInstance() { return m_owlInstance; }
    OwlClass getOwlClass() { return ::GetInstanceClass(getOwlInstance()); }
    virtual OwlModel getOwlModel() { return ::GetModel(getOwlInstance()); }
    bool isReferenced() { return ::GetInstanceInverseReferencesByIterator(getOwlInstance(), 0) != 0; }
    virtual bool isPlaceholder() const { return false; }
    const wchar_t* getName() const { return m_strName.c_str(); }
    const wchar_t* getUniqueName() const { return m_strUniqueName.c_str(); }

    // Geometry
    int32_t* getIndices() const { return m_pIndexBuffer != nullptr ? m_pIndexBuffer->data() : nullptr; }
    int64_t getIndicesCount() const { return m_pIndexBuffer != nullptr ? m_pIndexBuffer->size() : 0; }
    float* getVertices() const { return m_pVertexBuffer != nullptr ? m_pVertexBuffer->data() : nullptr; }
    int64_t getVerticesCount() const { return m_pVertexBuffer != nullptr ? m_pVertexBuffer->size() : 0; }
    uint32_t getVertexLength() { return (uint32_t)SetFormat(getOwlModel()) / sizeof(float); }
    int64_t getConceptualFacesCount() const { return m_iConceptualFacesCount; }
    bool getShow() const { return m_bShow; }
    void setShow(bool bShow) { m_bShow = bShow; }
    virtual bool hasGeometry() const { return (getVerticesCount() > 0) && (getIndicesCount() > 0); }

    // BB
    _matrix* getBBTransformation() const { return m_pmtxBBTransformation; }
    _vector3d* getBBMin() const { return m_pvecBBMin; }
    _vector3d* getBBMax() const { return m_pvecBBMax; }
    _vector3d* getAABBMin() const { return m_pvecAABBMin; }
    _vector3d* getAABBMax() const { return m_pvecAABBMax; }
    virtual bool ignoreBB() const { return isPlaceholder(); }

    // Primitives
    const vector<_primitives>& getTriangles() const { return m_vecTriangles; }
    const vector<_primitives>& getLines() const { return m_vecLines; }
    const vector<_primitives>& getPoints() const { return m_vecPoints; }
    const vector<_primitives>& getFacePolygons() const { return m_vecFacePolygons; }
    const vector<_primitives>& getConcFacePolygons() const { return m_vecConcFacePolygons; }

    // Cohorts
    vector<_cohortWithMaterial*>& concFacesCohorts() { return m_vecConcFacesCohorts; }
    vector<_cohort*>& facePolygonsCohorts() { return m_vecFacePolygonsCohorts; }
    vector<_cohort*>& concFacePolygonsCohorts() { return m_vecConcFacePolygonsCohorts; }
    vector<_cohortWithMaterial*>& linesCohorts() { return m_vecLinesCohorts; }
    vector<_cohortWithMaterial*>& pointsCohorts() { return m_vecPointsCohorts; }

    // Vectors
    vector<_cohort*>& normalVecsCohorts() { return m_vecNormalVecsCohorts; }
    vector<_cohort*>& biNormalVecsCohorts() { return m_vecBiNormalVecsCohorts; }
    vector<_cohort*>& tangentVecsCohorts() { return m_vecTangentVecsCohorts; }

    // Instances
    const vector<_instance*>& getInstances() { return m_vecInstances; }

    // VBO (OpenGL)
    GLuint& VBO() { return m_iVBO; }
    GLsizei& VBOOffset() { return m_iVBOOffset; }
};

// ************************************************************************************************
struct _geometriesComparator
{
    bool operator()(const _geometry* g1, const _geometry* g2) const
    {
        return wcscmp(g1->getName(), g2->getName()) < 0;
    }
};
