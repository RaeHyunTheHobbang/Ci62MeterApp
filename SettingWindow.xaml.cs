using MeterCi62App.ViewModel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace MeterCi62App
{
    /// <summary>
    /// Window1.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class SettingWindow : Window
    {
        MainViewModel CurVm;
        public SettingWindow(MainViewModel curVm)
        {
            InitializeComponent();
            CurVm = curVm;
            this.DataContext = CurVm;
            
            this.Closing += (s, e) =>
            {
                e.Cancel = true;
                Hide();
            };
        }
    }
}
