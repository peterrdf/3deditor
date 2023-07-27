#include "stdafx.h"
#include "RDFMeasurementsBuilder.h"

#define _USE_MATH_DEFINES
#include <math.h>

// ------------------------------------------------------------------------------------------------
CRDFMeasurementsBuilder::CRDFMeasurementsBuilder(const CString & strText, const CString & strTTFFile, const CString & strRDFFile, int iGeometry)
	: CText2RDF(strText, strTTFFile, strRDFFile, iGeometry)
{
}

// ------------------------------------------------------------------------------------------------
CRDFMeasurementsBuilder::CRDFMeasurementsBuilder(const CString & strText, const CString & strTTFFile, int64_t iModel, int iGeometry)
	: CText2RDF(strText, strTTFFile, iModel, iGeometry)
{
}

// ------------------------------------------------------------------------------------------------
CRDFMeasurementsBuilder::CRDFMeasurementsBuilder(const CString & strTTFFile, int64_t iModel, int iGeometry)
	: CText2RDF(strTTFFile, iModel, iGeometry)
{
}

// ------------------------------------------------------------------------------------------------
CRDFMeasurementsBuilder::~CRDFMeasurementsBuilder()
{
}

//		test by Peter.
double CRDFMeasurementsBuilder::DefineScaling(int64_t iInstance)
{
	double	transformationMatrix[12], startVector[3], endVector[3];

	GetBoundingBox(
			iInstance,
			transformationMatrix,
			startVector,
			endVector
		);
	
	double	maxDistance = endVector[0] - startVector[0];
	if (maxDistance < endVector[1] - startVector[1]) {
		maxDistance = endVector[1] - startVector[1];
	}
	if (maxDistance < endVector[2] - startVector[2]) {
		maxDistance = endVector[2] - startVector[2];
	}

	return	maxDistance;
}

double CRDFMeasurementsBuilder::PlaneDistance(
				const PLANE		* pPlane,
				const VECTOR3	* pPoint
			)
{
	double	numerator = pPoint->x * pPlane->a + pPoint->y * pPlane->b + pPoint->z * pPlane->c + pPlane->d,
			denominator = sqrt(pPlane->a * pPlane->a + pPlane->b * pPlane->b + pPlane->c * pPlane->c);

	if (denominator < -0.0000000000000001 || denominator > 0.0000000000000001) {
		return	numerator / denominator;
	}
	else {
		ASSERT(false);
		return	1000;//0;
	}
}

void CRDFMeasurementsBuilder::PlanePointOnPlane(
				VECTOR3			* pOut,
				const PLANE		* pPlane,
				const VECTOR3	* pV
			)
{
	double	distance = PlaneDistance(pPlane, pV);
	pOut->x = pV->x - distance * pPlane->a;
	pOut->y = pV->y - distance * pPlane->b;
	pOut->z = pV->z - distance * pPlane->c;
}

void CRDFMeasurementsBuilder::Vec3Cross(
				VECTOR3			* pOut,
				const VECTOR3	* pV1,
				const VECTOR3	* pV2
			)
{
	VECTOR3 v;

	v.x = pV1->y * pV2->z - pV1->z * pV2->y;
	v.y = pV1->z * pV2->x - pV1->x * pV2->z;
	v.z = pV1->x * pV2->y - pV1->y * pV2->x;

	pOut->x = v.x;
	pOut->y = v.y;
	pOut->z = v.z;
}

double CRDFMeasurementsBuilder::Vec3Normalize(
				VECTOR3	* pInOut
			)
{
	double	size, sqrtSize = 0,
			x = pInOut->x, y = pInOut->y, z = pInOut->z;

	size = x * x + y * y + z * z;

	if	(size > 0.0000000000000001) {
		sqrtSize = sqrt(size);
		pInOut->x = x / sqrtSize;
		pInOut->y = y / sqrtSize;
		pInOut->z = z / sqrtSize;
	} else {
		if	(pInOut->x) { pInOut->x = 1000; }
		if	(pInOut->y) { pInOut->y = 1000; }
		if	(pInOut->z) { pInOut->z = 1000; }
	}

	return	sqrtSize;
}

void CRDFMeasurementsBuilder::BuildMeasure(int64_t iInstance, int64_t iProperty, CString text, double scale)
{
	ASSERT(iInstance != 0);
	ASSERT(iProperty != 0);

	int64_t	card = 0;
	double	* values = nullptr;
	GetDatatypeProperty(iInstance, iProperty, (void**) &values, &card);

	values[0] += 0.1235;
	SetDatatypeProperty(iInstance, iProperty, (void*) values, card);

	bool	foundAutoMeasure = false;
	PLANE	foundPlane;
	foundPlane.a = 0;
	foundPlane.b = 0;
	foundPlane.c = 1;
	foundPlane.d = 0;

	double	eps = 0.000001;

	if (true) {
		int64_t	vertexBuffserSize = 0, indexBuffserSize = 0;
		CalculateInstance(iInstance, &vertexBuffserSize, &indexBuffserSize, nullptr);
		if (vertexBuffserSize && indexBuffserSize) {
			int		VERTEX_ELEMENT_SIZE = (6 + 2 + 4 + 6);
			float	* vertexBuffer = new float[VERTEX_ELEMENT_SIZE * vertexBuffserSize];
			UpdateInstanceVertexBuffer(iInstance, vertexBuffer);

			int32_t	* indexBuffer = new int32_t[indexBuffserSize];
			UpdateInstanceIndexBuffer(iInstance, indexBuffer);

			int64_t	cnt = (int) GetConceptualFaceCnt(iInstance);
			
			PLANE	* setPlane = new PLANE[cnt];
			int64_t	* setStartIndex = new int64_t[cnt];
			int64_t	* setNoTriangles = new int64_t[cnt];

			for (int64_t index = 0; index < cnt; index++) {
				int64_t	startIndex = 0, noTrianglesIndices = 0;
				GetConceptualFace(iInstance, index, &startIndex, &noTrianglesIndices, nullptr, nullptr, nullptr, nullptr);

				int64_t	noTriangles = noTrianglesIndices / 3;
				setStartIndex[index] = startIndex;
				setNoTriangles[index] = noTriangles;
				if (noTriangles) {
					setPlane[index].a = vertexBuffer[VERTEX_ELEMENT_SIZE * indexBuffer[startIndex] + 3];
					setPlane[index].b = vertexBuffer[VERTEX_ELEMENT_SIZE * indexBuffer[startIndex] + 4];
					setPlane[index].c = vertexBuffer[VERTEX_ELEMENT_SIZE * indexBuffer[startIndex] + 5];
					setPlane[index].d = -(
							vertexBuffer[VERTEX_ELEMENT_SIZE * indexBuffer[startIndex] + 0] * vertexBuffer[VERTEX_ELEMENT_SIZE * indexBuffer[startIndex] + 3] +
							vertexBuffer[VERTEX_ELEMENT_SIZE * indexBuffer[startIndex] + 1] * vertexBuffer[VERTEX_ELEMENT_SIZE * indexBuffer[startIndex] + 4] +
							vertexBuffer[VERTEX_ELEMENT_SIZE * indexBuffer[startIndex] + 2] * vertexBuffer[VERTEX_ELEMENT_SIZE * indexBuffer[startIndex] + 5]
						);
				}
			}

			//
			//	First algorithm:
			//		find tuples with the parrallel plane (and measure their distance)
			//
			for (int i = 0; i < cnt; i++) {
				for (int j = i + 1; j < cnt; j++) {
					if (setNoTriangles[i] && setNoTriangles[j]) {
						if ((setPlane[i].a == setPlane[j].a) &&
							(setPlane[i].b == setPlane[j].b) &&
							(setPlane[i].c == setPlane[j].c)) {
							double	distance = fabs(setPlane[i].d - setPlane[j].d);
							//
							//	This tuple has parrallel planes
							//
							if (fabs(distance - values[0]) < eps) {
								foundAutoMeasure = true;
								foundPlane.a = setPlane[i].a;
								foundPlane.b = setPlane[i].b;
								foundPlane.c = setPlane[i].c;
								foundPlane.d = setPlane[i].d;
							}
						}
						else if ((setPlane[i].a == -setPlane[j].a) &&
								 (setPlane[i].b == -setPlane[j].b) &&
								 (setPlane[i].c == -setPlane[j].c)) {
							double	distance = fabs(setPlane[i].d + setPlane[j].d);
							//
							//	This tuple has parrallel planes
							//
							if (fabs(distance - values[0]) < eps) {
								foundAutoMeasure = true;
								foundPlane.a = setPlane[i].a;
								foundPlane.b = setPlane[i].b;
								foundPlane.c = setPlane[i].c;
								foundPlane.d = setPlane[i].d;
							}
						}
					}
				}
			}
		}
	}

	values[0] -= 0.1235;
	SetDatatypeProperty(iInstance, iProperty, (void*) values, card);

	if (foundAutoMeasure) {
		Text() = text;

		Run();

		vector<int64_t> vecLetters(GetLetters());
		ASSERT(!vecLetters.empty());

		int64_t iCollectionClass = GetClassByName(GetModel(), "Collection");
		ASSERT(iCollectionClass != 0);

		int64_t iTransformationClass = GetClassByName(GetModel(), "Transformation");
		ASSERT(iTransformationClass != 0);

		int64_t iMatrixClass = GetClassByName(GetModel(), "Matrix");
		ASSERT(iMatrixClass != 0);

		int64_t iMatrixMultiplicationClass = GetClassByName(GetModel(), "MatrixMultiplication");
		ASSERT(iMatrixMultiplicationClass != 0);

		// Collection text (inner)
		int64_t iCollectionInstanceText = CreateInstance(iCollectionClass);
		ASSERT(iCollectionInstanceText != 0);

		// object
		SetObjectProperty(iCollectionInstanceText, GetPropertyByName(GetModel(), "objects"), vecLetters.data(), vecLetters.size());

		// Transformation outer
		int64_t iTransformationInstanceOuter = CreateInstance(iTransformationClass);
		ASSERT(iTransformationInstanceOuter != 0);

		// Transformation inner (text)
		int64_t iTransformationInstanceText = CreateInstance(iTransformationClass);
		ASSERT(iTransformationInstanceText != 0);

		// object
		SetObjectProperty(iTransformationInstanceText, GetPropertyByName(GetModel(), "object"), &iCollectionInstanceText, 1);

		// matrix multiplication
		int64_t iMatrixMultiplicationInstance = CreateInstance(iMatrixMultiplicationClass);
		ASSERT(iMatrixMultiplicationInstance != 0);

		// matrix outer I
		int64_t iMatrixInstanceOuterI = CreateInstance(iMatrixClass);
		ASSERT(iMatrixInstanceOuterI != 0);

		// matrix outer II
		int64_t iMatrixInstanceOuterII = CreateInstance(iMatrixClass);
		ASSERT(iMatrixInstanceOuterII != 0);

		// matrix inner (text)
		int64_t iMatrixInstanceText = CreateInstance(iMatrixClass);
		ASSERT(iMatrixInstanceText != 0);

		VECTOR3	refDirection, normal;
		
		refDirection.x = foundPlane.a;
		refDirection.y = foundPlane.b;
		refDirection.z = foundPlane.c;

		if ((fabs(refDirection.x) <= fabs(refDirection.y)) && (fabs(refDirection.x) <= fabs(refDirection.z))) {
			normal.x = 0;
			normal.y = -refDirection.z;
			normal.z = refDirection.y;
		}
		else if ((fabs(refDirection.y) <= fabs(refDirection.x)) && (fabs(refDirection.y) <= fabs(refDirection.z))) {
			normal.x = -refDirection.z;
			normal.y = 0;
			normal.z = refDirection.x;
		}
		else {
			normal.x = -refDirection.y;
			normal.y = refDirection.x;
			normal.z = 0;
		}
		
		Vec3Normalize(&normal);

		VECTOR3	axisDirection;

		Vec3Cross(&axisDirection, &normal, &refDirection);

//		refDirection.x *= scale;
//		refDirection.y *= scale;
//		refDirection.z *= scale;

//		axisDirection.x *= scale;
//		axisDirection.y *= scale;
//		axisDirection.z *= scale;

//		normal.x *= scale;
//		normal.y *= scale;
//		normal.z *= scale;

		SetDatatypeProperty(iMatrixInstanceOuterI, GetPropertyByName(GetModel(), "_11"), &refDirection.x, 1);
		SetDatatypeProperty(iMatrixInstanceOuterI, GetPropertyByName(GetModel(), "_12"), &refDirection.y, 1);
		SetDatatypeProperty(iMatrixInstanceOuterI, GetPropertyByName(GetModel(), "_13"), &refDirection.z, 1);

		SetDatatypeProperty(iMatrixInstanceOuterI, GetPropertyByName(GetModel(), "_21"), &axisDirection.x, 1);
		SetDatatypeProperty(iMatrixInstanceOuterI, GetPropertyByName(GetModel(), "_22"), &axisDirection.y, 1);
		SetDatatypeProperty(iMatrixInstanceOuterI, GetPropertyByName(GetModel(), "_23"), &axisDirection.z, 1);

		SetDatatypeProperty(iMatrixInstanceOuterI, GetPropertyByName(GetModel(), "_31"), &normal.x, 1);
		SetDatatypeProperty(iMatrixInstanceOuterI, GetPropertyByName(GetModel(), "_32"), &normal.y, 1);
		SetDatatypeProperty(iMatrixInstanceOuterI, GetPropertyByName(GetModel(), "_33"), &normal.z, 1);

		double	width = -values[0];
		SetDatatypeProperty(iMatrixInstanceOuterII, GetPropertyByName(GetModel(), "_41"), &width, 1);
		SetDatatypeProperty(iMatrixInstanceOuterII, GetPropertyByName(GetModel(), "_42"), &scale, 1);

		double	_scale = scale / 200;

//		double dTransformedTextWidth = GetTextWidth() / 2;
		double _11 = _scale;
		double _22 = _scale;
		double _33 = _scale;
		double _41 = ((values[0] / 2) - _scale * (GetTextWidth() / 2));
		double _42 = _scale * 4;// dYmax + (dYmax * .03);
		SetDatatypeProperty(iMatrixInstanceText, GetPropertyByName(GetModel(), "_11"), &_11, 1);
		SetDatatypeProperty(iMatrixInstanceText, GetPropertyByName(GetModel(), "_22"), &_22, 1);
		SetDatatypeProperty(iMatrixInstanceText, GetPropertyByName(GetModel(), "_33"), &_33, 1);
		SetDatatypeProperty(iMatrixInstanceText, GetPropertyByName(GetModel(), "_41"), &_41, 1);
		SetDatatypeProperty(iMatrixInstanceText, GetPropertyByName(GetModel(), "_42"), &_42, 1);

		SetObjectProperty(iTransformationInstanceText, GetPropertyByName(GetModel(), "matrix"), &iMatrixInstanceText, 1);

		/*
		* Line
		*/

//		VECTOR3	centroid;
//		GetCentroid(iInstance, nullptr, nullptr, &centroid.x);

		VECTOR3	arMin, arMax;
//		PlanePointOnPlane(&arMin, &foundPlane, &centroid);
		arMin.x = 0;
		arMin.y = 0;
		arMin.z = 0;

		//	BuildLineMeasurementArrows(arMin, arMax, dMaxDimension, dYmax + (dYmax * 0.02));
		arMax.x = values[0];
		arMax.y = 0;
		arMax.z = 0;

//		SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_41"), &arMin.x, 1);
//		SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_42"), &arMin.y, 1);
//		SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_43"), &arMin.z, 1);

		vector<int64_t> vecInstances = BuildLineMeasurementArrows(&arMin.x, &arMax.x, scale, _scale * 1.03);

		// Collection measure + text (outer)
		int64_t iCollectionInstanceOuter = CreateInstance(iCollectionClass);
		ASSERT(iCollectionInstanceOuter != 0);
		
//		vecInstances.push_back(iTransformationInstanceText);

		int64_t	_card = vecInstances.size(),
				* _values = new int64_t[_card + 1];
		for (int i = 0; i < _card; i++) {
			_values[i] = vecInstances[i];
		}
		_values[_card] = iTransformationInstanceText;
		SetObjectProperty(iCollectionInstanceOuter, GetPropertyByName(GetModel(), "objects"), _values, _card + 1);

		//
		//	Now apply the outer translation / rotation
		//

		SetObjectProperty(iTransformationInstanceOuter, GetPropertyByName(GetModel(), "matrix"), &iMatrixMultiplicationInstance, 1);
		SetObjectProperty(iTransformationInstanceOuter, GetPropertyByName(GetModel(), "object"), &iCollectionInstanceOuter, 1);

		SetObjectProperty(iMatrixMultiplicationInstance, GetPropertyByName(GetModel(), "firstMatrix"), &iMatrixInstanceOuterII, 1);
		SetObjectProperty(iMatrixMultiplicationInstance, GetPropertyByName(GetModel(), "secondMatrix"), &iMatrixInstanceOuterI, 1);
	}

	return;

//	MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Unsupported type.", L"Error", MB_ICONERROR | MB_OK);
}

// ------------------------------------------------------------------------------------------------
void CRDFMeasurementsBuilder::Build(int64_t iInstance, const double * arBoundingBoxMin, const double * arBoundingBoxMax)
{
	ASSERT(iInstance != 0);
	ASSERT(arBoundingBoxMin != nullptr);
	ASSERT(arBoundingBoxMax != nullptr);

	int64_t iClassInstance = GetInstanceClass(iInstance);
	ASSERT(iClassInstance != 0);

	char * szClassName = nullptr;
	GetNameOfClass(iClassInstance, &szClassName);

	if (string(szClassName) == string("Cube"))
	{
		double dXmin = arBoundingBoxMin[0];
		double dYmin = arBoundingBoxMin[1];
		double dZmin = arBoundingBoxMin[2];

		double dXmax = arBoundingBoxMax[0];
		double dYmax = arBoundingBoxMax[1];
		double dZmax = arBoundingBoxMax[2];

		double dMaxDimension = dXmax - dXmin;
		dMaxDimension = max(dMaxDimension, dYmax - dYmin);
		dMaxDimension = max(dMaxDimension, dZmax - dZmin);

		/*
		* Text
		*/

		Text() = L"length = ";
		Text() += std::to_wstring(dXmax - dXmin).c_str();

		Run();

		vector<int64_t> vecLetters(GetLetters());
		ASSERT(!vecLetters.empty());

		int64_t iCollectionClass = GetClassByName(GetModel(), "Collection");
		ASSERT(iCollectionClass != 0);

		int64_t iTransformationClass = GetClassByName(GetModel(), "Transformation");
		ASSERT(iTransformationClass != 0);

		int64_t iMatrixClass = GetClassByName(GetModel(), "Matrix");
		ASSERT(iMatrixClass != 0);		

		// Collection
		int64_t iCollectionInstance = CreateInstance(iCollectionClass);
		ASSERT(iCollectionInstance != 0);

		// object
		SetObjectProperty(iCollectionInstance, GetPropertyByName(GetModel(), "objects"), vecLetters.data(), vecLetters.size()); 

		// Transformation
		int64_t iTransformationInstance = CreateInstance(iTransformationClass);
		ASSERT(iTransformationInstance != 0);

		// object
		SetObjectProperty(iTransformationInstance, GetPropertyByName(GetModel(), "object"), &iCollectionInstance, 1);

		// matrix
		int64_t iMatrixInstance = CreateInstance(iMatrixClass);
		ASSERT(iMatrixInstance != 0);

		// Scale
		double _11 = (dMaxDimension / 5.) / GetTextWidth();
		double _22 = (dMaxDimension / 5.) / GetTextWidth();
		double _33 = (dMaxDimension / 5.) / GetTextWidth();
		SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_11"), &_11, 1);
		SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_22"), &_22, 1);
		SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_33"), &_33, 1);

		double dTransformedTextWidth = ((dMaxDimension / 5.) / GetTextWidth()) * GetTextWidth();

		// Translate
		double _41 = 0.;
		if ((dXmax - dXmin) > dTransformedTextWidth)
		{
			_41 = dXmin + ((dXmax - dXmin) - dTransformedTextWidth) / 2.;
		}
		else
		{
			_41 = dXmin;
		}

		double _42 = dYmax + (dYmax * .03);
		SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_41"), &_41, 1);		
		SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_42"), &_42, 1);

		SetObjectProperty(iTransformationInstance, GetPropertyByName(GetModel(), "matrix"), &iMatrixInstance, 1);

		/*
		* Line
		*/
		double arMin[3] = { dXmin, dYmax, dZmin };
		double arMax[3] = {dXmax, dYmax, dZmin};
		BuildLineMeasurementArrows(arMin, arMax, dMaxDimension, dYmax + (dYmax * 0.02));

		return;
	} // if (string(szClassName) == string("Cube"))

	if (string(szClassName) == string("Cone"))
	{
		double dXmin = arBoundingBoxMin[0];
		double dYmin = arBoundingBoxMin[1];
		double dZmin = arBoundingBoxMin[2];

		double dXmax = arBoundingBoxMax[0];
		double dYmax = arBoundingBoxMax[1];
		double dZmax = arBoundingBoxMax[2];

		double dMaxDimension = dXmax - dXmin;
		dMaxDimension = max(dMaxDimension, dYmax - dYmin);
		dMaxDimension = max(dMaxDimension, dZmax - dZmin);

		/*
		* Radius
		*/
		{
			/*
			* Text
			*/

			Text() = L"radius = ";
			Text() += std::to_wstring((dXmax - dXmin) / 2.).c_str();

			Run();

			vector<int64_t> vecLetters(GetLetters());
			ASSERT(!vecLetters.empty());

			int64_t iCollectionClass = GetClassByName(GetModel(), "Collection");
			ASSERT(iCollectionClass != 0);

			int64_t iTransformationClass = GetClassByName(GetModel(), "Transformation");
			ASSERT(iTransformationClass != 0);

			int64_t iMatrixClass = GetClassByName(GetModel(), "Matrix");
			ASSERT(iMatrixClass != 0);

			// Collection
			int64_t iCollectionInstance = CreateInstance(iCollectionClass);
			ASSERT(iCollectionInstance != 0);

			// object
			SetObjectProperty(iCollectionInstance, GetPropertyByName(GetModel(), "objects"), vecLetters.data(), vecLetters.size());

			// Transformation
			int64_t iTransformationInstance = CreateInstance(iTransformationClass);
			ASSERT(iTransformationInstance != 0);

			// object
			SetObjectProperty(iTransformationInstance, GetPropertyByName(GetModel(), "object"), &iCollectionInstance, 1);

			// matrix
			int64_t iMatrixInstance = CreateInstance(iMatrixClass);
			ASSERT(iMatrixInstance != 0);

			// Scale
			double _11 = (dMaxDimension / 5.) / GetTextWidth();
			double _22 = (dMaxDimension / 5.) / GetTextWidth();
			double _33 = (dMaxDimension / 5.) / GetTextWidth();
			SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_11"), &_11, 1);
			SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_22"), &_22, 1);
			SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_33"), &_33, 1);

			double dTransformedTextWidth = ((dMaxDimension / 5.) / GetTextWidth()) * GetTextWidth();

			// Translate
			double _41 = 0.;
			if (((dXmax - dXmin) / 2.) > dTransformedTextWidth)
			{
				_41 = dXmin + ((dXmax - dXmin) / 2.) + (((dXmax - dXmin) / 2.) - dTransformedTextWidth) / 2.;
			}
			else
			{
				_41 = dXmin + ((dXmax - dXmin) / 2.);
			}

			double _42 = dYmax + (dYmax * .03);
			SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_41"), &_41, 1);
			SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_42"), &_42, 1);

			SetObjectProperty(iTransformationInstance, GetPropertyByName(GetModel(), "matrix"), &iMatrixInstance, 1);

			/*
			* Line
			*/
			double arMin[3] = { dXmin + ((dXmax - dXmin) / 2.), dYmin + ((dYmax - dYmin) / 2.), dZmin };
			double arMax[3] = { dXmax, dYmin + ((dYmax - dYmin) / 2.), dZmin };
			BuildLineMeasurementArrows(arMin, arMax, dMaxDimension, dYmax + (dYmax * 0.02));
		}

		/*
		* Height
		*/
		{
			/*
			* Text
			*/

			Text() = L"height = ";
			Text() += std::to_wstring(dZmax - dZmin).c_str();

			Run();

			vector<int64_t> vecLetters(GetLetters());
			ASSERT(!vecLetters.empty());

			int64_t iCollectionClass = GetClassByName(GetModel(), "Collection");
			ASSERT(iCollectionClass != 0);

			int64_t iTransformationClass = GetClassByName(GetModel(), "Transformation");
			ASSERT(iTransformationClass != 0);

			int64_t iMatrixClass = GetClassByName(GetModel(), "Matrix");
			ASSERT(iMatrixClass != 0);

			// Collection
			int64_t iCollectionInstance = CreateInstance(iCollectionClass);
			ASSERT(iCollectionInstance != 0);

			// objects
			SetObjectProperty(iCollectionInstance, GetPropertyByName(GetModel(), "objects"), vecLetters.data(), vecLetters.size());

			// Transformation - Scale and Translate
			int64_t iTransformationInstance = CreateInstance(iTransformationClass);
			ASSERT(iTransformationInstance != 0);

			// object
			SetObjectProperty(iTransformationInstance, GetPropertyByName(GetModel(), "object"), &iCollectionInstance, 1);

			// matrix
			int64_t iMatrixInstance = CreateInstance(iMatrixClass);
			ASSERT(iMatrixInstance != 0);

			// Scale
			double _11 = (dMaxDimension / 5.) / GetTextWidth();
			double _22 = (dMaxDimension / 5.) / GetTextWidth();
			double _33 = (dMaxDimension / 5.) / GetTextWidth();
			SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_11"), &_11, 1);
			SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_22"), &_22, 1);
			SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_33"), &_33, 1);

			double dTransformedTextWidth = ((dMaxDimension / 5.) / GetTextWidth()) * GetTextWidth();

			// Translate
			double _41 = 0.;
			if ((dZmax - dZmin) > dTransformedTextWidth)
			{
				_41 = dXmin + ((dXmax - dXmin) / 2.) + ((dZmax - dZmin) - dTransformedTextWidth) / 2.;
			}
			else
			{
				_41 = dXmin + ((dXmax - dXmin) / 2.);
			}
			
			double _42 = dYmax + (dYmax * .03);
			SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_41"), &_41, 1);
			SetDatatypeProperty(iMatrixInstance, GetPropertyByName(GetModel(), "_42"), &_42, 1);

			SetObjectProperty(iTransformationInstance, GetPropertyByName(GetModel(), "matrix"), &iMatrixInstance, 1);			

			/*
			* Line
			*/
			double arMin[3] = { dXmin + ((dXmax - dXmin) / 2.), dYmin + ((dYmax - dYmin) / 2.), dZmin };
			double arMax[3] = { dXmin + ((dXmax - dXmin) / 2.) + (dZmax - dZmin), dYmin + ((dYmax - dYmin) / 2.), dZmin };
			vector<int64_t> vecInstances = BuildLineMeasurementArrows(arMin, arMax, dMaxDimension, dYmax + (dYmax * 0.02));

			vecInstances.push_back(iTransformationInstance);

			// Collection
			iCollectionInstance = CreateInstance(iCollectionClass);
			ASSERT(iCollectionInstance != 0);

			// object
			SetObjectProperty(iCollectionInstance, GetPropertyByName(GetModel(), "objects"), vecInstances.data(), vecInstances.size());

			// Transformation - Rotate
			int64_t iRotateTransformationInstance = CreateInstance(iTransformationClass);
			ASSERT(iRotateTransformationInstance != 0);

			// object
			SetObjectProperty(iRotateTransformationInstance, GetPropertyByName(GetModel(), "object"), &iCollectionInstance, 1);

			// matrix
			int64_t iRotateMatrixInstance = CreateInstance(iMatrixClass);
			ASSERT(iRotateMatrixInstance != 0);

			// Rotate
			_11 = cos(270. * M_PI / 180.);
			double _13 = -sin(270. * M_PI / 180.);
			double _31 = sin(270. * M_PI / 180.);
			_33 = cos(270. * M_PI / 180.);
			SetDatatypeProperty(iRotateMatrixInstance, GetPropertyByName(GetModel(), "_11"), &_11, 1);
			SetDatatypeProperty(iRotateMatrixInstance, GetPropertyByName(GetModel(), "_13"), &_13, 1);
			SetDatatypeProperty(iRotateMatrixInstance, GetPropertyByName(GetModel(), "_31"), &_31, 1);
			SetDatatypeProperty(iRotateMatrixInstance, GetPropertyByName(GetModel(), "_33"), &_33, 1);

			SetObjectProperty(iRotateTransformationInstance, GetPropertyByName(GetModel(), "matrix"), &iRotateMatrixInstance, 1);
		}

		return;
	} // if (string(szClassName) == string("Cone"))

	MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), L"Unsupported type.", L"Error", MB_ICONERROR | MB_OK);
}

// ------------------------------------------------------------------------------------------------
vector<int64_t> CRDFMeasurementsBuilder::BuildLineMeasurementArrows(const double * arStartPoint, const double * arEndPoint, double dMaxDimension, double dYOffset)
{
	ASSERT(arStartPoint != nullptr);
	ASSERT(arEndPoint != nullptr);

	vector<int64_t> vecInstances;

	int64_t iLine3DClass = GetClassByName(GetModel(), "Line3D");
	ASSERT(iLine3DClass != 0);
		
	/*
	* Line
	*/
	{
		int64_t iLine3DInstance = CreateInstance(iLine3DClass);
		ASSERT(iLine3DInstance != 0);

		double arPoints[6] =
		{
			arStartPoint[0], dYOffset, arStartPoint[2],
			arEndPoint[0], dYOffset, arEndPoint[2],
		};

		SetDatatypeProperty(iLine3DInstance, GetPropertyByName(GetModel(), "points"), &arPoints, 6);

		vecInstances.push_back(iLine3DInstance);
	}

	/*
	* Arrows
	*/
	const double ARROW_LENGTH = dMaxDimension * 0.015;
	const double ARROW_HEIGHT = ARROW_LENGTH / 4.;

	/*
	* Left arrow - up
	*/
	{
		int64_t iLine3DInstance = CreateInstance(iLine3DClass);
		ASSERT(iLine3DInstance != 0);

		double arPoints[6] =
		{
			arStartPoint[0], dYOffset, arStartPoint[2],
			arStartPoint[0] + ARROW_LENGTH, dYOffset + ARROW_HEIGHT, arStartPoint[2],
		};

		SetDatatypeProperty(iLine3DInstance, GetPropertyByName(GetModel(), "points"), &arPoints, 6);

		vecInstances.push_back(iLine3DInstance);
	}

	/*
	* Left arrow - down
	*/
	{
		int64_t iLine3DInstance = CreateInstance(iLine3DClass);
		ASSERT(iLine3DInstance != 0);

		double arPoints[6] =
		{
			arStartPoint[0], dYOffset, arStartPoint[2],
			arStartPoint[0] + ARROW_LENGTH, dYOffset - ARROW_HEIGHT, arStartPoint[2],
		};

		SetDatatypeProperty(iLine3DInstance, GetPropertyByName(GetModel(), "points"), &arPoints, 6);

		vecInstances.push_back(iLine3DInstance);
	}

	/*
	* Right arrow - up
	*/
	{
		int64_t iLine3DInstance = CreateInstance(iLine3DClass);
		ASSERT(iLine3DInstance != 0);

		double arPoints[6] =
		{
			arEndPoint[0], dYOffset, arEndPoint[2],
			arEndPoint[0] - ARROW_LENGTH, dYOffset + ARROW_HEIGHT, arEndPoint[2],
		};

		SetDatatypeProperty(iLine3DInstance, GetPropertyByName(GetModel(), "points"), &arPoints, 6);

		vecInstances.push_back(iLine3DInstance);
	}

	/*
	* Right arrow - down
	*/
	{
		int64_t iLine3DInstance = CreateInstance(iLine3DClass);
		ASSERT(iLine3DInstance != 0);

		double arPoints[6] =
		{
			arEndPoint[0], dYOffset, arEndPoint[2],
			arEndPoint[0] - ARROW_LENGTH, dYOffset - ARROW_HEIGHT, arEndPoint[2],
		};

		SetDatatypeProperty(iLine3DInstance, GetPropertyByName(GetModel(), "points"), &arPoints, 6);

		vecInstances.push_back(iLine3DInstance);
	}

	/*
	* Left line
	*/
	{
		int64_t iLine3DInstance = CreateInstance(iLine3DClass);
		ASSERT(iLine3DInstance != 0);

		double arPoints[6] =
		{
			arStartPoint[0], arStartPoint[1], arStartPoint[2],
			arStartPoint[0], dYOffset + (dYOffset * .01), arStartPoint[2],
		};

		SetDatatypeProperty(iLine3DInstance, GetPropertyByName(GetModel(), "points"), &arPoints, 6);

		vecInstances.push_back(iLine3DInstance);
	}

	/*
	* Right line
	*/
	{
		int64_t iLine3DInstance = CreateInstance(iLine3DClass);
		ASSERT(iLine3DInstance != 0);

		double arPoints[6] =
		{
			arEndPoint[0], arEndPoint[1], arEndPoint[2],
			arEndPoint[0], dYOffset + (dYOffset * .01), arEndPoint[2],
		};

		SetDatatypeProperty(iLine3DInstance, GetPropertyByName(GetModel(), "points"), &arPoints, 6);

		vecInstances.push_back(iLine3DInstance);
	}

	return vecInstances;
}


