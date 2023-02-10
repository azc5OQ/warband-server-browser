using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Security.Principal;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using Newtonsoft.Json;

namespace warband_server_list
{



public partial class Form1 : Form
    {

        [DllImport("core.dll", CharSet = CharSet.Ansi,
            CallingConvention = CallingConvention.Cdecl,
            EntryPoint = "get_server_details",
            ExactSpelling = true)]
        public static extern IntPtr GetServerDetails(string ip_address, ushort port, int thread_id);



        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool AllocConsole();


        public static string[] splitIpAddressStringToSmallerStrings(int numberOfStrings, string ipAddressString)
        {
            string[] result = new string[numberOfStrings];

            StringBuilder stringBuilder = new StringBuilder();

            int pipes_present_in_string = 0;

            for (int i = 0; i < ipAddressString.Length; i++)
            {
                char aa = ipAddressString[i];

                if (aa == 124)
                {
                    //Console.Write(aa);
                    pipes_present_in_string++;
                }
            }

            int pipesPerArray = pipes_present_in_string / (numberOfStrings - 1);
            int currentFoundPipesCount = 0;
            int currentIndexInArray = 0;

            for (int indexOfCharInString = 0; indexOfCharInString < ipAddressString.Length; indexOfCharInString++)
            {
                char singlechar = ipAddressString[indexOfCharInString];
                stringBuilder.Append(singlechar);

                if (singlechar == 124)
                {
                    currentFoundPipesCount++;

                    if (currentFoundPipesCount == pipesPerArray)
                    {
                        result[currentIndexInArray] = stringBuilder.ToString();
                        currentIndexInArray++;
                        stringBuilder.Clear();
                        currentFoundPipesCount = 0;
                    }
                }

                if (indexOfCharInString + 1 == ipAddressString.Length)
                {
                    if (stringBuilder.ToString().Length > 0)
                    {
                        result[currentIndexInArray] = stringBuilder.ToString();
                    }
                }
            }
            return result;
        }

        private async Task<string> GetServerListFromTaleworlds(string url)
        {
            string result = "";
            try
            {
                using (var client = new HttpClient())
                {
                    client.BaseAddress = new Uri(url);

                    HttpRequestMessage request = new HttpRequestMessage
                    {
                        Method = HttpMethod.Get,
                        RequestUri = client.BaseAddress
                    };
                    request.Headers.Add("User-Agent", "doesntmatter");

                    HttpResponseMessage httpResponseMessage = await client.SendAsync(request);
                    if (httpResponseMessage.IsSuccessStatusCode)
                    {
                        result = await httpResponseMessage.Content.ReadAsStringAsync();
                    }
                }
            }
            catch (Exception e)
            {
#if DEBUG
                System.Diagnostics.Debug.WriteLine(e.Message);
#endif
            }
            return result;
        }



        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.button1.Visible = false;
            dataGridView1.Rows.Clear();
            dataGridView1.Refresh();

            string url = "";

            if (this.listBox1.SelectedIndex == 0)
            {
                url = "http://warbandmain.taleworlds.com/handlerservers.ashx/?type=list";
            }
            else if (this.listBox1.SelectedIndex == 1)
            {
                url = "http://warbandmain.taleworlds.com/handlerservers.ashx?type=list&gametype=wfas";
            }



            var task2 = Task.Run(() => this.GetServerListFromTaleworlds(url));

            String httpResponseString = task2.Result;

            string[] strings1 = splitIpAddressStringToSmallerStrings(20, httpResponseString);


            for (int i = 0; i < strings1.Length; i++)
            {
                if (strings1[i] == null)
                {
                    continue;
                }
                ThreadUpdateList t = new ThreadUpdateList();
                t.stringToWorkWith = strings1[i]; //testing
                t.thread_id = i;
                t.dataGridView1 = this.dataGridView1;
                t.labelServersCount = this.label_serversonline_value;
                t.labelPlayersCount = this.label_playersonline_value;
                new Thread(new ThreadStart(t.Start)).Start();
            }
        }


        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}
