// MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Main.h"
#include "MainDlg.h"
#include "Spectro.h"
#include "Configure.h"
#include "CalibrateDlg.h"
#include <vector>

using namespace std;

UINT controls[]=
{
    IDC_COMMAND,
    IDC_CONNECT,
    IDC_DISCONNECT,
    IDC_CONFIGURE,
    IDC_MEASURE,
    IDC_CALIBRATE,
    IDC_RETRIEVE,
};

/////////////////////////////////////////////////////////////////////////////
// CMainDlg dialog

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
{
    m_iTimer = 0;
}

CMainDlg::~CMainDlg()
{
    if( spectro.IsLoaded() )
    {
        spectro.Disconnect();
        spectro.FreeDLL();
    }
}


void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_List);
    DDX_Control(pDX, IDC_STATUS, m_Status);
    DDX_Control(pDX, IDC_COMMAND, m_Command);
    DDX_Control(pDX, IDC_SPECTRO, m_Spectro);
    DDX_Control(pDX, IDC_VERSION, m_Version);
    DDX_Control(pDX, IDC_SCAN, m_Scan);
    DDX_Control(pDX, IDC_PREVIEW, m_chkPreview);
    DDX_Control(pDX, IDC_PICTURE, m_Picture);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, OnDisconnect)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_MEASURE, &CMainDlg::OnBnClickedMeasure)
	ON_BN_CLICKED(IDC_CALIBRATE, &CMainDlg::OnBnClickedCalibrate)
    ON_BN_CLICKED(IDC_CONFIGURE, &CMainDlg::OnBnClickedConfigure)
    ON_CBN_SELCHANGE(IDC_SPECTRO, &CMainDlg::OnSelchangeSpectro)
    ON_BN_CLICKED(IDC_RETRIEVE, &CMainDlg::OnBnClickedRetrieve)
    ON_BN_CLICKED(IDC_COPY, &CMainDlg::OnBnClickedCopy)
    ON_BN_CLICKED(IDC_SCAN, &CMainDlg::OnBnClickedScan)
    ON_BN_CLICKED(IDC_PREVIEW, &CMainDlg::OnBnClickedPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainDlg message handlers

BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

    isPreviewSupported = false;
    m_chkPreview.SetCheck(0);
    m_chkPreview.ShowWindow( SW_HIDE );
    m_Scan.SetCheck(0);
    m_Scan.ShowWindow( SW_HIDE );

    LOGFONT logfont;
    CFont* dlgFont = GetFont();
    dlgFont->GetLogFont( &logfont );

    wcscpy_s( logfont.lfFaceName, L"Lucida Console" );
    logfont.lfHeight = -12;
    logfont.lfPitchAndFamily = MONO_FONT;
    logfont.lfQuality = CLEARTYPE_QUALITY;
    CFont* lstFont = new CFont();
    lstFont->CreateFontIndirectW( &logfont );
    m_List.SetFont( lstFont );
    
    m_List.InsertColumn( 0, _T("") );
	m_Status.InsertColumn( 0, _T("") );

    // load combobox with available instruments
    CString available;
    DWORD n = GetPrivateProfileString( _T("Drivers"), _T("Available"), _T(""), available.GetBuffer(1000), 1000, _T(".\\spectro.ini") );
    available.ReleaseBuffer();

    vector<CString> driverList = split( available, _T(";") );
    for( size_t i = 0; i < driverList.size(); ++i )
    {
        m_Spectro.AddString( driverList[i] );
    }
    m_Spectro.SetCurSel( 0 );

    OnSelchangeSpectro();
    OnBnClickedPreview();

    return TRUE;
}

CString GetFileVersion( LPCTSTR pFile )
{
	UINT uiLen;
	CString sVersion;
	BYTE RawData[1000];
	VS_FIXEDFILEINFO* pInfo = NULL;

	GetFileVersionInfo( (LPTSTR)pFile, NULL, sizeof( RawData ), RawData );
	VerQueryValue( RawData, _T("\\"), (void**)&pInfo, &uiLen );
	if( uiLen > 0 && pInfo )
	{
        sVersion.Format( _T("v%d.%d.%d"), HIWORD( pInfo->dwFileVersionMS ), LOWORD( pInfo->dwFileVersionMS ), HIWORD( pInfo->dwFileVersionLS ) );
	}
	return sVersion;
}

void CMainDlg::OnSelchangeSpectro()
{
    CString name, dllPath, msg;

    if( spectro.IsLoaded() && spectro.IsConnected() )
    {
        OnDisconnect();
    }

    m_Scan.SetCheck(0);
    m_Scan.ShowWindow( SW_HIDE );
    m_chkPreview.SetCheck(0);
    m_chkPreview.ShowWindow( SW_HIDE );

    for( int i = 0; i < sizeof(controls) / sizeof(controls[0]); ++i )
        ::EnableWindow( ::GetDlgItem( GetSafeHwnd(), controls[i] ), FALSE );

    m_List.DeleteAllItems();

    m_Spectro.GetWindowText( name );
    isPreviewSupported = (name.Left(3).CompareNoCase(L"ci7") == 0);
    if( isPreviewSupported )
        m_chkPreview.ShowWindow( SW_SHOW );

    GetPrivateProfileString( _T("Drivers"), name, _T(""), dllPath.GetBuffer(_MAX_PATH), _MAX_PATH, _T(".\\spectro.ini") );
    dllPath.ReleaseBuffer();

    // load instrument DLL
    if( !spectro.LoadDLL( dllPath ) )
	{
        m_Version.SetWindowText( _T("") );
        print( _T("FAILED to Load Driver") );
        return;
	}

    // display dll version and Interface version
    CString dllVersion = GetFileVersion( dllPath );
    interfaceVersion = spectro.InterfaceVersion();
    msg.Format( _T("%s - Interface v%s"), dllVersion, interfaceVersion );
    m_Version.SetWindowTextW( msg );

    for( int i = 0; i < sizeof(controls) / sizeof(controls[0]); ++i )
        ::EnableWindow( ::GetDlgItem( GetSafeHwnd(), controls[i] ), TRUE );

    UpdateStatus();
}

void CMainDlg::OnClose() 
{
	OnDisconnect();
	CDialog::OnClose();
}

void CMainDlg::OnCancel()
{
    OnDisconnect();
    CDialog::OnCancel();
}

void CMainDlg::OnConnect() 
{
    CWaitCursor wait;
    UpdateData();

	m_List.DeleteAllItems();
	
    // disconnect first and update status
    spectro.Disconnect();
    UpdateStatus();

	if( !spectro.Connect() )
	{
        PrintError();
		return;
	}

    UpdateStatus();

    // configure for special features

    // turn scan mode off
    m_Scan.SetCheck(0);
    OnBnClickedScan();

    // turn camera preview off
    m_chkPreview.SetCheck(0);
    OnBnClickedPreview();

    if( interfaceVersion == "1.1" )
    {
        isScanSupported = false;
    }
    else
    {
        isScanSupported = spectro.ScanIsSupported();
        if( isScanSupported )
            m_Scan.ShowWindow( SW_SHOW );
    }

    // watch for user triggered measurements
    EnablePolling();

	m_Command.SetFocus();
    m_Command.SetEditSel( 0, -1 );
}


void CMainDlg::OnDisconnect() 
{
    CWaitCursor wait;
	DisablePolling();
    if( spectro.IsLoaded() )
    {
        spectro.Disconnect();
        UpdateStatus();
    }
}


void CMainDlg::OnOK() 
{
    CWaitCursor wait;
    CString text;
	CStringA cmd, arg, rtn;

    if( !spectro.IsConnected() )
    {
        print( _T("Not Connected") );
        return;
    }

    m_Command.GetWindowText( text );
    text.Trim();
	if( text.IsEmpty() )
		return;

    if( m_Command.FindStringExact( 0, text ) < 0 )
        m_Command.AddString( text );

    m_Command.SetEditSel( 0, -1 );

    cmd = text;
    if( cmd.Left(13).CompareNoCase( "ExecuteBinary" ) == 0 )
    {
        if( interfaceVersion == "1.1" )
        {
            print( L"ExecuteBinary not supported" );
            return;
        }

        int pos = 0;
        cmd = text.Tokenize( _T(" "), pos );
        arg = text.Tokenize( _T("\0"), pos );

        char data[1000];
        long length = spectro.ExecuteBinary( arg, arg.GetLength(), 3000, data, sizeof(data) );
        if( length < 0 )
        {
            PrintError();
	    	return;
        }
        data[length] = 0;
        rtn = data;
        print( CString(rtn) );
    }
    else
    {
        rtn = spectro.Execute( cmd );
	    if( rtn.IsEmpty() )
	    {
            PrintError();
		    return;
	    }
        print( CString(rtn) );
    }
}


void CMainDlg::OnBnClickedMeasure()
{
    if( !spectro.IsConnected() )
    {
        print( _T("Not Connected") );
        return;
    }

    CWaitCursor wait;
    print( _T("Measuring...") );
	if( ! spectro.Measure() )
	{
    	PrintError();
		return;
	}

	GetReflData();
}

CString CMainDlg::AutoCalibration(vector<CStringA>& stepList)
{
    CString msg = "Prepare the instument for calibration.\nThen click 'OK'";
    if( MessageBox( msg, L"Calibration", MB_OKCANCEL ) != IDOK )
    {
        msg = "Calibration Aborted";
        return msg;
    }

    CWaitCursor wait; 
    if( !spectro.CalibrateStep( stepList[0] ) )
        return CString( spectro.GetLastErrorString() );
    else
        return _T("Success");
}

CString CMainDlg::DeviceDisplayCalibration(vector<CStringA>& stepList)
{
    CString msg;
    if( !spectro.CalibrateStep( stepList[0] ) )
    {
        msg = CString(spectro.GetLastErrorString());
    }
    else
    {
        msg = "Follow the calibration instructions on your instrument display.\nWhen calibration is finished, click 'OK'";
        if( MessageBox( msg, L"Calibration", MB_OKCANCEL ) != IDOK )
        {
            spectro.AbortCalibration();
            msg = "Calibration Aborted";
        }
        else
        {
            msg = "Success";
        }
    }

    UpdateStatus();
    return msg;
}

CString CMainDlg::SynchronousCalibration(vector<CStringA>& stepList)
{
    CString msg;
    CStringA step;

    for( size_t i = 0; i < stepList.size(); ++i )
    {
        step = stepList[i];

        // display appropriate prompt
        if( step.CompareNoCase( "White" ) == 0 )
        {
            msg = _T("Position the Instrument White Calibration Standard.\nThen click 'OK'");
        }
        else
        if( step.CompareNoCase( "Black" ) == 0 )
        {
            msg = _T("Position the Instrument Black Trap and click 'OK'");
        }
        else
        if( step.Left(2).CompareNoCase( "UV" ) == 0 )
        {
            msg = _T("Position the UV Whiteness Standard and click 'OK'");
        }
        else
        {
            msg = _T("Position the Custom Calibration Standard and click 'OK'");
        }

        if( MessageBox( msg, L"Calibration", MB_OKCANCEL ) != IDOK )
        {
            return _T("Calibration Aborted");
        }

        // perform step
        {
            CWaitCursor wait; 
            if( !spectro.CalibrateStep( step ) )
            {
                return CString(spectro.GetLastErrorString());
            }
        }
    }
    return _T("Success");
}

CString CMainDlg::AsynchronousCalibration(vector<CStringA>& stepList)
{
    CalibrateDlg dlg( this, &spectro, stepList );
    INT_PTR status = dlg.DoModal();
    switch( status )
    {
        case 0: return _T("Success");

        case IDCANCEL: return _T("Calibration Aborted");

        default:    //Error
        {
            return CString( spectro.GetLastErrorString() );
        }
    }
}

void CMainDlg::OnBnClickedCalibrate()
{
    CStringW msg;
    if( !spectro.IsConnected() )
    {
        print( _T("Not Connected") );
        return;
    }

    print( _T("Calibrating...") );

    CStringA calMode = spectro.GetCalMode();
    if( calMode.IsEmpty() )
    {
        PrintError();
        return;
    }

    CStringA str = spectro.GetCalSteps();
    vector<CStringA> stepList = split( str, ";" ); 
    CStringA step = stepList[0];

    // if calibration is triggered by Software, the calibration process can be Synchronous.
    // if calibration is triggered by Preassure or Button, the calibration process must be Asynchronous to allow the user to Cancel.
    bool m_bAsynchronous = ( calMode.CompareNoCase( "Software" ) != 0 );

    // check for 4 possible cases
    if( step.CompareNoCase( "Auto" ) == 0 )
    {
        msg = AutoCalibration( stepList );
    }
    else
    if( step.CompareNoCase( "DeviceDisplay" ) == 0 )
    {
        msg = DeviceDisplayCalibration( stepList );
    }
    else
    if( m_bAsynchronous )
    {
        msg = AsynchronousCalibration( stepList );
    }
    else
    {
        msg = SynchronousCalibration( stepList );
    }

    print( msg );
    UpdateStatus();
}


void CMainDlg::OnBnClickedConfigure()
{
    CConfigure dlg;
    dlg.m_spectro = &spectro;
    dlg.DoModal();

    UpdateStatus();
}

void CMainDlg::OnBnClickedRetrieve()
{
    CStringW msg;
    if( !spectro.IsConnected() )
    {
        print( _T("Not Connected") );
        return;
    }

    int nSamples = spectro.GetSampleCount();
    msg.Format( _T("Number of Stored Samples = %d"), nSamples );
    print( msg );

    for( int i = 1; i <= nSamples; ++i )
    {
        if( !spectro.SetCurrentSample( i ) )
        {
            PrintError();
            return;
        }

        msg.Format( _T("Sample #%d"), i );
        print( msg );

	    CString str, num;
        int wlCount = spectro.GetWavelengthCount();
        int datSetCount = spectro.GetSpectralSetCount();

        str = "WL   ";
        for( int wl = 0; wl < wlCount; ++wl )
        {
            int value = spectro.GetWavelengthValue( wl );
            num.Format( _T("%7d"), value );
            str += num;
        }
        print( str );

        for( int dataSet = 0; dataSet < datSetCount; ++dataSet )
	    {
            str = spectro.GetSpectralSetName( dataSet );
            while( str.GetLength() < 5 )
                str += ' ';

            for( int wl = 0; wl < wlCount; ++wl )
            {
                float value = spectro.GetSampleData( dataSet, wl );
                num.Format( _T("%7.2f"), value );
                str += num;
            }
            print( str );
	    }
    }
    print( _T(" ") );
}

void CMainDlg::OnBnClickedPreview()
{
    CRect r0, r1, r2;

    GetDlgItem(IDC_CONFIGURE)->GetWindowRect(&r0);
    m_List.GetWindowRect(&r1);
    m_Picture.GetWindowRect(&r2);

    if( m_chkPreview.GetCheck() )
    {
        r1.right = r2.left - 5;
        m_Picture.ShowWindow( SW_SHOW );
    }
    else
    {
        r1.right = r0.right;
        m_Picture.SetBitmap( NULL );
        m_Picture.ShowWindow( SW_HIDE );
    }
    m_List.SetWindowPos(NULL, 0, 0, r1.Width(), r1.Height(), SWP_NOMOVE | SWP_NOOWNERZORDER );
}

void CMainDlg::DisplayPreview()
{
    CString str;
    CStringA cmd;

    #define BUFFERSIZE 310000               // 320 x 320 x 3 = 307200 + header
    char* data = new char[BUFFERSIZE];
    bool isError = false;

    ULONG bytesWritten;
    IStream* stream = SHCreateMemStream( NULL, NULL );
    if( stream == NULL )
        return;

    try
    {
        CStringA cmd = "CaptureImage";
        long length = spectro.ExecuteBinary( cmd, cmd.GetLength(), 3000, data, BUFFERSIZE );
        if( length < 0 )
            throw 1;

        if( stream->Write( data, length, &bytesWritten ) != S_OK )
            throw 2;

        using namespace Gdiplus;
        HBITMAP hBitmap = NULL;
        ULONG_PTR gdiplusToken;
	    GdiplusStartupInput gdiplusStartupInput;
	    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

        // create bitmap from IStream and set it to Bitmap Button
        Bitmap* bitmap = Bitmap::FromStream(stream);
        bitmap->GetHBITMAP( NULL, &hBitmap );
        m_Picture.SetBitmap( hBitmap );

        delete bitmap;
        GdiplusShutdown(gdiplusToken);
    }
    catch(...)
    {
        isError = true;
        m_Picture.SetBitmap( NULL );
    }
    // free data buffer and stream
    delete[] data;
    stream->Release();
}

void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
    if( !spectro.IsLoaded() || !spectro.IsConnected() )
        return;

    KillTimer( nIDEvent );

    if( spectro.IsDataReady() )
	{
		// on measurement - get reflectance data
		GetReflData();
	}

    if( isScanSupported && m_Scan.GetCheck() )
    {
        int scanStatus = spectro.ScanGetStatus();
        switch(scanStatus)
        {
            // on scan - get scan data
            case SCANCOMPLETE: GetScanData(); break;

            // on paper measurement - get reflectance data
            case PAPERCOMPLETE: GetReflData(); break;
        }
    }

    if( isPreviewSupported && m_chkPreview.GetCheck() )
    {
        // if preview active - capture and display camera image
        DisplayPreview();
    }

    m_iTimer = SetTimer( nIDEvent, 100, NULL );
}

/////////////////////////////////////////////////////////////////
// helpers

// use timer to watch for measurements
void CMainDlg::EnablePolling()
{
	m_iTimer = SetTimer( 1, 100, NULL );
};

void CMainDlg::DisablePolling()
{
    if( m_iTimer > 0 )
        KillTimer(m_iTimer);
	m_iTimer = 0;
};

void CMainDlg::PrintError()
{
	print( CString( spectro.GetLastErrorString() ) );
}

void CMainDlg::print( LPCTSTR _str )
{
    int index;
    int pos = 0;
    CString str = _str;

    vector<CString> lines = split( str, _T("\r\n") );
    for( size_t i = 0; i < lines.size(); ++i )
	{
		index = m_List.GetItemCount();
		m_List.InsertItem( index, lines[i] );
	}
	m_List.SetColumnWidth( 0, LVSCW_AUTOSIZE );
	m_List.EnsureVisible( index, FALSE );
	m_List.UpdateWindow();
}

void CMainDlg::GetReflData()
{
	CString str, num;

    int wlCount = spectro.GetWavelengthCount();
    int datSetCount = spectro.GetSpectralSetCount();

    str = "WL   ";
    for( int wl = 0; wl < wlCount; ++wl )
    {
        int value = spectro.GetWavelengthValue( wl );
        num.Format( _T("%7d"), value );
        str += num;
    }
    print( str );

    for( int dataSet = 0; dataSet < datSetCount; ++dataSet )
	{
        str = spectro.GetSpectralSetName( dataSet );
        while( str.GetLength() < 5 )
            str += ' ';

        for( int wl = 0; wl < wlCount; ++wl )
        {
            float value = spectro.GetSpectralData( dataSet, wl );
            num.Format( _T("%7.2f"), value );
            str += num;
        }
        print( str );
	}
    print( _T(" ") );

    UpdateStatus();
}

void CMainDlg::GetScanData()
{
	CString str, rtn;

    if( !isScanSupported )
        return;

    int wlCount = spectro.GetWavelengthCount();
    int datSetCount = spectro.GetSpectralSetCount();
    int patchCount = spectro.ScanGetCount();

    for( int patch = 0; patch < patchCount; ++patch )
    {
        for( int dataSet = 0; dataSet < datSetCount; ++dataSet )
	    {
            rtn = spectro.GetSpectralSetName( dataSet );
            str.Format( L"Patch %d:%s ", patch+1, rtn );

            rtn = spectro.ScanGetData( patch, dataSet );
            if( rtn.IsEmpty() )
            {
                PrintError();
                return;
            }
            str += rtn;
            print( str );
	    }
    }
    print( _T(" ") );

    UpdateStatus();
}

void CMainDlg::UpdateStatus()
{
	m_Status.DeleteAllItems();

    CStringA available, setting, option, entry;

    int index = -1;
    if( !spectro.IsConnected() )
    {
        m_Status.InsertItem( ++index, _T("Disconnected") );
    }
    else
    {
        m_Status.InsertItem( ++index, _T("Connected") );

        entry.Format( "Serial Number=%s", spectro.GetSerialNum() );
        m_Status.InsertItem( ++index, CString(entry) );

        entry.Format( "Calibration Standard=%s", spectro.GetCalibrationStandard() );
        m_Status.InsertItem( ++index, CString(entry) );

        switch( spectro.GetCalStatus() )
        {
            case -1: entry = "Calibration Status=Unknown"; break;
            case 0: entry = "Calibration Status=Calibrated"; break;
            default: entry = "Calibration Status=Not Calibrated"; break;
        }
        m_Status.InsertItem( ++index, CString(entry) );
    }

    available = spectro.GetAvailableSettings();
    vector<CStringA> settingList = split( available, ";" );

    for( size_t i = 0; i < settingList.size(); ++i )
    {
        setting = settingList[i];
        option = spectro.GetOption( setting );

        setting.Replace("_", " ");
        entry =  setting + "=" + option;
        m_Status.InsertItem( ++index, CString(entry) );
    }

    m_Status.SetColumnWidth( 0, LVSCW_AUTOSIZE );
}

void CMainDlg::OnBnClickedCopy()
{
    int n;
    CStringA str, value;

    POSITION pos = m_List.GetFirstSelectedItemPosition();
    for(;;)
    {
        n = m_List.GetNextSelectedItem(pos);
        if( n < 0 )
            break;
        CStringA text = m_List.GetItemText( n, 0 );

        int pos = 0;
        value = text.Tokenize( " ", pos );
        while( !value.IsEmpty() )
        {
            str += value;
            str += "\t";
            value = text.Tokenize( " ", pos );
        }
        str += "\r\n";
    }

    OpenClipboard();
    EmptyClipboard();
    int len = str.GetLength() + 1;
    HANDLE hMem = GlobalAlloc( NULL, len );
    char* ptr = (char*)GlobalLock( hMem );
    memcpy( ptr, (LPCSTR)str, len );
    ptr[len-1] = 0;
    GlobalUnlock( hMem );
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}


void CMainDlg::OnBnClickedScan()
{
    if( !isScanSupported )
        return;

    if( !m_Scan.GetCheck() )
    {
        spectro.ScanAbort();
        return;
    }

    if( !spectro.ScanConfig( 5, 5.0 ) )
    {
        PrintError();
        return;
    }

    if( !spectro.ScanStart() )
    {
        PrintError();
        return;
    }
    print( L"Ready to Scan" );
}

