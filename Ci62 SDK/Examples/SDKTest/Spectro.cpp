// Spectro.cpp : Defines the class behaviors for the application.
//

#include "Spectro.h"

void Spectro::Init()
{
    FreeDLL();

    GetInterfaceVersion = NULL;
    Connect = NULL;
    Disconnect = NULL;
    IsConnected = NULL;
    GetSpectralSetCount = NULL;
    GetWavelengthCount = NULL;
    GetSpectralSetName = NULL;
    GetWavelengthValue = NULL;
    GetCalibrationStandard = NULL;
    Measure = NULL;
    GetSpectralData = NULL;
    IsDataReady = NULL;
    GetSerialNum = NULL;
    GetCalStatus = NULL;
    GetCalSteps = NULL;
    GetCalMode = NULL;
    GetCalProgress = NULL;
    AbortCalibration = NULL;
    CalibrateStep = NULL;
    ClearSamples = NULL;
    GetSampleCount = NULL;
    SetCurrentSample = NULL;
    GetSampleData = NULL;
    GetAvailableSettings = NULL;
    GetSettingOptions = NULL;
    GetOption = NULL;
    SetOption = NULL;
    GetLastErrorCode = NULL;
    GetLastErrorString = NULL;
    Execute = NULL;
    ExecuteBinary = NULL;
    ScanIsSupported = NULL;
    ScanConfig = NULL;
    ScanStart = NULL;
    ScanAbort = NULL;
    ScanGetStatus = NULL;
    ScanGetCount = NULL;
    ScanGetData = NULL;
}

bool Spectro::LoadDLL( LPCTSTR dll )
{
    Init();

    m_hLib = LoadLibrary( dll );
	if( !m_hLib )
	{
		return FALSE;
	}

	Connect = (_Connect*)GetProcAddress( m_hLib, "Connect" );
	if( !Connect )
		goto error;
	Disconnect = (_Disconnect*)GetProcAddress( m_hLib, "Disconnect" );
	if( !Disconnect )
		goto error;
	IsConnected = (_IsConnected*)GetProcAddress( m_hLib, "IsConnected" );
	if( !IsConnected )
		goto error;
	GetSpectralSetCount = (_GetSpectralSetCount*)GetProcAddress( m_hLib, "GetSpectralSetCount" );
	if( !GetSpectralSetCount )
		goto error;
	GetWavelengthCount = (_GetWavelengthCount*)GetProcAddress( m_hLib, "GetWavelengthCount" );
	if( !GetWavelengthCount )
		goto error;
	GetSpectralSetName = (_GetSpectralSetName*)GetProcAddress( m_hLib, "GetSpectralSetName" );
	if( !GetSpectralSetName )
		goto error;
	GetWavelengthValue = (_GetWavelengthValue*)GetProcAddress( m_hLib, "GetWavelengthValue" );
	if( !GetWavelengthValue )
		goto error;
	GetCalibrationStandard = (_GetCalibrationStandard*)GetProcAddress( m_hLib, "GetCalibrationStandard" );
	if( !GetCalibrationStandard )
		goto error;
	Measure = (_Measure*)GetProcAddress( m_hLib, "Measure" );
	if( !Measure )
		goto error;
	GetSpectralData = (_GetSpectralData*)GetProcAddress( m_hLib, "GetSpectralData" );
	if( !GetSpectralData )
		goto error;
	IsDataReady = (_IsDataReady*)GetProcAddress( m_hLib, "IsDataReady" );
	if( !IsDataReady )
		goto error;
	GetSerialNum = (_GetSerialNum*)GetProcAddress( m_hLib, "GetSerialNum" );
	if( !GetSerialNum )
		goto error;
	GetCalStatus = (_GetCalStatus*)GetProcAddress( m_hLib, "GetCalStatus" );
	if( !GetCalStatus )
		goto error;
	GetCalSteps = (_GetCalSteps*)GetProcAddress( m_hLib, "GetCalSteps" );
	if( !GetCalSteps )
		goto error;
	GetCalMode = (_GetCalMode*)GetProcAddress( m_hLib, "GetCalMode" );
	if( !GetCalMode )
		goto error;
	CalibrateStep = (_CalibrateStep*)GetProcAddress( m_hLib, "CalibrateStep" );
	if( !CalibrateStep )
		goto error;
	GetCalProgress = (_GetCalProgress*)GetProcAddress( m_hLib, "GetCalProgress" );
	if( !GetCalProgress )
		goto error;
	AbortCalibration = (_AbortCalibration*)GetProcAddress( m_hLib, "AbortCalibration" );
	if( !AbortCalibration )
		goto error;
    ClearSamples = (_ClearSamples*)GetProcAddress( m_hLib, "ClearSamples" );
	if( !ClearSamples )
		goto error;
	GetSampleCount = (_GetSampleCount*)GetProcAddress( m_hLib, "GetSampleCount" );
	if( !GetSampleCount )
		goto error;
	SetCurrentSample = (_SetCurrentSample*)GetProcAddress( m_hLib, "SetCurrentSample" );
	if( !SetCurrentSample )
		goto error;
	GetSampleData = (_GetSampleData*)GetProcAddress( m_hLib, "GetSampleData" );
	if( !GetSampleData )
		goto error;
    GetAvailableSettings = (_GetAvailableSettings*)GetProcAddress( m_hLib, "GetAvailableSettings" );
	if( !GetAvailableSettings )
		goto error;
    GetSettingOptions = (_GetSettingOptions*)GetProcAddress( m_hLib, "GetSettingOptions" );
	if( !GetSettingOptions )
		goto error;
    GetOption = (_GetOption*)GetProcAddress( m_hLib, "GetOption" );
	if( !GetOption )
		goto error;
    SetOption = (_SetOption*)GetProcAddress( m_hLib, "SetOption" );
	if( !SetOption )
		goto error;
    GetLastErrorCode = (_GetLastErrorCode*)GetProcAddress( m_hLib, "GetLastErrorCode" );
	if( !GetLastErrorCode )
		goto error;
	GetLastErrorString = (_GetLastErrorString*)GetProcAddress( m_hLib, "GetLastErrorString" );
	if( !GetLastErrorString )
		goto error;
	Execute = (_Execute*)GetProcAddress( m_hLib, "Execute" );
	if( !Execute )
		goto error;

    // new functions added to v1.2 interface
   	GetInterfaceVersion = (_GetInterfaceVersion*)GetProcAddress( m_hLib, "GetInterfaceVersion" );
    ScanIsSupported = (_ScanIsSupported*)GetProcAddress( m_hLib, "ScanIsSupported" );
    ScanConfig = (_ScanConfig*)GetProcAddress( m_hLib, "ScanConfig" );
	ScanStart = (_ScanStart*)GetProcAddress( m_hLib, "ScanStart" );
	ScanAbort = (_ScanAbort*)GetProcAddress( m_hLib, "ScanAbort" );
	ScanGetStatus = (_ScanGetStatus*)GetProcAddress( m_hLib, "ScanGetStatus" );
	ScanGetCount = (_ScanGetCount*)GetProcAddress( m_hLib, "ScanGetCount" );
	ScanGetData = (_ScanGetData*)GetProcAddress( m_hLib, "ScanGetData" );
	ExecuteBinary = (_ExecuteBinary*)GetProcAddress( m_hLib, "ExecuteBinary" );

	return true;

	error:

	Init();

	return false;
}

void Spectro::FreeDLL()
{
	// free library if loaded
	if( m_hLib )
	{
		FreeLibrary(m_hLib);
		m_hLib = NULL;
	}
}
