
#pragma once

#include "RDFView.h"
#include "RDFPropertyRestriction.h"
#include "RDFController.h"

#include <map>
using namespace std;

// ************************************************************************************************
class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

// ************************************************************************************************
class CApplicationPropertyData
{

private:  // Members

	enumApplicationProperty m_enPropertyType;

public: // Methods

	CApplicationPropertyData(enumApplicationProperty enApplicationProperty);

	enumApplicationProperty GetType() const { return m_enPropertyType; }
};

// ************************************************************************************************
class CLightPropertyData : public CApplicationPropertyData
{

private:  // Members

	// --------------------------------------------------------------------------------------------
	// Zero-based index
	int m_iLightIndex;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CLightPropertyData(enumApplicationProperty enApplicationProperty, int iLightIndex);

	// --------------------------------------------------------------------------------------------
	// Getter
	int GetLightIndex() const;
};

// ************************************************************************************************
class CApplicationProperty : public CMFCPropertyGridProperty
{

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CApplicationProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData);

	// --------------------------------------------------------------------------------------------
	// ctor
	CApplicationProperty(const CString & strGroupName, DWORD_PTR dwData, BOOL bIsValueList);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CApplicationProperty();
};

// ************************************************************************************************
class CColorApplicationProperty : public CMFCPropertyGridColorProperty
{

public: // Methods

	CColorApplicationProperty(const CString & strName, const COLORREF & color, CPalette * pPalette, LPCTSTR szDescription, DWORD_PTR dwData);
	virtual ~CColorApplicationProperty();
};

// ************************************************************************************************
class CRDFInstanceData
{

private:  // Members

	_rdf_controller* m_pController;
	_rdf_instance* m_pInstance;

public: // Methods

	CRDFInstanceData(_rdf_controller* pController, _rdf_instance* pInstance);
	virtual ~CRDFInstanceData();

public: // Properties

	_rdf_controller* GetController() const { return m_pController; }
	_rdf_instance* GetInstance() const { return m_pInstance; }
};

// ************************************************************************************************
class CRDFInstancePropertyData : public CRDFInstanceData
{

private:  // Members

	_rdf_property * m_pProperty;
	int64_t m_iCard;

public: // Methods

	CRDFInstancePropertyData(_rdf_controller * pController, _rdf_instance * pInstance, _rdf_property * pProperty, int64_t iCard);


public: // Properties

	_rdf_property * GetProperty() const;
	int64_t GetCard() const;
	void SetCard(int64_t iCard);
};

// ************************************************************************************************
class CRDFInstanceProperty : public CMFCPropertyGridProperty
{

public: // Methods

	CRDFInstanceProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData);
	virtual ~CRDFInstanceProperty();

	virtual BOOL HasButton() const;
	virtual void OnClickButton(CPoint point);
	virtual CString FormatProperty();
	virtual BOOL TextToVar(const CString & strText);
	virtual CWnd * CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat);

	void EnableSpinControlInt64();
};

// ************************************************************************************************
class CRDFInstanceObjectProperty : public CMFCPropertyGridProperty
{

private: // Members

	map<wstring, int64_t> m_mapValues;

public: // Methods

	CRDFInstanceObjectProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData);
	virtual ~CRDFInstanceObjectProperty();

	void AddValue(const wstring & strValue, int64_t iInstance);

	int64_t GetInstance(const wstring & strValue) const;
};

// ************************************************************************************************
class CRDFColorSelectorProperty : public CMFCPropertyGridColorProperty
{

public: // Methods

	CRDFColorSelectorProperty(const CString & strName, const COLORREF & color, CPalette * pPalette, LPCTSTR szDescription, DWORD_PTR dwData);
	virtual ~CRDFColorSelectorProperty();
};

// ************************************************************************************************
class CAddRDFInstanceProperty : public CMFCPropertyGridProperty
{

public: // Methods

	CAddRDFInstanceProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData);	
	virtual ~CAddRDFInstanceProperty();

	virtual BOOL HasButton() const;
	virtual void OnClickButton(CPoint point);

	void SetModified(BOOL bModified);
};

// ************************************************************************************************
class CPropertiesWnd
	: public CDockablePane
	, public CRDFView
{

public: // Methods

	// _view
	virtual void postModelLoaded() override;
	virtual void onInstanceSelected(_view* pSender) override;
	virtual void onApplicationPropertyChanged(_view* pSender, enumApplicationProperty enApplicationProperty) override;

	// _rdf_view
	virtual void onInstancePropertySelected(_view* pSender) override;
	virtual void onShowBaseInformation(_view* pSender, _rdf_instance* pInstance) override;
	virtual void onShowMetaInformation(_view* pSender, _rdf_instance* pInstance) override;
	virtual void onInstanceRenamed(_view* pSender, _rdf_instance* pInstance) override;
	virtual void onInstanceCreated(_view* pSender, _rdf_instance* pInstance) override;
	virtual void onInstanceDeleted(_view* pSender, _rdf_instance* pInstance) override;
	virtual void onInstancesDeleted(_view* pSender) override;

protected: // Methods
	
	// Support for properties
	afx_msg LRESULT OnPropertyChanged(__in WPARAM wparam, __in LPARAM lparam);

private: // Methods

// Construction
public:
	CPropertiesWnd();

	void AdjustLayout();

// Attributes
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

protected:
	CFont m_fntPropList;
	CComboBox m_wndObjectCombo;
	CPropertiesToolBar m_toolBar;
	CMFCPropertyGridCtrl m_wndPropList;

// Implementation
public:
	virtual ~CPropertiesWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnProperties1();
	afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	afx_msg void OnProperties2();
	afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnViewModeChanged();
	afx_msg LRESULT OnLoadInstancePropertyValues(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadInstanceProperties(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	void LoadApplicationProperties();
	void LoadInstanceProperties();
	void AddInstanceProperty(CMFCPropertyGridProperty* pInstanceGroup, _rdf_instance* pInstance, _rdf_property* pProperty);
	void AddInstancePropertyCardinality(CMFCPropertyGridProperty* pPropertyGroup, _rdf_instance* pInstance, _rdf_property* pProperty);
	void AddInstancePropertyValues(CMFCPropertyGridProperty* pPropertyGroup, _rdf_instance* pInstance, _rdf_property* pProperty);
	void LoadBaseInformation(_rdf_instance* pInstance);
	void LoadMetaInformation(_rdf_instance* pInstance);
	void SetPropListFont();

	int m_nComboHeight;
public:
	afx_msg void OnDestroy();
};

