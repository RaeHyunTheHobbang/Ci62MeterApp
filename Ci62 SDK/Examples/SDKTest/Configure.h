#pragma once

#include "Spectro.h"

// CConfigure dialog

class CConfigure : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigure)

public:
	CConfigure(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigure();

// Dialog Data
	enum { IDD = IDD_CONFIGURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    Spectro* m_spectro;
    afx_msg void OnBnClickedOk();
    CComboBox m_Options1;
    CComboBox m_Options2;
    CComboBox m_Options3;
    CComboBox m_Options4;
    CComboBox m_Options5;
    CComboBox m_Options6;
    CComboBox m_Options7;
    CStatic m_Setting1;
    CStatic m_Setting2;
    CStatic m_Setting3;
    CStatic m_Setting4;
    CStatic m_Setting5;
    CStatic m_Setting6;
    CStatic m_Setting7;
    virtual BOOL OnInitDialog();
};
