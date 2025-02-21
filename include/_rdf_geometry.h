#include "_geometry.h"

// ************************************************************************************************
class _rdf_geometry : public _geometry
{

private: // Members

	_vertices_f* m_pOriginalVertexBuffer; // Vertices
	bool m_bNeedsRefresh; // The data (geometry) is out of date

public: // Methods

	_rdf_geometry(OwlInstance owlInstance);
	virtual ~_rdf_geometry();

	void loadName();

protected: // Methods

	// _geometry
	virtual void calculateCore() override;
};