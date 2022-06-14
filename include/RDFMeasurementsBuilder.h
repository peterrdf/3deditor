#pragma once

#include "Text2RDF.h"

// ------------------------------------------------------------------------------------------------
class CRDFMeasurementsBuilder : public CText2RDF
{

private: // Members
	struct PLANE
	{
		double	a, b, c, d;
	};

	struct VECTOR3
	{
		double	x, y, z;
	};


public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFMeasurementsBuilder(const CString & strText, const CString & strTTFFile, const CString & strRDFFile, int iGeometry);

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFMeasurementsBuilder(const CString & strText, const CString & strTTFFile, int64_t iModel, int iGeometry);

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFMeasurementsBuilder(const CString & strTTFFile, int64_t iModel, int iGeometry);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CRDFMeasurementsBuilder();

	//
	//		test of peter
	//
	double DefineScaling(int64_t iInstance);
	void BuildMeasure(int64_t iInstance, int64_t iProperty, CString text, double scale);

	// --------------------------------------------------------------------------------------------
	// Creates measurements
	void Build(int64_t iInstance, const double * arBoundingBoxMin, const double * arBoundingBoxMax);

private: // Methods

	double	PlaneDistance(const PLANE * pPlane, const VECTOR3 * pPoint);
	void	PlanePointOnPlane(VECTOR3 * pOut, const PLANE * pPlane, const VECTOR3 * pV);
	void	Vec3Cross(VECTOR3 * pOut, const VECTOR3 * pV1, const VECTOR3 * pV2);
	double	Vec3Normalize(VECTOR3 * pInOut);

	// --------------------------------------------------------------------------------------------
	// Line Measurement - lines & arrows
	vector<int64_t> BuildLineMeasurementArrows(const double * arStartPoint, const double * arEndPoint, double dMaxDimension, double dYOffset);
};

