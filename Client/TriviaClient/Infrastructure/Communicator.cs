using System;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

using TriviaClient.Constants;

namespace TriviaClient.Infrastructure
{
    public class Communicator
    {
        // Attributes -
        private readonly TcpClient m_socket;
        private readonly NetworkStream stream;
        public bool IsConnected { get; private set; }

        // Constructor - 
        public Communicator()
        {
            try
            {
                m_socket = new TcpClient(NetworkConstants.SERVER_IP_ADDRESS, NetworkConstants.SERVER_PORT);
                stream = m_socket.GetStream();
                IsConnected = true;
            }

            catch (Exception ex)
            {
                Console.WriteLine("Communicator: [ERROR]: " + ex.Message);
                IsConnected = false;

                throw new Exception("Communicator: [ERROR]: Could not connect to server.", ex);
            }
        }
        // Communication Methods - 

        public byte[] ReceiveFromServer()
        {
            try
            {
                if (!IsConnected)
                {
                    throw new InvalidOperationException("Communicator Receiving: [ERROR]: Not connected to the server.");
                }

                byte[] responseBuffer = new byte[NetworkConstants.BUFFER_SIZE];
                int bytesRead = stream.Read(responseBuffer, 0, responseBuffer.Length);

                byte[] response = new byte[bytesRead];
                Array.Copy(responseBuffer, response, bytesRead);

                return response;
            }

            catch (Exception ex)
            {
                Console.WriteLine("Communicator: [ERROR]: Error with sending the request: " + ex.Message);
                return Array.Empty<byte>();
            }
        }
        public void SendToServer(byte[] message)
        {
            try
            {
                if (!IsConnected)
                {
                    throw new InvalidOperationException("Communicator Sending: [ERROR]: Not connected to the server.");
                }

                stream.Write(message, 0, message.Length);
                stream.Flush();
            }

            catch (Exception ex)
            {
                Console.WriteLine("Communicator: [ERROR]: Error with sending the request: " + ex.Message);
            }
        }

        public void CloseConnection()
        {
            try
            {
                stream?.Close();
                m_socket?.Close();
                IsConnected = false;
            }

            catch (Exception ex)
            {
                Console.WriteLine("Communicator: [ERROR]: Error with closing connection: " + ex.Message);
            }
        }
    }
}
