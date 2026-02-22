using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Diagnostics;
using  MeterCi62App.Data;
using System.IO;


namespace MeterCi62App.Model
{
    internal partial class Ci62Cmd
    {

        string _priorSN = "027293";
        public bool IsUsbMode { get; private set; }

        public bool IsDeviceConnected
        {
            get
            {
                return IsConnected();
            }

        }
        public bool IsDeviceDataReady
        {
            get { return IsDataReady(); }

        }
        public bool IsCallibrated
        {
            get
            {
                return (GetCalStatus() == 0) ? true : false;
            }

        }

        

        internal Ci62Cmd() { }

        public bool DeviceConnect()
        {
            bool result=Connect();

            if (result && !IsUsbMode)
            {
                _priorSN = GetSerailNum();
            }

            return result;
        }


        public int GetSpectrumDataCount()
        {
            return GetSampleCount();
        }


        public bool SetBluetoothMode()
        {
            
            bool result = SetOption("Connection_Method", $"Cixx_{_priorSN }");

            if (result)
            {
                Debug.WriteLine("bluetooth옵션 설정 성공!");
                IsUsbMode = false;
                return true;
            }
            else
            {
                Debug.WriteLine("bluetooth옵션 설정 실패..");
                return false;
            }
        }

        public bool SetUsbMode()
        {
            // 입력 파라미터(const char*)는 그냥 string으로 넣으면 됩니다.
            bool result = SetOption("Connection_Method", "USB");

            if (result)
            {
                Debug.WriteLine("USB옵션 설정 성공!");
                IsUsbMode = true;
                return true;
            }
            else
            {
                Debug.WriteLine("USB옵션 설정 실패..");
                return false;
            }
        }



        public string GetSerailNum()
        {
            IntPtr ptr = GetSerialNum();
            if (ptr == IntPtr.Zero)
            {
                return null; // 또는 빈 문자열 ""
            }
            string result = Marshal.PtrToStringAnsi(ptr);
            return result;
        }


        public string GetSpectralName(int spectralset)
        {
            // (1) C 함수 호출 -> 주소(포인터)만 가져옴
            IntPtr ptr = GetSpectralSetName(spectralset);

            // (2) NULL 포인터 체크 (필수)
            if (ptr == IntPtr.Zero)
            {
                return null; // 또는 빈 문자열 ""
            }

            // (3) 주소에 있는 문자열을 C# string으로 복사 (ANSI 기준)
            // DLL 내부 메모리는 건드리지 않고 내용만 베껴옴
            string result = Marshal.PtrToStringAnsi(ptr);

            return result;
        }



        public void ReadSpectrumRange()
        {
            try
            {
                if (!IsConnected())
                    throw new Exception("장치가 연결되어 있지 않습니다.");

                Debug.WriteLine("파장별 값 읽기:");

                // 예: 400nm ~ 700nm (10nm 간격)으로 값을 읽는다고 가정
                for (int nm = 0; nm < GetWavelengthCount(); nm += 1)
                {
                    // 함수 호출
                    int value = GetWavelengthValue(nm);

                    Debug.WriteLine($"{nm}nm의 값: {value}");
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"ReadSpectrumRange Error: {ex.Message}");
            }

        }


        public float[]? GetReflectance()
        {
            try
            {   
                if(!IsConnected())
                    throw new Exception("장치가 연결되어 있지 않습니다.");

                if (!IsDataReady())
                    throw new Exception("측정결과가 없습니다.");



                // 400~700nm (10nm 간격) -> 총 31개
                float[] spectrum = new float[GetWavelengthCount()];

                int index = 0;
                for (int nm = 0; nm < GetWavelengthCount(); nm += 1)
                {

                    float value = GetSpectralData(0, nm); // 0:Spin
                    
                    spectrum[index] = value;
                    index++;
                }

                return spectrum;
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"GetReflectance Error: {ex.Message}");
                return null;

            }
        }


        //Sample 중 최신 Data의 반사율 가져오기
        public float[]? GetPriorReflectance()
        {
            try
            {
                if (!IsConnected())
                    throw new Exception("장치가 연결되어 있지 않습니다.");


                int latestSampleIndex = GetSampleCount() - 1;
                if (latestSampleIndex < 0)
                    throw new Exception("측정된 샘플이 없습니다.");


                Debug.WriteLine($"sample LatestIdx:{latestSampleIndex}");

                bool result = SetCurrentSample(latestSampleIndex);
                if (!result)
                    throw new Exception("샘플 설정 실패");


                Debug.WriteLine($"현재 Sample index:{latestSampleIndex} {result}");


                // 400~700nm (10nm 간격) -> 총 31개
                float[] spectrum = new float[GetWavelengthCount()];

                int index = 0;
                for (int nm = 0; nm < GetWavelengthCount(); nm += 1)
                {

                    float value = GetSampleData(0, nm); // 0:Spin

                    spectrum[index] = value;
                    index++;
                }

                return spectrum;
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"GetPriorReflectance Error: {ex.Message}");
                return null;

            }
        }

        public bool ClearAllSamples()
        {
            return ClearSamples();
        }

        public bool SetSample(int index)
        {
            return SetCurrentSample(index);
        }

    }

    partial class Ci62Cmd
    {
        const string Ci62DllPath = "Ci62.dll";


        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall)]
        static extern bool Connect();

        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool IsConnected();


        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool IsDataReady();


        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        static extern IntPtr GetOption(string setting);

        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        static extern IntPtr GetSerialNum();


        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool SetCurrentSample(int sample);

        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall)]

        static extern int ScanGetCount();


        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall)]
        static extern float GetSampleData(int spectralset, int wl);


        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool SetOption(string setting, string option);



        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall)]
        static extern int GetWavelengthCount();


        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool Measure();


        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall)]
        static extern int GetCalStatus();


        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        static extern IntPtr GetSpectralSetName(int spectralset);
        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall)]
        static extern int GetWavelengthValue(int wl);

        // Measure을 src code에서 호출하고 나서 반사율 가져오는거 메소드
        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall)]
        static extern float GetSpectralData(int spectralset, int wl);


        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall)]
        static extern int GetSampleCount();

        [DllImport(Ci62DllPath, CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool ClearSamples();
    }

    public class SpectralConverter
    {



        /// <summary>
        /// 분광 데이터(31개)를 입력받아 Lab로 변환
        /// </summary>
        /// <param name="reflectance">400~700nm 10nm 간격의 반사율 (0.0 ~ 100.0% )</param>
        /// <param name="curMode">광원 모드</param>
        public static Lab? CalculateLab(float[] reflectance,Illuminant curMode)
        {
            try
            {
                if (reflectance.Length != 31)
                    throw new ArgumentException("데이터 개수는 반드시 31개여야 합니다.");

                /// Step0. 현재 광원 모드에 따른 가중치 테이블 로드

                W_Table curTable = LoadTableData(curMode);
                if (curTable is null)
                    throw new Exception("가중치 테이블 로드 실패");



                double[] Wx = curTable.GetWx();
                double[] Wy = curTable.GetWy();
                double[] Wz = curTable.GetWz();
                double Xn = curTable.GetXn();
                double Yn = curTable.GetYn();
                double Zn = curTable.GetZn();


                double slope_left = (reflectance[1] - reflectance[0]);   // per 10nm step (%)
                double slope_right = (reflectance[30] - reflectance[29]);  // per 10nm step (%)

                double X = 0, Y = 0, Z = 0;

                for (int i = 0; i < 43; i++) // 360..780
                {
                    double R_pct; // % 단위로 통일

                    //if (i <= 3) // 360,370,380,390
                    //{

                    //    R_pct = reflectance[0] - slope_left * (4 - i);
                    //}
                    //else if (i >= 35) // 710..780  
                    //{

                    //    R_pct = reflectance[30] + slope_right * (i - 34);
                    //}
                    //else // 400..700 => i=4..34
                    //{
                    //    R_pct = reflectance[i - 4];
                    //}
                    if (i <= 3) // 360,370,380,390
                    {

                        R_pct = reflectance[0];
                    }
                    else if (i >= 35) // 710..780  
                    {

                        R_pct = reflectance[30];
                    }
                    else // 400..700 => i=4..34
                    {
                        R_pct = reflectance[i - 4];
                    }

                    double R = Math.Clamp(R_pct * 0.01, 0.0, 1.0);

                    X += R * Wx[i];
                    Y += R * Wy[i];
                    Z += R * Wz[i];
                }


                // ------------------------------------------------------
                // Step 2. XYZ -> Lab 변환 (CIE 공식)
                // ------------------------------------------------------
                //double var_X = F(X / Xn);
                //double var_Y = F(Y / Yn);
                //double var_Z = F(Z / Zn);

                //음수 방지
                double var_X= F(X / Xn);
                double var_Y = F(Y / Yn);
                double var_Z = F(Z / Zn);

                Lab result = new Lab();
                result.L = Math.Round(116.0 * var_Y - 16.0,2, MidpointRounding.AwayFromZero);
                result.a = Math.Round(500.0 * (var_X - var_Y),2, MidpointRounding.AwayFromZero);
                result.b = Math.Round(200.0 * (var_Y - var_Z),2, MidpointRounding.AwayFromZero);

                return result;
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"CalculateLab Error: {ex.Message}");
                return null;
            }
        }


        //XYZ->Lab 공간 변환 메소드
        static double F(double t)
        {
            const double epsLocal = 216.0 / 24389.0;
            const double kappaLocal = 24389.0 / 27.0;
            return (t >= epsLocal) ? Math.Cbrt(t) : (kappaLocal * t + 16.0) / 116.0;
        }


        static W_Table? LoadTableData(Illuminant curMode)
        {
            try
            {
                W_Table curTable;
                switch (curMode)
                {

                    case Illuminant.A_10:

                        curTable = new Data.W_Table_A_10();
                        
                        break;
                    case Illuminant.C_2:
                        curTable = new Data.W_Table_C_2();
                        break;

                    case Illuminant.D65_10:
                        curTable = new Data.W_Table_D65_10();
                        break;
                    case Illuminant.D75_10:
                        curTable = new Data.W_Table_D75_10();
                        break;
                    case Illuminant.F2_10:
                        curTable = new Data.W_Table_F2_10();
                        break;
                    default:

                        throw new Exception(" 지원하지 않는 광원 모드입니다.");
                        break;

                }


                return curTable;

            }
            catch (Exception ex)
            {
                Debug.WriteLine($"SetData Error: {ex.Message}");
                return null;

            }
                
        }

    }

}
