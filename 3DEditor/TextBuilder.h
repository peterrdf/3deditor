#pragma once

#include "engine.h"

#include <string>
#include <vector>
using namespace std;

// ************************************************************************************************
class CTextBuilder
{

private: // Fields

	OwlModel m_iModel;

public: // Methods

	CTextBuilder();
	virtual ~CTextBuilder();

	void Initialize(OwlModel iModel);

	OwlInstance BuildText(const string& strText, bool bCenter);

private: // Methods

	OwlInstance Translate(
		OwlInstance iInstance,
		double dX, double dY, double dZ);
};

