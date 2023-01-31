using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace warband_server_list
{
    public class GameServerEntry
    {
        public string ip_address { get; set; }
        public int port { get; set; }
        public string server_name { get; set; }
        public int compatible_game_version { get; set; }
        public int compatible_module_version { get; set; }

        public string module_name { get; set; }
        public int site_no { get; set; }
        public string game_server_map { get; set; }
        public int mission_template_no { get; set; }
        public string game_server_mode { get; set; }
        public int current_players { get; set; }
        public int max_players { get; set; }
        public bool password_protected { get; set; }
        public bool dedicated { get; set; }

    }
}
