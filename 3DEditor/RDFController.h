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
class _rdf_property;

// ***********************************************************************************************
class CRDFController : public _rdf_controller
{

private: // Members

	CSceneRDFModel* m_pSceneModel;
	CNavigatorRDFModel* m_pNavigatorModel;	

	// --------------------------------------------------------------------------------------------
	// Updating model - disable all notifications
	bool m_bUpdatingModel;

public: // Methods

	CRDFController();
	virtual ~CRDFController();

	// --------------------------------------------------------------------------------------------
	// Getter
	CSceneRDFModel* GetSceneModel() const { return m_pSceneModel; }
	CNavigatorRDFModel* GetNavigatorModel() const { return m_pNavigatorModel; }	

	CRDFModel* GetModel();

	// --------------------------------------------------------------------------------------------
	// Test mode
	virtual void LoadModel(LPCTSTR szFileName);

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
	// Import
	//void ImportModel(CRDFView* pSender, const wchar_t* szPath);

	// --------------------------------------------------------------------------------------------
	// Enable/Disable support
	void OnInstancesEnabledStateChanged();

	// Tests
	void BeginTestMode();
	void EndTestMode();
	BOOL IsTestMode();
};




