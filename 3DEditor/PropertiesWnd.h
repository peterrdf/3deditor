
#pragma once

#include "RDFView.h"
#include "RDFPropertyRestriction.h"
#include "RDFController.h"

#include <map>
using namespace std;

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

// ------------------------------------------------------------------------------------------------
class CApplicationPropertyData
{

private:  // Members

	// --------------------------------------------------------------------------------------------
	// Type
	enumApplicationProperty m_enPropertyType;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CApplicationPropertyData(enumApplicationProperty enApplicationProperty);

	// --------------------------------------------------------------------------------------------
	// Getter
	enumApplicationProperty GetType() const;
};

// ------------------------------------------------------------------------------------------------
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

// ------------------------------------------------------------------------------------------------
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

// ------------------------------------------------------------------------------------------------
class CColorApplicationProperty : public CMFCPropertyGridColorProperty
{

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CColorApplicationProperty(const CString & strName, const COLORREF & color, CPalette * pPalette, LPCTSTR szDescription, DWORD_PTR dwData);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CColorApplicationProperty();
};

// ------------------------------------------------------------------------------------------------
class CRDFInstanceData
{

private:  // Members

	// --------------------------------------------------------------------------------------------
	// Controller
	_rdf_controller * m_pController;

	// --------------------------------------------------------------------------------------------
	// Instance
	_rdf_instance * m_pInstance;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFInstanceData(_rdf_controller * pController, _rdf_instance * pInstance);

	// --------------------------------------------------------------------------------------------
	// Getter
	_rdf_controller * GetController() const;

	// --------------------------------------------------------------------------------------------
	// Getter
	_rdf_instance * GetInstance() const;
};

// ------------------------------------------------------------------------------------------------
class CRDFInstancePropertyData : public CRDFInstanceData
{

private:  // Members

	// --------------------------------------------------------------------------------------------
	// Property
	CRDFProperty * m_pProperty;

	// --------------------------------------------------------------------------------------------
	// Card
	int64_t m_iCard;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFInstancePropertyData(_rdf_controller * pController, _rdf_instance * pInstance, CRDFProperty * pProperty, int64_t iCard);

	// --------------------------------------------------------------------------------------------
	// Getter
	CRDFProperty * GetProperty() const;

	// --------------------------------------------------------------------------------------------
	// Getter
	int64_t GetCard() const;

	// --------------------------------------------------------------------------------------------
	// Setter
	void SetCard(int64_t iCard);
};

// ------------------------------------------------------------------------------------------------
class CRDFInstanceProperty : public CMFCPropertyGridProperty
{

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFInstanceProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CRDFInstanceProperty();

	// --------------------------------------------------------------------------------------------
	// Overridden
	virtual BOOL HasButton() const;

	// --------------------------------------------------------------------------------------------
	// Overridden
	virtual void OnClickButton(CPoint point);

	// --------------------------------------------------------------------------------------------
	// Overridden
	virtual CString FormatProperty();

	// --------------------------------------------------------------------------------------------
	// Overridden
	virtual BOOL TextToVar(const CString & strText);

	// --------------------------------------------------------------------------------------------
	// Overridden
	virtual CWnd * CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat);

	// --------------------------------------------------------------------------------------------
	// Support for int64_t
	void EnableSpinControlInt64();
};

// ------------------------------------------------------------------------------------------------
class CRDFInstanceObjectProperty : public CMFCPropertyGridProperty
{

private: // Members

	// --------------------------------------------------------------------------------------------
	// VALUE : INSTANCE
	map<wstring, int64_t> m_mapValues;

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFInstanceObjectProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CRDFInstanceObjectProperty();

	// --------------------------------------------------------------------------------------------
	// Adds a pair - VALUE : INSTANCE
	void AddValue(const wstring & strValue, int64_t iInstance);

	// --------------------------------------------------------------------------------------------
	// Gets an instance by value
	int64_t GetInstance(const wstring & strValue) const;
};

// ------------------------------------------------------------------------------------------------
class CRDFColorSelectorProperty : public CMFCPropertyGridColorProperty
{

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CRDFColorSelectorProperty(const CString & strName, const COLORREF & color, CPalette * pPalette, LPCTSTR szDescription, DWORD_PTR dwData);

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CRDFColorSelectorProperty();
};

// ------------------------------------------------------------------------------------------------
class CAddRDFInstanceProperty : public CMFCPropertyGridProperty
{

public: // Methods

	// --------------------------------------------------------------------------------------------
	// ctor
	CAddRDFInstanceProperty(const CString & strName, const COleVariant & vtValue, LPCTSTR szDescription, DWORD_PTR dwData);	

	// --------------------------------------------------------------------------------------------
	// dtor
	virtual ~CAddRDFInstanceProperty();

	// --------------------------------------------------------------------------------------------
	// Access to a protected member function
	void SetModified(BOOL bModified);

	// --------------------------------------------------------------------------------------------
	// Overridden
	virtual BOOL HasButton() const;

	// --------------------------------------------------------------------------------------------
	// Overridden
	virtual void OnClickButton(CPoint point);
};


class CPropertiesWnd
	: public CDockablePane
	, public CRDFView
{

public: // Methods

	// _view
	virtual void postModelLoaded() override;
	virtual void onApplicationPropertyChanged(_view* pSender, enumApplicationProperty enApplicationProperty) override;

	// _rdf_view
	virtual void onShowBaseInformation(_view* pSender, _rdf_instance* pInstance) override;
	virtual void onShowMetaInformation(_view* pSender, _rdf_instance* pInstance) override;
	
	// CRDFView
	//#todo
	
	virtual void OnInstanceSelected(CRDFView* pSender);
	virtual void OnInstancePropertySelected();
	virtual void OnInstanceNameEdited(CRDFView* pSender, _rdf_instance* pInstance);
	virtual void OnNewInstanceCreated(CRDFView* pSender, _rdf_instance * pInstance);
	virtual void OnInstanceDeleted(CRDFView* pSender, int64_t iInstance);
	//virtual void OnApplicationPropertyChanged(CRDFView* pSender, enumApplicationProperty enApplicationProperty);

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
	void AddInstanceProperty(CMFCPropertyGridProperty* pInstanceGroup, _rdf_instance* pInstance, CRDFProperty* pProperty);
	void AddInstancePropertyCardinality(CMFCPropertyGridProperty* pPropertyGroup, _rdf_instance* pInstance, CRDFProperty* pProperty);
	void AddInstancePropertyValues(CMFCPropertyGridProperty* pPropertyGroup, _rdf_instance* pInstance, CRDFProperty* pProperty);
	void LoadBaseInformation(_rdf_instance* pInstance);
	void LoadMetaInformation(_rdf_instance* pInstance);
	void SetPropListFont();

	int m_nComboHeight;
public:
	afx_msg void OnDestroy();
};

