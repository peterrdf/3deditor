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

	//CSceneRDFModel* m_pSceneModel;
	//CNavigatorRDFModel* m_pNavigatorModel;	

	// --------------------------------------------------------------------------------------------
	// Updating model - disable all notifications
	//bool m_bUpdatingModel;

public: // Methods

	CRDFController();
	virtual ~CRDFController();

	// --------------------------------------------------------------------------------------------
	// Getter
	//CSceneRDFModel* GetSceneModel() const { return m_pSceneModel; }
	//CNavigatorRDFModel* GetNavigatorModel() const { return m_pNavigatorModel; }	
	
// REMOVE!!!	
	CRDFModel* GetModel();

// MOVE !!!
	bool DeleteInstance(CRDFView * pSender, CRDFInstance * pInstance);
	bool DeleteInstanceTree(CRDFView * pSender, CRDFInstance * pInstance);
	bool DeleteInstanceTreeRecursive(CRDFView* pSender, CRDFInstance* pInstance);
	bool DeleteInstances(CRDFView * pSender, vector<CRDFInstance *> vecInstances);
	//void ImportModel(CRDFView* pSender, const wchar_t* szPath);

// MOVE!!!
	void OnInstancesEnabledStateChanged();

	// Tests
	virtual void LoadModel(LPCTSTR szFileName);
	void BeginTestMode();
	void EndTestMode();
	BOOL IsTestMode();
};




