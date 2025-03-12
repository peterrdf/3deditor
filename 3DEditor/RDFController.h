#pragma once

#include "_rdf_mvc.h"

#include <set>
#include <vector>
using namespace std;

// ------------------------------------------------------------------------------------------------
class CRDFModel;
class CSceneRDFModel;
class CNavigatorRDFModel;
class CRDFView;
enum class enumApplicationProperty;
class CRDFInstance;
class CRDFProperty;

// ------------------------------------------------------------------------------------------------
// Controller - MVC
class CRDFController : public _rdf_controller
{

private: // Members

	CSceneRDFModel* m_pSceneModel;
	CNavigatorRDFModel* m_pNavigatorModel;	

	// --------------------------------------------------------------------------------------------
	// Updating model - disable all notifications
	bool m_bUpdatingModel;

	// --------------------------------------------------------------------------------------------
	// Selected instance
	CRDFInstance * m_pSelectedInstance;

	// --------------------------------------------------------------------------------------------
	// Selected instance property
	pair<CRDFInstance *, CRDFProperty *> m_prSelectedInstanceProperty;

	// --------------------------------------------------------------------------------------------
	// UI properties
	//int m_iVisibleValuesCountLimit;

	// --------------------------------------------------------------------------------------------
	// UI properties
	//BOOL m_bScaleAndCenter;
	//BOOL m_bModelCoordinateSystem;

public: // Methods

	CRDFController();
	virtual ~CRDFController();

	// --------------------------------------------------------------------------------------------
	// Getter
	CSceneRDFModel* GetSceneModel() const { return m_pSceneModel; }
	CNavigatorRDFModel* GetNavigatorModel() const { return m_pNavigatorModel; }	

	// --------------------------------------------------------------------------------------------
	// Setter
	void SetModel(CRDFModel * pModel);
	CRDFModel* GetModel();

	// --------------------------------------------------------------------------------------------
	// Test mode
	virtual void LoadModel(LPCTSTR szFileName);

	// --------------------------------------------------------------------------------------------
	// Zoom to an instance
	void ZoomToInstance(int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// Zoom to all enabled instances
	void ZoomOut();

	// --------------------------------------------------------------------------------------------
	// [-1, 1]
	void ScaleAndCenter();

	// --------------------------------------------------------------------------------------------
	// Save
	void Save(CRDFInstance* pInstance);

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
	//int GetVisibleValuesCountLimit() const;

	// --------------------------------------------------------------------------------------------
	// UI properties
	///void SetVisibleValuesCountLimit(int iVisibleValuesCountLimit);

	// --------------------------------------------------------------------------------------------
	// UI properties
	//BOOL GetScaleAndCenter() const;

	// --------------------------------------------------------------------------------------------
	// UI properties
	//void SetScaleAndCenter(BOOL bScaleAndCenter);

	// --------------------------------------------------------------------------------------------
	// UI properties
	//BOOL GetModelCoordinateSystem() const;

	// --------------------------------------------------------------------------------------------
	// UI properties
	//void SetModelCoordinateSystem(BOOL bValue);

	// --------------------------------------------------------------------------------------------
	// UI properties
	//void UpdateCoordinateSystem();

	// --------------------------------------------------------------------------------------------
	// Edit properties support
	void OnInstancePropertyEdited(CRDFInstance* pInstance, CRDFProperty* pProperty);

	// --------------------------------------------------------------------------------------------
	// Edit properties support
	//void OnApplicationPropertyChanged(CRDFView* pSender, enumApplicationProperty enApplicationProperty);

	// --------------------------------------------------------------------------------------------
	// Factory
	CRDFInstance* CreateNewInstance(CRDFView* pSender, int64_t iClassInstance);
	void RenameInstance(CRDFView* pSender, CRDFInstance* pInstance, LPCTSTR szName);

	// --------------------------------------------------------------------------------------------
	// Removes an instance
	bool DeleteInstance(CRDFView * pSender, CRDFInstance * pInstance);

	// --------------------------------------------------------------------------------------------
	// Removes an instance tree
	bool DeleteInstanceTree(CRDFView * pSender, CRDFInstance * pInstance);
	bool DeleteInstanceTreeRecursive(CRDFView* pSender, CRDFInstance* pInstance);

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

	// Tests
	void BeginTestMode();
	void EndTestMode();
	BOOL IsTestMode();
};




