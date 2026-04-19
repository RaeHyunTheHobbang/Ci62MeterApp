using MeterCi62App.Data;
using MeterCi62App.Model;
using MeterCi62App.ViewModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Security.Cryptography.Pkcs;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace MeterCi62App
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        Ci62Cmd CurCmd;
        MainViewModel curVm;
        SettingWindow curSetWindow;
        bool CloseFlag;

        public MainWindow()
        {
            CurCmd = new Ci62Cmd();
            curVm = new MainViewModel(CurCmd);
            

            curVm.SettingWindowShowEvent += SettingShow;
            this.DataContext = curVm;
            this.Closing  += CloseHandler;

            curSetWindow = new SettingWindow(curVm);
            curSetWindow.DataContext = curVm;
            

        }

        void SettingShow(object s,bool e)
        {

            curSetWindow.Left = this.Left + (this.Width - curSetWindow.Width) / 2;
            curSetWindow.Top = this.Top + (this.Height - curSetWindow.Height) / 2;
            if (curSetWindow.IsVisible)
            {
                curSetWindow.Activate();
            }
            curSetWindow.Show();
        }

        void CloseHandler(object sender,CancelEventArgs e)
        {
            e.Cancel= !CloseFlag;
            
            if (!CloseFlag)
                this.WindowState = WindowState.Minimized;

            curSetWindow.Close();

        }




        

        void test()
        {
            var result = CurCmd.GetReflectance();
            if (result is not null)
            {
                for (int i = 0; i < result.Length; i++)
                {
                    Debug.WriteLine($"Index {i}: {result[i]}");
                }

                foreach (Illuminant illum in System.Enum.GetValues(typeof(Illuminant)))
                {
                    var labValues = SpectralConverter.CalculateLab(result, illum);

                    Debug.WriteLine($"Illuminant: {illum.ToString()} => {labValues.ToString()}");
                }

            }
        }


    }
}