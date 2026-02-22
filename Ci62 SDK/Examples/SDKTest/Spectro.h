#ifndef _SPECTRO_H_
#define _SPECTRO_H_

#include <Windows.h>

typedef const char* _stdcall    _GetInterfaceVersion();                     // returns the interface version
typedef bool        _stdcall    _Connect();                                 // connect to instrument
typedef bool        _stdcall    _Disconnect();                              // disconnect from instrument
typedef bool        _stdcall    _IsConnected();                             // returns connection status
typedef int         _stdcall    _GetSpectralSetCount();                     // number of spectral sets
typedef int         _stdcall    _GetWavelengthCount();                      // number of wavelengths
typedef const char* _stdcall    _GetSpectralSetName( int dataSet );         // string representation of spectral set
typedef int         _stdcall    _GetWavelengthValue(int index);             // returns the Wavelength in nm (ex. 400 for index=0)
typedef const char* _stdcall    _GetCalibrationStandard();                  // string representation of calibration standard
typedef bool        _stdcall    _Measure();                                 // commands instrument to take a measurement
typedef float       _stdcall    _GetSpectralData(int angle, int wl);        // returns current measurement value for the specified angle and wavelengh indexes
typedef bool        _stdcall    _IsDataReady();                             // returns measurement status (TRUE after measurment is taken)
typedef const char* _stdcall    _GetSerialNum();                            // returns instrument Serial Number
typedef int         _stdcall    _GetCalStatus();                            // returns calibration status (0 = calibrated)
typedef const char* _stdcall    _GetCalSteps();                             // returns a list of calibration steps
typedef const char* _stdcall    _GetCalMode();                              // returns calibration trigger mode ("Software" or "Device")
typedef bool        _stdcall    _CalibrateStep(const char* calstep);        // performs the specified calibration step
typedef int         _stdcall    _GetCalProgress();                          // check progress of asynchronous calibration step
typedef bool        _stdcall    _AbortCalibration();                        // abort calibration
typedef bool        _stdcall    _ClearSamples();                            // clears all existing Samples in the instrument
typedef int         _stdcall    _GetSampleCount();                          // returns count of stored samples
typedef bool        _stdcall    _SetCurrentSample( int sample );            // makes the specified sample active order to retrieve its data
typedef float       _stdcall    _GetSampleData( int angle, int wl );        // returns value for the specified angle and wavlength of the active stored sample
typedef const char* _stdcall    _GetAvailableSettings();                    // returns a list of available settings
typedef const char* _stdcall    _GetSettingOptions(const char* setting);    // returns a list of options for the specified settings
typedef const char* _stdcall    _GetOption(const char* setting);            // returns the current option for the specified settings
typedef bool        _stdcall    _SetOption(const char* setting, const char* option);    // sets the current option for the specified setting
typedef int         _stdcall    _GetLastErrorCode();                        // retrieves the last error code as an integer
typedef const char* _stdcall    _GetLastErrorString();                      // retrieves the last error as a string (in language selected by SetLanguage function)
typedef const char* _stdcall    _Execute( const char* cmdString );          // executes a task or function specified by a string
                                                                            // an empty returned string ("") indicates an error.
                                                                            // call GetLastErrorCode() or GetLastErrorString() for more information

typedef long        _stdcall    _ExecuteBinary(                             // General purpose function similar to "Execute", but allowing for
                                    const char* inputBuffer,                // binary data to be sent and/or received:  return value is number of bytes received.
                                    long inputLength,                       // The caller is responsible for all memory management.
                                    long timeout,
                                    char* outputBuffer,
                                    long outputBufferSize );


/////////////////////////////////////////////////////////////////////
// interface additions for Scanning support

enum SCANSTATUS
{
    SCANERROR = -1,
    IDLE = 0,
    READYTOSCAN = 1,
    SCANINPROGRESS = 2,
    SCANCOMPLETE = 3,
    PAPERINPROGRESS = 4,
    PAPERCOMPLETE = 5
};

typedef bool        _stdcall    _ScanIsSupported();
typedef bool        _stdcall    _ScanConfig( int patchCount, float patchWidth );
typedef bool        _stdcall    _ScanStart();
typedef bool        _stdcall    _ScanAbort();
typedef int         _stdcall    _ScanGetStatus();
typedef int         _stdcall    _ScanGetCount();
typedef const char* _stdcall    _ScanGetData( int patchIndex, int dataSet );



class Spectro
{
private:
    HMODULE m_hLib;
    void Init();

public:
    _GetInterfaceVersion*       GetInterfaceVersion;
    _Connect*                   Connect;
    _Disconnect*                Disconnect;
    _IsConnected*               IsConnected;
    _GetSpectralSetCount*       GetSpectralSetCount;
    _GetWavelengthCount*        GetWavelengthCount;
    _GetSpectralSetName*        GetSpectralSetName;
    _GetWavelengthValue*        GetWavelengthValue;
    _GetCalibrationStandard*    GetCalibrationStandard;
    _Measure*                   Measure;
    _GetSpectralData*           GetSpectralData;
    _IsDataReady*               IsDataReady;
    _GetSerialNum*              GetSerialNum;
    _GetCalStatus*              GetCalStatus;
    _GetCalSteps*               GetCalSteps;
    _GetCalMode*                GetCalMode;
    _GetCalProgress*            GetCalProgress;
    _AbortCalibration*          AbortCalibration;
    _CalibrateStep*             CalibrateStep;
    _ClearSamples*              ClearSamples;
    _GetSampleCount*            GetSampleCount;
    _SetCurrentSample*          SetCurrentSample;
    _GetSampleData*             GetSampleData;
    _GetAvailableSettings*      GetAvailableSettings;
    _GetSettingOptions*         GetSettingOptions;
    _GetOption*                 GetOption;
    _SetOption*                 SetOption;
    _GetLastErrorCode*          GetLastErrorCode;
    _GetLastErrorString*        GetLastErrorString;
    _Execute*                   Execute;
    _ExecuteBinary*             ExecuteBinary;
    _ScanIsSupported*           ScanIsSupported;
    _ScanConfig*                ScanConfig;
    _ScanStart*                 ScanStart;
    _ScanAbort*                 ScanAbort;
    _ScanGetStatus*             ScanGetStatus;
    _ScanGetCount*              ScanGetCount;
    _ScanGetData*               ScanGetData;


    Spectro() 
    {
        Init();
    };

    ~Spectro()
    {
        FreeDLL();
    }

    bool IsLoaded()
    {
        return (m_hLib != NULL);
    };

    const char* InterfaceVersion()
    {
        if( GetInterfaceVersion == NULL )
            return "1.1";
        else
            return GetInterfaceVersion();
    }

    bool LoadDLL(LPCTSTR dll);
    void FreeDLL();
};

#endif // _SPECTRO_H_
