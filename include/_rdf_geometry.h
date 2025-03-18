#include "_geometry.h"

// ************************************************************************************************
class _rdf_geometry : public _geometry
{

private: // Fields

    _vertices_f* m_pOriginalVertexBuffer; // Vertices

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
    void reload();
    void recalculate();

private: // Methods

    void setRDFFormatSettings();
};