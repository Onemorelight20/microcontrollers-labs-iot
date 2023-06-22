using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO.Ports;
using System.Security.Policy;
using System.Text;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace WindowsFormsAppLab6
{
    public partial class Form1 : Form
    {
        private string selectedPort;
        const byte SLAVE1_ADDRESS = 0xDE;
        const byte SLAVE2_ADDRESS = 0x7B;
        const string RECEIVING_SLAVE1_INFO = "Receiving data from Slave1:";
        const string RECEIVING_SLAVE2_INFO = "Receiving data from Slave2:";
        private byte currentMessageFromSlaveCount = 0;
        private byte currentByteReceivingCount = 0;
        bool receivingFromSlave1 = false;
        bool receivingFromSlave2 = false;
        private List<byte> receivedDataByteList = new List<byte>();
        private List<byte> checkSumList = new List<byte>();
        private const string ALL_GOOD_MARKER_CRC = " [ALL GOOD] ";

        private const byte MSG_LEN_BYTES = 11;


        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            string[] portNames = SerialPort.GetPortNames();
            comPortComboBox.Items.AddRange(portNames);
        }

        private void ComPortComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            selectedPort = comPortComboBox.SelectedItem.ToString();
        }

        private void ConnectButton_Click(object sender, EventArgs e)
        {
            try
            {
                serialPort.PortName = selectedPort;
                serialPort.Open();
                serialPort.Encoding = Encoding.UTF8;
                infoLabel.ForeColor = Color.Green;
                infoLabel.Text = "Connection successfull";
                connectButton.Enabled = false;
                disconnectButton.Enabled = true;
                comPortComboBox.Enabled = false;
                this.Slave1Button.Enabled = true;
                this.Slave2Button.Enabled = true;
            }
            catch (Exception ex)
            {
                infoLabel.ForeColor = Color.Red;
                infoLabel.Text = "Connection failed: " + ex.Message;
            }
        }

        private void DisconnectButton_Click(object sender, EventArgs e)
        {
            try
            {
                serialPort.Close();
                infoLabel.ForeColor = Color.Green;
                infoLabel.Text = "Successfully disconnected";
                disconnectButton.Enabled = false;
                connectButton.Enabled = true;
                comPortComboBox.Enabled = true;
                this.Slave1Button.Enabled = false;
                this.Slave2Button.Enabled = false;

                currentMessageFromSlaveCount = 0;
                currentByteReceivingCount = 0;
                receivingFromSlave1 = false;
                receivingFromSlave2 = false;
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

        private void Timer1_Tick(object sender, EventArgs e)
        {

        }

        private void Slave1Button_Click(object sender, EventArgs e)
        {
            byte[] b1 = new byte[3];
            b1[0] = SLAVE1_ADDRESS;
            b1[1] = 0xB1;
            serialPort.Write(b1, 0, 2);
            receivingFromSlave1 = true;
            SetText(RECEIVING_SLAVE1_INFO + Environment.NewLine);
        }

        private void Slave2Button_Click(object sender, EventArgs e)
        {
            byte[] b1 = new byte[3];
            b1[0] = SLAVE2_ADDRESS;
            b1[1] = 0xB1;
            serialPort.Write(b1, 0, 2);
            receivingFromSlave2 = true;
            SetText(RECEIVING_SLAVE2_INFO + Environment.NewLine);
        }

        private void DataReceivedHandler(
                    object sender,
                    SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            byte receivedByte = (byte)sp.ReadByte();

            if (receivingFromSlave1)
            {
                ProcessReceivedByte(receivedByte, MSG_LEN_BYTES);
            }
            else if (receivingFromSlave2)
            {
                ProcessReceivedByte(receivedByte, MSG_LEN_BYTES);
            }

            if (currentMessageFromSlaveCount == 5)
            {
                currentMessageFromSlaveCount = 0;
                receivingFromSlave1 = false;
                receivingFromSlave2 = false;
            }
        }

        private void ProcessReceivedByte(byte receivedByte, int messageLength)
        {
            if (currentByteReceivingCount < messageLength - 2)
            {
                receivedDataByteList.Add(receivedByte);
            }
            else
            {
                checkSumList.Add(receivedByte);
            }
            currentByteReceivingCount++;
            if (currentByteReceivingCount == messageLength)
            {
                byte[] bytes = receivedDataByteList.ToArray();
                byte[] checkSumbytes = checkSumList.ToArray();
                float temperature = BitConverter.ToSingle(bytes, 1);
                float humidity = BitConverter.ToSingle(bytes, 5);

                SetText("Temperature: " + temperature + ", humidity: " + humidity);

                ushort checkSumValue = (ushort)CombineBytes(checkSumbytes[0], checkSumbytes[1]);
                ushort mycheckSumResult = Crc16DectX(bytes);
                if (mycheckSumResult == checkSumValue)
                {
                    SetText(ALL_GOOD_MARKER_CRC);
                }
                else
                {
                    SetText(" [expected: " + checkSumValue.ToString() + ", got: " + mycheckSumResult.ToString() + "] ");
                }
                receivedDataByteList = new List<byte>();
                checkSumList = new List<byte>();
                SetText(Environment.NewLine);
                currentByteReceivingCount = 0;
                currentMessageFromSlaveCount++;
            }
        }

        delegate void SetTextCallback(string text);

        private void SetText(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.receivedDataTextBox.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetText);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.receivedDataTextBox.AppendText(text);
            }
        }

        public static ushort Crc16DectX(byte[] data)
        {
            ushort crc = 0x0000;
            ushort poly = 0x589;

            for (int i = 0; i < data.Length; i++)
            {
                crc ^= (ushort)(data[i] << 8);

                for (int j = 0; j < 8; j++)
                {
                    if ((crc & 0x8000) != 0)
                    {
                        crc = (ushort)((crc << 1) ^ poly);
                    }
                    else
                    {
                        crc <<= 1;
                    }
                }
            }

            return crc;
        }

        public int CombineBytes(byte b1, byte b2)
        {
            int combined = b1 << 8 | b2;
            return combined;
        }

        void byte_array_to_float(byte[] byte_array, out float num)
        {
            if (byte_array.Length != sizeof(float))
            {
                throw new ArgumentException("Byte array size must be " + sizeof(float) + " bytes.");
            }
            unsafe
            {
                fixed (byte* p = byte_array)
                {
                    num = *(float*)p;
                }
            }
        }
    }
}
