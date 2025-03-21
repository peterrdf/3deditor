#pragma once

#include "_rdf_mvc.h"
#include "ascii.h"

#include <string>
#include <vector>
using namespace std;

// ************************************************************************************************
// TTF
#define DOUBLE_FROM_26_6(t) ((double)(t) / 64.0)

// ************************************************************************************************
class _text_builder
{

private: // Fields

	OwlModel m_owlModel;

public: // Methods

	_text_builder()
		: m_owlModel(0)
	{}

	virtual ~_text_builder()
	{}

	void Initialize(OwlModel owlModel)
	{
		m_owlModel = owlModel;
		assert(m_owlModel != 0);

		ascii::importChars(m_owlModel);
	}

	OwlInstance BuildText(const string& strText, bool bCenter)
	{
		int64_t iOffsetX = 0;

		vector<OwlInstance> vecChars;
		for (size_t iCharIndex = 0; iCharIndex < strText.size(); iCharIndex++) {
			OwlInstance owlCharInstance = ascii::getCharInstance(strText.at(iCharIndex));
			assert(owlCharInstance != 0);

			int64_t iCard = 0;
			int64_t* piValue = nullptr;
			GetDatatypeProperty(owlCharInstance, GetPropertyByName(m_owlModel, "ttf:advance:x"), (void**)&piValue, &iCard);
			assert(iCard == 1);

			if (iCharIndex > 0) {
				OwlInstance owlMatrixInstance = CreateInstance(GetClassByName(m_owlModel, "Matrix"));
				assert(owlMatrixInstance != 0);

				double d41 = DOUBLE_FROM_26_6(iOffsetX);
				SetDatatypeProperty(owlMatrixInstance, GetPropertyByName(m_owlModel, "_41"), &d41, 1);

				OwlInstance owlTransformationInstance = CreateInstance(GetClassByName(m_owlModel, "Transformation"));
				assert(owlTransformationInstance != 0);

				SetObjectProperty(owlTransformationInstance, GetPropertyByName(m_owlModel, "object"), &owlCharInstance, 1);
				SetObjectProperty(owlTransformationInstance, GetPropertyByName(m_owlModel, "matrix"), &owlMatrixInstance, 1);

				vecChars.push_back(owlTransformationInstance);
			} else {
				vecChars.push_back(owlCharInstance);
			}

			iOffsetX += piValue[0];
		} // for (size_t iCharIndex = ...

		OwlInstance owlCollectionInstance = CreateInstance(GetClassByName(m_owlModel, "Collection"));
		assert(owlCollectionInstance != 0);

		SetObjectProperty(
			owlCollectionInstance,
			GetPropertyByName(m_owlModel, "objects"),
			vecChars.data(),
			vecChars.size());

		if (bCenter) {
			double arAABBMin[] = { 0., 0., 0. };
			double arAABBMax[] = { 0., 0., 0. };
			GetBoundingBox(
				owlCollectionInstance,
				(double*)&arAABBMin,
				(double*)&arAABBMax);

			return _rdf_model::translateTransformation(
				m_owlModel,
				owlCollectionInstance,
				-(arAABBMin[0] + arAABBMax[0]) / 2., -(arAABBMin[1] + arAABBMax[1]) / 2., -(arAABBMin[2] + arAABBMax[2]) / 2.);
		}

		return owlCollectionInstance;
	}
};