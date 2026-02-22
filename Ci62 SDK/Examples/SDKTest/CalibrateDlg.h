#pragma once

#include "Spectro.h"

// CalibrateDlg dialog

class CalibrateDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CalibrateDlg)

public:
	CalibrateDlg(CWnd* pParent, Spectro* spectro, vector<CStringA>& stepList);
	virtual ~CalibrateDlg();

    Spectro* m_spectro;
    vector<CStringA> m_stepList;
    int m_currentStep;
    bool m_bAbort;
    bool m_bCalStarted;

// Dialog Data
	enum { IDD = IDD_CALIBRATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual void OnCancel();
    CStatic m_Msg;
};
