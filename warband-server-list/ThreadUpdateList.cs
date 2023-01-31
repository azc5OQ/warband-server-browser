using Newtonsoft.Json;
using System;
using System.Net.Http;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows.Forms;
using warband_server_list;

namespace warband_server_list
{
    public class ThreadUpdateList
    {
        private async Task<string> GetServerListFromTaleworlds()
        {
            string result = "";
            /*old way of sending Http request in C#, would be using WebRequest object*/
            try
            {
                using (var client = new HttpClient())
                {
                    client.BaseAddress = new Uri("http://warbandmain.taleworlds.com/handlerservers.ashx/?type=list");

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

        private int playersOnline;
        private int serversOnline = 0;

        public Label labelServersCount { get; set; }
        public Label labelPlayersCount { get; set; }

        public DataGridView dataGridView1 { get; set; }
        public void Start()
        {
            var task2 = Task.Run(() => this.GetServerListFromTaleworlds());

            string[] arr = task2.Result.Split('|');

            foreach (string s in arr)
            {
                //richTextBox1.Text += s;
                //richTextBox1.Text += "\n";

                string[] ip_and_port = s.Split(':');

                ushort portNumber = 0;
                if (ip_and_port.Length == 1)
                {
                    portNumber = 7240;
                }
                else
                {
                    portNumber = (ushort)Int32.Parse(ip_and_port[1]);
                }

                // MessageBox.Show(ip_and_port[0] + " : " + portNumber);

                IntPtr blabla = Form1.GetServerDetails(ip_and_port[0], portNumber);

                string strjson = Marshal.PtrToStringAnsi(blabla);

                GameServerEntry gameServerEntry = JsonConvert.DeserializeObject<GameServerEntry>(strjson);

                if (gameServerEntry != null)
                {
                    serversOnline += 1;
                    playersOnline += gameServerEntry.current_players;

                    labelServersCount.Invoke((MethodInvoker)delegate {
                        dataGridView1.Rows.Add(
                         gameServerEntry.ip_address,
                         gameServerEntry.port,
                         gameServerEntry.server_name,
                         gameServerEntry.module_name,
                         gameServerEntry.game_server_map,
                         gameServerEntry.current_players,
                         gameServerEntry.max_players,
                         gameServerEntry.password_protected
                         );
                    });

                    labelServersCount.Invoke((MethodInvoker)delegate {
                        labelServersCount.Text =  serversOnline.ToString();
                    });


                    labelPlayersCount.Invoke((MethodInvoker)delegate {
                        labelPlayersCount.Text = playersOnline.ToString();
                    });
                }
            }
        }
    }
}