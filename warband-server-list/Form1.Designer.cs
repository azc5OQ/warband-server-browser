namespace warband_server_list
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.button1 = new System.Windows.Forms.Button();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.ip_address = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.port = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.server_name = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.module_name = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.map_name = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.current_players = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.max_players = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.password_protected = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.label_serversonline = new System.Windows.Forms.Label();
            this.label_serversonline_value = new System.Windows.Forms.Label();
            this.label_playersonline = new System.Windows.Forms.Label();
            this.label_playersonline_value = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.listBox1 = new System.Windows.Forms.ListBox();

            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(698, 450);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(112, 25);
            this.button1.TabIndex = 0;
            this.button1.Text = "get-server-list";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // dataGridView1
            // 
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ip_address,
            this.port,
            this.server_name,
            this.module_name,
            this.map_name,
            this.current_players,
            this.max_players,
            this.password_protected});
            this.dataGridView1.Location = new System.Drawing.Point(-1, 2);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.Size = new System.Drawing.Size(845, 390);
            this.dataGridView1.TabIndex = 1;
            // 
            // ip_address
            // 
            this.ip_address.HeaderText = "ip address";
            this.ip_address.Name = "ip_address";
            // 
            // port
            // 
            this.port.HeaderText = "UDP port";
            this.port.Name = "port";
            // 
            // server_name
            // 
            this.server_name.HeaderText = "name";
            this.server_name.Name = "server_name";
            // 
            // module_name
            // 
            this.module_name.HeaderText = "module";
            this.module_name.Name = "module_name";
            // 
            // map_name
            // 
            this.map_name.HeaderText = "map";
            this.map_name.Name = "map_name";
            // 
            // current_players
            // 
            this.current_players.HeaderText = "used slots";
            this.current_players.Name = "current_players";
            // 
            // max_players
            // 
            this.max_players.HeaderText = "max slots";
            this.max_players.Name = "max_players";
            // 
            // password_protected
            // 
            this.password_protected.HeaderText = "password";
            this.password_protected.Name = "password_protected";
            // 
            // label_serversonline
            // 
            this.label_serversonline.AutoSize = true;
            this.label_serversonline.Location = new System.Drawing.Point(13, 424);
            this.label_serversonline.Name = "label_serversonline";
            this.label_serversonline.Size = new System.Drawing.Size(72, 13);
            this.label_serversonline.TabIndex = 2;
            this.label_serversonline.Text = "servers online";
            // 
            // label_serversonline_value
            // 
            this.label_serversonline_value.AutoSize = true;
            this.label_serversonline_value.Location = new System.Drawing.Point(102, 424);
            this.label_serversonline_value.Name = "label_serversonline_value";
            this.label_serversonline_value.Size = new System.Drawing.Size(13, 13);
            this.label_serversonline_value.TabIndex = 3;
            this.label_serversonline_value.Text = "0";
            // 
            // label_playersonline
            // 
            this.label_playersonline.AutoSize = true;
            this.label_playersonline.Location = new System.Drawing.Point(13, 450);
            this.label_playersonline.Name = "label_playersonline";
            this.label_playersonline.Size = new System.Drawing.Size(71, 13);
            this.label_playersonline.TabIndex = 4;
            this.label_playersonline.Text = "players online";
            // 
            // label_playersonline_value
            // 
            this.label_playersonline_value.AutoSize = true;
            this.label_playersonline_value.Location = new System.Drawing.Point(102, 450);
            this.label_playersonline_value.Name = "label_playersonline_value";
            this.label_playersonline_value.Size = new System.Drawing.Size(13, 13);
            this.label_playersonline_value.TabIndex = 5;
            this.label_playersonline_value.Text = "0";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(636, 413);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(33, 13);
            this.label1.TabIndex = 6;
            this.label1.Text = "game";
            // 
            // listBox1
            // 
            this.listBox1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.listBox1.FormattingEnabled = true;
            this.listBox1.Items.AddRange(new object[] {
            "warband",
            "with fire and sword"});
            this.listBox1.Location = new System.Drawing.Point(698, 407);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(112, 26);
            this.listBox1.TabIndex = 7;
            this.listBox1.SelectedIndex = 0;

            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(842, 492);
            this.Controls.Add(this.listBox1);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.label_playersonline_value);
            this.Controls.Add(this.label_playersonline);
            this.Controls.Add(this.label_serversonline_value);
            this.Controls.Add(this.label_serversonline);
            this.Controls.Add(this.dataGridView1);
            this.Controls.Add(this.button1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "mount and blade server browser";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.DataGridViewTextBoxColumn ip_address;
        private System.Windows.Forms.DataGridViewTextBoxColumn port;
        private System.Windows.Forms.DataGridViewTextBoxColumn server_name;
        private System.Windows.Forms.DataGridViewTextBoxColumn module_name;
        private System.Windows.Forms.DataGridViewTextBoxColumn map_name;
        private System.Windows.Forms.DataGridViewTextBoxColumn current_players;
        private System.Windows.Forms.DataGridViewTextBoxColumn max_players;
        private System.Windows.Forms.DataGridViewTextBoxColumn password_protected;
        private System.Windows.Forms.Label label_serversonline;
        private System.Windows.Forms.Label label_serversonline_value;
        private System.Windows.Forms.Label label_playersonline;
        private System.Windows.Forms.Label label_playersonline_value;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ListBox listBox1;
    }
}

