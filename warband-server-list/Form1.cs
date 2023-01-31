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
        public static extern IntPtr GetServerDetails(string ip_address, ushort port);

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            dataGridView1.Rows.Clear();
            dataGridView1.Refresh();
            ThreadUpdateList t = new ThreadUpdateList();
            t.dataGridView1 = this.dataGridView1;
            t.labelServersCount = this.label_serversonline_value;
            t.labelPlayersCount = this.label_playersonline_value;

            new Thread(new ThreadStart(t.Start)).Start();

        }



        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}
