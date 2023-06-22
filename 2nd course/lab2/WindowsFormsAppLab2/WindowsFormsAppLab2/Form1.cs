using System;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsAppLab2
{
    public partial class Form1 : Form
    {
        private string selectedPort;
        private int algoTimerCounter;
        private int algoNum;
        private int button1Num = 0;
        private int button2Num = 0;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            string[] portNames = SerialPort.GetPortNames();
            comPortComboBox.Items.AddRange(portNames);
        }

        private void comPortComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            selectedPort = comPortComboBox.SelectedItem.ToString();
        }

        private void connectButton_Click(object sender, EventArgs e)
        {
            try
            {
                serialPort = new SerialPort(selectedPort, 9600);
                serialPort.Open();
                this.serialPort.StopBits = StopBits.Two;
                this.serialPort.DataReceived += new SerialDataReceivedEventHandler(this.serialPort1_DataReceived);
                serialPort.Encoding = Encoding.UTF8;
                infoLabel.ForeColor = Color.Green;
                infoLabel.Text = "Connection successfull";
                connectButton.Enabled = false;
                disconnectButton.Enabled = true;
            }
            catch (Exception ex)
            {
                infoLabel.ForeColor = Color.Red;
                infoLabel.Text = "Connection failed: " + ex.Message;
            }
        }

        private void disconnectButton_Click(object sender, EventArgs e)
        {
            try
            {
                serialPort.Close();
                serialPort = null;
                infoLabel.ForeColor = Color.Green;
                infoLabel.Text = "Successfully disconnected";
                disconnectButton.Enabled = false;
                connectButton.Enabled = true;
            }
            catch (Exception ex)
            {
                infoLabel.ForeColor = Color.Red;
                infoLabel.Text = "Failed to disconnect: " + ex.Message;
            }
        }

        private void infoLabel_Click(object sender, EventArgs e)
        {

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (algoNum == 3)
            {
                execAlgo3();
            } else if (algoNum == 5)
            {
                execAlgo5();
            }
        }

        private void algo1Button_Click(object sender, EventArgs e)
        {
            try
            {
                byte[] myBytes = {3};
                serialPort.Write(myBytes, 0, myBytes.Length);

            }
            catch (Exception ex)
            {
                infoLabel.ForeColor = Color.Red;
                infoLabel.Text = "Failed to send Algo1 info: " + ex.Message;
            }
            
        }

        private void algo2Button_Click(object sender, EventArgs e)
        {
            try
            {
                byte[] myBytes = { 5 };
                serialPort.Write(myBytes, 0, myBytes.Length);
            }
            catch (Exception ex)
            {
                infoLabel.ForeColor = Color.Red;
                infoLabel.Text = "Failed to send Algo2 info: " + ex.Message;
            }
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            string incomingData =  serialPort.ReadByte().ToString();
            if (incomingData.Equals("3"))
            {
               
                algoNum = 3;
                algoTimerCounter = 0;
            }
            else if (incomingData.Equals("5"))
            {

                algoNum = 5;
                algoTimerCounter = 0;
    }
        }

        private void execAlgo3() {
            if (algoTimerCounter == 5) {
                algoNum = 0;
                button1Num = 0;
                button2Num = 0;
                clearButtons();
            } else
            {
                if (button1Num == 0 && button2Num == 0)
                {
                    button1Num = 1;
                    button2Num = 8;
                    getButtonNamed($"button{button1Num}").BackColor = Color.Red;
                    getButtonNamed($"button{button2Num}").BackColor = Color.Red;
                } else {
                    getButtonNamed($"button{button1Num}").BackColor = Color.White;
                    getButtonNamed($"button{button2Num}").BackColor = Color.White;
                    button1Num += 1;
                    button2Num -= 1;
                    getButtonNamed($"button{button1Num}").BackColor = Color.Red;
                    getButtonNamed($"button{button2Num}").BackColor = Color.Red;
                }
                algoTimerCounter++;
            }
        }

        private void execAlgo5() {
            if (algoTimerCounter == 8)
            {
                algoNum = 0;
                button1Num = 0;
                button2Num = 0;
                clearButtons();
            }
            else
            {
                if (button1Num == 0 && button2Num == 0)
                {
                    button1Num = 1;
                    button2Num = button1Num;
                    getButtonNamed($"button{button2Num}").BackColor = Color.Red;
                }
                else
                {
                    getButtonNamed($"button{button1Num}").BackColor = Color.White;
                    if (algoTimerCounter < 4)
                    {
                        button2Num = button2Num + 2;
                    } else
                    {
                        button2Num = 8 - (algoTimerCounter % 4) * 2;
                    }
                    button1Num = button2Num;
                    getButtonNamed($"button{button2Num}").BackColor = Color.Red;
                }
                algoTimerCounter++;
            }
        }


        private Button getButtonNamed(string buttonName)
        {
            Button foundButton = (Button)Controls.Find(buttonName, false).FirstOrDefault();
            return foundButton;
        }

        private void clearButtons() {
            for (int i = 1; i <= 8; i++)
            {
                getButtonNamed($"button{i}").BackColor = Color.White;
            }

        }
    }
}
