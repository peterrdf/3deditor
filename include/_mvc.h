#pragma once

#include "_geometry.h"
#include "_settings_storage.h"
#include "_texture.h"

#include <set>
#include <string>
using namespace std;

// ************************************************************************************************
enum class enumApplicationProperty : int
{
    All, // Reset
    Projection,
    View,
    GhostView,
    GhostViewTransparency,
    ShowFaces,
    CullFaces,
    ShowFacesWireframes,
    ShowConceptualFacesWireframes,
    ShowLines,
    ShowPoints,
    ShowNormalVectors,
    ShowTangenVectors,
    ShowBiNormalVectors,
    ScaleVectors,
    ShowBoundingBoxes,
    RotationMode,
    ShowCoordinateSystem,
    CoordinateSystemType,
    ShowNavigator,
    PointLightingLocation,
    AmbientLightWeighting,
    SpecularLightWeighting,
    DiffuseLightWeighting,
    MaterialShininess,
    Contrast,
    Brightness,
    Gamma,
    VisibleValuesCountLimit,
    ScalelAndCenterAllVisibleGeometry,
};

// ************************************************************************************************
class _controller;

// ************************************************************************************************
static int64_t s_iInstanceID = 1;

// ************************************************************************************************
class _model
{
    friend class _controller;

protected: // Fields

    wstring m_strPath;
    bool m_bEnable;

    const _model* m_pWorld;

    // http://rdf.bg/gkdoc/CP64/SetVertexBufferOffset.html
    bool m_bUpdteVertexBuffers;
    double m_dOriginalBoundingSphereDiameter;

    // World's dimensions
    float m_fXmin;
    float m_fXmax;
    float m_fYmin;
    float m_fYmax;
    float m_fZmin;
    float m_fZmax;
    float m_fBoundingSphereDiameter;

private: // Fields

    vector<_geometry*> m_vecGeometries;
    // 1...*
    vector<_instance*> m_vecInstances;
    map<int64_t, _instance*> m_mapID2Instance;

    map<wstring, _texture*> m_mapTextures;

public: // Methods

    _model();
    virtual ~_model();

    virtual _instance* loadInstance(int64_t /*iInstance*/) PURE;

    template<typename T>
    T* as()
    {
        return dynamic_cast<T*>(this);
    }

    virtual void scale();
    virtual void zoomTo(_instance* pInstance);
    virtual void zoomToInstances(const set<_instance*>& setInstances);
    virtual void zoomOut();

    _instance* getInstanceByID(int64_t iID) const;

    static int64_t getNextInstanceID() { return s_iInstanceID++; }
    static wstring getInstanceName(OwlInstance owlInstance);
    static const wchar_t* getInstanceClassName(OwlInstance owlInstance);
    static int64_t getInstanceObjectProperty(OwlInstance owlInstance, char* szPropertyName);
    static double getInstanceDoubleProperty(OwlInstance owlInstance, char* szPropertyName);

    _texture* getTexture(const wstring& strTexture);
    virtual _texture* getDefaultTexture() { return nullptr; };

    virtual void resetInstancesEnabledState() { assert(false); }

protected: // Methods

    void setVertexBufferOffset(OwlInstance owlInstance);

    virtual void addGeometry(_geometry* pGeometry);
    virtual void deleteGeometry(_geometry* pGeometry);
    virtual void addInstance(_instance* pInstance);    

    virtual void clean(bool bCloseModel = true);

public: // Properties

    virtual OwlModel getOwlModel() const PURE;

    bool isDecoration() const { return m_strPath.empty(); }
    const wchar_t* getPath() const { return m_strPath.c_str(); }
    bool getEnable() const { return m_bEnable; }
    void setEnable(bool bEnable) { m_bEnable = bEnable; }
    uint64_t getVertexLength() const { return SetFormat(getOwlModel()) / sizeof(float); }

    void getDimensions(float& fXmin, float& fXmax, float& fYmin, float& fYmax, float& fZmin, float& fZmax) const;
    void setDimensions(_model* pSource);
    double getOriginalBoundingSphereDiameter() const { return m_dOriginalBoundingSphereDiameter; }
    float getBoundingSphereDiameter() const { return m_fBoundingSphereDiameter; }

    const vector<_geometry*>& getGeometries() const { return m_vecGeometries; }
    const vector<_instance*>& getInstances() const { return m_vecInstances; }
};

// ************************************************************************************************
class _view
{

protected: // Fields

    _controller* m_pController;

public: // Methods

    _view()
        : m_pController(nullptr)
    {}

    virtual ~_view()
    {}

    // Events
    virtual void preModelLoaded() {}
    virtual void onModelLoaded() {}
    virtual void onModelUpdated() {}
    virtual void postModelLoaded() {}
    virtual void onWorldDimensionsChanged() {}
    virtual void onShowMetaInformation() {}
    virtual void onTargetInstanceChanged(_view* /*pSender*/) {}
    virtual void onInstanceSelected(_view* /*pSender*/) {}
    // 0 - Enable/Disable
    // 1 - Disable All But This
    virtual void onInstanceEnabledStateChanged(_view* /*pSender*/, _instance* /*pInstance*/, int /*iFlag*/) {}
    virtual void onInstancesEnabledStateChanged(_view* /*pSender*/) {}
    virtual void onInstancesShowStateChanged(_view* /*pSender*/) {}
    virtual void onApplicationPropertyChanged(_view* /*pSender*/, enumApplicationProperty /*enApplicationProperty*/) {}
    virtual void onControllerChanged() {}

public: // Properties

    void setController(_controller* pController)
    {
        assert(pController != nullptr);

        m_pController = pController;

        onControllerChanged();
    }

    _controller* getController() const { return m_pController; }
};

// ************************************************************************************************
class _controller
{

private: // Fields

    vector<_model*> m_vecModels;
    set<_view*> m_setViews;
    _settings_storage* m_pSettingsStorage;

protected:

    // Disable all notifications	
    bool m_bUpdatingModel;

    // Selection
    vector<_instance*> m_vecSelectedInstances;

    // Target
    _instance* m_pTargetInstance;

public: // Methods

    _controller();
    virtual ~_controller();

    void setModel(_model* pModel);
    void setModels(const vector<_model*>& vecModels);
    void enableModelsAddIfNeeded(const vector<_model*>& vecModels);
    _instance* loadInstance(int64_t iInstance);

    void getWorldDimensions(float& fXmin, float& fXmax, float& fYmin, float& fYmax, float& fZmin, float& fZmax) const;
    float getWorldBoundingSphereDiameter() const;

    _model* getModelByInstance(OwlModel owlModel) const;
    _instance* getInstanceByID(int64_t iID) const;

    // Events
    void registerView(_view* pView);
    void unRegisterView(_view* pView);

    const set<_view*>& getViews();
    template <class T>
    T* getViewAs()
    {
        set<_view*>::const_iterator itView = m_setViews.begin();
        for (; itView != m_setViews.end(); itView++) {
            T* pView = dynamic_cast<T*>(*itView);
            if (pView != nullptr) {
                return pView;
            }
        }

        return nullptr;
    }

    // Zoom
    void zoomToInstance(_instance* pInstance);
    void zoomToInstances(const set<_instance*>& setInstances);
    void zoomOut();

    // Selection
    void setTargetInstance(_view* pSender, _instance* pInstance);
    _instance* getTargetInstance() const { return m_pTargetInstance; }
    void selectInstance(_view* pSender, _instance* pInstance, bool bAdd = false);
    virtual void selectInstances(_view* pSender, const vector<_instance*>& vecInstance, bool bAdd = false);
    _instance* getSelectedInstance() const; // kept for backward compatibility
    const vector<_instance*>& getSelectedInstances() const { return m_vecSelectedInstances; }
    bool isInstanceSelected(_instance* pInstance) const;

    // Save	
    void saveInstance(OwlInstance owlInstance);
    virtual void saveInstance(_instance* pInstance);
    static wstring validateFileName(const wchar_t* szFileName);

    // Events
    void onModelUpdated();
    void onShowMetaInformation(_instance* /*pInstance*/) { assert(false); }
    void onInstanceEnabledStateChanged(_view* pSender, _instance* pInstance, int iFlag);
    void resetInstancesEnabledState(_view* pSender);
    virtual void onInstancesEnabledStateChanged(_view* pSender);
    virtual void onInstancesShowStateChanged(_view* pSender);
    virtual void onApplicationPropertyChanged(_view* pSender, enumApplicationProperty enApplicationProperty);

protected: // Methods

    virtual void clean();
    virtual void cleanSelection();

public: // Properties

    _model* getModel() const; // kept for backward compatibility
    const vector<_model*>& getModels() const { return m_vecModels; }
    _settings_storage* getSettingsStorage() const { return m_pSettingsStorage; }
};

