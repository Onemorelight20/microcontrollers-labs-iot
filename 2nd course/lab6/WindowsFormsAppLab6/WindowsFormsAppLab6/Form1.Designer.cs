using System.IO.Ports;
using System.Text;

namespace WindowsFormsAppLab6
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
            this.components = new System.ComponentModel.Container();
            this.comPortComboBox = new System.Windows.Forms.ComboBox();
            this.connectButton = new System.Windows.Forms.Button();
            this.infoLabel = new System.Windows.Forms.Label();
            this.disconnectButton = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.serialPort = new System.IO.Ports.SerialPort(this.components);
            this.Slave1Button = new System.Windows.Forms.Button();
            this.Slave2Button = new System.Windows.Forms.Button();
            this.receivedDataTextBox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // comPortComboBox
            // 
            this.comPortComboBox.Font = new System.Drawing.Font("Arial", 9.818182F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comPortComboBox.FormattingEnabled = true;
            this.comPortComboBox.Location = new System.Drawing.Point(23, 19);
            this.comPortComboBox.Name = "comPortComboBox";
            this.comPortComboBox.Size = new System.Drawing.Size(191, 25);
            this.comPortComboBox.TabIndex = 0;
            this.comPortComboBox.SelectedIndexChanged += new System.EventHandler(this.ComPortComboBox_SelectedIndexChanged);
            // 
            // connectButton
            // 
            this.connectButton.Font = new System.Drawing.Font("Arial", 9.818182F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.connectButton.Location = new System.Drawing.Point(286, 19);
            this.connectButton.Name = "connectButton";
            this.connectButton.Size = new System.Drawing.Size(184, 30);
            this.connectButton.TabIndex = 1;
            this.connectButton.Text = "Connect";
            this.connectButton.UseVisualStyleBackColor = true;
            this.connectButton.Click += new System.EventHandler(this.ConnectButton_Click);
            // 
            // infoLabel
            // 
            this.infoLabel.Font = new System.Drawing.Font("Arial", 9.818182F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.infoLabel.Location = new System.Drawing.Point(20, 76);
            this.infoLabel.Name = "infoLabel";
            this.infoLabel.Size = new System.Drawing.Size(191, 30);
            this.infoLabel.TabIndex = 3;
            this.infoLabel.Click += new System.EventHandler(this.infoLabel_Click);
            // 
            // disconnectButton
            // 
            this.disconnectButton.Font = new System.Drawing.Font("Arial", 9.818182F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.disconnectButton.Location = new System.Drawing.Point(476, 19);
            this.disconnectButton.Name = "disconnectButton";
            this.disconnectButton.Size = new System.Drawing.Size(184, 30);
            this.disconnectButton.TabIndex = 4;
            this.disconnectButton.Text = "Disconnect";
            this.disconnectButton.UseVisualStyleBackColor = true;
            this.disconnectButton.Click += new System.EventHandler(this.DisconnectButton_Click);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 500;
            this.timer1.Tick += new System.EventHandler(this.Timer1_Tick);
            // 
            // serialPort
            // 
            this.serialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.DataReceivedHandler);
            // 
            // Slave1Button
            // 
            this.Slave1Button.Enabled = false;
            this.Slave1Button.Font = new System.Drawing.Font("Arial", 9.818182F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Slave1Button.Location = new System.Drawing.Point(286, 76);
            this.Slave1Button.Name = "Slave1Button";
            this.Slave1Button.Size = new System.Drawing.Size(184, 30);
            this.Slave1Button.TabIndex = 5;
            this.Slave1Button.Text = "Slave1";
            this.Slave1Button.UseVisualStyleBackColor = true;
            this.Slave1Button.Click += new System.EventHandler(this.Slave1Button_Click);
            // 
            // Slave2Button
            // 
            this.Slave2Button.Enabled = false;
            this.Slave2Button.Font = new System.Drawing.Font("Arial", 9.818182F);
            this.Slave2Button.Location = new System.Drawing.Point(476, 76);
            this.Slave2Button.Name = "Slave2Button";
            this.Slave2Button.Size = new System.Drawing.Size(182, 30);
            this.Slave2Button.TabIndex = 6;
            this.Slave2Button.Text = "Slave2";
            this.Slave2Button.UseVisualStyleBackColor = true;
            this.Slave2Button.Click += new System.EventHandler(this.Slave2Button_Click);
            // 
            // receivedDataTextBox
            // 
            this.receivedDataTextBox.Font = new System.Drawing.Font("Arial", 9.818182F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.receivedDataTextBox.Location = new System.Drawing.Point(20, 126);
            this.receivedDataTextBox.Multiline = true;
            this.receivedDataTextBox.Name = "receivedDataTextBox";
            this.receivedDataTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.receivedDataTextBox.Size = new System.Drawing.Size(654, 247);
            this.receivedDataTextBox.TabIndex = 8;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(689, 385);
            this.Controls.Add(this.receivedDataTextBox);
            this.Controls.Add(this.Slave2Button);
            this.Controls.Add(this.Slave1Button);
            this.Controls.Add(this.disconnectButton);
            this.Controls.Add(this.infoLabel);
            this.Controls.Add(this.connectButton);
            this.Controls.Add(this.comPortComboBox);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button connectButton;
        private System.Windows.Forms.Label infoLabel;
        private System.Windows.Forms.Button disconnectButton;
        private System.Windows.Forms.Timer timer1;
        private System.IO.Ports.SerialPort serialPort;
        private System.Windows.Forms.Button Slave1Button;
        private System.Windows.Forms.Button Slave2Button;
        private System.Windows.Forms.TextBox receivedDataTextBox;
        public System.Windows.Forms.ComboBox comPortComboBox;
    }
}

