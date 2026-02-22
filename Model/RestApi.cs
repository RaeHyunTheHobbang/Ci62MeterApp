using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Http;
using System.Text.Json;
using System.Threading.Tasks;
using System.ComponentModel;

namespace MeterCi62App.Model
{
    public class KCCtype
    {
        public string SN { get; set; }
        public double LValue { get; set; }
        public double AValue { get; set; }
        public double BValue { get; set; }
        public string Time { get; set; }
    }
    internal class RestApi
    {

        internal RestApi()
        {
        }

        public async Task<bool> Post(string url, string serialNum, double L, double a, double b, string time)
        {
            KCCtype curData = new KCCtype
            {
                SN = serialNum,
                LValue = L,
                AValue = a,
                BValue = b,
                Time = time
            };
            using (HttpClient client = new HttpClient())
            {
                try
                {
                    var content = _wrapData(curData);
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
