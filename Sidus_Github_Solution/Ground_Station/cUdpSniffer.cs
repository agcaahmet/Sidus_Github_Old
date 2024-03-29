﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
namespace Ground_Station
{
    class cUdpSniffer
    {
        //Class Members
        public bool clientConnected;
        public IPEndPoint remoteIpEndPoint;
        private byte[] receivedData;
        public bool receivedDataFresh;
        public bool connectionHanged;
        public double connectionThresholdTime;
        private DateTime lastTime;
        public TimeSpan timeSpan;
        private UdpClient udpServer;
        public int port;
        //Constructors
        public cUdpSniffer(int portNum)
        {
            port = portNum;
            clientConnected = false;
            udpServer = new UdpClient(port);
            receivedDataFresh = false;
            lastTime = DateTime.Now;
            connectionThresholdTime = 150;
            connectionHanged = true;
            //Set default remote IP EP
            remoteIpEndPoint = new IPEndPoint(IPAddress.Parse(cConfig.DEFAULT_REMOTE_IP), port);
        }
        public string GetLocalIPv4()
        {
            NetworkInterfaceType _type = NetworkInterfaceType.Wireless80211;
            string output = "";
            foreach (NetworkInterface item in NetworkInterface.GetAllNetworkInterfaces())
            {
                if (item.NetworkInterfaceType == _type && item.OperationalStatus == OperationalStatus.Up)
                {
                    IPInterfaceProperties adapterProperties = item.GetIPProperties();

                    if (adapterProperties.GatewayAddresses.FirstOrDefault() != null)
                    {
                        foreach (UnicastIPAddressInformation ip in adapterProperties.UnicastAddresses)
                        {
                            if (ip.Address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
                            {
                                output = ip.Address.ToString();
                            }
                        }
                    }
                }
            }
            return output;
        }
        public string GetRemoteIP()
        {
            return remoteIpEndPoint.Address.ToString();
        }
        //Class Methods
        public bool SendPacket(byte[] data, int sizeInBytes)
        {
            //if (!clientConnected) { return false; }
            //Even  if the client was not connected, send to the default EP
            if (sizeInBytes == udpServer.Send(data, sizeInBytes, remoteIpEndPoint))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        public void ReceivePacket()
        {
            while (true)
            {
                IPEndPoint remoteEP = new IPEndPoint(IPAddress.Any, port);
                receivedData = udpServer.Receive(ref remoteEP);
                receivedDataFresh = true;
                timeSpan = DateTime.Now - lastTime;
                lastTime = DateTime.Now;

                if (timeSpan.TotalMilliseconds < connectionThresholdTime)
                    connectionHanged = false;
                else
                    connectionHanged = true;

                if (!clientConnected)
                {
                    remoteIpEndPoint = remoteEP;
                    clientConnected = true;
                }
            }
        }
        public byte[] getReceivedData()
        {
            receivedDataFresh = false;
            return receivedData;
        }
    }
}
