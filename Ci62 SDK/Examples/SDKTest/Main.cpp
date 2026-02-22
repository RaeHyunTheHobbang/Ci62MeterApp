// Main.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Main.h"
#include "MainDlg.h"
#include "Spectro.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainApp

BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainApp construction

CMainApp::CMainApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMainApp object

CMainApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainApp initialization


BOOL CMainApp::InitInstance()
{
    // set executable location as working directory
	CString sDir = m_pszHelpFilePath;
	sDir = sDir.Left( sDir.ReverseFind( '\\' ) );
    SetCurrentDirectory( sDir );
    
    // start main dialog
	CMainDlg dlg;
	dlg.DoModal();

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Helper functions

vector<CStringA> split( CStringA& theString, LPCSTR theDelimiter )
{
    vector<CStringA> result;

    int pos = 0;
    CStringA item = theString.Tokenize( theDelimiter, pos );
    while( !item.IsEmpty() )
    {
        result.push_back( item );
        item = theString.Tokenize( theDelimiter, pos );
    }

    return result;
}


vector<CStringW> split( CStringW& theString, LPCWSTR theDelimiter )
{
    vector<CStringW> result;

    int pos = 0;
    CStringW item = theString.Tokenize( theDelimiter, pos );
    while( !item.IsEmpty() )
    {
        result.push_back( item );
        item = theString.Tokenize( theDelimiter, pos );
    }

    return result;
}
