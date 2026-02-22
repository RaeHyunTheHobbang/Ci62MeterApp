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
    class MainViewModel:ViewData
    {

        Thread connectThread;
        Thread calculateLabTread;
        Ci62Cmd deviceCmd;

        int connectCheckDelay = 50; //ms
        bool tmpConnectCheck;
        int tmpDataCount;

        RestApi curApi;
        string curURL;
        const string UrlRegex =
        @"^(https?:\/\/)" +                          // http or https
        @"(([a-zA-Z0-9\-]+\.)+[a-zA-Z]{2,}" +         // domain
        @"|localhost" +                               // localhost
        @"|(\d{1,3}\.){3}\d{1,3})" +                   // IPv4
        @"(:\d{1,5})?" +                               // port
        @"(\/[^\s]*)?$";                              // path/query




        public MainViewModel(Ci62Cmd curCmd)
        {
            deviceCmd = curCmd;
            curURL = "";
            
            defaultSetting();

        }

        void defaultSetting()
        {
            WriteLog("DisConnected");
            connectVisualLoad();

            curApi = new RestApi();

            viewBtnCtrl = new BasicCommand(btnControl);



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
                    }
                    MessageBox.Show(
                                    $"측정시 '{targetUrl}'로 전송됩니다.",
                                    "알림",
                                    MessageBoxButton.OK,
                                    MessageBoxImage.Information
                                    );
                    //targetUrl
                    break;

                default:
                    break;
            }
        }
        bool isValidUrl(string url)
        {   
            if(string.IsNullOrEmpty(url)) return false;
            return Regex.IsMatch(url, UrlRegex);
        }

        void getPriorLab()
        {
            int timeStrLen = new logMsg("").timeStamp.Length;
            string spaces = new string(' ', timeStrLen + 5);

            float[]? curSpectrumData=deviceCmd.GetPriorReflectance();
            if(curSpectrumData is not null)
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

        #region Thread Controller Method
        void connectThreadControl()
        {
            bool conResult = false;
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
                        conResult = deviceCmd.DeviceConnect();

                        if (!conResult)
                        {
                            deviceCmd.SetUsbMode();
                            conResult = deviceCmd.DeviceConnect();
                        }

                    }
                    else
                    {
                        deviceCmd.SetUsbMode();
                        conResult = deviceCmd.DeviceConnect();
                    }

                    Debug.WriteLine($"현재 연결 상태:{conResult}");
                    tmpConnectCheck = conResult;

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
                            calculatedTime  = DateTime.Now.ToString("MM-dd HH:mm:ss");
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

                                    foreach (string curillum in curData.Keys)
                                    {
                                        Lab curLab = curData[curillum];
                                        bool postResult = curApi.Post(curURL, serialNum, curLab.L, curLab.a, curLab.b, calculatedTime).Result;
                                        string logMsg = (postResult) ? $"Data Send Success to \"{curURL}\" " : $"Data Send Failed to \"{curURL}\" ";
                                        WriteLog(logMsg);
                                    }

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
    }
}
