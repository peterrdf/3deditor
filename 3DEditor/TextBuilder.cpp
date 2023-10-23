#include "stdafx.h"
#include "TextBuilder.h"

#include "ascii.h"

// TTF
#define DOUBLE_FROM_26_6(t) ((double)(t) / 64.0)

CTextBuilder::CTextBuilder()
	: m_iModel(0)
{}

/*virtual*/ CTextBuilder::~CTextBuilder()
{}

void CTextBuilder::Initialize(OwlModel iModel)
{
	m_iModel = iModel;
	ASSERT(m_iModel != 0);

	ascii::init(m_iModel);
}

OwlInstance CTextBuilder::BuildText(const string& strText, bool bCenter)
{
	int64_t iOffsetX = 0;

	vector<OwlInstance> vecChars;
	for (size_t iCharIndex = 0; iCharIndex < strText.size(); iCharIndex++)
	{
		OwlInstance iCharInstance = ascii::getCharInstance(strText.at(iCharIndex));
		ASSERT(iCharInstance != 0);

		int64_t iCard = 0;
		int64_t* piValue = nullptr;
		GetDatatypeProperty(iCharInstance, GetPropertyByName(m_iModel, "ttf:advance:x"), (void**)&piValue, &iCard);
		ASSERT(iCard == 1);

		if (iCharIndex > 0)
		{
			OwlInstance iMatrixInstance = CreateInstance(GetClassByName(m_iModel, "Matrix"));
			ASSERT(iMatrixInstance != 0);

			double d41 = DOUBLE_FROM_26_6(iOffsetX);
			SetDatatypeProperty(iMatrixInstance, GetPropertyByName(m_iModel, "_41"), &d41, 1);

			OwlInstance iTransformationInstance = CreateInstance(GetClassByName(m_iModel, "Transformation"));
			ASSERT(iTransformationInstance != 0);

			SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "object"), &iCharInstance, 1);
			SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "matrix"), &iMatrixInstance, 1);

			vecChars.push_back(iTransformationInstance);
		}
		else
		{
			vecChars.push_back(iCharInstance);
		}

		iOffsetX += piValue[0];
	} // for (size_t iCharIndex = ...

	OwlInstance iCollectionInstance = CreateInstance(GetClassByName(m_iModel, "Collection"));
	ASSERT(iCollectionInstance != 0);

	SetObjectProperty(
		iCollectionInstance, 
		GetPropertyByName(m_iModel, "objects"), 
		vecChars.data(), 
		vecChars.size());

	if (bCenter)
	{
		double arAABBMin[] = { 0., 0., 0. };
		double arAABBMax[] = { 0., 0., 0. };
		GetBoundingBox(
			iCollectionInstance,
			(double*)&arAABBMin,
			(double*)&arAABBMax);

		return Translate(
			iCollectionInstance,
			-(arAABBMin[0] + arAABBMax[0]) / 2., -(arAABBMin[1] + arAABBMax[1]) / 2., -(arAABBMin[2] + arAABBMax[2]) / 2.);
	}

	return iCollectionInstance;
}

OwlInstance CTextBuilder::Translate(
	OwlInstance iInstance,
	double dX, double dY, double dZ)
{
	ASSERT(iInstance != 0);

	int64_t iMatrixInstance = CreateInstance(GetClassByName(m_iModel, "Matrix"));
	ASSERT(iMatrixInstance != 0);

	vector<double> vecTransformationMatrix =
	{
		1., 0., 0.,
		0., 1., 0.,
		0., 0., 1.,
		dX, dY, dZ,
	};

	SetDatatypeProperty(
		iMatrixInstance,
		GetPropertyByName(m_iModel, "coordinates"),
		vecTransformationMatrix.data(),
		vecTransformationMatrix.size());

	int64_t iTransformationInstance = CreateInstance(GetClassByName(m_iModel, "Transformation"), "Translate");
	ASSERT(iTransformationInstance != 0);

	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "matrix"), &iMatrixInstance, 1);
	SetObjectProperty(iTransformationInstance, GetPropertyByName(m_iModel, "object"), &iInstance, 1);

	return iTransformationInstance;
}