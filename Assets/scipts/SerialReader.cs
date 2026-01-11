using UnityEngine;
using System.IO.Ports;
using System.Threading;
using System.Globalization;

public class SerialReader : MonoBehaviour
{
    public float angle;
    public float Gx;
    public float Gy;
    public float Gz;

    SerialPort serialPort = new SerialPort("COM3", 115200);
    private Thread readThread;
    private bool isRunning = false;

    void Start()
    {
        try
        {
            serialPort.Open();
            serialPort.ReadTimeout = 50;
            isRunning = true;

            readThread = new Thread(ReadSerial);
            readThread.Start();

            Debug.Log("Serial Port Opened Successfully!");
        }
        catch
        {
            Debug.LogError("Failed to open Serial Port!");
        }
    }

    void ReadSerial()
    {
        while (isRunning && serialPort.IsOpen)
        {
            try
            {
                string data = serialPort.ReadLine();
                ParseData(data);
            }
            catch (System.TimeoutException) { }
            catch (System.Exception ex)
            {
                Debug.LogError("Serial read error: " + ex.Message);
            }
        }
    }

    void ParseData(string data)
    {
        // Expected:  -32.45, (12.10,-4.32,88.90)

        try
        {
            Debug.Log(data);
            data = data.Replace("(", "").Replace(")", "");
            string[] parts = data.Split(',');

            if (parts.Length != 4) return;

            angle = float.Parse(parts[0], CultureInfo.InvariantCulture);
            Gx = float.Parse(parts[1], CultureInfo.InvariantCulture);
            Gy = float.Parse(parts[2], CultureInfo.InvariantCulture);
            Gz = float.Parse(parts[3], CultureInfo.InvariantCulture);
        }
        catch { }
    }

    void OnApplicationQuit()
    {
        isRunning = false;

        if (readThread != null && readThread.IsAlive)
            readThread.Join();

        if (serialPort.IsOpen)
            serialPort.Close();
    }
}

//using UnityEngine;
//using System.IO.Ports;
//using System.Threading;

//public class SerialReader : MonoBehaviour
//{
//    public float SensorValue;
//    SerialPort serialPort = new SerialPort("COM3", 9600);
//    private Thread readThread;
//    private bool isRunning = false;

//    void Start()
//    {
//        try
//        {
//            serialPort.Open();
//            serialPort.ReadTimeout = 50;
//            isRunning = true;

//            // Start background thread for serial reading
//            readThread = new Thread(ReadSerial);
//            readThread.Start();

//            Debug.Log("Serial Port Opened Successfully!");
//        }
//        catch
//        {
//            Debug.LogError("Failed to open Serial Port!");
//        }
//    }

//    void ReadSerial()
//    {
//        while (isRunning && serialPort.IsOpen)
//        {
//            try
//            {
//                string data = serialPort.ReadLine();
//                if (float.TryParse(data, out float value))
//                {
//                    SensorValue = value;
//                }
//            }
//            catch (System.TimeoutException)
//            {
//                // no data available yet — ignore
//            }
//            catch (System.Exception ex)
//            {
//                Debug.LogError("Serial read error: " + ex.Message);
//            }
//        }
//    }

//    void OnApplicationQuit()
//    {
//        isRunning = false;
//        if (readThread != null && readThread.IsAlive)
//            readThread.Join();

//        if (serialPort.IsOpen)
//            Debug.LogError("serial Monitor Closed");
//            serialPort.Close();
//    }
//}
