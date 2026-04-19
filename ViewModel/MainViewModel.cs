using MeterCi62App.Data;
using MeterCi62App.Model;
using Microsoft.VisualBasic;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media.Imaging;

namespace MeterCi62App.ViewModel
{
    public class MainViewModel:ViewData
    {
        public event EventHandler<bool> SettingWindowShowEvent;
        Thread connectThread;
        Thread calculateLabTread;
        Ci62Cmd deviceCmd;

        int connectCheckDelay = 50; //ms
        bool tmpConnectCheck;
        int tmpDataCount;

        RestApi curApi;
        string curURL;
        bool curD65OnlyMode;
        //const string UrlRegex =
        //@"^(https?:\/\/)" +                          // http or https
        //@"(([a-zA-Z0-9\-]+\.)+[a-zA-Z]{2,}" +         // domain
        //@"|localhost" +                               // localhost
        //@"|(\d{1,3}\.){3}\d{1,3})" +                   // IPv4
        //@"(:\d{1,5})?" +                               // port
        //@"(\/[^\s]*)?$";                              // path/query


        bool _isConnect;
        public bool IsConnect
        {
            get => _isConnect;
            set
            {
                _isConnect = value;
                OnPropertyChanged(nameof(IsConnect));
            }   
        }
        string _onOffInfoText;
        public string onOffInfoText
        {
            get => _onOffInfoText;
            set
            {
                _onOffInfoText = value;
                OnPropertyChanged(nameof(onOffInfoText));
            }
        }

        BitmapSource _settingBtnSrc;
        public BitmapSource settingBtnSrc
        {
            get => _settingBtnSrc;
            set
            {
                _settingBtnSrc = value;
                OnPropertyChanged(nameof(settingBtnSrc));
            }
        }

        BitmapSource _exitBtnSrc;
        public BitmapSource exitBtnSrc
        {
            get => _exitBtnSrc;
            set
            {
                _exitBtnSrc = value;
                OnPropertyChanged(nameof(exitBtnSrc));
            }
        }

        public MainViewModel(Ci62Cmd curCmd)
        {
            deviceCmd = curCmd;

            //기본 URL세팅
            targetUrl = "https://kcc-routing.kccworld.co.kr";
            curURL = targetUrl;

            //D65 Only 모드 세팅
            D65OnlyOn = false;

            defaultSetting();

        }

        void defaultSetting()
        {
            WriteLog("DisConnected");
            connectVisualLoad();
            onOffInfoText = "DisConnect";
            curApi = new RestApi();
            


            viewBtnCtrl = new BasicCommand(btnControl);
            exitBtnSrc = new IconBitmapDecoder(
                            new Uri("Resources/Imgs/OnOffBtn.ico", UriKind.Relative),
                            BitmapCreateOptions.None,
                            BitmapCacheOption.Default).Frames[0];
            settingBtnSrc = new IconBitmapDecoder(
                            new Uri("Resources/Imgs/SettingBtn.ico", UriKind.Relative),
                            BitmapCreateOptions.None,
                            BitmapCacheOption.Default).Frames[0];

            connectThread = new Thread(connectThreadControl);
            connectThread.IsBackground = true;
            connectThread.Start();

            calculateLabTread = new Thread(calculateLabThreadControl);
            calculateLabTread.IsBackground = true;
            calculateLabTread.Start();
        }

        
        void connectVisualLoad(bool? isConnected=null)
        {

            Application.Current.Dispatcher.Invoke(() =>
            {
                switch (isConnected)
                {
                    case true:

                        onOffImgSrc = new BitmapImage(new Uri(onImgPath, UriKind.Relative));
                        IsConnect = true;
                        onOffInfoText = "Connect";
                        switch (deviceCmd.IsUsbMode)
                        {
                            case true:
                                conectIconSrc = new BitmapImage(new Uri(usbIconPath, UriKind.Relative));
                                break;
                            case false:
                                conectIconSrc = new BitmapImage(new Uri(bluetoothIconPath, UriKind.Relative));
                                break;
                        }
                        serialNum = deviceCmd.GetSerailNum();

                        break;
                    case false:

                        onOffImgSrc = new BitmapImage(new Uri(offImgPath, UriKind.Relative));
                        conectIconSrc = null;
                        serialNum = "";
                        IsConnect = false;
                        onOffInfoText = "DisConnect";
                        break;

                    default:

                        onOffImgSrc = new BitmapImage(new Uri(offImgPath, UriKind.Relative));
                        break;
                }
                 

            });
                


        }
        
        void WriteLog(string curMsg,string timeStamp=null)
        {
            Application.Current.Dispatcher.Invoke(() =>
            {   
                logSrc.Add(new logMsg(curMsg,timeStamp).ToString());
            });
        }

        void btnControl(string curParam)
        {
            switch (curParam)
            {
                case "urlSendStart":
                    if(!isValidUrl(targetUrl))
                    {
                        MessageBox.Show(
                                    $"올바른 URL형식을 입력하십시오",
                                    "알림",
                                    MessageBoxButton.OK,
                                    MessageBoxImage.Exclamation
                                    );
                        break;
                    }
                    lock(curURL)
                    {
                        curURL = targetUrl;
                        curD65OnlyMode = D65OnlyOn;
                    }
                    string curMsg = (curD65OnlyMode) ? $"측정시 '{targetUrl}'로 D65만 전송됩니다." : $"측정시 '{targetUrl}'로 전송됩니다.";
                    MessageBox.Show(
                                    curMsg,
                                    "알림",
                                    MessageBoxButton.OK,
                                    MessageBoxImage.Information
                                    );
                    //targetUrl
                    break;

                case "Setting":
                    SettingWindowShowEvent?.Invoke(this, true);

                    break;


                case "CloseAll":
                    MessageBoxResult userAnswer=MessageBox.Show("프로그램을 종료하시겠습니까?", "알림", MessageBoxButton.YesNo, MessageBoxImage.Question);
                    if(userAnswer == MessageBoxResult.Yes)
                        Environment.Exit(0);
                    break;
                default:
                    break;
            }
        }
        bool isValidUrl(string url)
        {   
            if(string.IsNullOrEmpty(url)) return false;
            //return Regex.IsMatch(url, UrlRegex);
            return true;
        }

        

        #region Thread Controller Method
        void connectThreadControl()
        {
            bool checkCon = false;
            bool isBluetoothMode = false;
            while (true)
            {

                if (!deviceCmd.IsDeviceConnected)
                {
                    if (tmpConnectCheck) //연결되었다 끊긴경우
                    {
                        connectVisualLoad(false);
                        WriteLog("DisConnected");
                    }

                    isBluetoothMode = deviceCmd.SetBluetoothMode();
                    if (isBluetoothMode)
                    {
                        checkCon = deviceCmd.DeviceConnect();

                        if (!checkCon)
                        {
                            deviceCmd.SetUsbMode();
                            checkCon = deviceCmd.DeviceConnect();
                        }

                    }
                    else
                    {
                        deviceCmd.SetUsbMode();
                        checkCon = deviceCmd.DeviceConnect();
                    }

                    Debug.WriteLine($"현재 연결 상태:{checkCon}");
                    tmpConnectCheck = checkCon;

                    if (tmpConnectCheck)
                    {
                        connectVisualLoad(true);
                        WriteLog("Connected");

                        //Debug.WriteLine(deviceCmd.GetSerailNum());

                        //getPriorLab(); //이전에 수행한 측색data get
                        //Debug.WriteLine(deviceCmd.GetSpectrumDataCount());

                    }

                }


                Task.Delay(connectCheckDelay).Wait();
            }
        }

        void calculateLabThreadControl()
        {
            int timeStrLen = new logMsg("").timeStamp.Length;
            string spaces = new string(' ', timeStrLen+5);
            string calculatedTime;
            string serialNum;
            Dictionary<string, Lab> curData;
            while (true)
            {
                if (deviceCmd.IsDeviceConnected)
                {
                    if (deviceCmd.IsDeviceDataReady)
                    {

                        float[]? curSpectrumData = deviceCmd.GetReflectance();
                        serialNum = deviceCmd.GetSerailNum();

                        if (curSpectrumData is not null)
                        {   
                            StringBuilder curSb = new StringBuilder();
                            //calculatedTime  = DateTime.Now.ToString("MM-dd HH:mm:ss");
                            calculatedTime = DateTime.Now.ToString("HH:mm:ss");
                            curData = new Dictionary<string, Lab>();

                            curSb.AppendLine("## L a b Data ## ");
                            foreach (Illuminant illum in System.Enum.GetValues(typeof(Illuminant)))
                            {
                                Lab? labValues = SpectralConverter.CalculateLab(curSpectrumData, illum);

                                if(labValues is not null)
                                {   
                                    string illumKey = illum.ToString();
                                    curSb.AppendLine(spaces + $"{illum.ToString()} ==> {labValues.ToString()}");
                                    curData.Add(illumKey, labValues.Value);
                                }
                            }
                            WriteLog(curSb.ToString(), calculatedTime);


                            //API Post 시작!
                            lock (curURL)
                            {
                        
                                if (!string.IsNullOrEmpty(curURL))
                                { 
                                    bool postResult = curApi.Post(curURL, serialNum, curData, calculatedTime, curD65OnlyMode).Result;
                                    string logMsg = (postResult) ? $"Data Send Success to \"{curURL}\" " : $"Data Send Failed to \"{curURL}\" ";
                                    WriteLog(logMsg);
                                }
                            }
                                
                            //tmpDataCount = deviceCmd.GetSpectrumDataCount();
                        }


                    }

                }

                Task.Delay(connectCheckDelay+100).Wait();
            }
        }

        #endregion




        #region Not used
        void getPriorLab()
        {
            int timeStrLen = new logMsg("").timeStamp.Length;
            string spaces = new string(' ', timeStrLen + 5);

            float[]? curSpectrumData = deviceCmd.GetPriorReflectance();
            if (curSpectrumData is not null)
            {
                StringBuilder curSb = new StringBuilder();
                curSb.AppendLine("## L a b Data ## ");
                foreach (Illuminant illum in System.Enum.GetValues(typeof(Illuminant)))
                {
                    Lab? labValues = SpectralConverter.CalculateLab(curSpectrumData, illum);

                    if (labValues is not null)
                    {
                        curSb.AppendLine(spaces + $"{illum.ToString()} ==> {labValues.ToString()}");
                    }
                }
                WriteLog(curSb.ToString());

            }
        }
        #endregion
    }
}
