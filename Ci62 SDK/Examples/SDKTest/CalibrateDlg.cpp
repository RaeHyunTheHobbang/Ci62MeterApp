// CalibrateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "main.h"
#include "CalibrateDlg.h"

// CalibrateDlg dialog

IMPLEMENT_DYNAMIC(CalibrateDlg, CDialogEx)

CalibrateDlg::CalibrateDlg(CWnd* pParent, Spectro* spectro, vector<CStringA>& stepList)
	: CDialogEx(CalibrateDlg::IDD, pParent)
{
    this->m_spectro = spectro;
    this->m_stepList = stepList;
}

CalibrateDlg::~CalibrateDlg()
{
}

void CalibrateDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MSG, m_Msg);
}


BEGIN_MESSAGE_MAP(CalibrateDlg, CDialogEx)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CalibrateDlg message handlers


BOOL CalibrateDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CStringA str = m_spectro->GetCalSteps();
    m_stepList = split( str, ";" ); 
    m_currentStep = -1;

    m_bAbort = FALSE;
    m_bCalStarted = FALSE;
    SetTimer( 1, 100, NULL );

    return TRUE;
}


void CalibrateDlg::OnTimer(UINT_PTR nIDEvent)
{
    CDialogEx::OnTimer(nIDEvent);
    KillTimer( nIDEvent );

    // check for User Abort
    if( m_bAbort )
    {
        CWaitCursor wait;
        if( m_spectro->AbortCalibration() )
            EndDialog( IDCANCEL );
        else
            EndDialog( m_spectro->GetLastErrorCode() );
        return;
    }

    // if calibration step started - check for still in progress, done, or error
    if( m_bCalStarted )
    {
        int status = m_spectro->GetCalProgress();
        switch( status )
        {
            case 0:
                m_bCalStarted = FALSE;
            break;

            case 1:
                CDialogEx::OnTimer(nIDEvent);
                SetTimer( nIDEvent, 100, NULL );
                return;
            break;

            default:
                EndDialog( m_spectro->GetLastErrorCode() );
                return;
            break;
        }
    }

    // increment step and check for all steps completed
    ++m_currentStep;
    if( m_currentStep == m_stepList.size() )
    {
        EndDialog( 0 ); //completed successfully
        return;
    }

    // set message Prompt and initiate step
    CStringA step = m_stepList[m_currentStep];
    if( step.CompareNoCase( "White" ) == 0 )
    {
        m_Msg.SetWindowText( _T("Measure the Instrument White Calibration Standard." ) );
    }
    else
    if( step.CompareNoCase( "Black" ) == 0 )
    {
        m_Msg.SetWindowText( _T("Measure the Instrument Black Trap.") );
    }

    if( !m_spectro->CalibrateStep( step ) ) // Asynchronous calibration steps return immediately
    {
        EndDialog( m_spectro->GetLastErrorCode() );
        return;
    }

    m_bCalStarted = TRUE;
    SetTimer( nIDEvent, 100, NULL );
}


void CalibrateDlg::OnCancel()
{
    m_bAbort = TRUE;
}
