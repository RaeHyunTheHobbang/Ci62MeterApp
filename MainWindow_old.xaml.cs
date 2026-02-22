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
    /// MainWindow_old.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow_old : Window
    {
        public MainWindow_old()
        {
            InitializeComponent();
        }

        private void logListUpdated(object sender, DataTransferEventArgs e)
        {
            // 데이터 바인딩이 업데이트될 때 스크롤 이동
            logListBox.ScrollIntoView(logListBox.Items[logListBox.Items.Count - 1]);
        }

    }
}
