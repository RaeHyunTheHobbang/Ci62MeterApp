// Main.h : main header file for the MAIN application
//

#pragma once

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMainApp:
// See Main.cpp for the implementation of this class
//

class CMainApp : public CWinApp
{
public:
	CMainApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// Helper Functions

vector<CStringA> split( CStringA& theString, LPCSTR theDelimiter );
vector<CStringW> split( CStringW& theString, LPCWSTR theDelimiter );

