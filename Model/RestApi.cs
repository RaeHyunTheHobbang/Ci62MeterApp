using MeterCi62App.Data;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace MeterCi62App.Model
{
    public class KCCtype
    {

        public string SN { get; set; }

        //D65 10
        public double LValue { get; set; }
        public double AValue { get; set; }
        public double BValue { get; set; }

        //A10
        public double LValue1 { get; set; }
        public double AValue1 { get; set; }
        public double BValue1 { get; set; }
        //F2 10
        public double LValue2 { get; set; }
        public double AValue2 { get; set; }
        public double BValue2 { get; set; }
        public string Time { get; set; }

    }
    internal class RestApi
    {

        internal RestApi()
        {
        }



        public async Task<bool> Post(string url, string serialNum, Dictionary<string, Lab> curData , string time)
        {
            KCCtype sendData = new KCCtype
            {
                SN = serialNum,
                LValue = curData[Illuminant.D65_10.ToString()].L,
                AValue = curData[Illuminant.D65_10.ToString()].a,
                BValue = curData[Illuminant.D65_10.ToString()].b,

                LValue1 = curData[Illuminant.A_10.ToString()].L,
                AValue1 = curData[Illuminant.A_10.ToString()].a,
                BValue1 = curData[Illuminant.A_10.ToString()].b,

                LValue2 = curData[Illuminant.F2_10.ToString()].L,
                AValue2 = curData[Illuminant.F2_10.ToString()].a,
                BValue2 = curData[Illuminant.F2_10.ToString()].b,

                Time = time
            };
            using (HttpClient client = new HttpClient())
            {
                try
                {
                    var content = _wrapData(sendData);
                    HttpResponseMessage response = await client.PostAsync(url, content!);
                    response.EnsureSuccessStatusCode();
                    string responseBody = await response.Content.ReadAsStringAsync();
                    Console.WriteLine("Response: " + responseBody);
                    return true;
                }
                catch (Exception e)
                {
                    Console.WriteLine("Request error: " + e.Message);
                    return false;
                }
            }
        }

        StringContent? _wrapData(KCCtype curData)
        {

            var json = JsonSerializer.Serialize(curData);
            var content = new StringContent(json, Encoding.UTF8, "application/json");
            return content;
        }

    }

    
}
