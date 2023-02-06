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
        public String stringToWorkWith { get; set; }

        public int thread_id { get; set; }
        public bool isThreadRunning { get; set; }

        public Label labelServersCount { get; set; }
        public Label labelPlayersCount { get; set; }

        public DataGridView dataGridView1 { get; set; }
        public void Start()
        {
            isThreadRunning = true;
            string[] arr = stringToWorkWith.Split('|');

            foreach (string s in arr)
            {
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

                IntPtr blabla = Form1.GetServerDetails(ip_and_port[0], portNumber, this.thread_id);

                string strjson = Marshal.PtrToStringAnsi(blabla);

                GameServerEntry gameServerEntry = JsonConvert.DeserializeObject<GameServerEntry>(strjson);

                if (gameServerEntry != null)
                {
               
                    dataGridView1.Invoke((MethodInvoker)delegate
                    {

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

                    labelServersCount.Invoke((MethodInvoker)delegate
                    {
                        int serversOnline = int.Parse(labelServersCount.Text);
                        serversOnline += 1;
                        labelServersCount.Text = serversOnline.ToString();
                    });


                    labelPlayersCount.Invoke((MethodInvoker)delegate
                    {
                        int playersOnline = int.Parse(labelPlayersCount.Text);
                        playersOnline += gameServerEntry.current_players; 
                        labelPlayersCount.Text = playersOnline.ToString();
                    });
                }
            }
        }
    }
}
