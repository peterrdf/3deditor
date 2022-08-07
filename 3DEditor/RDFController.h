#pragma once

#include <set>
#include <vector>

using namespace std;

// ------------------------------------------------------------------------------------------------
class CRDFModel;
class CRDFView;
class CRDFInstance;
class CRDFProperty;

// ------------------------------------------------------------------------------------------------
// Controller - MVC
class CRDFController
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// Model - MVC
	CRDFModel * m_pModel;

	// --------------------------------------------------------------------------------------------
	// Updating model - disable all notifications
	bool m_bUpdatingModel;

	// --------------------------------------------------------------------------------------------
	// Views - MVC
	set<CRDFView *> m_setViews;

	// --------------------------------------------------------------------------------------------
	// Selected instance
	CRDFInstance * m_pSelectedInstance;

	// --------------------------------------------------------------------------------------------
	// Selected instance property
	pair<CRDFInstance *, CRDFProperty *> m_prSelectedInstanceProperty;

	// --------------------------------------------------------------------------------------------
	// UI properties
	int m_iVisibleValuesCountLimit;

	// --------------------------------------------------------------------------------------------
	// UI properties
	BOOL m_bScaleAndCenter;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFController();

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CRDFController();

	// --------------------------------------------------------------------------------------------
	// Getter
	CRDFModel * GetModel() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void SetModel(CRDFModel * pModel);

	// --------------------------------------------------------------------------------------------
	// Support for notifications
	void RegisterView(CRDFView * pView);

	// --------------------------------------------------------------------------------------------
	// Support for notifications
	void UnRegisterView(CRDFView * pView);

	// --------------------------------------------------------------------------------------------
	// Getter
	const set<CRDFView *> & GetViews();

	// --------------------------------------------------------------------------------------------
	// Getter
	template <class T>
	T * GetView()
	{
		set<CRDFView *>::const_iterator itView = m_setViews.begin();
		for (; itView != m_setViews.end(); itView++)
		{
			T * pView = dynamic_cast<T *>(*itView);
			if (pView != NULL)
			{
				return pView;
			}
		}

		return NULL;
	}

	// --------------------------------------------------------------------------------------------
	// Test mode
	virtual void LoadModel(LPCTSTR szFileName);

	// --------------------------------------------------------------------------------------------
	// Zoom to an instance
	void ZoomToInstance(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// [-1, 1]
	void ScaleAndCenter();

	// --------------------------------------------------------------------------------------------
	// Meta information support
	void ShowMetaInformation(CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// Selection support
	void SelectInstance(CRDFView * pSender, CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// Selection support
	CRDFInstance * GetSelectedInstance() const;

	// --------------------------------------------------------------------------------------------
	// Selection support
	void SelectInstanceProperty(CRDFInstance * pInstance, CRDFProperty * pProperty);

	// --------------------------------------------------------------------------------------------
	// Selection support
	pair<CRDFInstance *, CRDFProperty *> GetSelectedInstanceProperty() const;

	// --------------------------------------------------------------------------------------------
	// UI properties
	int GetVisibleValuesCountLimit() const;

	// --------------------------------------------------------------------------------------------
	// UI properties
	void SetVisibleValuesCountLimit(int iVisibleValuesCountLimit);

	// --------------------------------------------------------------------------------------------
	// UI properties
	BOOL GetScaleAndCenter() const;

	// --------------------------------------------------------------------------------------------
	// UI properties
	void SetScaleAndCenter(BOOL bScaleAndCenter);

	// --------------------------------------------------------------------------------------------
	// Edit properties support
	void OnInstancePropertyEdited(CRDFInstance * pInstance, CRDFProperty * pProperty);

	// --------------------------------------------------------------------------------------------
	// Factory
	CRDFInstance* CreateNewInstance(CRDFView * pSender, int64_t iClassInstance);

	// --------------------------------------------------------------------------------------------
	// Factory
	CRDFInstance* OnOctreeInstanceCreated(CRDFView* pSender, GEOM::Instance pThing);

	// --------------------------------------------------------------------------------------------
	// Removes an instance
	bool DeleteInstance(CRDFView * pSender, CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// Removes an instance tree
	bool DeleteInstanceTree(CRDFView * pSender, CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// Removes instances
	bool DeleteInstances(CRDFView * pSender, vector<CRDFInstance *> vecInstances);

	// --------------------------------------------------------------------------------------------
	// Adds measurements/annotations
	void AddMeasurements(CRDFView * pSender, CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// Import
	void ImportModel(CRDFView* pSender, const wchar_t* szPath);

	// --------------------------------------------------------------------------------------------
	// Enable/Disable support
	void OnInstancesEnabledStateChanged();

	// --------------------------------------------------------------------------------------------
	// Tests
	void BeginTestMode();

	// --------------------------------------------------------------------------------------------
	// Tests
	void EndTestMode();

	// --------------------------------------------------------------------------------------------
	// Tests
	BOOL IsTestMode();
};




