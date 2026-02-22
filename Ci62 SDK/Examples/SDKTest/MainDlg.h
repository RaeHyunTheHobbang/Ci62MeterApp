// MainDlg.h : header file
//
#pragma once

#include "Spectro.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CMainDlg dialog

class CMainDlg : public CDialog
{
// Construction
public:
	CMainDlg(CWnd* pParent = NULL);	// standard constructor
    ~CMainDlg();

    Spectro spectro;
    CString interfaceVersion;
    bool isPreviewSupported;
    bool isScanSupported;
    UINT_PTR m_iTimer;
    CStringA m_step;

// Dialog Data
	enum { IDD = IDD_MAIN_DIALOG };
	CListCtrl m_List;
    CListCtrl m_Status;
    CComboBox m_Command;
    CComboBox m_Spectro;

	// ClassWizard generated virtual function overrides
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
public:
	void EnablePolling();
	void DisablePolling();
	void print( LPCTSTR str );
	void GetReflData();
    void GetScanData();
	void PrintError();
    void UpdateStatus();
    void DisplayPreview();
    CString AutoCalibration(vector<CStringA>& stepList);
    CString DeviceDisplayCalibration(vector<CStringA>& stepList);
    CString SynchronousCalibration(vector<CStringA>& stepList);
    CString AsynchronousCalibration(vector<CStringA>& stepList);

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	// Generated message map functions
   	DECLARE_MESSAGE_MAP()
    CButton m_Scan;
    CStatic m_Version;
    CButton m_chkPreview;
    CStatic m_Picture;
    virtual void OnCancel();
    afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnBnClickedMeasure();
	afx_msg void OnBnClickedCalibrate();
    afx_msg void OnBnClickedConfigure();
    afx_msg void OnBnClickedDownload();
    afx_msg void OnSelchangeSpectro();
    afx_msg void OnBnClickedRetrieve();
    afx_msg void OnBnClickedCopy();
    afx_msg void OnBnClickedScan();
    afx_msg void OnBnClickedPreview();
};

