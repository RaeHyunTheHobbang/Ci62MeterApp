// Configure.cpp : implementation file
//

#include "stdafx.h"
#include "main.h"
#include "Configure.h"


// CConfigure dialog

IMPLEMENT_DYNAMIC(CConfigure, CDialogEx)

CConfigure::CConfigure(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConfigure::IDD, pParent)
{

}

CConfigure::~CConfigure()
{
}

void CConfigure::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_OPTIONS1, m_Options1);
    DDX_Control(pDX, IDC_OPTIONS2, m_Options2);
    DDX_Control(pDX, IDC_OPTIONS3, m_Options3);
    DDX_Control(pDX, IDC_OPTIONS4, m_Options4);
    DDX_Control(pDX, IDC_OPTIONS5, m_Options5);
    DDX_Control(pDX, IDC_OPTIONS6, m_Options6);
    DDX_Control(pDX, IDC_OPTIONS7, m_Options7);
    DDX_Control(pDX, IDC_SETTING1, m_Setting1);
    DDX_Control(pDX, IDC_SETTING2, m_Setting2);
    DDX_Control(pDX, IDC_SETTING3, m_Setting3);
    DDX_Control(pDX, IDC_SETTING4, m_Setting4);
    DDX_Control(pDX, IDC_SETTING5, m_Setting5);
    DDX_Control(pDX, IDC_SETTING6, m_Setting6);
    DDX_Control(pDX, IDC_SETTING7, m_Setting7);
}


BEGIN_MESSAGE_MAP(CConfigure, CDialogEx)
    ON_BN_CLICKED(IDOK, &CConfigure::OnBnClickedOk)
END_MESSAGE_MAP()


// CConfigure message handlers

BOOL CConfigure::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CStatic* settingCtrls[] =
    {
        &m_Setting1,
        &m_Setting2,
        &m_Setting3,
        &m_Setting4,
        &m_Setting5,
        &m_Setting6,
        &m_Setting7,
    };
    CComboBox* optionsCtrls[] =
    {
        &m_Options1,
        &m_Options2,
        &m_Options3,
        &m_Options4,
        &m_Options5,
        &m_Options6,
        &m_Options7,
    };
    size_t controlCount = sizeof(settingCtrls) / sizeof(settingCtrls[0]);

    CStringA setting, options, currentOption;
    CStringA str = m_spectro->GetAvailableSettings();
    vector<CStringA> settingList = split( str, ";" );

    for( size_t i = 0; i < settingList.size(); ++i )
    {
        setting = settingList[i];

        str = m_spectro->GetSettingOptions( setting );
        vector<CStringA> optionList = split( str, ";" );
        for( size_t j = 0; j < optionList.size(); ++j )
        {
            optionsCtrls[i]->AddString( CString(optionList[j]) );
        }
        currentOption = m_spectro->GetOption( setting );
        if( optionsCtrls[i]->SelectString( -1, CStringW(currentOption) ) < 0 )
            optionsCtrls[i]->SetCurSel(0);

        if( optionsCtrls[i]->GetCount() < 2 )
            optionsCtrls[i]->EnableWindow( FALSE );

        setting.Replace( "_", " " );
        settingCtrls[i]->SetWindowTextW( CString(setting) );

    }

    for( size_t i = settingList.size(); i < controlCount; ++i )
    {
        settingCtrls[i]->ShowWindow( SW_HIDE );
        optionsCtrls[i]->ShowWindow( SW_HIDE );
    }

    return TRUE;
}

void CConfigure::OnBnClickedOk()
{
    CStatic* settingCtrls[] =
    {
        &m_Setting1,
        &m_Setting2,
        &m_Setting3,
        &m_Setting4,
        &m_Setting5,
        &m_Setting6,
        &m_Setting7,
    };
    CComboBox* optionsCtrls[] =
    {
        &m_Options1,
        &m_Options2,
        &m_Options3,
        &m_Options4,
        &m_Options5,
        &m_Options6,
        &m_Options7,
    };

    CString text;
    CStringA setting, option;
    CStringA str = m_spectro->GetAvailableSettings();
    vector<CStringA> settingList = split( str, ";" );

    for( size_t i = 0; i < settingList.size(); ++i )
    {
        if( settingCtrls[i]->IsWindowVisible() )
        {
            settingCtrls[i]->GetWindowText( text );
            setting = CStringA( text );
            setting.Replace( " ", "_" );

            optionsCtrls[i]->GetWindowTextW( text );
            option = CStringA( text );

            if( ! m_spectro->SetOption( setting, option ) )
            {
                MessageBox( CStringW(m_spectro->GetLastErrorString()) );
            }
        }
    }

    CDialogEx::OnOK();
}


