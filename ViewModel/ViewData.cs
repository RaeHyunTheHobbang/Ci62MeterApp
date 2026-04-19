using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;
using System.Drawing;
using System.Collections.ObjectModel;

namespace MeterCi62App.ViewModel
{
    public class ViewData : Notifier
    {

        protected string onImgPath = "/Resources/Imgs/Ci62OnImg.png";
        protected string offImgPath = "/Resources/Imgs/Ci62OffImg.png";
        protected string usbIconPath = "/Resources/Imgs/usbIcon.png";
        protected string bluetoothIconPath = "/Resources/Imgs/bluetoothIcon.png";

        public BasicCommand viewBtnCtrl { get; set; }


        BitmapSource _onOffImgSrc;
        BitmapSource _conectIconSrc;
        ObservableCollection<string> _logSrc = new ObservableCollection<string>();
        string _targetUrl;

        string _serialNum;

        public string serialNum
        {
            get
            {
                return _serialNum;
            }
            set
            {
                _serialNum = value;
                OnPropertyChanged(nameof(serialNum));
            }
        }
        public BitmapSource onOffImgSrc
        {
            get
            {
                return _onOffImgSrc;
            }
            set
            {
                _onOffImgSrc = value;
                OnPropertyChanged(nameof(onOffImgSrc));
            }
        }

        public BitmapSource conectIconSrc
        {
            get
            {
                return _conectIconSrc;
            }
            set
            {
                _conectIconSrc = value;
                OnPropertyChanged(nameof(conectIconSrc));
            }
        }


        public ObservableCollection<string> logSrc
        {
            get
            {
                return _logSrc;
            }
            set
            {
                _logSrc = value;
                OnPropertyChanged(nameof(logSrc));
            }
        }

        public string targetUrl
        {
            get
            {
                return _targetUrl;
            }
            set
            {
                _targetUrl = value;
                OnPropertyChanged(nameof(targetUrl));
            }
        }

        public struct logMsg(string curMsg,string curTime=null)
        {
            public string timeStamp =(curTime is null)? $"[{DateTime.Now.ToString("MM-dd HH:mm:ss")}]" : $"[{curTime}]";
            string msg = curMsg;


            public override string ToString()
            {
                return $"{timeStamp} {msg}";
            }


        }
    }
    
}
