#include "_geometry.h"

// ************************************************************************************************
class _rdf_geometry : public _geometry
{

private: // Members

    _vertices_f* m_pOriginalVertexBuffer; // Vertices
    bool m_bNeedsRefresh; // Geometry is out of date

public: // Methods

    _rdf_geometry(OwlInstance owlInstance);
    virtual ~_rdf_geometry();

protected: // Methods

    // _geometry
    virtual void preCalculate() override;
    virtual void calculateCore() override;
    virtual void clean() override;

public: // Methods

    void loadName();
    void loadOriginalData();
    void recalculate();

private: // Methods

    void setRDFFormatSettings();

public: // Properties

    bool& needsRefresh() { return m_bNeedsRefresh; }
};